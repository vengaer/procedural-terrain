#version 450

layout(location = 0) in vec3 position_;
layout(location = 2) in vec2 tex_coords_;

out vec2 tex_coords;

void main() {
    gl_Position = vec4(position_.xy, 0.0, 1.0);
    tex_coords = tex_coords_;
}
