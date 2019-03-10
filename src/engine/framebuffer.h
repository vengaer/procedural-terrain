#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#pragma once
#include "exception.h"
#include "viewport.h"
#include <algorithm>
#include <cstddef>
#include <GL/glew.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

template <std::size_t N = 1u>
class Framebuffer {
    public:
        Framebuffer(float width_ratio, float height_ratio);
        ~Framebuffer();

        template <std::size_t M = N, typename = std::enable_if_t<M == 1u>>
        GLuint texture() const;
        template <std::size_t M = N, typename = std::enable_if_t<(M > 1u)>>
        std::array<GLuint, N> textures() const;

        void bind() const;
        static void unbind();

        static void reallocate();
        
        static float constexpr FULL_WIDTH = 1.f;
        static float constexpr FULL_HEIGHT = 1.f;

    private:
        float width_ratio_, height_ratio_;
        GLuint fbo_{};
        std::array<GLuint, N> textures_{};
    
        static std::vector<std::reference_wrapper<Framebuffer>> instances_;
        static std::array<GLuint, N> color_attachments_;

        void init();
        void setup_texture_environment();
        void resize();

};

#include "framebuffer.tcc"
#endif
