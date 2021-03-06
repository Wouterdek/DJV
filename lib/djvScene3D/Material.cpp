// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2019-2020 Darby Johnston
// All rights reserved.

#include <djvScene3D/Material.h>

#include <djvRender3D/Material.h>

using namespace djv::Core;

namespace djv
{
    namespace Scene3D
    {
        std::shared_ptr<DefaultMaterial> DefaultMaterial::create()
        {
            auto out = std::shared_ptr<DefaultMaterial>(new DefaultMaterial);
            return out;
        }

        std::shared_ptr<Render3D::IMaterial> DefaultMaterial::createMaterial(const std::shared_ptr<System::Context>& context)
        {
            auto out = Render3D::DefaultMaterial::create(context);
            out->setAmbient(_ambient);
            out->setDiffuse(_diffuse);
            out->setEmission(_emission);
            out->setSpecular(_specular);
            out->setShine(_shine);
            out->setTransparency(_transparency);
            out->setReflectivity(_reflectivity);
            out->setDisableLighting(_disableLighting);
            return out;
        }

    } // namespace Scene3D
} // namespace djv

