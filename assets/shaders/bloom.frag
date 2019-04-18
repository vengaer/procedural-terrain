#version 450

layout(location = 0) out vec4 frag_color;
layout(location = 1) out vec4 bloom_color;

in vec2 tex_coords;

uniform vec4 clear_col;
uniform sampler2D texture_;

void main() {
    frag_color = texture(texture_, tex_coords);
    float luminance = dot(frag_color.rgb, vec3(0.2126, 0.7152, 0.0722));
    

    bloom_color = vec4(0);

    // Hide HDR object in RT0 as it might cause scaling issues otherwise
    if(luminance > 0.8) {
        bloom_color = frag_color * luminance * 1.5;
        frag_color = clear_col;
    }
}
