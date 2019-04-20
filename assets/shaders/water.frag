#version 450

out vec4 frag_color;

in vec4 clip_space;
in vec3 to_camera;
in vec3 from_sun;
in vec3 normal;
in vec2 tex_coords;

in float to_cam_length;
in float near;
in float far;

uniform float ufrm_dudv_offset;

uniform sampler2D refl_texture;
uniform sampler2D refr_texture;
uniform sampler2D dudv_map;
uniform sampler2D normal_map;
uniform sampler2D depth_map;

const float magnitude = 0.04;
const vec3 sun_color = vec3(1.25, 0.85, 0.85);

const float shine = 40.0;
const float reflectivity = 0.6;

float linearize(float depth) {
    return 2.0 * near * far / (far + near - (2.0 * depth - 1.0) * (far - near));
}

float water_depth(vec2 coords) {
    float depth = texture(depth_map, coords).r;
    float floor_dist = linearize(depth);
    float water_dist = linearize(gl_FragCoord.z);
    
    return floor_dist - water_dist;
}

vec2 distort_tex_coords(float depth) {
    vec2 dist_coords = texture(dudv_map, vec2(tex_coords.x + ufrm_dudv_offset, tex_coords.y)).rg*0.1;
    dist_coords = tex_coords + vec2(dist_coords.x, dist_coords.y + ufrm_dudv_offset);
    return (texture(dudv_map, dist_coords).rg * 2.0 - 1.0) * magnitude * clamp(depth/20.0, 0.0, 1.0);
}

vec3 calculate_normal(vec2 coords) {
    vec4 normal_color = texture(normal_map, coords);
    vec3 normal = vec3(normal_color.r * 2.0 - 1.0, normal_color.b * 3.0, normal_color.g * 2.0 - 1.0);
    return normalize(normal);
}

float fresnel(vec3 view_dir, vec3 normal) {
    float refr_factor = dot(view_dir, normal);
    refr_factor = pow(refr_factor, 0.8);
    return clamp(refr_factor, 0.0, 1.0);
}

vec3 specular(vec3 view_dir, vec3 normal) {
    vec3 refl_light = reflect(normalize(from_sun), normal);
    float specular = max(dot(refl_light, view_dir), 0.0);
    specular = pow(specular, shine);
    return sun_color * specular * reflectivity;
}

float refr_offset(vec3 normal, float depth) {
    float theta1 = acos(dot(normal, normalize(to_camera)));
    float theta2 = asin(1 / 1.33 * sin(theta1));
    float d = to_cam_length * sin(theta1);
    float d_prime = to_cam_length * sin(theta2);
    return depth * 0.02 * (d - d_prime);
}

void main() {
    vec4 ndc = clip_space / clip_space.w;

    vec2 refr_coords = ndc.xy * 0.5 + 0.5;
    vec2 refl_coords = vec2(refr_coords.x, 1.0-refr_coords.y);

    float depth = water_depth(refr_coords);

    vec2 dist_coords = distort_tex_coords(depth);

    vec3 normal = calculate_normal(dist_coords);

    refr_coords.y -= refr_offset(normal, depth);
    refr_coords += dist_coords;
    refl_coords += dist_coords;

    refr_coords = clamp(refr_coords, 0.001, 0.999);
    refl_coords = clamp(refl_coords, 0.001, 0.999);

    vec3 view_dir = normalize(to_camera);
    float refr_factor = fresnel(view_dir, normal);

    vec4 refl_col = texture(refl_texture, refl_coords);
    vec4 refr_col = texture(refr_texture, refr_coords);

    vec3 highlight = specular(view_dir, normal);
    
    frag_color = mix(refl_col, refr_col, refr_factor);
    frag_color = mix(frag_color, vec4(0.0, 0.3, 0.5, 1.0), 0.2) + vec4(highlight, 0.0);
    frag_color.a = clamp(depth/5.0, 0.0, 1.0);
}
