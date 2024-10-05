#pragma once

#include <string>
#include <unordered_map>

#include <GL/glew.h>

namespace GL {

class Shader {
private:
    GLuint m_id;
    bool m_valid = true;

    std::unordered_map<std::string, int> m_locations;

    int get_uniform_location(const std::string& name);

public:
    Shader(const std::string& path);

    bool is_valid() const { return m_valid; };

    void bind() const;
    void unbind() const;

    void set_uniform(const std::string& name,
                     float x, float y, float z, float w);
    void set_uniform(const std::string& name,
                     float x, float y, float z);
    void set_uniform(const std::string& name, int x);
};

}
