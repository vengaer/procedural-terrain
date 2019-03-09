#include "mix.h"
#include "viewport.h"

Mix::Mix() : shader_{"assets/shaders/mix.vert", Shader::Type::Vertex, "assets/shaders/mix.frag", Shader::Type::Fragment} { 
    setup_texture_environment();
}

Mix::~Mix() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &texture_);
}

void Mix::apply(GLuint texture0, GLuint texture1) const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    shader_.enable();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture1);
}

GLuint Mix::texture() const {
    return texture_;
}

void Mix::setup_texture_environment() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA16F,
                 Viewport::width,
                 Viewport::height,
                 0,
                 GL_RGB,
                 GL_FLOAT,
                 nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_2D, 0);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           texture_,
                           0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    shader_.upload_uniform("scene_", 0);
    shader_.upload_uniform("highlight_", 1);
}
