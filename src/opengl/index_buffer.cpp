#include "opengl/index_buffer.hpp"

#include <cstring>

#include "opengl/gl_errors.hpp"

namespace GL {

IndexBuffer IndexBuffer::bind_new()
{
    IndexBuffer index_buffer;

    gl(GenBuffers, 1, &index_buffer.m_id);

    index_buffer.bind();

    return index_buffer;
}

IndexBuffer::~IndexBuffer()
{
    gl(DeleteBuffers, 1, &m_id);
}

void IndexBuffer::bind() const
{
    gl(BindBuffer, GL_ELEMENT_ARRAY_BUFFER, m_id);
}

void IndexBuffer::unbind() const
{
    gl(BindBuffer, GL_ELEMENT_ARRAY_BUFFER, 0);
}

void IndexBuffer::resize(std::size_t added_indices)
{
    auto added_size = added_indices * sizeof(unsigned int);

    auto old_size = m_index_capacity * sizeof(unsigned int);
    auto old_data = new unsigned char[old_size]();
    gl(GetBufferSubData, GL_ELEMENT_ARRAY_BUFFER, 0, old_size, old_data);

    auto new_size = old_size + added_size;
    auto new_data = new unsigned char[new_size]();
    std::memcpy(new_data, old_data, old_size);

    gl(BufferData, GL_ELEMENT_ARRAY_BUFFER, new_size, new_data, GL_DYNAMIC_DRAW);

    delete[] old_data;
    delete[] new_data;

    m_index_capacity = new_size / sizeof(unsigned int);
}

void IndexBuffer::push_index(GLuint index)
{
    auto added_index_count = 1;

    if ((m_index_count + added_index_count) > m_index_capacity) {
        resize(added_index_count);
    }

    gl(BufferSubData, GL_ELEMENT_ARRAY_BUFFER, m_index_count * sizeof(unsigned int),
       added_index_count * sizeof(unsigned int), &index);

    m_index_count += added_index_count;
}

}