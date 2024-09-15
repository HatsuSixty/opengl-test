#pragma once

#include <cstddef>

#include <GL/glew.h>

namespace GL {

class IndexBuffer {
private:
    GLuint m_id;

    std::size_t m_index_count = 0;
    std::size_t m_index_capacity = 0;

public:
    static IndexBuffer bind_new();
    ~IndexBuffer();

    void bind() const;
    void unbind() const;

    void clear() { m_index_count = 0; };
    void resize(std::size_t added_indices);
    void push_index(GLuint index);

    std::size_t index_count() { return m_index_count; }
};

}