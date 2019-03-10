#version 450

out vec4 frag_color;

in vec4 clip_space;
in vec3 to_camera;
in float time;

uniform sampler2D refl_texture;
uniform sampler2D refr_texture;

const float magnitude = 0.02;
const float time_scale = 0.3;

void main() {
    vec4 ndc = clip_space / clip_space.w;

    vec2 refr_coords = ndc.xy * 0.5 + 0.5;
    vec2 refl_coords = vec2(refr_coords.x, 1.0-refr_coords.y);

    vec3 view_dir = normalize(to_camera);
    float refr_factor = dot(view_dir, vec3(0.0, 1.0, 0.0));
    refr_factor = pow(refr_factor, 1.5);

    vec4 refl_col = texture(refl_texture, refl_coords);
    vec4 refr_col = texture(refr_texture, refr_coords);
    
    frag_color = mix(refl_col, refr_col, refr_factor);
    frag_color = mix(frag_color, vec4(0.0, 0.3, 0.5, 1.0), 0.2);
}
