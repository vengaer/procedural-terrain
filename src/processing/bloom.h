#ifndef BLOOM_H
#define BLOOM_H

#pragma once
#include "framebuffer.h"
#include "shader.h"
#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class Bloom {
    using Framebuffer = Framebuffer<2u>;
    public:
        Bloom(int width, int height);

        void apply(GLuint texture) const;
        std::array<GLuint, 2> texture_ids() const;

        static std::string const CLEAR_COLOR_UNIFORM_NAME;

    private:
        Shader shader_;
        Framebuffer fb_;
        mutable glm::vec4 clear_color_;
};

#endif
