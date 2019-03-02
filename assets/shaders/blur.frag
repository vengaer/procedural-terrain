#version 450

in vec2 tex_coords;

out vec4 frag_color;

uniform sampler2D texture_;

uniform int ufrm_direction;
float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);

void main() {
	vec2 offset = 1.0 / textureSize(texture_, 0);
	vec3 result = texture(texture_, tex_coords).rgb * weights[0];

	if(ufrm_direction == 1) {
		for(int i = 1; i < 5; i++) {
			result += texture(texture_, tex_coords + vec2(offset.x * i, 0.0)).rgb * weights[i];
			result += texture(texture_, tex_coords - vec2(offset.x * i, 0.0)).rgb * weights[i];
		}
	}
	else {
		for(int i = 1; i < 5; i++) {
			result += texture(texture_, tex_coords + vec2(offset.y * i, 0.0)).rgb * weights[i];
			result += texture(texture_, tex_coords - vec2(offset.y * i, 0.0)).rgb * weights[i];
		}
	}

	frag_color = vec4(result, 1.0);
}
