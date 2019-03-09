#version 450

layout(location = 0) in vec3 position_;
layout(location = 1) in vec3 normal_;
layout(location = 2) in vec2 tex_coords_;

out vec3 sun_position;
out vec3 position;
out vec3 camera_view;
out vec3 normal;

out float terrain_amplitude;

uniform float ufrm_time;

uniform mat4 ufrm_model;
uniform mat4 ufrm_projection;
uniform mat4 ufrm_view;

uniform vec3 ufrm_sun_position;
uniform float ufrm_terrain_amplitude;

uniform vec4 ufrm_clipping_plane;

void main() {
    gl_ClipDistance[0] = dot(vec4(position_, 1.0), ufrm_clipping_plane);
	gl_Position = ufrm_projection * ufrm_view * ufrm_model * vec4(position_, 1.0);

    sun_position = ufrm_sun_position;
    position = vec3(ufrm_model * vec4(position_, 1.0));
    camera_view = vec3(ufrm_view[0][3], ufrm_view[1][3], ufrm_view[2][3]);
    normal = normal_;
    terrain_amplitude = ufrm_terrain_amplitude;
}
