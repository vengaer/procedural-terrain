#ifndef MIX_H
#define MIX_H

#pragma once
#include "framebuffer.h"
#include "shader.h"
#include <GL/glew.h>

class Mix {
    using Framebuffer = Framebuffer<1u>;
    public:
        Mix();

        void apply(GLuint texture0, GLuint texture1) const;
        GLuint texture() const;

    private:
        Shader shader_;
        Framebuffer fb_;

        void init();
};

#endif
