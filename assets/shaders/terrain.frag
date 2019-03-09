#version 450

out vec4 frag_color;

in vec3 sun_position;
in vec3 position;
in vec3 camera_view;
in vec3 normal;

in float terrain_amplitude;

const float ambient_strength = 0.1;
const vec3 sun_color = vec3(1.0, 1.0, 1.0);

void main() {
    vec3 ambient = ambient_strength * sun_color.rgb;

    vec3 norm = normalize(normal);
    vec3 light_dir = normalize(sun_position - position);

    float diffuse = max(dot(norm, light_dir), 0.0);

    float y_pos = float(position.y);
    float weight = (y_pos + terrain_amplitude) / (2*terrain_amplitude);
    
    vec3 ridge_color  = vec3(0.96, 0.86, 0.65);
    vec3 valley_color = vec3(0.0, 0.1, 0.1);

    vec3 color = mix(valley_color, ridge_color, weight); 

    vec3 res = (ambient + diffuse) * color;
	frag_color = vec4(res, 1.0);
}
