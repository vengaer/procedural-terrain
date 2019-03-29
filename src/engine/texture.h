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

        void bind(std::size_t unit = 0u) const;
        GLuint id() const;

        std::size_t width() const;
        std::size_t height() const;

    private:
        std::size_t width_{}, height_{};
        GLuint id_{};

        void init(unsigned char* data);
};

#endif
