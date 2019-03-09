#version 450

out vec4 frag_color;

in vec2 blur_tex_coords[11];

uniform sampler2D texture_;

float weights[6] = float[](0.0093, 0.028002, 0.065984, 0.121703, 0.175713, 0.198596);

void main() {
    frag_color = vec4(0.0);
    for(int i = 0; i < 11; i++) {
        frag_color += texture(texture_, blur_tex_coords[i]) * weights[5 - abs(5-i)];
    }
}
