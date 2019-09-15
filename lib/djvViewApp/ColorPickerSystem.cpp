//------------------------------------------------------------------------------
// Copyright (c) 2004-2019 Darby Johnston
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

#include <djvViewApp/ColorPickerSystem.h>

#include <djvViewApp/ColorPickerSettings.h>
#include <djvViewApp/ColorPickerWidget.h>

#include <djvUI/Action.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Shortcut.h>

#include <djvCore/Context.h>
#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct ColorPickerSystem::Private
        {
            std::shared_ptr<ColorPickerSettings> settings;
            std::map<std::string, std::shared_ptr<UI::Action> > actions;
            std::map<std::string, std::shared_ptr<ValueObserver<bool> > > actionObservers;
            std::shared_ptr<ValueObserver<std::string> > localeObserver;
        };

        void ColorPickerSystem::_init(const std::shared_ptr<Core::Context>& context)
        {
            IToolSystem::_init("djv::ViewApp::ColorPickerSystem", context);
            DJV_PRIVATE_PTR();

            p.settings = ColorPickerSettings::create(context);
            _setWidgetGeom(p.settings->getWidgetGeom());

            p.actions["ColorPicker"] = UI::Action::create();
            p.actions["ColorPicker"]->setIcon("djvIconColorPicker");
            p.actions["ColorPicker"]->setShortcut(GLFW_KEY_2);

            auto weak = std::weak_ptr<ColorPickerSystem>(std::dynamic_pointer_cast<ColorPickerSystem>(shared_from_this()));
            p.localeObserver = ValueObserver<std::string>::create(
                context->getSystemT<TextSystem>()->observeCurrentLocale(),
                [weak](const std::string & value)
            {
                if (auto system = weak.lock())
                {
                    system->_textUpdate();
                }
            });
        }

        ColorPickerSystem::ColorPickerSystem() :
            _p(new Private)
        {}

        ColorPickerSystem::~ColorPickerSystem()
        {
            DJV_PRIVATE_PTR();
            _closeWidget("ColorPicker");
            p.settings->setWidgetGeom(_getWidgetGeom());
        }

        std::shared_ptr<ColorPickerSystem> ColorPickerSystem::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<ColorPickerSystem>(new ColorPickerSystem);
            out->_init(context);
            return out;
        }

        ToolActionData ColorPickerSystem::getToolAction() const
        {
            return
            {
                _p->actions["ColorPicker"],
                "B"
            };
        }

        void ColorPickerSystem::setCurrentTool(bool value)
        {
            if (value)
            {
                if (auto context = getContext().lock())
                {
                    _openWidget("ColorPicker", ColorPickerWidget::create(context));
                }
            }
            else
            {
                _closeWidget("ColorPicker");
            }
        }

        std::map<std::string, std::shared_ptr<UI::Action> > ColorPickerSystem::getActions() const
        {
            return _p->actions;
        }

        void ColorPickerSystem::_textUpdate()
        {
            DJV_PRIVATE_PTR();
            p.actions["ColorPicker"]->setText(_getText(DJV_TEXT("Color Picker")));
            p.actions["ColorPicker"]->setTooltip(_getText(DJV_TEXT("Color picker tooltip")));
        }

        void ColorPickerSystem::_closeWidget(const std::string& value)
        {
            DJV_PRIVATE_PTR();
            const auto i = p.actions.find(value);
            if (i != p.actions.end())
            {
                i->second->setChecked(false);
            }
            IToolSystem::_closeWidget(value);
        }

    } // namespace ViewApp
} // namespace djv

