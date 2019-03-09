#version 450

layout(location = 0) in vec3 position_;
layout(location = 2) in vec2 tex_coords_;

out vec2 blur_tex_coords[11];

uniform int ufrm_direction;
uniform float ufrm_dim;

void main() {
    gl_Position = vec4(position_.xy, 0.0, 1.0);

    float size = 1.0 / ufrm_dim;

    for(int i = 0; i < 11; i++) {
        float x_pos = ufrm_direction == 1 ? size * (i-5) : 0.0;
        float y_pos = ufrm_direction == 0 ? size * (i-5) : 0.0;
        blur_tex_coords[i] = tex_coords_ + vec2(x_pos, y_pos);
    }
}
