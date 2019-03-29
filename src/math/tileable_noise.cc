#include "constants.h"
#include "tileable_noise.h"
#ifndef STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#endif
#include <algorithm>
#include <cstddef>
#include <cmath>
#include <numeric>
#include <random>

Texture TileableNoise::operator()() const {
    std::size_t constexpr size = 3u * size_;
    image_t<unsigned char, size> data;

    float freq = 1.f/32.f;

    for(auto i = 0u; i < size; i++) {
        for(auto j = 0u; j < size; j += 3) {
            float fi = static_cast<float>(i) * freq;
            float fj = static_cast<float>(j) * freq;

            data[i][j]   = static_cast<unsigned char>((generate(4.f*fj, 4.f*fi, period_) + 1.f) * 0.5f * 255);
            data[i][j+1] = static_cast<unsigned char>((generate(4.f*fi, 4.f*fj, period_) + 1.f) * 0.5f * 255);
            data[i][j+2] = 0u;
        }
    }

    return Texture{&data[0][0], size_, size_};
}

float TileableNoise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float TileableNoise::surflet(float x, float z, int grid_x, int grid_z, unsigned period) {
    float dist_x = std::abs(x - grid_x);
    float dist_z = std::abs(z - grid_z);

    float poly_x = 1.f - fade(dist_x);
    float poly_z = 1.f - fade(dist_z);

    float hashed = permutation_[permutation_[grid_x % period] + grid_z % period];

    float grad = (x - grid_x) * directions_[hashed][0] + (z - grid_z) * directions_[hashed][1];

    return poly_x * poly_z * grad;
}

float TileableNoise::generate(float x, float z, unsigned period) {
    int ix = static_cast<int>(x);
    int iz = static_cast<int>(z);

    return surflet(x, z, ix, iz, period) + surflet(x, z, ix + 1, iz, period) +
           surflet(x, z, ix, iz + 1, period) + surflet(x, z, ix + 1, iz + 1, period);
}

std::array<GLuint, 512> TileableNoise::permutate() {
    std::array<GLuint, 256> p;
    std::iota(std::begin(p), std::end(p), 0u);
    std::shuffle(std::begin(p), std::end(p), std::mt19937{std::random_device{}()});

    std::array<GLuint, 512> perm;
    auto it = std::begin(perm);

    for(auto i = 0; i < 2; i++)
        it = std::copy(std::begin(p), std::end(p), it);

    return perm;
}

std::array<glm::vec2, 256> TileableNoise::compute_directions() {
    std::array<glm::vec2, 256> dirs;
    
    std::generate(std::begin(dirs), std::end(dirs), [n = 0] () mutable {
        auto i = n++;
        return glm::vec2{std::cos(i * 2.f * PI / 256),
                         std::sin(i * 2.f * PI / 256)};
    });
    
    return dirs;
}

std::array<GLuint, 512> TileableNoise::permutation_ = TileableNoise::permutate();
std::array<glm::vec2, 256> TileableNoise::directions_ = TileableNoise::compute_directions();
