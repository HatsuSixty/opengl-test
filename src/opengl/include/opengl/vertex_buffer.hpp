#pragma once

#include <cstddef>
#include <iostream>
#include <typeinfo>
#include <vector>

#include <GL/glew.h>

namespace GL {

struct VertexAttributeComponent {
    GLenum type;
    std::size_t size;

    template <typename T>
    static VertexAttributeComponent from_type()
    {
        VertexAttributeComponent component;

        if (typeid(T) == typeid(float)) {
            component.type = GL_FLOAT;
            component.size = sizeof(GLfloat);

        } else if (typeid(T) == typeid(char)) {
            component.type = GL_BYTE;
            component.size = sizeof(GLbyte);

        } else if (typeid(T) == typeid(unsigned char)) {
            component.type = GL_UNSIGNED_BYTE;
            component.size = sizeof(GLubyte);

        } else {
            std::cerr << "FATAL ERROR: invalid attribute component `"
                      << typeid(T).name() << "`\n";
            throw;
        }

        return component;
    }
};

struct VertexAttribute {
    bool normalized;
    VertexAttributeComponent component;
    std::size_t component_count;
    std::size_t offset;
};

struct VertexLayout {
    std::vector<VertexAttribute> attributes;
    std::size_t stride = 0;

    template <typename T>
    void add_attribute(std::size_t count, bool normalized)
    {
        auto component = VertexAttributeComponent::from_type<T>();

        VertexAttribute attribute = {
            .normalized = normalized,
            .component = component,
            .component_count = count,
            .offset = stride,
        };

        stride += count * component.size;
        attributes.push_back(attribute);
    }
};

class VertexBuffer {
private:
    GLuint m_id;
    VertexLayout m_layout;

    std::size_t m_size = 0;
    std::size_t m_capacity = 0;

public:
    VertexBuffer(const VertexLayout& layout);
    ~VertexBuffer();

    void bind() const;
    void unbind() const;

    void clear() { m_size = 0; };
    void resize(std::size_t added_size);
    void push_vertex(const void* data, std::size_t data_size);

    std::size_t vertex_count() { return m_size / m_layout.stride; }

    void set_attribute(int vertex_index, int attribute_index,
                       const void* data, std::size_t data_size);
};

}
