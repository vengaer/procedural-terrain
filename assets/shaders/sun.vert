#version 450

layout(location = 0) in vec3 position_;
layout(location = 1) in vec3 normal_;
layout(location = 2) in vec2 tex_coords_;

uniform float ufrm_time;

uniform mat4 ufrm_model;
uniform mat4 ufrm_projection;
uniform mat4 ufrm_view;

void main() {
	gl_Position = ufrm_projection * ufrm_view * ufrm_model * vec4(position_, 1.0);
}
