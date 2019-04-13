#include "height_generator.h"
#include "interpolation.h"
#include <cmath>

HeightGenerator::HeightGenerator(float amplitude) : amplitude_{amplitude}, mt_{std::random_device{}()}, dist_{-amplitude_, amplitude_}, seed_{seed_dist_(mt_)} { }

float HeightGenerator::generate(int x, int z) {
    float height = 0.f;
    float divisor = std::pow(2.f, OCTAVES-1u);
    for(auto i = 0u; i < OCTAVES; i++) {
        float frequency = std::pow(2.f, i) / divisor;
        float amp = std::pow(ROUGHNESS, i);
        height += interpolated_noise(static_cast<float>(x)*frequency, 
                                     static_cast<float>(z)*frequency) * amp;
    }

    return height;
}

float HeightGenerator::generate_noise(int x, int z) {
    auto hash = x + 10'000 * z;

    auto it = cache_.find(hash);
    if(it == std::end(cache_)) {
        mt_.seed(seed_ + x * 1993 + z * 1290);
        it = cache_.emplace_hint(std::end(cache_), hash, dist_(mt_));
    }
    
    return it->second;

}

float HeightGenerator::generate_smooth_noise(int x, int z) {
    float corners = (generate_noise(x-1, z-1) + 
                     generate_noise(x+1, z-1) + 
                     generate_noise(x-1, x+1) +
                     generate_noise(x+1, z+1)) / 16.f;

    float sides = (generate_noise(x-1, z) +
                   generate_noise(x+1, z) +
                   generate_noise(x, z-1) +
                   generate_noise(x, z+1)) / 8.f;

    float center = generate_noise(x,z) / 4.f;
    
    return corners + sides + center;
}

float HeightGenerator::interpolated_noise(float x, float z) {
    int x_i = static_cast<int>(x);
    int z_i = static_cast<int>(z);
    
    float rem_x = x - static_cast<float>(x_i);  
    float rem_z = z - static_cast<float>(z_i);  

    float vert1 = generate_smooth_noise(x_i, z_i);
    float vert2 = generate_smooth_noise(x_i+1, z_i);
    float vert3 = generate_smooth_noise(x_i, z_i+1);
    float vert4 = generate_smooth_noise(x_i+1, z_i+1);

    float interp1 = interpolation::cosine(rem_x, vert1, vert2);
    float interp2 = interpolation::cosine(rem_x, vert3, vert4);

    return interpolation::cosine(rem_z, interp1, interp2);
}

std::uniform_int_distribution<int> HeightGenerator::seed_dist_{
    -DISTR_MAX_MIN,
     DISTR_MAX_MIN
};
