#include <cstring>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl/gl_errors.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "opengl/vertex_array.hpp"
#include "opengl/vertex_buffer.hpp"

bool prev_keys_pressed[GLFW_KEY_LAST] = { false };
bool keys_pressed[GLFW_KEY_LAST] = { false };

bool is_key_pressed(int key)
{
    return keys_pressed[key];
}

bool is_key_just_pressed(int key)
{
    return keys_pressed[key] && !prev_keys_pressed[key];
}

static void on_window_resize(GLFWwindow* window, int width, int height)
{
    (void)window;
    gl(Viewport, (0, 0, width, height));
}

static void on_key(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    (void)window;
    (void)scancode;
    (void)mods;

    if (action == GLFW_PRESS)
        keys_pressed[key] = true;
    else if (action == GLFW_RELEASE)
        keys_pressed[key] = false;
}

static GL::Texture read_texture_from_file(const std::string& filepath)
{
    int width;
    int height;
    int components;
    unsigned char* pixels = stbi_load(filepath.c_str(), &width, &height, &components, 4);

    auto texture = GL::Texture(pixels, width, height,
                               GL::PixelFormat::R8G8B8A8,
                               GL::TextureType::TWO_DIMS);

    stbi_image_free(pixels);

    return texture;
}

struct Vector2 {
    float x;
    float y;
};
#define V2X(v) v.x, v.y

struct Vector4 {
    float x;
    float y;
    float z;
    float w;
};
#define V4X(v) v.x, v.y, v.z, v.w

class Renderer {
private:
    GL::VertexArray* m_va;
    GL::VertexBuffer* m_vb;
    GL::IndexBuffer* m_ib;

    GL::Shader* m_shader;
    GL::Texture* m_default_texture;

public:
    static Renderer new_renderer()
    {
        Renderer renderer;

        renderer.m_va = new GL::VertexArray();
        renderer.m_va->bind();

        GL::VertexLayout vertex_layout;
        vertex_layout.add_attribute<float>(2, false);
        vertex_layout.add_attribute<float>(2, false);
        vertex_layout.add_attribute<float>(4, false);

        renderer.m_vb = renderer.m_va->bind_vertex_buffer(vertex_layout);

        renderer.m_ib = renderer.m_va->bind_index_buffer();

        renderer.m_va->unbind_all();

        renderer.m_shader = new GL::Shader("./resources/shaders/simple_renderer.glsl");

        unsigned char pixels[] = { 0xFF, 0xFF, 0xFF, 0xFF };
        renderer.m_default_texture = new GL::Texture(pixels,
                                                     1,
                                                     1,
                                                     GL::PixelFormat::R8G8B8A8,
                                                     GL::TextureType::TWO_DIMS);

        return renderer;
    }

    ~Renderer()
    {
        delete m_default_texture;
        delete m_va;
        delete m_shader;
    }

    void begin_drawing()
    {
        m_va->bind();
        m_ib->bind();
        m_vb->bind();
    }

    void end_drawing()
    {
    }

    void draw_texture(const GL::Texture& texture,
                      const Vector2& dst_position,
                      const Vector2& dst_size,
                      const Vector4& color_tint)
    {
        Vector2 a = { dst_position.x, dst_position.y + dst_size.y };
        Vector2 b = { dst_position.x + dst_size.x, dst_position.y + dst_size.y };
        Vector2 c = { dst_position.x + dst_size.x, dst_position.y };
        Vector2 d = dst_position;

        float a_vertex[] = { V2X(a), 0.0f, 0.0f, V4X(color_tint) };
        float b_vertex[] = { V2X(b), 1.0f, 0.0f, V4X(color_tint) };
        float c_vertex[] = { V2X(c), 1.0f, 1.0f, V4X(color_tint) };
        float d_vertex[] = { V2X(d), 0.0f, 1.0f, V4X(color_tint) };

        m_vb->push_vertex(a_vertex, sizeof(a_vertex));
        m_vb->push_vertex(b_vertex, sizeof(b_vertex));
        m_vb->push_vertex(c_vertex, sizeof(c_vertex));
        m_vb->push_vertex(d_vertex, sizeof(d_vertex));

        m_ib->push_index(0);
        m_ib->push_index(1);
        m_ib->push_index(2);

        m_ib->push_index(2);
        m_ib->push_index(3);
        m_ib->push_index(0);

        texture.bind(0);
        m_shader->bind();

        m_shader->set_uniform("u_texture_slot", 0);

        gl(DrawElements, (GL_TRIANGLES, m_ib->index_count(), GL_UNSIGNED_INT, 0));

        m_vb->clear();
        m_ib->clear();
    }

    void draw_triangle(const Vector2& a,
                       const Vector2& b,
                       const Vector2& c,
                       const Vector4& color)
    {
        float a_vertex[] = { V2X(a), 0.0f, 0.0f, V4X(color) };
        float b_vertex[] = { V2X(b), 1.0f, 0.0f, V4X(color) };
        float c_vertex[] = { V2X(c), 1.0f, 1.0f, V4X(color) };

        m_vb->push_vertex(a_vertex, sizeof(a_vertex));
        m_vb->push_vertex(b_vertex, sizeof(b_vertex));
        m_vb->push_vertex(c_vertex, sizeof(c_vertex));

        m_ib->push_index(0);
        m_ib->push_index(1);
        m_ib->push_index(2);

        m_default_texture->bind(0);
        m_shader->bind();

        m_shader->set_uniform("u_texture_slot", 0);

        gl(DrawElements, (GL_TRIANGLES, m_ib->index_count(), GL_UNSIGNED_INT, 0));

        m_vb->clear();
        m_ib->clear();
    }
};

int main(void)
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    bool is_core = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE) == GLFW_OPENGL_CORE_PROFILE;
    std::cout << "[INFO] Context version: OpenGL "
              << major_ver << "." << minor_ver
              << (is_core ? " (Core Profile)" : "")
              << "\n";

    glfwSetKeyCallback(window, on_key);
    glfwSetWindowSizeCallback(window, on_window_resize);

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: could not load OpenGL\n";
        std::cerr << "    > GLEW initialization failed!\n";
        glfwTerminate();
        return 1;
    }

    gl(Enable, (GL_BLEND));
    gl(BlendFunc, (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    auto renderer = new Renderer(Renderer::new_renderer());

    GL::Texture* texture = new GL::Texture(read_texture_from_file("./resources/textures/image.png"));

    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        renderer->begin_drawing();

        renderer->draw_triangle({ -0.5f, -0.5f },
                                { +0.5f, -0.5f },
                                { +0.5f, +0.5f },
                                { 1.0f, 0.0f, 0.0f, 1.0f });

        renderer->draw_texture(*texture, { 0, 0 }, { 1, 1 }, { 1, 1, 1, 1 });
        renderer->draw_texture(*texture, { -1, -1 }, { 1, 1 }, { 1, 1, 1, 1 });

        renderer->end_drawing();
        std::memcpy(prev_keys_pressed, keys_pressed, sizeof(keys_pressed));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete renderer;
    delete texture;
    glfwTerminate();
}
