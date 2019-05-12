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

#include <djvDesktopApp/Application.h>

#include <djvUIComponents/LineGraphWidget.h>

#include <djvUI/GroupBox.h>
#include <djvUI/RowLayout.h>
#include <djvUI/Window.h>

#include <djvCore/Error.h>
#include <djvCore/Timer.h>

using namespace djv;

int main(int argc, char ** argv)
{
    int r = 0;
    try
    {
        auto app = Desktop::Application::create(argc, argv);

        auto layout = UI::VerticalLayout::create(app.get());
        layout->setMargin(UI::MetricsRole::MarginLarge);
        layout->setSpacing(UI::MetricsRole::SpacingLarge);

        std::vector<std::shared_ptr<UI::LineGraphWidget> > lineGraphWidgets;
        auto vLayout = UI::VerticalLayout::create(app.get());
        for (size_t i = 0; i < 3; ++i)
        {
            auto lineGraphWidget = UI::LineGraphWidget::create(app.get());
            lineGraphWidgets.push_back(lineGraphWidget);
            vLayout->addChild(lineGraphWidget);
        }
        auto groupBox = UI::GroupBox::create("LINE GRAPHS", app.get());
        groupBox->addChild(vLayout);
        layout->addChild(groupBox);

        auto window = UI::Window::create(app.get());
        window->addChild(layout);
        window->show();

        float v = 0.f;
        auto timer = Core::Time::Timer::create(app.get());
        timer->setRepeating(true);
        timer->start(
            std::chrono::milliseconds(40),
            [lineGraphWidgets, &v](float diff)
        {
            float f = 5.f;
            for (size_t i = 0; i < lineGraphWidgets.size(); ++i)
            {
                const float vv = sinf(v * f);
                lineGraphWidgets[i]->addSample(vv);

                f *= .5f;
            }
            v += diff;
        });

        return app->run();
    }
    catch (const std::exception & e)
    {
        std::cout << Core::Error::format(e) << std::endl;
    }
    return r;
}