#include "post_processing.h"
#include "viewport.h"
PostProcessing::PostProcessing() : h_blur_{Viewport::width, Viewport::height}, v_blur_{Viewport::width, Viewport::height}, bloom_{Viewport::width, Viewport::height} { 
    instantiated_ = true;
}

void PostProcessing::perform() const {
    auto scene = Shader::scene_texture();

    bloom_.apply(scene);
    canvas_.render();
    h_blur_.apply(bloom_.textures()[1]);
    canvas_.render();
    v_blur_.apply(h_blur_.texture());
    canvas_.render();
    mix_.apply(bloom_.textures()[0], v_blur_.texture());
    canvas_.render();

    glBindTexture(GL_TEXTURE_2D, mix_.texture());
}

bool PostProcessing::enabled() {
    return instantiated_;
}

bool PostProcessing::instantiated_{false};
