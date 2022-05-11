#pragma once
#include "GL/glew.h"


#include "stb.h"
#include <assert.h>
#include <iostream>
#include <unordered_map>
#include <vector>

#include "../Logger.hpp"

namespace vengine
{

class TextureManager
{
    public:

        TextureManager() = default;
        ~TextureManager() = default;

        GLuint64& get_handle(const unsigned int id)
        {
            assert_valid_id(id);

            return m_id_to_texture_handle.at(id);
        }

        GLuint& get_texture_id(const unsigned int id)
        {
            assert_valid_id(id);

            auto& texture_id = m_id_to_texture_id.at(id);

            return texture_id;
        }

        void delete_texture(const unsigned int id);

        // Texture 2D
        [[nodiscard]] unsigned int gen_bindless_texture2d(unsigned int width, unsigned int height, void* data,
                                                          bool useNearestParameters = false, GLint internalFormat = GL_RGBA,
                                                          GLint format = GL_RGBA, GLenum type = GL_UNSIGNED_BYTE, bool mipMaps = true);

        [[nodiscard]] unsigned int load_texture_SRGBA_2d(std::string path);
        [[nodiscard]] unsigned int load_texture_RGBA_2d(std::string path);
        [[nodiscard]] unsigned int load_texture_2d(std::string path, bool nearest, GLint internalFormat, GLint format, GLenum type,
                                                   bool mipMaps);

        // Cube map
        [[nodiscard]] unsigned int gen_bindless_cube_map(const std::vector<unsigned int>& widths, const std::vector<unsigned int>& heights,
                                                         const std::vector<void*>& datas, GLint internalFormat = GL_RGB,
                                                         GLint format = GL_RGB, GLenum type = GL_UNSIGNED_BYTE, bool mipMaps = true);

    private:

        std::unordered_map<unsigned int, GLuint64> m_id_to_texture_handle{};
        std::unordered_map<unsigned int, GLuint>   m_id_to_texture_id{};
        unsigned int                               m_num_created_ids = 0;

        inline static void parameters_cube_map();
        inline static void parameters_linear();
        inline static void parameters_nearest();

        void generate_bindless_texture2d(unsigned int& textureId, GLuint64& handle, unsigned int width, unsigned int height, void* data,
                                         bool nearest, GLint internalFormat, GLint format, GLenum type, bool mipMaps);

        void generate_bindless_cube_map(unsigned int& textureId, GLuint64& handle, const std::vector<unsigned int>& widths,
                                        const std::vector<unsigned int>& heights, const std::vector<void*>& datas, GLint internalFormat,
                                        GLint format, GLenum type, bool mipMaps);

        // msc
        inline void assert_valid_id(const int id)
        {
            VE_DEBUG_DO(
                //
                if (m_id_to_texture_handle.find(id) == m_id_to_texture_handle.end()) //
                {
                    VE_LOG_ERROR("Invalid texture id:" << id << " so cannot fetch opengl texture id. " << __FILE__ << " : " << __LINE__);
                    std::abort();
                }
                //
            );

            assert(m_id_to_texture_handle.find(id) != m_id_to_texture_handle.end() && "Error, invalid id.");
        }
};

} // namespace vengine
