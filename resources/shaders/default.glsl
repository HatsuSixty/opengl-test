#shader vertex
#version 330 core

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_tex_coord;

out vec2 v_tex_coord;

void main() {
    gl_Position = a_position;
    v_tex_coord = a_tex_coord;
}


#shader fragment
#version 330 core

layout(location = 0) out vec4 frag_color;

uniform vec4 u_color;
uniform sampler2D u_texture_slot;

in vec2 v_tex_coord;

void main() {
    vec4 tex_color = texture(u_texture_slot, v_tex_coord);
    frag_color = tex_color * u_color;
}