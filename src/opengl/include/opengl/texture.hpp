#pragma once

#include <GL/glew.h>

namespace GL {

enum class TextureType {
    TWO_DIMS,
    THREE_DIMS,
};

enum class PixelFormat {
    R8G8B8A8,
    R8G8B8,
};

class Texture {
private:
    GLuint m_id;
    TextureType m_type;

public:
    static Texture new_texture(unsigned char* pixels, std::size_t width, std::size_t height,
                               PixelFormat pixel_format, TextureType type);
    ~Texture();

    void bind(GLuint slot) const;
    void unbind() const;
};

void set_magnification_filter(GLenum filter);
void set_minification_filter(GLenum filter);
void set_texture_wrap_s(GLenum wrap);
void set_texture_wrap_t(GLenum wrap);

}