#include "post_processing.h"
#include "texture.h"
#include "viewport.h"
PostProcessing::PostProcessing() : h_blur_{Viewport::width / 16, Viewport::height / 16}, v_blur_{Viewport::width / 16, Viewport::height / 16}, bloom_{Viewport::width, Viewport::height} { 
    instantiated_ = true;
}

void PostProcessing::perform() const {
    canvas_.bind();
    auto scene = Shader::scene_texture();
    bloom_.apply(scene);
    canvas_.draw();

    auto to_blur = bloom_.texture_ids()[1];
    for(auto i = 0u; i < NUM_BLUR_PASSES; i++) {
        h_blur_.apply(to_blur);
        canvas_.draw();

        v_blur_.apply(h_blur_.texture_id());
        canvas_.draw();

        to_blur = v_blur_.texture_id();
    }
    mix_.apply(bloom_.texture_ids()[0], v_blur_.texture_id());
    canvas_.draw();
    canvas_.unbind();

    Texture::bind(mix_.texture_id());
}

bool PostProcessing::enabled() {
    return instantiated_;
}

bool PostProcessing::instantiated_{false};
