//------------------------------------------------------------------------------
// Copyright (c) 2018 Darby Johnston
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// * Redistributions of source code must retain the above copyright notice,
//   this list of conditions, and the following disclaimer.
// * Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions, and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
// * Neither the names of the copyright holders nor the names of any
//   contributors may be used to endorse or promote products derived from this
//   software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//------------------------------------------------------------------------------

#include <djvUI/MDICanvas.h>

#include <djvUI/Icon.h>
#include <djvUI/Label.h>
#include <djvUI/MDIWindow.h>
#include <djvUI/RowLayout.h>
#include <djvUI/StackLayout.h>

#include <djvAV/Render2DSystem.h>

#include <set>

using namespace djv::Core;

namespace djv
{
    namespace UI
    {
        namespace MDI
        {
            struct Canvas::Private
            {
                glm::vec2 canvasSize = glm::vec2(2000.f, 2000.f);
                std::vector<std::shared_ptr<Widget> > windows;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > moveHandleToWindow;
                std::map<std::shared_ptr<IObject>, std::shared_ptr<Widget> > resizeHandleToWindow;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToMoveHandle;
                std::map<std::shared_ptr<Widget>, std::shared_ptr<IObject> > windowToResizeHandle;
                Event::PointerID pressed = 0;
                glm::vec2 pressedPos;
                glm::vec2 pressedOffset;
                std::function<void(const std::shared_ptr<Widget> &)> activeWindowCallback;
            };

            void Canvas::_init(Context * context)
            {
                Widget::_init(context);

                setClassName("djv::UI::MDI::Canvas");
            }

            Canvas::Canvas() :
                _p(new Private)
            {}

            Canvas::~Canvas()
            {}

            std::shared_ptr<Canvas> Canvas::create(Context * context)
            {
                auto out = std::shared_ptr<Canvas>(new Canvas);
                out->_init(context);
                return out;
            }

            const glm::vec2 & Canvas::getCanvasSize() const
            {
                return _p->canvasSize;
            }

            void Canvas::setCanvasSize(const glm::vec2 & size)
            {
                DJV_PRIVATE_PTR();
                if (size == p.canvasSize)
                    return;
                p.canvasSize = size;
                _resize();
            }

            std::shared_ptr<Widget> Canvas::getActiveWindow() const
            {
                const auto & children = getChildrenT<Widget>();
                return children.size() ? children.back() : nullptr;
            }

            void Canvas::nextWindow()
            {
                const auto & children = getChildrenT<Widget>();
                const size_t size = children.size();
                if (size > 1)
                {
                    children.back()->moveToBack();
                }
            }

            void Canvas::prevWindow()
            {
                const auto & children = getChildrenT<Widget>();
                const size_t size = children.size();
                if (size > 1)
                {
                    children.front()->moveToFront();
                }
            }

            void Canvas::setActiveWindowCallback(const std::function<void(const std::shared_ptr<Widget> &)> & value)
            {
                _p->activeWindowCallback = value;
            }

            void Canvas::_preLayoutEvent(Event::PreLayout&)
            {
                _setMinimumSize(_p->canvasSize);
            }

            void Canvas::_layoutEvent(Event::Layout&)
            {}

