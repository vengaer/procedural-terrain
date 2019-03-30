#ifndef WATER_H
#define WATER_H

#pragma once
#include "camera.h"
#include "framebuffer.h"
#include "frametime.h"
#include "math.h"
#include "plane.h"
#include "shader.h"
#include "shader_handler.h"
#include "tileable_noise.h"
#include "texture.h"
#include "traits.h"
#include <cmath>
#include <cstddef>
#include <GL/glew.h>
#include <memory>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

template <typename ShaderPolicy = manual_shader_handler>
class Water : public Plane<ShaderPolicy> {
    using plane_t = Plane<ShaderPolicy>;
    using ReflFb = Framebuffer<1u>;
    using RefrFb = Framebuffer<1u, TexType::Color | TexType::Depth>;
    static GLuint constexpr TEXTURE_SIZE{128};
    using image_t = std::array<std::array<unsigned char, 3u * TEXTURE_SIZE>, 3u * TEXTURE_SIZE>;


    public:
        Water(std::shared_ptr<Shader> const& shader,
              std::shared_ptr<Camera> const& cam,
              float terrain_height,
              ShaderPolicy policy = {});

        template <typename SceneRenderer, typename... Shaders>
        void prepare(SceneRenderer renderer, Shaders const&... shaders);

        void render();
        void translate(glm::vec3 direction);

    private:
        std::shared_ptr<Shader> shader_;
        std::shared_ptr<Camera> camera_;
        ReflFb refl_fb_;
        RefrFb refr_fb_;
        Texture dudv_{}, normal_{};
        float terrain_height_, clip_height_{}, dudv_offset_{};
        glm::vec4 refr_clip_, refl_clip_;

        static GLfloat constexpr WAVE_SPEED{0.02f};
        static glm::vec4 const NO_CLIP;
        static float constexpr NOISE_FREQUENCY{1.f/32.f};
        static image_t map_data_;
    
        using plane_t::render;       /* Force private */
        using plane_t::translate;    /* Force private */
    
        void init();

        template <typename Uniform, typename Tuple, std::size_t... Is>
        void upload_to_shaders(std::string const& name, Uniform const& ufrm, 
                               Tuple const& shaders, std::index_sequence<Is...>);

        static image_t generate_map_data();
        Texture dudv_map();
        Texture normal_map();
};

#include "water.tcc"
#endif
