// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#pragma once

#include <djvScene3D/IO.h>

namespace djv
{
    namespace Scene3D
    {
        namespace IO
        {
            //! This namespace provides OBJ file I/O.
            namespace OBJ
            {
                static const std::string pluginName = "OBJ";
                static const std::set<std::string> fileExtensions = { ".obj" };

                //! This struct provides the OBJ file I/O options.
                struct Options
                {
                };

                //! This class provides the OBJ file reader.
                class Read : public IRead
                {
                    DJV_NON_COPYABLE(Read);

                protected:
                    Read();

                public:
                    ~Read() override;

                    static std::shared_ptr<Read> create(
                        const System::File::Info&,
                        const std::shared_ptr<System::TextSystem>&,
                        const std::shared_ptr<System::ResourceSystem>&,
                        const std::shared_ptr<System::LogSystem>&);

                    std::future<Info> getInfo() override;
                    std::future<std::shared_ptr<Scene> > getScene() override;

                private:
                    DJV_PRIVATE();
                };

                //! This class provides the OBJ file I/O plugin.
                class Plugin : public IPlugin
                {
                    DJV_NON_COPYABLE(Plugin);

                protected:
                    Plugin();

                public:
                    ~Plugin() override;

                    static std::shared_ptr<Plugin> create(const std::shared_ptr<System::Context>&);

                    rapidjson::Value getOptions(rapidjson::Document::AllocatorType&) const override;
                    void setOptions(const rapidjson::Value&) override;

                    std::shared_ptr<IRead> read(const System::File::Info&) const override;

                private:
                    DJV_PRIVATE();
                };

            } // namespace OBJ
        } // namespace IO
    } // namespace Scene3D
    
    rapidjson::Value toJSON(const Scene3D::IO::OBJ::Options&, rapidjson::Document::AllocatorType&);

    //! Throws:
    //! - std::exception
    void fromJSON(const rapidjson::Value&, Scene3D::IO::OBJ::Options&);

} // namespace djv

