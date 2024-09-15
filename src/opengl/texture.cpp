#include "opengl/texture.hpp"

#include <iostream>

#include "opengl/gl_errors.hpp"

GLenum gl_magnification_filter = GL_LINEAR;
GLenum gl_minification_filter = GL_LINEAR;
GLenum gl_texture_wrap_s = GL_CLAMP_TO_EDGE;
GLenum gl_texture_wrap_t = GL_CLAMP_TO_EDGE;

template <typename Enumeration>
auto as_integer(Enumeration const value)
    -> typename std::underlying_type<Enumeration>::type
{
    return static_cast<typename std::underlying_type<Enumeration>::type>(value);
}

struct PixelFormat {
    GLenum format;
    GLenum type;
};

static PixelFormat gl_pixel_format(GL::PixelFormat format)
{
    switch (format) {

    case GL::PixelFormat::R8G8B8: return {
        .format = GL_RGB,
        .type = GL_UNSIGNED_BYTE,
    };

    case GL::PixelFormat::R8G8B8A8: return {
        .format = GL_RGBA,
        .type = GL_UNSIGNED_BYTE,
    };

    default:
        std::cerr << "FATAL ERROR: invalid pixel format: "
                  << as_integer(format) << "\n";
        throw;
    }
}

static GLenum gl_texture_type(GL::TextureType type)
{
    switch (type) {

    case GL::TextureType::TWO_DIMS: return GL_TEXTURE_2D;
    case GL::TextureType::THREE_DIMS: return GL_TEXTURE_3D;

    default:
        std::cerr << "FATAL ERROR: invalid texture type: "
                  << as_integer(type) << "\n";
        throw;
    }
}

namespace GL {

Texture Texture::new_texture(unsigned char* pixels, std::size_t width, std::size_t height,
                             PixelFormat pixel_format, TextureType type)
{
    Texture texture;
    texture.m_type = type;

    gl(GenTextures, 1, &texture.m_id);

    gl(BindTexture, gl_texture_type(type), texture.m_id);

    // Set parameters
    gl(TexParameteri, gl_texture_type(type), GL_TEXTURE_MIN_FILTER, gl_minification_filter);
    gl(TexParameteri, gl_texture_type(type), GL_TEXTURE_MAG_FILTER, gl_magnification_filter);
    gl(TexParameteri, gl_texture_type(type), GL_TEXTURE_WRAP_S, gl_texture_wrap_s);
    gl(TexParameteri, gl_texture_type(type), GL_TEXTURE_WRAP_T, gl_texture_wrap_t);

    // Upload texture data
    auto format = gl_pixel_format(pixel_format);
    gl(TexImage2D, gl_texture_type(type), 0, GL_RGBA8, width, height,
                   0, format.format, format.type, pixels);

    gl(BindTexture, gl_texture_type(type), 0);

    return texture;
}

Texture::~Texture()
{
    gl(DeleteTextures, 1, &m_id);
}

void Texture::bind(GLuint slot) const
{
    gl(ActiveTexture, GL_TEXTURE0 + slot);
    gl(BindTexture, gl_texture_type(m_type), m_id);
}

void Texture::unbind() const
{
    gl(ActiveTexture, GL_TEXTURE0);
    gl(BindTexture, gl_texture_type(m_type), 0);
}

void set_magnification_filter(GLenum filter) { gl_magnification_filter = filter; }
void set_minification_filter(GLenum filter) { gl_minification_filter = filter; }
void set_texture_wrap_s(GLenum wrap) { gl_texture_wrap_s = wrap; }
void set_texture_wrap_t(GLenum wrap) { gl_texture_wrap_t = wrap; }

}