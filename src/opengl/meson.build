opengl_inc = include_directories('include')

opengl = static_library('opengl', [
    'gl_errors.cpp',
    'index_buffer.cpp',
    'vertex_buffer.cpp',
    'vertex_array.cpp',
    'shader.cpp',
    'texture.cpp',
], dependencies : [
    dependency('glew'),
], include_directories : [
    opengl_inc,
])