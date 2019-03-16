#ifndef WATER_H
#define WATER_H

#pragma once
#include "camera.h"
#include "framebuffer.h"
#include "frametime.h"
#include "plane.h"
#include "shader.h"
#include "shader_handler.h"
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
        Texture dudv_, normal_;
        float terrain_height_, clip_height_{}, dudv_offset_{};
        glm::vec4 refr_clip_, refl_clip_;

        static std::string const DUDV_MAP;
        static std::string const NORMAL_MAP;
        static GLfloat const WAVE_SPEED;
        static glm::vec4 const no_clip_;
    
        using plane_t::render;       /* Force private */
        using plane_t::translate;    /* Force private */
    
        void init();

        template <typename Uniform, typename Tuple, std::size_t... Is>
        void upload_to_shaders(std::string const& name, Uniform const& ufrm, 
                               Tuple const& shaders, std::index_sequence<Is...>);
};

#include "water.tcc"
#endif
