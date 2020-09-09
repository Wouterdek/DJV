// SPDX-License-Identifier: BSD-3-Clause
// Copyright (c) 2004-2020 Darby Johnston
// All rights reserved.

#include <djvAV/ImageConvert.h>

#include <djvAV/OpenGLMesh.h>
#include <djvAV/OpenGLOffscreenBuffer.h>
#include <djvAV/OpenGLShader.h>
#include <djvAV/OpenGLTexture.h>
#include <djvAV/Shader.h>
#include <djvAV/Shape.h>
#include <djvAV/TriangleMesh.h>

#include <djvCore/TextSystem.h>
#include <djvCore/ResourceSystem.h>

#include <glm/gtc/matrix_transform.hpp>

using namespace djv::Core;

namespace djv
{
    namespace AV
    {
        namespace Image
        {
            struct Convert::Private
            {
                std::shared_ptr<TextSystem> textSystem;
                std::shared_ptr<ResourceSystem> resourceSystem;
                Size size;
                Mirror mirror;
                std::shared_ptr<OpenGL::OffscreenBuffer> offscreenBuffer;
                std::shared_ptr<AV::OpenGL::Texture> texture;
                std::shared_ptr<AV::OpenGL::VBO> vbo;
                std::shared_ptr<AV::OpenGL::VAO> vao;
                std::shared_ptr<AV::OpenGL::Shader> shader;
                glm::mat4x4 mvp = glm::mat4x4(1.F);
            };

            void Convert::_init(
                const std::shared_ptr<TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem)
            {
                DJV_PRIVATE_PTR();
                p.textSystem = textSystem;
                p.resourceSystem = resourceSystem;
                const FileSystem::Path shaderPath = resourceSystem->getPath(Core::FileSystem::ResourcePath::Shaders);
                p.shader = AV::OpenGL::Shader::create(Render::Shader::create(
                    FileSystem::Path(shaderPath, "djvAVImageConvertVertex.glsl"),
                    FileSystem::Path(shaderPath, "djvAVImageConvertFragment.glsl")));
            }

            Convert::Convert() :
                _p(new Private)
            {}

            Convert::~Convert()
            {}

            std::shared_ptr<Convert> Convert::create(
                const std::shared_ptr<TextSystem>& textSystem,
                const std::shared_ptr<ResourceSystem>& resourceSystem)
            {
                auto out = std::shared_ptr<Convert>(new Convert);
                out->_init(textSystem, resourceSystem);
                return out;
            }

            void Convert::process(const Data& data, const Info& info, Data& out)
            {
                DJV_PRIVATE_PTR();
                bool create = !p.offscreenBuffer;
                create |= p.offscreenBuffer && info.size != p.offscreenBuffer->getSize();
                create |= p.offscreenBuffer && info.type != p.offscreenBuffer->getColorType();
                if (create)
                {
                    p.offscreenBuffer = OpenGL::OffscreenBuffer::create(info.size, info.type, p.textSystem);
                }
                const OpenGL::OffscreenBufferBinding binding(p.offscreenBuffer);

                if (!p.texture || (p.texture && data.getInfo() != p.texture->getInfo()))
                {
                    p.texture = OpenGL::Texture::create(data.getInfo());
                }
                p.texture->bind();
                p.texture->copy(data);

                p.shader->bind();
                p.shader->setUniform("textureSampler", 0);
                
                if (info.size != p.size)
                {
                    p.size = info.size;
                    glm::mat4x4 modelMatrix(1);
                    modelMatrix = glm::rotate(modelMatrix, Core::Math::deg2rad(90.F), glm::vec3(1.F, 0.F, 0.F));
                    modelMatrix = glm::scale(modelMatrix, glm::vec3(info.size.w, 0.F, info.size.h));
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(.5F, 0.F, -.5F));
                    glm::mat4x4 viewMatrix(1);
                    glm::mat4x4 projectionMatrix(1);
                    projectionMatrix = glm::ortho(
                        0.F,
                        static_cast<float>(info.size.w) - 1.F,
                        0.F,
                        static_cast<float>(info.size.h) - 1.F,
                        -1.F,
                        1.F);
                    p.mvp = projectionMatrix * viewMatrix * modelMatrix;
                }
                p.shader->setUniform("transform.mvp", p.mvp);

                if (!p.vbo || (p.vbo && data.getLayout().mirror != p.mirror))
                {
                    p.mirror = data.getLayout().mirror;
                    AV::Geom::Square square;
                    AV::Geom::TriangleMesh mesh;
                    square.triangulate(mesh);
                    if (p.mirror.x)
                    {
                        auto tmp = mesh.t[0].x;
                        mesh.t[0].x = mesh.t[1].x;
                        mesh.t[1].x = tmp;
                        tmp = mesh.t[2].x;
                        mesh.t[2].x = mesh.t[3].x;
                        mesh.t[3].x = tmp;
                    }
                    if (p.mirror.y)
                    {
                        auto tmp = mesh.t[0].y;
                        mesh.t[0].y = mesh.t[2].y;
                        mesh.t[2].y = tmp;
                        tmp = mesh.t[1].y;
                        mesh.t[1].y = mesh.t[3].y;
                        mesh.t[3].y = tmp;
                    }
                    p.vbo = AV::OpenGL::VBO::create(2 * 3, AV::OpenGL::VBOType::Pos3_F32_UV_U16);
                    p.vbo->copy(AV::OpenGL::VBO::convert(mesh, p.vbo->getType()));
                    p.vao = AV::OpenGL::VAO::create(p.vbo->getType(), p.vbo->getID());
                }
                p.vao->bind();

                glViewport(0, 0, info.size.w, info.size.h);
                glClearColor(0.F, 0.F, 0.F, 0.F);
                glClear(GL_COLOR_BUFFER_BIT);
                glActiveTexture(GL_TEXTURE0);

                p.vao->draw(GL_TRIANGLES, 0, 6);

                glPixelStorei(GL_PACK_ALIGNMENT, 1);
#if !defined(DJV_OPENGL_ES2)
                glPixelStorei(GL_PACK_SWAP_BYTES, info.layout.endian != Memory::getEndian());
#endif
                glReadPixels(
                    0, 0, info.size.w, info.size.h,
                    info.getGLFormat(),
                    info.getGLType(),
                    out.getData());
            }

        } // namespace Image
    } // namespace AV
} // namespace djv
