#ifndef BLOOM_H
#define BLOOM_H

#pragma once
#include "shader.h"
#include <array>
#include <GL/glew.h>

class Bloom {
    public:
        Bloom(int width, int height);
       ~Bloom();

        void apply(GLuint texture) const;
        std::array<GLuint, 2> textures() const;
        

    private:
        int width_, height_;
        Shader shader_;
        GLuint fbo_{};
        std::array<GLuint, 2> textures_{};
        static std::array<GLuint, 2> constexpr attachments_ {
            GL_COLOR_ATTACHMENT0,
            GL_COLOR_ATTACHMENT1
        };

        
        void setup_texture_environment();
};

#endif
