#version 450

layout(location = 0) in vec3 position_;
layout(location = 2) in vec2 tex_coords_;

out vec4 clip_space;
out vec3 to_camera;
out vec2 tex_coords;
out vec3 from_sun;

out float near;
out float far;

uniform float ufrm_time;

uniform mat4 ufrm_model;
uniform mat4 ufrm_projection;
uniform mat4 ufrm_view;
uniform vec3 ufrm_camera_pos;
uniform vec3 ufrm_sun_position;

const float tiling = 4.0;

void main() {
    vec4 world_pos = ufrm_model * vec4(position_, 1.0);

    clip_space = ufrm_projection * ufrm_view * world_pos;

    gl_Position = clip_space;
    to_camera = ufrm_camera_pos - world_pos.xyz;
    tex_coords = tex_coords_ * tiling;

    from_sun = world_pos.xyz - ufrm_sun_position;

    float c = ufrm_projection[2][2];
    float d = ufrm_projection[2][3];
    near = d/(c-1.0);
    far  = d/(c+1.0);
}
