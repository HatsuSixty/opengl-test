# OpenGL Test

[opengl-learn](https://github.com/HatsuSixty/opengl-learn), the sequel.  

This one features more advanced OpenGL bindings that use dynamically allocated vertex and index buffers, in a way that allows multiple `push_*()`s and `clear()`s per frame, without big performance overheads due to buffer reallocation.

This one also has a simple renderer class in `simple_renderer.cpp`, inside `src/test_opengl/`.

## Quick Start

This project uses the [meson](https://mesonbuild.com/) build system.

```console
$ meson builddir
$ meson compile -C builddir
```