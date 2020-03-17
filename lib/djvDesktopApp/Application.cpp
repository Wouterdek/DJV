//------------------------------------------------------------------------------
// Copyright (c) 2004-2020 Darby Johnston
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

#include <djvDesktopApp/EventSystem.h>
#include <djvDesktopApp/GLFWSystem.h>

#include <djvUI/SettingsSystem.h>
#include <djvUI/UISystem.h>

#include <djvAV/AVSystem.h>
#include <djvAV/GLFWSystem.h>
#include <djvAV/IO.h>
#include <djvAV/Render2D.h>

#include <djvCore/TextSystem.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>

using namespace djv::Core;

namespace djv
{
    namespace Desktop
    {
        namespace
        {
            //! \todo Should this be configurable?
            const size_t frameRate = 60;
        
        } // namespace

        struct Application::Private
        {
            bool resetSettings = false;
        };

        void Application::_init(std::list<std::string>& args)
        {
            CmdLine::Application::_init(args);
            DJV_PRIVATE_PTR();

            // Parse the command-line.
            auto arg = args.begin();
            while (arg != args.end())
            {
                if ("-init_settings" == *arg)
                {
                    arg = args.erase(arg);
                    p.resetSettings = true;
                }
                else
                {
                    ++arg;
                }
            }

            // Create the systems.
            auto glfwSystem = GLFWSystem::create(shared_from_this());
            auto uiSystem = UI::UISystem::create(p.resetSettings, shared_from_this());
            auto avGLFWSystem = getSystemT<AV::GLFW::System>();
            auto glfwWindow = avGLFWSystem->getGLFWWindow();
            auto eventSystem = EventSystem::create(glfwWindow, shared_from_this());
        }
        
        Application::Application() :
            _p(new Private)
        {}
        
        Application::~Application()
        {}

        std::shared_ptr<Application> Application::create(std::list<std::string>& args)
        {
            auto out = std::shared_ptr<Application>(new Application);
            out->_init(args);
            return out;
        }

        void Application::printUsage()
        {
            auto textSystem = getSystemT<Core::TextSystem>();
            std::cout << " " << textSystem->getText(DJV_TEXT("cli_ui_options")) << std::endl;
            std::cout << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings")) << std::endl;
            std::cout << "   " << textSystem->getText(DJV_TEXT("cli_option_init_settings_description")) << std::endl;
            std::cout << std::endl;

            CmdLine::Application::printUsage();
        }

        void Application::run()
        {
            DJV_PRIVATE_PTR();
            auto avGLFWSystem = getSystemT<AV::GLFW::System>();
            if (auto glfwWindow = avGLFWSystem->getGLFWWindow())
            {
                glfwShowWindow(glfwWindow);
                auto start = std::chrono::steady_clock::now();
                auto delta = Time::Duration::zero();
                auto frameTime = std::chrono::microseconds(1000000 / frameRate);
                _setRunning(true);
                while (_isRunning() && glfwWindow && !glfwWindowShouldClose(glfwWindow))
                {
                    glfwPollEvents();
                    tick(start, delta);
                    const auto systemTime = std::chrono::steady_clock::now();

                    auto end = std::chrono::steady_clock::now();
                    delta = std::chrono::duration_cast<Time::Duration>(end - start);
                    Time::sleep(frameTime - delta);
                    end = std::chrono::steady_clock::now();
                    //delta = std::chrono::duration_cast<Time::Unit>(end - start);
                    //std::cout << "frame: " <<
                    //    std::chrono::duration_cast<Time::Unit>(systemTime - start).count() << "/" <<
                    //    delta.count() << "/" <<
                    //    (1000000 / frameRate) << std::endl;
                    start = end;
                }
            }
        }

    } // namespace Desktop
} // namespace Gp
