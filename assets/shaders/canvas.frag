#version 450

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D scene;

void main() {
	vec3 col = texture(scene, tex_coords).rgb;
	
	frag_color = vec4(col, 1.0);
}
