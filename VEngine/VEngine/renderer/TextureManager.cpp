#include "TextureManager.h"

using namespace vengine;

/*

    TEXTURE 2D

*/

void TextureManager::delete_texture(const unsigned int id)
{
    assert_valid_id(id);

    auto& texture_handles = m_id_to_texture_handle;
    auto& texture_ids = m_id_to_texture_id;
    auto& texture_id = texture_ids.at(id);

    texture_ids.erase(id);
    texture_handles.erase(id);

    glDeleteTextures(1, &texture_id);
}

unsigned int TextureManager::gen_bindless_texture2d(unsigned int width, unsigned int height, void* data, bool useNearestParameters,
                                                    GLint internalFormat, GLint format, GLenum type, bool mipMaps)
{
    auto& texture_ids = m_id_to_texture_id;
    auto& texture_handles = m_id_to_texture_handle;

    unsigned int id = m_num_created_ids;
    texture_handles.emplace(id, 0);
    texture_ids.emplace(id, 0);

    generate_bindless_texture2d(texture_ids.at(id), texture_handles.at(id), width, height, data, useNearestParameters, internalFormat,
                                format, type, mipMaps);

    m_num_created_ids++;

    return id;
}

unsigned int TextureManager::load_texture_SRGBA_2d(std::string path)
{

    return load_texture_2d(path, false, GL_SRGB_ALPHA, GL_RGBA, GL_UNSIGNED_BYTE, true);
}

unsigned int TextureManager::load_texture_RGBA_2d(std::string path)
{

    return load_texture_2d(path, false, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, true);
}

unsigned int TextureManager::load_texture_2d(std::string path, bool nearest, GLint internalFormat, GLint format, GLenum type, bool mipMaps)
{

    int            width = 0, height = 0, nrChannels = 0;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    unsigned int id = gen_bindless_texture2d(width, height, data, nearest, internalFormat, format, type, mipMaps);

    if (!data)
    {
        VE_LOG_ERROR("Texture Loading error " << stbi_failure_reason());
        return -1;
    }


    stbi_image_free(data);

    return id;
}


/*

    CUBE MAPS

*/

unsigned int TextureManager::gen_bindless_cube_map(const std::vector<unsigned int>& widths, const std::vector<unsigned int>& heights,
                                                   const std::vector<void*>& datas, GLint internalFormat, GLint format, GLenum type,
                                                   bool mipMaps)
{
    auto& texture_ids = m_id_to_texture_id;
    auto& texture_handles = m_id_to_texture_handle;

    unsigned int id = m_num_created_ids;
    texture_handles.emplace(id, 0);
    texture_ids.emplace(id, 0);

    generate_bindless_cube_map(texture_ids.at(id), texture_handles.at(id), widths, heights, datas, internalFormat, format, type, mipMaps);


    m_num_created_ids++;

    return id;
}


/*

    PRIVATE UTIL

*/

void TextureManager::parameters_linear()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
void TextureManager::parameters_nearest()
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void TextureManager::parameters_cube_map()
{
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
}

void TextureManager::generate_bindless_texture2d(unsigned int& textureId, GLuint64& handle, unsigned int width, unsigned int height,
                                                 void* data, bool nearest, GLint internalFormat, GLint format, GLenum type, bool mipMaps)
{
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);


    if (nearest)
    {
        parameters_nearest();
    }
    else
    {
        parameters_linear();
    }

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, type, data);

    if (mipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    handle = glGetTextureHandleARB(textureId);
    glMakeTextureHandleResidentARB(handle);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextureManager::generate_bindless_cube_map(unsigned int& textureId, GLuint64& handle, const std::vector<unsigned int>& widths,
                                                const std::vector<unsigned int>& heights, const std::vector<void*>& datas,
                                                GLint internalFormat, GLint format, GLenum type, bool mipMaps)
{

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);

    parameters_cube_map();

    for (int i = 0; i < datas.size(); i++)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, widths.at(i), heights.at(i), 0, format, type, datas.at(i));
    }

    if (mipMaps)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    }

    handle = glGetTextureHandleARB(textureId);
    glMakeTextureHandleResidentARB(handle);

    glBindTexture(GL_TEXTURE_2D, 0);
}
