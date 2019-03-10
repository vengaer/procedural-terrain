#include "mix.h"
#include "viewport.h"

Mix::Mix() : shader_{"assets/shaders/mix.vert", Shader::Type::Vertex, "assets/shaders/mix.frag", Shader::Type::Fragment}, fb_{Framebuffer::FULL_WIDTH, Framebuffer::FULL_HEIGHT} { 
    init();
}

void Mix::apply(GLuint texture0, GLuint texture1) const {
    fb_.bind();

    shader_.enable();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
}

GLuint Mix::texture() const {
    return fb_.texture();
}

void Mix::init() {
    shader_.upload_uniform("scene_", 0);
    shader_.upload_uniform("highlight_", 1);
}
