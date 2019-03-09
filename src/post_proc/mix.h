#ifndef MIX_H
#define MIX_H

#pragma once
#include "shader.h"
#include <GL/glew.h>

class Mix {
    public:
        Mix();
        ~Mix();

        void apply(GLuint texture0, GLuint texture1) const;
        GLuint texture() const;

    private:
        Shader shader_;
        GLuint fbo_{}, texture_{};

        void setup_texture_environment();
};

#endif
