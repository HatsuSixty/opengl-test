#include "opengl/gl_errors.hpp"

#include <iostream>

#include <GL/glew.h>

namespace GL {

void clear_errors()
{
    while (glGetError() != GL_NO_ERROR);
}

void check_errors(const char* file_path, int line)
{
    while (GLenum error = glGetError()) {
        std::cerr << file_path << ":" << line
                  << ": ERROR: OpenGL error: error code 0x"
                  << std::hex << error << std::dec << std::endl;
    }
}

}