#version 450

layout(location = 0) in vec3 position_;
layout(location = 2) in vec2 tex_coords_;

out vec2 tex_coords;

uniform float ufrm_time;

uniform mat4 ufrm_model;
uniform mat4 ufrm_projection;
uniform mat4 ufrm_view;

void main() {
    gl_Position = ufrm_projection * ufrm_view * ufrm_model * vec4(position_, 1.0);
    tex_coords = tex_coords_;
}
