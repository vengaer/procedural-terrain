#include "exception.h"
#include "texture.h"
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <cmath>
#include "stb_image.h"
#endif

Texture::Texture(GLuint tex_id) : id_{tex_id} {
    glBindTexture(GL_TEXTURE_2D, id_);
    int width, height;
    int miplevel = 0;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_WIDTH, &width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, miplevel, GL_TEXTURE_HEIGHT, &height);
    width_ = width;
    height_ = height;
}

Texture::Texture(std::string const& path) {
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if(!data)
        throw TextureLoadingException{"Failed to load texture"};
    width_ = width;
    height_ = height;
    init(data);
    stbi_image_free(data);
}

Texture::Texture(unsigned char* data, std::size_t width, std::size_t height) : width_{width}, height_{height} {
    init(data);
}

Texture::~Texture() {
    glDeleteTextures(1, &id_);
}

void Texture::bind(GLuint id, std::size_t unit) {
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, id);
}

GLuint Texture::id() const {
    return id_;
}

std::size_t Texture::width() const {
    return width_;
}

std::size_t Texture::height() const {
    return height_;
}

void Texture::init(unsigned char* data) {

    glGenTextures(1, &id_);
    glBindTexture(GL_TEXTURE_2D, id_);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 width_,
                 height_,
                 0, 
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, 0);
}
