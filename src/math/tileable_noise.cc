#include "constants.h"
#include "tileable_noise.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <numeric>
#include <random>

namespace tileable_noise {
    float fade(float t);
    float surflet(float x, float y, int grid_x, int grid_y, unsigned period);
    std::array<GLuint, 512> permutate();
    std::array<glm::vec2, 256> compute_directions();

    std::array<GLuint, 512> permutation{};
    std::array<glm::vec2, 256> directions{};

    bool first_call{true};
}

float tileable_noise::generate(float x, float y, unsigned period) {
    if(first_call) {
        permutation = permutate();
        directions = compute_directions();
        first_call = false;
    }

    int ix = static_cast<int>(x);
    int iy = static_cast<int>(y);

    return surflet(x, y, ix, iy, period) + surflet(x, y, ix + 1, iy, period) +
           surflet(x, y, ix, iy + 1, period) + surflet(x, y, ix + 1, iy + 1, period);
}

Texture tileable_noise::texture() {
    std::size_t constexpr period = 128, image_size = 128;
    std::size_t constexpr size = 3u * image_size;

    using image_t = std::array<std::array<unsigned char, size>,size>;
    image_t data;

    float freq = 1.f/32.f;

    for(auto i = 0u; i < size; i++) {
        for(auto j = 0u; j < size; j += 3) {
            float fi = static_cast<float>(i) * freq;
            float fj = static_cast<float>(j) * freq;

            data[i][j]   = static_cast<unsigned char>((tileable_noise::generate(4.f*fj, 4.f*fi, period) + 1.f) * 0.5f * 255);
            data[i][j+1] = static_cast<unsigned char>((tileable_noise::generate(4.f*fi, 4.f*fj, period) + 1.f) * 0.5f * 255);
            data[i][j+2] = 0u;
        }
    }

    return Texture{&data[0][0], image_size, image_size};

}


float tileable_noise::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float tileable_noise::surflet(float x, float y, int grid_x, int grid_y, unsigned period) {
    float dist_x = std::abs(x - grid_x);
    float dist_y = std::abs(y - grid_y);

    float poly_x = 1.f - fade(dist_x);
    float poly_y = 1.f - fade(dist_y);

    float hashed = permutation[permutation[grid_x % period] + grid_y % period];

    float grad = (x - grid_x) * directions[hashed][0] + (y - grid_y) * directions[hashed][1];

    return poly_x * poly_y * grad;
}


std::array<GLuint, 512> tileable_noise::permutate() {
    std::array<GLuint, 256> p;
    std::iota(std::begin(p), std::end(p), 0u);
    std::shuffle(std::begin(p), std::end(p), std::mt19937{std::random_device{}()});

    std::array<GLuint, 512> perm;
    auto it = std::begin(perm);

    for(auto i = 0; i < 2; i++)
        it = std::copy(std::begin(p), std::end(p), it);

    return perm;
}

std::array<glm::vec2, 256> tileable_noise::compute_directions() {
    std::array<glm::vec2, 256> dirs;
    
    std::generate(std::begin(dirs), std::end(dirs), [n = 0] () mutable {
        auto i = n++;
        return glm::vec2{std::cos(i * 2.f * PI / 256),
                         std::sin(i * 2.f * PI / 256)};
    });
    
    return dirs;
}

