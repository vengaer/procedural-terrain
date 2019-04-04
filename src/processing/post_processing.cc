#include "post_processing.h"
#include "texture.h"
#include "viewport.h"
PostProcessing::PostProcessing() : h_blur_{Viewport::width / 16, Viewport::height / 16}, v_blur_{Viewport::width / 16, Viewport::height / 16}, bloom_{Viewport::width, Viewport::height} { 
    instantiated_ = true;
}

void PostProcessing::perform() const {
    auto scene = Shader::scene_texture();
    bloom_.apply(scene);
    canvas_.render();

    auto to_blur = bloom_.textures()[1];
    for(auto i = 0u; i < NUM_BLUR_PASSES; i++) {
        h_blur_.apply(to_blur);
        canvas_.render();
        v_blur_.apply(h_blur_.texture());
        canvas_.render();
        to_blur = v_blur_.texture();
    }
    mix_.apply(bloom_.textures()[0], v_blur_.texture());
    canvas_.render();

    Texture::bind(mix_.texture());
}

bool PostProcessing::enabled() {
    return instantiated_;
}

bool PostProcessing::instantiated_{false};
