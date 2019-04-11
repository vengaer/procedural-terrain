#ifndef BLUR_H
#define BLUR_H

#pragma once
#include "framebuffer.h"
#include "viewport.h"
#include "shader.h"
#include "texture.h"
#include <algorithm>
#include <functional>
#include <GL/glew.h>
#include <memory>
#include <vector>

namespace impl {
    enum class Direction{Vertical, Horizontal};

    template <Direction dir>
    class Blur {
        using Framebuffer = Framebuffer<1u>;
        public:
            Blur(int width, int height);
            ~Blur();

            void apply(GLuint texture) const;
            GLuint texture_id() const;

            static void update_size();

        private:
            Shader shader_;
            float width_ratio_{}, height_ratio_{};
            Framebuffer fb_;

            static std::vector<std::reference_wrapper<Blur>> instances_;

            void init();
            void resize() const;
            static int constexpr VERTICAL = 0, HORIZONTAL = 1;
    };
}

using HorizontalBlur = impl::Blur<impl::Direction::Horizontal>;
using VerticalBlur = impl::Blur<impl::Direction::Vertical>;

#include "blur.tcc"
#endif
