#include "opengl/vertex_array.hpp"

#include "opengl/gl_errors.hpp"

namespace GL {

VertexArray VertexArray::bind_new()
{
    VertexArray vertex_array;

    gl(GenVertexArrays, 1, &vertex_array.m_id);

    vertex_array.bind();

    return vertex_array;
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
    VertexBuffer* vertex_buffer = new VertexBuffer(VertexBuffer::bind_new(layout));

    m_vertex_buffers.push_back(vertex_buffer);

    return vertex_buffer;
}

IndexBuffer* VertexArray::bind_index_buffer()
{
    IndexBuffer* index_buffer = new IndexBuffer(IndexBuffer::bind_new());

    m_index_buffers.push_back(index_buffer);

    return index_buffer;
}

}