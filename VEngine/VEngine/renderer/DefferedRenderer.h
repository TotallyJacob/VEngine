#pragma once

#include <array>
#include <iostream>
#include <vector>

#include "MultiIndirectDrawer.h"
#include "ShaderStorageManager.h"
#include "SyncBuffer.h"
#include "TextureManager.h"

namespace vengine
{

class DefferedRenderer
{
    public:

        struct TextureLayout
        {
                bool   useNearestParameters = false;
                GLint  internalFormat = GL_RGBA;
                GLint  format = GL_RGBA;
                GLenum type = GL_UNSIGNED_BYTE;
                bool   useMipMaps = true;
        };
        inline static const TextureLayout rgb16f_texture_layout = {true, GL_RGB16F, GL_RGB, GL_FLOAT, false};
        inline static const TextureLayout rgba16f_data_layout = {true, GL_RGBA16F, GL_RGBA, GL_FLOAT, false};
        inline static const TextureLayout rgba16f_texture_layout = {true, GL_RGBA16F, GL_RGBA, GL_UNSIGNED_BYTE, false};
        inline static const TextureLayout rgba_texture_layout = {true, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, false};

        struct RenderbufferLayout
        {
                GLenum internalFormat = GL_DEPTH24_STENCIL8;
                GLenum attachment = GL_DEPTH_STENCIL_ATTACHMENT;
        };
        inline static const RenderbufferLayout default_renderbuf_layout = {};

        DefferedRenderer(TextureManager* textureManager, ShaderStorageManager* shaderStorageManager, MultiIndirectDrawer* drawer);
        void init(unsigned int width, unsigned int height);

        // Drawing
        void draw_deffered_pass(unsigned int fbProgram);
        void draw_lighting_pass(unsigned int screenProgram);

        // Attachments
        void        update_fbo_attachment_size(int width, int height);
        inline void add_texture2d_attachment(const TextureLayout& layout)
        {
            m_fbo_texture_layouts.push_back(layout);
        }
        inline void set_renderbuf_layout(const RenderbufferLayout& layout)
        {
            this->m_rbo_layout = layout;
        }

    private:

        // Managers
        TextureManager*       m_texture_manager;
        ShaderStorageManager* m_shader_storage_manager;
        MultiIndirectDrawer*  m_drawer;

        // Rbo and Fbo stuff
        RenderbufferLayout                                           m_rbo_layout = {};
        SyncBuffer                                                   m_syncbuf = {1};
        MutableShaderStorage<GLuint64, GL_SHADER_STORAGE_BUFFER, 1>* m_texture_handles;
        std::vector<TextureLayout>                                   m_fbo_texture_layouts{};
        std::vector<unsigned int>                                    m_fbo_texture_ids{};
        unsigned int                                                 m_fbo = 0;
        unsigned int                                                 m_rbo = 0;

        // Quad rendering
        unsigned int m_quad_vao = 0, m_quad_vbo = 0;

        inline void draw_quad()
        {
            glBindVertexArray(m_quad_vao);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
            glBindVertexArray(0);
        }

        // init stuff
        inline void gen_fbo()
        {
            glGenFramebuffers(1, &m_fbo);
        }
        void init_ssbos();
        void init_fbo_data();
        void init_framebuf(unsigned int width, unsigned int height);
        void init_quad_rendering();
};

}; // namespace vengine
