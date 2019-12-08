//------------------------------------------------------------------------------
// Copyright (c) 2019 Darby Johnston
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

#pragma once

#include <djvUIComponents/SceneWidget.h>
#include <djvUIComponents/FileBrowserDialog.h>

#include <djvUI/Action.h>
#include <djvUI/ActionGroup.h>
#include <djvUI/MDICanvas.h>
#include <djvUI/Window.h>

#include <djvCore/FileInfo.h>
#include <djvCore/Timer.h>
#include <djvCore/ValueObserver.h>

class CameraTool;
class InfoTool;
class RenderTool;

class MainWindow : public djv::UI::Window
{
    DJV_NON_COPYABLE(MainWindow);

protected:
    void _init(const std::shared_ptr<djv::Core::Context>&);
    MainWindow();

public:
    virtual ~MainWindow();

    static std::shared_ptr<MainWindow> create(const std::shared_ptr<djv::Core::Context>&);

    void setScene(const std::shared_ptr<djv::Scene::Scene>&);

    void setOpenCallback(const std::function<void(const djv::Core::FileSystem::FileInfo)>&);
    void setReloadCallback(const std::function<void(void)>&);
    void setExitCallback(const std::function<void(void)>&);

protected:
    void _initEvent(djv::Core::Event::Init&) override;

private:
    void _open();

    std::map<std::string, std::map<std::string, std::shared_ptr<djv::UI::Action> > > _actions;
    std::shared_ptr<djv::UI::ActionGroup> _sceneRotateActionGroup;

    std::shared_ptr<djv::UI::SceneWidget> _sceneWidget;
    std::shared_ptr<CameraTool> _cameraTool;
    std::shared_ptr<InfoTool> _infoTool;
    std::shared_ptr<RenderTool> _renderTool;
    std::shared_ptr<djv::UI::MDI::Canvas> _mdiCanvas;

    std::shared_ptr<djv::UI::FileBrowser::Dialog> _fileBrowserDialog;
    djv::Core::FileSystem::Path _fileBrowserPath = djv::Core::FileSystem::Path(".");
    
    std::function<void(const djv::Core::FileSystem::FileInfo)> _openCallback;
    std::function<void(void)> _reloadCallback;
    std::function<void(void)> _exitCallback;
    
    std::map<std::string, std::map<std::string, std::shared_ptr<djv::Core::ValueObserver<bool> > > > _actionObservers;
    std::shared_ptr<djv::Core::ValueObserver<djv::UI::CameraInfo> > _cameraInfoObserver;
    std::shared_ptr<djv::Core::ValueObserver<djv::Core::BBox3f> > _bboxObserver;
    std::shared_ptr<djv::Core::ValueObserver<size_t> > _primitivesCountObserver;
    std::shared_ptr<djv::Core::ValueObserver<size_t> > _triangleCountObserver;

    std::shared_ptr<djv::Core::Time::Timer> _statsTimer;
};