#version 450

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D texture_;

void main() {
    vec4 col = texture(texture_, tex_coords);
    
    frag_color = vec4(1.0) - col;
}
