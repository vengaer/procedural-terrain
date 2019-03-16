#ifndef TEXTURE_H
#define TEXTURE_H

#pragma once
#include <GL/glew.h>
#include <string>

class Texture {
    public:
        Texture(std::string const& path);
        ~Texture();

        void bind() const;
        void bind(int unit) const;
        GLuint id() const;

    private:
        int width_{}, height_{}, channels_{};
        unsigned char* data_{};
        GLuint texture_{};
        

        void init(std::string const& path);
};

#endif