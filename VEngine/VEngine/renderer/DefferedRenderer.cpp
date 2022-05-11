#include "DefferedRenderer.h"

using namespace vengine;

DefferedRenderer::DefferedRenderer(TextureManager* textureManager, ShaderStorageManager* shaderStorageManager, MultiIndirectDrawer* drawer)
{

    this->m_texture_manager = textureManager;
    this->m_shader_storage_manager = shaderStorageManager;
    this->m_drawer = drawer;
}

void DefferedRenderer::init(unsigned int width, unsigned int height)
{
    gen_fbo();
    init_ssbos();
    init_fbo_data();
    init_framebuf(width, height);
    init_quad_rendering();
}


void DefferedRenderer::draw_deffered_pass(unsigned int fbProgram)
{
    glUseProgram(fbProgram);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    m_drawer->draw(fbProgram);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glUseProgram(0);
}

void DefferedRenderer::draw_lighting_pass(unsigned int screenProgram)
{
    glUseProgram(screenProgram);

    draw_quad();

    glUseProgram(0);
}

void DefferedRenderer::update_fbo_attachment_size(int width, int height)
{
    VE_LOG_DEBUG("updating fbo attachment size");

    for (const unsigned int& id : m_fbo_texture_ids)
    {
        m_texture_manager->delete_texture(id);
    }

    glDeleteRenderbuffers(1, &m_rbo);

    init_framebuf(width, height);
};

void DefferedRenderer::init_ssbos()
{
    m_texture_handles = &m_shader_storage_manager->make_mutable_shader_storage<GLuint64, GL_SHADER_STORAGE_BUFFER, 1>(
        "handle_other", m_fbo_texture_layouts.size());

    std::vector<GLuint64> handles;
    handles.resize(m_fbo_texture_layouts.size());

    m_texture_handles->set_initial_data(&handles.front(), handles.size());
}

void DefferedRenderer::init_fbo_data()
{
    m_fbo_texture_ids.resize(m_fbo_texture_layouts.size());
}


void DefferedRenderer::init_framebuf(unsigned int width, unsigned int height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Gen and attach/bind renderbuffer
    glGenRenderbuffers(1, &m_rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, m_rbo_layout.internalFormat, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, m_rbo_layout.attachment, GL_RENDERBUFFER, m_rbo);

    std::vector<GLuint64>     handles{};
    std::vector<unsigned int> attachments{};
    unsigned int              index = 0;
    for (const auto& l : m_fbo_texture_layouts)
    {

        // Gen the texture
        int texture_id = m_texture_manager->gen_bindless_texture2d(width, height, NULL, l.useNearestParameters, l.internalFormat, l.format,
                                                                   l.type, l.useMipMaps);

        // Attach texture to framebuffer
        unsigned int& gl_texture_id = m_texture_manager->get_texture_id(texture_id);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + index, GL_TEXTURE_2D, gl_texture_id, 0);

        // Setting external stuff
        m_fbo_texture_ids.at(index) = texture_id;
        attachments.push_back(GL_COLOR_ATTACHMENT0 + index);
        handles.push_back({m_texture_manager->get_handle(texture_id)});
        index++;
    }

    // Attachments
    glDrawBuffers(attachments.size(), &attachments.front());

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // SSBOs
    if (!m_texture_handles->readbuf_sync_invalid())
    {
        m_texture_handles->delete_readbuf_sync(); // even if it's already deleted it doesn't make a difference
    }
    else
    {
        VE_LOG_DEBUG("Sync invalid, thus already deleted baby. ");
    }
    m_texture_handles->insert_sync_on_readbuf();
    m_texture_handles->standard_wait_sync<true, true>(m_texture_handles->get_readbuf_sync(), " gBuffer texture resize ");
    m_texture_handles->set_updatebuf_data(&handles.front(), handles.size(), 0);
    m_texture_handles->delete_readbuf_sync();
}

void DefferedRenderer::init_quad_rendering()
{
    float quadVertices[] = {
        // positions        // texture Coords
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &m_quad_vao);

    glGenBuffers(1, &m_quad_vbo);

    glBindVertexArray(m_quad_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_quad_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
