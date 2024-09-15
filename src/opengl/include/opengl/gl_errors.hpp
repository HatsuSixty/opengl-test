#pragma once

#define gl(name, ...)                         \
    do {                                      \
        GL::clear_errors();                   \
        gl##name(__VA_ARGS__);                \
        GL::check_errors(__FILE__, __LINE__); \
    } while (0);

#define gl_call(...)                          \
    do {                                      \
        GL::clear_errors();                   \
        __VA_ARGS__;                          \
        GL::check_errors(__FILE__, __LINE__); \
    } while (0);

namespace GL {

void clear_errors();
void check_errors(const char* file_path, int line);

}