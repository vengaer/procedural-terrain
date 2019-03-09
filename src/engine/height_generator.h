#ifndef HEIGHT_GENERATOR_H
#define HEIGHT_GENERATOR_H

#pragma once
#include <array>
#include <cstddef>
#include <random>
#include <unordered_map>

class HeightGenerator {
    public:
        HeightGenerator(float amplitude);

        float generate(int x, int z);

        static std::size_t constexpr OCTAVES = 3u;
        static float constexpr ROUGHNESS = 0.2f; 
    private:
        float amplitude_;
        std::random_device rd_;
        std::mt19937 mt_;
        std::uniform_real_distribution<float> dist_;
        std::unordered_map<int, float> cache_{};

        float generate_noise(int x, int z);
        float generate_smooth_noise(int x, int z);
        float interpolated_noise(float x, float z);
};

#endif
