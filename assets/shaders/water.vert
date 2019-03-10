#version 450

layout(location = 0) in vec3 position_;

out vec4 clip_space;
out float time;
out vec3 to_camera;

uniform float ufrm_time;

uniform mat4 ufrm_model;
uniform mat4 ufrm_projection;
uniform mat4 ufrm_view;
uniform vec3 ufrm_camera_pos;

void main() {
    vec4 world_pos = ufrm_model * vec4(position_, 1.0);

    clip_space = ufrm_projection * ufrm_view * world_pos;
    time = ufrm_time;

    gl_Position = clip_space;
    to_camera = ufrm_camera_pos - world_pos.xyz;
}
