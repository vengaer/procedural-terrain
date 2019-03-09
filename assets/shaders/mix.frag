#version 450

out vec4 frag_color;

in vec2 tex_coords;

uniform sampler2D scene_;
uniform sampler2D highlight_;

void main() {
    vec4 scene_col = texture(scene_, tex_coords);
    vec4 highlight_col = texture(highlight_, tex_coords);

    frag_color = scene_col + highlight_col * 4.0;
}
