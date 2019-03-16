#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#pragma once
#include "bitmask.h"
#include "collections.h"
#include "exception.h"
#include "viewport.h"
#include <algorithm>
#include <cstddef>
#include <GL/glew.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>

struct TexType {
    static std::size_t constexpr Color = 0x1;
    static std::size_t constexpr Depth = 0x2;
};

template <std::size_t T>
struct has_color_attachment : std::bool_constant<(T & TexType::Color) == TexType::Color> { };

template <std::size_t T>
struct has_depth_attachment : std::bool_constant<(T & TexType::Depth) == TexType::Depth> { };

template <std::size_t T>
inline bool constexpr has_color_attachment_v = has_color_attachment<T>::value;

template <std::size_t T>
inline bool constexpr has_depth_attachment_v = has_depth_attachment<T>::value;


template <std::size_t N = 1u, std::size_t T = TexType::Color>
class Framebuffer {
    public:
        Framebuffer(float width_ratio, float height_ratio);
        ~Framebuffer();

        template <std::size_t M = N, typename = std::enable_if_t<has_color_attachment_v<T> && M == 1u>>
        GLuint texture() const;
        template <std::size_t M = N, typename = std::enable_if_t<has_color_attachment_v<T> && (M > 1u)>>
        std::array<GLuint, N> textures() const;

        template <std::size_t M = N, typename = std::enable_if_t<has_depth_attachment_v<T> && M == 1u>>
        GLuint depth_texture() const;
        template <std::size_t M = N, typename = std::enable_if_t<has_depth_attachment_v<T> && (M > 1u)>>
        std::array<GLuint, N> depth_textures() const;

        void bind() const;
        void unbind() const;

        static void reallocate();
        
        static float constexpr FULL_WIDTH = 1.f;
        static float constexpr FULL_HEIGHT = 1.f;

    private:
        float width_ratio_, height_ratio_;
        GLuint fbo_{};
        std::array<GLuint, N> textures_{};
        std::array<GLuint, N> rbos_{};
        std::array<GLuint, N> depth_textures_{};
    
        static std::vector<std::reference_wrapper<Framebuffer>> instances_;
        static std::array<GLuint, N> color_attachments_;

        void init();
        void setup_texture_environment();
        void resize();
    
        static bool constexpr has_color_attachment();
        static bool constexpr has_depth_attachment();

};

#include "framebuffer.tcc"
#endif
