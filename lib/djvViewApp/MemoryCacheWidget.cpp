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

#include <djvViewApp/MemoryCacheWidget.h>

#include <djvViewApp/FileSettings.h>
#include <djvViewApp/FileSystem.h>

#include <djvUI/FormLayout.h>
#include <djvUI/IntSlider.h>
#include <djvUI/Label.h>
#include <djvUI/SettingsSystem.h>
#include <djvUI/ToggleButton.h>

#include <djvCore/Context.h>
#include <djvCore/OS.h>

using namespace djv::Core;

namespace djv
{
    namespace ViewApp
    {
        struct MemoryCacheWidget::Private
        {
            std::shared_ptr<UI::ToggleButton> enabledButton;
            std::shared_ptr<UI::IntSlider> maxGBSlider;
            std::shared_ptr<UI::Label> percentageLabel;
            std::shared_ptr<UI::FormLayout> layout;
            std::shared_ptr<ValueObserver<bool> > enabledObserver;
            std::shared_ptr<ValueObserver<int> > maxGBObserver;
            std::shared_ptr<ValueObserver<float> > percentageObserver;
        };

        void MemoryCacheWidget::_init(const std::shared_ptr<Core::Context>& context)
        {
            Widget::_init(context);
            DJV_PRIVATE_PTR();
            setClassName("djv::ViewApp::MemoryCacheWidget");

            p.enabledButton = UI::ToggleButton::create(context);

            p.maxGBSlider = UI::IntSlider::create(context);
            p.maxGBSlider->setRange(IntRange(1, OS::getRAMSize() / Memory::gigabyte));

            p.percentageLabel = UI::Label::create(context);
            p.percentageLabel->setTextHAlign(UI::TextHAlign::Left);

            p.layout = UI::FormLayout::create(context);
            p.layout->addChild(p.enabledButton);
            p.layout->addChild(p.maxGBSlider);
            p.layout->addChild(p.percentageLabel);
            addChild(p.layout);

            auto contextWeak = std::weak_ptr<Context>(context);
            p.enabledButton->setCheckedCallback(
                [contextWeak](bool value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheEnabled(value);
                        }
                    }
                });
            p.maxGBSlider->setValueCallback(
                [contextWeak](int value)
                {
                    if (auto context = contextWeak.lock())
                    {
                        auto settingsSystem = context->getSystemT<UI::Settings::System>();
                        if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
                        {
                            fileSettings->setCacheMaxGB(value);
                        }
                    }
                });

            auto weak = std::weak_ptr<MemoryCacheWidget>(
                std::dynamic_pointer_cast<MemoryCacheWidget>(shared_from_this()));
            auto settingsSystem = context->getSystemT<UI::Settings::System>();
            if (auto fileSettings = settingsSystem->getSettingsT<FileSettings>())
            {
                p.enabledObserver = ValueObserver<bool>::create(
                    fileSettings->observeCacheEnabled(),
                    [weak](bool value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->enabledButton->setChecked(value);
                        }
                    });

                p.maxGBObserver = ValueObserver<int>::create(
                    fileSettings->observeCacheMaxGB(),
                    [weak](int value)
                    {
                        if (auto widget = weak.lock())
                        {
                            widget->_p->maxGBSlider->setValue(value);
                        }
                    });
            }

            if (auto fileSystem = context->getSystemT<FileSystem>())
            {
                p.percentageObserver = ValueObserver<float>::create(
                    fileSystem->observeCachePercentage(),
                    [weak](float value)
                    {
                        if (auto widget = weak.lock())
                        {
                            std::stringstream ss;
                            ss.precision(2);
                            ss << std::fixed << value;
                            ss << "%";
                            widget->_p->percentageLabel->setText(ss.str());
                        }
                    });
            }
        }

        MemoryCacheWidget::MemoryCacheWidget() :
            _p(new Private)
        {}

        MemoryCacheWidget::~MemoryCacheWidget()
        {}

        std::shared_ptr<MemoryCacheWidget> MemoryCacheWidget::create(const std::shared_ptr<Core::Context>& context)
        {
            auto out = std::shared_ptr<MemoryCacheWidget>(new MemoryCacheWidget);
            out->_init(context);
            return out;
        }

        void MemoryCacheWidget::_preLayoutEvent(Event::PreLayout&)
        {
            const auto& style = _getStyle();
            _setMinimumSize(_p->layout->getMinimumSize() + getMargin().getSize(style));
        }
        
        void MemoryCacheWidget::_layoutEvent(Event::Layout&)
        {
            const auto& style = _getStyle();
            _p->layout->setGeometry(getMargin().bbox(getGeometry(), style));
        }

        void MemoryCacheWidget::_localeEvent(Event::Locale & event)
        {
            Widget::_localeEvent(event);
            DJV_PRIVATE_PTR();
            p.layout->setText(p.enabledButton, _getText(DJV_TEXT("Enabled")) + ":");
            p.layout->setText(p.maxGBSlider, _getText(DJV_TEXT("Size (gigabytes)")) + ":");
            p.layout->setText(p.percentageLabel, _getText(DJV_TEXT("Percentage used")) + ":");
        }

    } // namespace ViewApp
} // namespace djv

