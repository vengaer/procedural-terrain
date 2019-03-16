#version 450

out vec4 frag_color;

in vec4 clip_space;
in vec3 to_camera;
in vec3 from_sun;
in vec3 normal;
in vec2 tex_coords;

in float near;
in float far;

uniform float dudv_offset;

uniform sampler2D refl_texture;
uniform sampler2D refr_texture;
uniform sampler2D dudv_map;
uniform sampler2D normal_map;
uniform sampler2D depth_map;

const float magnitude = 0.04;
const vec3 sun_color = vec3(1.0, 1.0, 1.0);

const float shine = 40.0;
const float reflectivity = 0.5;

float linearize(float depth) {
    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

void main() {
    vec4 ndc = clip_space / clip_space.w;

    vec2 refr_coords = ndc.xy * 0.5 + 0.5;
    vec2 refl_coords = vec2(refr_coords.x, 1.0-refr_coords.y);

    float depth = texture(depth_map, refr_coords).r;
    float floor_dist = linearize(depth);
    float water_dist = linearize(gl_FragCoord.z);

    depth = floor_dist - water_dist;

    vec2 dist_coords = texture(dudv_map, vec2(tex_coords.x + dudv_offset, tex_coords.y)).rg*0.1;
    dist_coords = tex_coords + vec2(dist_coords.x, dist_coords.y + dudv_offset);
    dist_coords = (texture(dudv_map, dist_coords).rg * 2.0 - 1.0) * magnitude * clamp(depth/20.0, 0.0, 1.0);


    vec4 normal_color = texture(normal_map, dist_coords);
    vec3 normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b * 3.0, normal_color.g * 2.0 - 1.0);
    normal = normalize(normal);

    refr_coords += dist_coords;
    refl_coords += dist_coords;

    refr_coords = clamp(refr_coords, 0.001, 0.999);
    refl_coords = clamp(refl_coords, 0.001, 0.999);

    vec3 view_dir = normalize(to_camera);
    float refr_factor = dot(view_dir, normal);
    refr_factor = pow(refr_factor, 1.5);
    refr_factor = clamp(refr_factor, 0.0, 1.0);

    vec4 refl_col = texture(refl_texture, refl_coords);
    vec4 refr_col = texture(refr_texture, refr_coords);

    vec3 refl_light = reflect(normalize(from_sun), normal);
    float specular = max(dot(refl_light, view_dir), 0.0);
    specular = pow(specular, shine);
    vec3 hightlight = sun_color * specular * reflectivity;
    
    frag_color = mix(refl_col, refr_col, refr_factor);
    frag_color = mix(frag_color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(hightlight, 0.0);
    frag_color.a = clamp(depth/5.0, 0.0, 1.0);
}
