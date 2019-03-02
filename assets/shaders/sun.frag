#version 450

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

void main() {
	frag_color = vec4(100.9, 0.3, 0.1, 1.0);
	
	float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));

	if(brightness > 1.0)
		bright_color = vec4(frag_color.rgb, 1.0);
	else
		bright_color = vec4(0.0, 0.0, 0.0, 1.0);
}
