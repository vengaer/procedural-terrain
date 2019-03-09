#ifndef SCENE_H
#define SCENE_H

#pragma once
#include "canvas.h"
#include "color_type.h"
#include "post_processing.h"

template <typename ShaderPolicy = manual_shader_handler>
class Scene {
    public:
		using Color = ColorType<float>;
		Scene(Color clear_color = Color{1.f, 1.f, 1.f, 1.f});
		Scene(ShaderPolicy policy, Color clear_color = Color{1.f, 1.f, 1.f, 1.f});

        void render() const;
    

    private:
        Canvas<ShaderPolicy> canvas_;
        Color color_;

};

#include "scene.tcc"
#endif
