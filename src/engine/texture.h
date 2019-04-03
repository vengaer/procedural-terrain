#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
#include <cstddef>
#include <GL/glew.h>
#include <string>

class Texture {
    public:
        Texture() = default;
        Texture(GLuint tex_id);
        Texture(std::string const& path);
        Texture(unsigned char* data, std::size_t width, std::size_t height);
        ~Texture();

        static void bind(GLuint id, std::size_t unit = 0u);
        GLuint id() const;

        std::size_t width() const;
        std::size_t height() const;

        static std::size_t constexpr Unit0  = 0u;
        static std::size_t constexpr Unit1  = 1u;
        static std::size_t constexpr Unit2  = 2u;
        static std::size_t constexpr Unit3  = 3u;
        static std::size_t constexpr Unit4  = 4u;
        static std::size_t constexpr Unit5  = 5u;
        static std::size_t constexpr Unit6  = 6u;
        static std::size_t constexpr Unit7  = 7u;
        static std::size_t constexpr Unit8  = 8u;
        static std::size_t constexpr Unit9  = 9u;
        static std::size_t constexpr Unit10 = 10u;
    private:
        std::size_t width_{}, height_{};
        GLuint id_{};

        void init(unsigned char* data);
};

#endif
