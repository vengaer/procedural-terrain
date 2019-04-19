
template <InterpolationMethod IM>
HeightGenerator<IM>::HeightGenerator(float amplitude) : amplitude_{amplitude}, mt_{std::random_device{}()}, dist_{-amplitude_, amplitude_}, seed_{seed_dist_(mt_)} { }

template <InterpolationMethod IM>
float HeightGenerator<IM>::generate(int x, int z) {
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

template <InterpolationMethod IM>
float HeightGenerator<IM>::generate_noise(int x, int z) {
    auto hash = x + 10'000 * z;

    auto it = cache_.find(hash);
    if(it == std::end(cache_)) {
        mt_.seed(seed_ + x * 1993 + z * 1290);
        it = cache_.emplace_hint(std::end(cache_), hash, dist_(mt_));
    }
    
    return it->second;

}

template <InterpolationMethod IM>
float HeightGenerator<IM>::generate_smooth_noise(int x, int z) {
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

template <InterpolationMethod IM>
float HeightGenerator<IM>::interpolated_noise(float x, float z) {
    int x_i = static_cast<int>(x);
    int z_i = static_cast<int>(z);
    
    float rem_x = x - static_cast<float>(x_i);  
    float rem_z = z - static_cast<float>(z_i);  

    if constexpr(IM == InterpolationMethod::Bicubic) {
        float const p0_z0 = generate_smooth_noise(x_i - 1, z_i - 1);
        float const p1_z0 = generate_smooth_noise(x_i, z_i - 1);
        float const p2_z0 = generate_smooth_noise(x_i + 1, z_i - 1);
        float const p3_z0 = generate_smooth_noise(x_i + 2, z_i - 1);
        float const p0_z1 = generate_smooth_noise(x_i - 1, z_i);
        float const p1_z1 = generate_smooth_noise(x_i, z_i);
        float const p2_z1 = generate_smooth_noise(x_i + 1, z_i);
        float const p3_z1 = generate_smooth_noise(x_i + 2, z_i);
        float const p0_z2 = generate_smooth_noise(x_i - 1, z_i + 1);
        float const p1_z2 = generate_smooth_noise(x_i, z_i + 1);
        float const p2_z2 = generate_smooth_noise(x_i + 1, z_i + 1);
        float const p3_z2 = generate_smooth_noise(x_i + 2, z_i + 1);
        float const p0_z3 = generate_smooth_noise(x_i - 1, z_i + 1);
        float const p1_z3 = generate_smooth_noise(x_i, z_i + 1);
        float const p2_z3 = generate_smooth_noise(x_i + 1, z_i + 1);
        float const p3_z3 = generate_smooth_noise(x_i + 2, z_i + 1);

        float const z0 = interpolation::cubic(rem_x, p0_z0, p1_z0, p2_z0, p3_z0);
        float const z1 = interpolation::cubic(rem_x, p0_z1, p1_z1, p2_z1, p3_z1);
        float const z2 = interpolation::cubic(rem_x, p0_z2, p1_z2, p2_z2, p3_z2);
        float const z3 = interpolation::cubic(rem_x, p0_z3, p1_z3, p2_z3, p3_z3);

        return interpolation::cubic(rem_z, z0, z1, z2, z3);
    }
    else {
        float const p1_z1 = generate_smooth_noise(x_i, z_i);
        float const p2_z1 = generate_smooth_noise(x_i + 1, z_i);
        float const p1_z2 = generate_smooth_noise(x_i, z_i + 1);
        float const p2_z2 = generate_smooth_noise(x_i + 1, z_i + 1);


        if constexpr(IM == InterpolationMethod::Cosine) {
            float const z1 = interpolation::cosine(rem_x, p1_z1, p2_z1);
            float const z2 = interpolation::cosine(rem_x, p1_z2, p2_z2);

            return interpolation::cosine(rem_z, z1, z2);
        }
        else {
            float const z1 = interpolation::linear(rem_x, p1_z1, p2_z1);
            float const z2 = interpolation::linear(rem_x, p1_z2, p2_z2);

            return interpolation::linear(rem_z, z1, z2);
        }
    }
}

template <InterpolationMethod IM>
std::uniform_int_distribution<int> HeightGenerator<IM>::seed_dist_{
    -DISTR_MAX_MIN,
     DISTR_MAX_MIN
};
