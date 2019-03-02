#version 450

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D texture_;

void main() {
	frag_color = texture(texture_, tex_coords);
}
