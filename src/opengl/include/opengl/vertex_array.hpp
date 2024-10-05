#pragma once

#include <vector>

#include <GL/glew.h>

#include "opengl/index_buffer.hpp"
#include "opengl/vertex_buffer.hpp"

namespace GL {

class VertexArray {
private:
    GLuint m_id;

    std::vector<VertexBuffer*> m_vertex_buffers;
    std::vector<IndexBuffer*> m_index_buffers;

public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void unbind() const;

    void unbind_all() const;

    VertexBuffer* bind_vertex_buffer(const VertexLayout& layout);
    IndexBuffer* bind_index_buffer();
};

}
