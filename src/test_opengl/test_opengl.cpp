#include <cstring>
#include <fstream>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "opengl/gl_errors.hpp"
#include "opengl/shader.hpp"
#include "opengl/texture.hpp"
#include "opengl/vertex_array.hpp"

bool prev_keys_pressed[GLFW_KEY_LAST] = { false };
bool keys_pressed[GLFW_KEY_LAST] = { false };

// static bool is_key_pressed(int key)
// {
//     return keys_pressed[key];
// }

static bool is_key_just_pressed(int key)
{
    return keys_pressed[key] && !prev_keys_pressed[key];
}

static GL::Texture read_texture_from_file(const std::string& filepath)
{
    stbi_set_flip_vertically_on_load(1);

    int width;
    int height;
    int components;
    unsigned char* pixels = stbi_load(filepath.c_str(), &width, &height, &components, 4);

    auto texture = GL::Texture::new_texture(pixels, width, height,
                                            GL::PixelFormat::R8G8B8A8,
                                            GL::TextureType::TWO_DIMS);

    stbi_image_free(pixels);

    return texture;
}

static void on_window_resize(GLFWwindow* window, int width, int height)
{
    (void)window;
    gl(Viewport, 0, 0, width, height);
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

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Hello World", nullptr, nullptr);
    if (!window) {
        std::cerr << "ERROR: could not initialize GLFW\n";
        glfwTerminate();
        return 1;
    }

    glfwSetWindowSizeCallback(window, on_window_resize);
    glfwSetKeyCallback(window, on_key);

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        std::cerr << "ERROR: could not load OpenGL\n";
        std::cerr << "    > GLEW initialization failed!\n";
        glfwTerminate();
        return 1;
    }

    int major_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    int minor_ver = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    bool is_core = glfwGetWindowAttrib(window, GLFW_OPENGL_PROFILE) == GLFW_OPENGL_CORE_PROFILE;
    std::cout << "[INFO] Context version: OpenGL "
              << major_ver << "." << minor_ver
              << (is_core ? " (Core Profile)" : "")
              << "\n";

    gl(Enable, GL_BLEND);
    gl(BlendFunc, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /*                    *
     *   Setup vertexes   *
     *                    */

    GL::VertexArray* va = new GL::VertexArray(GL::VertexArray::bind_new());

    float vertexes[] = {
        // x   y       tex coord
        -0.5f,
        -0.5f,
        0.0f,
        0.0f,
        +0.5f,
        -0.5f,
        1.0f,
        0.0f,
        +0.5f,
        +0.5f,
        1.0f,
        1.0f,
        -0.5f,
        +0.5f,
        0.0f,
        1.0f,
    };

    GL::VertexLayout vertexes_layout;
    vertexes_layout.add_attribute<float>(2, false); // Position
    vertexes_layout.add_attribute<float>(2, false); // Texture coordinate

    GL::VertexBuffer* vb = va->bind_vertex_buffer(vertexes_layout);
    vb->push_vertex(&vertexes[0], vertexes_layout.stride);
    vb->push_vertex(&vertexes[4], vertexes_layout.stride);
    vb->push_vertex(&vertexes[8], vertexes_layout.stride);
    vb->push_vertex(&vertexes[12], vertexes_layout.stride);

    GL::IndexBuffer* ib = va->bind_index_buffer();

    ib->push_index(0);
    ib->push_index(1);
    ib->push_index(2);

    ib->push_index(2);
    ib->push_index(3);
    ib->push_index(0);

    va->unbind_all();

    /*                   *
     *   Setup texture   *
     *                   */

    GL::Texture* texture = new GL::Texture(read_texture_from_file("./resources/textures/image.png"));

    /*                  *
     *   Setup shader   *
     *                  */

    std::string shader_path = "./resources/shaders/default.glsl";
    GL::Shader* shader = new GL::Shader(GL::Shader::bind_new(shader_path));
    if (!shader->is_valid()) {
        return 1;
    }

    shader->set_uniform("u_color", 1.0f, 1.0f, 1.0f, 1.0f);

    shader->unbind();

    /*               *
     *   Main loop   *
     *               */

    while (!glfwWindowShouldClose(window)) {
        gl(Clear, GL_COLOR_BUFFER_BIT);

        vb->bind();
        if (is_key_just_pressed(GLFW_KEY_ENTER)) {
            float new_pos[] = { -0.9, 0.9 };
            vb->set_attribute(3, 0, new_pos, sizeof(new_pos));
        }
        vb->unbind();

        shader->bind();

        texture->bind(0);
        shader->set_uniform("u_texture_slot", 0);

        va->bind();
        gl(DrawElements, GL_TRIANGLES, ib->index_count(),
           GL_UNSIGNED_INT, nullptr);

        std::memcpy(prev_keys_pressed, keys_pressed, sizeof(keys_pressed));
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    delete va;
    delete shader;
    delete texture;
    glfwTerminate();

    return 0;
}