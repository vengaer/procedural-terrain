#version 450

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bloom_color;

in vec2 tex_coords;

uniform sampler2D texture_;

void main() {
    frag_color = texture(texture_, tex_coords);
    float brightness = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    
    bloom_color = frag_color * brightness * 2.0;
}
