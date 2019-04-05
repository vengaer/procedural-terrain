#include "bloom.h"
#include "texture.h"
#include "viewport.h"

Bloom::Bloom(int width, int height) : shader_{"assets/shaders/bloom.vert", Shader::Type::Vertex, "assets/shaders/bloom.frag", Shader::Type::Fragment}, fb_{static_cast<float>(width) / static_cast<float>(Viewport::width), static_cast<float>(height) / static_cast<float>(Viewport::height)} { } 

void Bloom::apply(GLuint texture) const {
    Texture::bind(texture);
    fb_.bind();
    shader_.enable();
}

std::array<GLuint, 2> Bloom::texture_ids() const {
    return fb_.texture_ids();
}
