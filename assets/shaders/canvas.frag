#version 450

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D scene;
uniform sampler2D bloom;


void main() {
	const float gamma = 2.2;

	vec3 hdr_col = texture(scene, tex_coords).rgb;
	vec3 bloom_col = texture(bloom, tex_coords).rgb;
	
	hdr_col += bloom_col;
	
	vec3 result = vec3(1.0) - exp(-hdr_col * 0.1);
	result = pow(result, vec3(1.0 / gamma));

	frag_color = vec4(result, 1.0);
}
