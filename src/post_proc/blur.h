#ifndef BLUR_H
#define BLUR_H

#pragma once
#include "exception.h"
#include "viewport.h"
#include "shader.h"
#include <GL/glew.h>

namespace impl {
    enum class Direction{Vertical, Horizontal};

    template <Direction dir>
    class Blur {
        public:
            Blur(int width, int height);
            ~Blur();

            void apply(GLuint texture) const;
            void bind() const;
            GLuint texture() const;

        private:
            GLuint fbo_{};
            GLuint texture_{};
            Shader shader_;
            int width_{}, height_{};

            void init();
            void setup_texture_environment();
            static int constexpr VERTICAL = 0, HORIZONTAL = 1;
    };
}

using HorizontalBlur = impl::Blur<impl::Direction::Horizontal>;
using VerticalBlur = impl::Blur<impl::Direction::Vertical>;

#include "blur.tcc"
#endif
