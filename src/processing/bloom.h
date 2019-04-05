#ifndef BLOOM_H
#define BLOOM_H

#pragma once
#include "framebuffer.h"
#include "shader.h"
#include <array>
#include <GL/glew.h>

class Bloom {
    using Framebuffer = Framebuffer<2u>;
    public:
        Bloom(int width, int height);

        void apply(GLuint texture) const;
        std::array<GLuint, 2> texture_ids() const;

    private:
        Shader shader_;
        Framebuffer fb_;
};

#endif
