#include "bloom.h"

Bloom::Bloom(int width, int height) : width_{width}, height_{height}, shader_{"assets/shaders/bloom.vert", Shader::Type::Vertex, "assets/shaders/bloom.frag", Shader::Type::Fragment} {
    setup_texture_environment();
}

Bloom::~Bloom() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(textures_.size(), &textures_[0]);
}

void Bloom::apply(GLuint texture) const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glDrawBuffers(2, &attachments_[0]);
    shader_.enable();
    glClear(GL_COLOR_BUFFER_BIT);
}

std::array<GLuint, 2> Bloom::textures() const {
    return textures_;
}

void Bloom::setup_texture_environment() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

    glGenTextures(textures_.size(), &textures_[0]);

    for(auto i = 0u; i < textures_.size(); i++) {
        glBindTexture(GL_TEXTURE_2D, textures_[i]);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGB16F,
                     width_,
                     height_,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     nullptr);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               GL_COLOR_ATTACHMENT0 + i,
                               GL_TEXTURE_2D,
                               textures_[i],
                               0);
    }
    glBindTexture(GL_TEXTURE_2D, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
