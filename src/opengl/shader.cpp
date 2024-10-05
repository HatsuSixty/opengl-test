#include "opengl/shader.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include "opengl/gl_errors.hpp"
#include "opengl/shader.hpp"

struct ShaderSources {
    std::string vertex;
    std::string fragment;
};

static ShaderSources parse_shader(const std::string& path)
{
    std::ifstream stream(path);

    std::string vertex_source;
    std::string fragment_source;
    GLenum shader_mode = GL_VERTEX_SHADER;

    std::string line;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                shader_mode = GL_VERTEX_SHADER;
            else if (line.find("fragment") != std::string::npos)
                shader_mode = GL_FRAGMENT_SHADER;

            continue;
        }

        if (shader_mode == GL_VERTEX_SHADER) {
            vertex_source.append(line);
            vertex_source.append("\n");
        } else if (shader_mode == GL_FRAGMENT_SHADER) {
            fragment_source.append(line);
            fragment_source.append("\n");
        }
    }

    return (ShaderSources) {
        .vertex = vertex_source,
        .fragment = fragment_source,
    };
}

static GLuint compile_shader(GLenum type, const std::string& source)
{
    GLuint id;
    gl_call(id = glCreateShader(type));

    const char* c_source = source.c_str();
    gl(ShaderSource, id, 1, &c_source, nullptr);
    gl(CompileShader, id);

    int result;
    gl(GetShaderiv, id, GL_COMPILE_STATUS, &result);

    if (result == GL_FALSE) {
        int error_length;
        gl(GetShaderiv, id, GL_INFO_LOG_LENGTH, &error_length);

        char* error = (char*)alloca(error_length);
        gl(GetShaderInfoLog, id, error_length, &error_length, error);

        const char* shader_type = type == GL_VERTEX_SHADER ? "vertex" : "fragment";

        std::cerr << "ERROR: " << shader_type << " shader compilation: "
                  << error;

        gl(DeleteShader, id);

        return 0;
    }

    return id;
}

namespace GL {

int Shader::get_uniform_location(const std::string& name)
{
    if (m_locations.find(name) != m_locations.end()) {
        return m_locations.at(name);
    }

    int location;
    gl_call(location = glGetUniformLocation(m_id, name.c_str()));

    m_locations[name] = location;

    return location;
}

Shader Shader::bind_new(const std::string& path)
{
    Shader shader;

    ShaderSources shader_sources = parse_shader(path);

    gl_call(shader.m_id = glCreateProgram());

    GLuint vert_shader = compile_shader(GL_VERTEX_SHADER,
                                        shader_sources.vertex);
    if (vert_shader == 0) {
        gl(DeleteProgram, shader.m_id);
        shader.m_valid = false;
    }

    GLuint frag_shader = compile_shader(GL_FRAGMENT_SHADER,
                                        shader_sources.fragment);
    if (frag_shader == 0) {
        gl(DeleteProgram, shader.m_id);
        shader.m_valid = false;
    }

    gl(AttachShader, shader.m_id, vert_shader);
    gl(AttachShader, shader.m_id, frag_shader);

    gl(LinkProgram, shader.m_id);
    gl(ValidateProgram, shader.m_id);

    gl(DeleteShader, vert_shader);
    gl(DeleteShader, frag_shader);

    shader.bind();

    return shader;
}

void Shader::bind() const
{
    gl(UseProgram, m_id);
}

void Shader::unbind() const
{
    gl(UseProgram, 0);
}

void Shader::set_uniform(const std::string& name,
                         float x, float y, float z, float w)
{
    int location = get_uniform_location(name);
    gl(Uniform4f, location, x, y, z, w);
}

void Shader::set_uniform(const std::string& name,
                         float x, float y, float z)
{
    int location = get_uniform_location(name);
    gl(Uniform3f, location, x, y, z);
}

void Shader::set_uniform(const std::string& name, int x)
{
    int location = get_uniform_location(name);
    gl(Uniform1i, location, x);
}

}