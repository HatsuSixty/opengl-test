#include "opengl/vertex_array.hpp"

#include "opengl/gl_errors.hpp"

namespace GL {

VertexArray::VertexArray()
{
    gl(GenVertexArrays, 1, &m_id);
}

VertexArray::~VertexArray()
{
    for (VertexBuffer* vb : m_vertex_buffers) {
        delete vb;
    }

    for (IndexBuffer* ib : m_index_buffers) {
        delete ib;
    }
}

void VertexArray::bind() const
{
    gl(BindVertexArray, m_id);
}

void VertexArray::unbind() const
{
    gl(BindVertexArray, 0);
}

void VertexArray::unbind_all() const
{
    unbind();
    gl(BindBuffer, GL_ARRAY_BUFFER, 0);
    gl(BindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
}

VertexBuffer* VertexArray::bind_vertex_buffer(const VertexLayout& layout)
{
    VertexBuffer* vertex_buffer = new VertexBuffer(layout);
    vertex_buffer->bind();

    m_vertex_buffers.push_back(vertex_buffer);

    return vertex_buffer;
}

IndexBuffer* VertexArray::bind_index_buffer()
{
    IndexBuffer* index_buffer = new IndexBuffer();
    index_buffer->bind();

    m_index_buffers.push_back(index_buffer);

    return index_buffer;
}

}
