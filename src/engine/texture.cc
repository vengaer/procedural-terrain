#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif
#include "exception.h"
#include "texture.h"

Texture::Texture(std::string const& path) {
    init(path);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture_);
    stbi_image_free(data_);
}

void Texture::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

void Texture::bind(int unit) const {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

GLuint Texture::id() const {
    return texture_;
}

void Texture::init(std::string const& path) {
    data_ = stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
    if(!data_)
        throw TextureLoadingException{"Failed to init texture " + path};

    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width_,
                 height_,
                 0, 
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data_);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}
