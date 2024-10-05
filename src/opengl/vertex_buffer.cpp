#include "opengl/vertex_buffer.hpp"

#include <cstring>

#include "opengl/gl_errors.hpp"

namespace GL {

VertexBuffer::VertexBuffer(const VertexLayout& layout)
{
    m_layout = layout;

    gl(GenBuffers, (1, &m_id));

    bind();

    for (std::size_t i = 0; i < layout.attributes.size(); ++i) {
        const VertexAttribute& attribute = layout.attributes[i];

        gl(EnableVertexAttribArray, (i));
        gl(VertexAttribPointer, (i, attribute.component_count, attribute.component.type, attribute.normalized, layout.stride, reinterpret_cast<GLvoid*>(attribute.offset)));
    }

    unbind();
}

VertexBuffer::~VertexBuffer()
{
    gl(DeleteBuffers, (1, &m_id));
}

void VertexBuffer::bind() const
{
    gl(BindBuffer, (GL_ARRAY_BUFFER, m_id));
}

void VertexBuffer::unbind() const
{
    gl(BindBuffer, (GL_ARRAY_BUFFER, 0));
}

void VertexBuffer::resize(std::size_t added_size)
{
    auto old_size = m_capacity;
    auto old_data = new unsigned char[old_size]();
    gl(GetBufferSubData, (GL_ARRAY_BUFFER, 0, old_size, old_data));

    auto new_size = old_size + added_size;
    auto new_data = new unsigned char[new_size]();
    std::memcpy(new_data, old_data, old_size);

    gl(BufferData, (GL_ARRAY_BUFFER, new_size, new_data, GL_DYNAMIC_DRAW));

    delete[] old_data;
    delete[] new_data;

    m_capacity = new_size;
}

void VertexBuffer::push_vertex(const void* data, std::size_t data_size)
{
    if (m_layout.stride != data_size) {
        std::cerr << "FATAL ERROR: push_vertex: "
                  << "vertex size (layout stride) does not match `data_size`\n";
        throw;
    }

    if ((m_size + data_size) > m_capacity) {
        resize(data_size);
    }

    gl(BufferSubData, (GL_ARRAY_BUFFER, m_size, data_size, data));

    m_size += data_size;
}

void VertexBuffer::set_attribute(int vertex_index, int attribute_index,
                                 const void* data, std::size_t data_size)
{
    const VertexAttribute& attribute = m_layout.attributes[attribute_index];
    const auto attribute_size = attribute.component_count * attribute.component.size;

    if (data_size != attribute_size) {
        std::cerr << "FATAL ERROR: set_attribute: "
                  << "data size does not match attribute size\n";
        throw;
    }

    gl(BufferSubData, (GL_ARRAY_BUFFER, vertex_index * m_layout.stride + attribute.offset, data_size, data));
}

}
