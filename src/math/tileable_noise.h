#ifndef TILEABLE_NOISE_H
#define TILEABLE_NOISE_H

#pragma once
#include "texture.h"
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class TileableNoise {
    template <typename T, std::size_t N>
    using image_t = std::array<std::array<T,N>,N>;
    public:
        Texture operator()() const;

    private:
        static std::array<GLuint, 512> permutation_;
        static std::array<glm::vec2, 256> directions_;

        static GLuint constexpr size_{128};
        static GLuint constexpr period_{128};

        static std::array<GLuint, 512> permutate();
        static std::array<glm::vec2, 256> compute_directions();

        static float fade(float t);
        static float surflet(float x, float z, int grid_x, int grid_z, unsigned period);
        static float generate(float x, float z, unsigned period);
};

#endif