            void Canvas::_paintEvent(Event::Paint& event)
            {
                Widget::_paintEvent(event);
                if (auto render = _getRenderSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float s = style->getMetric(Style::MetricsRole::Shadow);
                        render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Shadow)));
                        for (const auto & i : getChildrenT<Widget>())
                        {
                            BBox2f g = i->getGeometry();
                            g.min.x += s;
                            g.min.y += s;
                            g.max.x += s;
                            g.max.y += s;
                            render->drawRectangle(g);
                        }
                    }
                }
            }

            void Canvas::_paintOverlayEvent(Event::PaintOverlay& event)
            {
                if (auto render = _getRenderSystem().lock())
                {
                    if (auto style = _getStyle().lock())
                    {
                        const float b = style->getMetric(Style::MetricsRole::Border);
                        const auto & children = getChildrenT<Widget>();
                        if (children.size())
                        {
                            const BBox2f g = children.back()->getGeometry().margin(b);
                            render->setFillColor(_getColorWithOpacity(style->getColor(Style::ColorRole::Checked)));
                            render->drawRectangle(BBox2f(g.min, glm::vec2(g.max.x, g.min.y + b)));
                            render->drawRectangle(BBox2f(glm::vec2(g.min.x, g.min.y + b), glm::vec2(g.min.x + b, g.max.y - b)));
                            render->drawRectangle(BBox2f(glm::vec2(g.max.x - b, g.min.y + b), glm::vec2(g.max.x, g.max.y - b)));
                            render->drawRectangle(BBox2f(glm::vec2(g.min.x, g.max.y - b), glm::vec2(g.max.x, g.max.y)));
                        }
                    }
                }
            }

            void Canvas::_childAddedEvent(Event::ChildAdded & value)
            {
                if (auto window = std::dynamic_pointer_cast<IWindow>(value.getChild()))
                {
                    auto moveHandle = window->getMoveHandle();
                    auto resizeHandle = window->getResizeHandle();
                    moveHandle->installEventFilter(shared_from_this());
                    resizeHandle->installEventFilter(shared_from_this());

                    DJV_PRIVATE_PTR();
                    p.windows.push_back(window);
                    p.moveHandleToWindow[moveHandle] = window;
                    p.resizeHandleToWindow[resizeHandle] = window;
                    p.windowToMoveHandle[window] = moveHandle;
                    p.windowToResizeHandle[window] = resizeHandle;

                    _redraw();

                    if (p.activeWindowCallback)
                    {
                        p.activeWindowCallback(window);
                    }
                }
            }

            void Canvas::_childRemovedEvent(Event::ChildRemoved & value)
            {
                if (auto window = std::dynamic_pointer_cast<IWindow>(value.getChild()))
                {
                    DJV_PRIVATE_PTR();
                    {
                        const auto j = std::find(p.windows.begin(), p.windows.end(), window);
                        if (j != p.windows.end())
                        {
                            const bool active = j == p.windows.begin();
                            p.windows.erase(j);
                            if (active)
                            {
                                if (p.windows.size())
                                {
                                    if (p.activeWindowCallback)
                                    {
                                        p.activeWindowCallback(p.windows.back());
                                    }
                                }
                                else
                                {
                                    if (p.activeWindowCallback)
                                    {
                                        p.activeWindowCallback(nullptr);
                                    }
                                }
                            }
                        }
                    }
                    {
                        const auto j = p.windowToMoveHandle.find(window);
                        if (j != p.windowToMoveHandle.end())
                        {
                            const auto k = p.moveHandleToWindow.find(j->second);
                            if (k != p.moveHandleToWindow.end())
                            {
                                p.moveHandleToWindow.erase(k);
                            }
                            p.windowToMoveHandle.erase(j);
                        }
                    }
                    {
                        const auto j = p.windowToResizeHandle.find(window);
                        if (j != p.windowToResizeHandle.end())
                        {
                            const auto k = p.resizeHandleToWindow.find(j->second);
                            if (k != p.resizeHandleToWindow.end())
                            {
                                p.resizeHandleToWindow.erase(k);
                            }
                            p.windowToResizeHandle.erase(j);
                        }
                    }
                    _redraw();
                }
            }

            bool Canvas::_eventFilter(const std::shared_ptr<IObject>& object, Event::IEvent& event)
            {
                /*{
                    std::stringstream ss;
                    ss << event.getEventType();
                    _log(ss.str());
                }*/
                switch (event.getEventType())
                {
                case Event::Type::PointerMove:
                {
                    Event::PointerMove& pointerMoveEvent = static_cast<Event::PointerMove&>(event);
                    pointerMoveEvent.accept();
                    DJV_PRIVATE_PTR();
                    if (pointerMoveEvent.getPointerInfo().id == p.pressed)
                    {
                        if (auto style = _getStyle().lock())
                        {
                            const float shadow = style->getMetric(Style::MetricsRole::Shadow);
                            const BBox2f& g = getGeometry();
                            const auto moveHandleToWindow = p.moveHandleToWindow.find(object);
                            const auto resizeHandleToWindow = p.resizeHandleToWindow.find(object);
                            if (moveHandleToWindow != p.moveHandleToWindow.end())
                            {
                                const auto window = std::find(p.windows.begin(), p.windows.end(), moveHandleToWindow->second);
                                if (window != p.windows.end())
                                {
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + p.pressedOffset - g.min;
                                    glm::vec2 windowPos;
                                    windowPos.x = Math::clamp(pos.x, -shadow, g.w() - (*window)->getWidth() + shadow);
                                    windowPos.y = Math::clamp(pos.y, -shadow, g.h() - (*window)->getHeight() + shadow);
                                    (*window)->move(windowPos);
                                }
                            }
                            else if (resizeHandleToWindow != p.resizeHandleToWindow.end())
                            {
                                const auto window = std::find(p.windows.begin(), p.windows.end(), resizeHandleToWindow->second);
                                if (window != p.windows.end())
                                {
                                    const glm::vec2 min = (*window)->getMinimumSize();
                                    const BBox2f & g = (*window)->getGeometry();
                                    const glm::vec2 pos = pointerMoveEvent.getPointerInfo().projectedPos + p.pressedOffset - g.min;
                                    glm::vec2 pos2;
                                    pos2.x = Math::clamp(pos.x, g.min.x + min.x, g.w() + shadow);
                                    pos2.y = Math::clamp(pos.y, g.min.y + min.y, g.h() + shadow);
                                    (*window)->resize(pos2 - g.min + g.min);
                                }
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonPress:
                {
                    Event::ButtonPress& buttonPressEvent = static_cast<Event::ButtonPress&>(event);
                    DJV_PRIVATE_PTR();
                    if (!p.pressed)
                    {
                        const auto i = p.moveHandleToWindow.find(object);
                        const auto j = p.resizeHandleToWindow.find(object);
                        if (i != p.moveHandleToWindow.end())
                        {
                            event.accept();
                            p.pressed = buttonPressEvent.getPointerInfo().id;
                            p.pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            i->second->moveToFront();
                            p.pressedOffset = i->second->getGeometry().min - p.pressedPos;
                            if (p.activeWindowCallback)
                            {
                                p.activeWindowCallback(i->second);
                            }
                        }
                        else if (j != p.resizeHandleToWindow.end())
                        {
                            event.accept();
                            p.pressed = buttonPressEvent.getPointerInfo().id;
                            p.pressedPos = buttonPressEvent.getPointerInfo().projectedPos;
                            j->second->moveToFront();
                            p.pressedOffset = j->second->getGeometry().max - p.pressedPos;
                            if (p.activeWindowCallback)
                            {
                                p.activeWindowCallback(j->second);
                            }
                        }
                    }
                    return true;
                }
                case Event::Type::ButtonRelease:
                {
                    Event::ButtonRelease& buttonReleaseEvent = static_cast<Event::ButtonRelease&>(event);
                    DJV_PRIVATE_PTR();
                    if (p.pressed == buttonReleaseEvent.getPointerInfo().id)
                    {
                        event.accept();
                        p.pressed = 0;
                    }
                    return true;
                }
                default: break;
                }
                return false;
            }

        } // namespace MDI
    } // namespace UI
} // namespace djdv
