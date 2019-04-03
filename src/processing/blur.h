#ifndef BLUR_H
#define BLUR_H

#pragma once
#include "framebuffer.h"
#include "viewport.h"
#include "shader.h"
#include "texture.h"
#include <GL/glew.h>

namespace impl {
    enum class Direction{Vertical, Horizontal};

    template <Direction dir>
    class Blur {
        using Framebuffer = Framebuffer<1u>;
        public:
            Blur(int width, int height);

            void apply(GLuint texture) const;
            GLuint texture() const;

        private:
            Shader shader_;
            int width_{}, height_{};
            Framebuffer fb_;

            void init();
            static int constexpr VERTICAL = 0, HORIZONTAL = 1;
    };
}

using HorizontalBlur = impl::Blur<impl::Direction::Horizontal>;
using VerticalBlur = impl::Blur<impl::Direction::Vertical>;

#include "blur.tcc"
#endif
