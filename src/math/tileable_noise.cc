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
        return glm::vec2{std::cos(i * 2.f * math::PI / 256),
                         std::sin(i * 2.f * math::PI / 256)};
    });
    
    return dirs;
}

