#include "bloom.h"
#include "texture.h"
#include "viewport.h"

Bloom::Bloom(int width, int height) : shader_{"assets/shaders/bloom.vert", Shader::Type::Vertex, "assets/shaders/bloom.frag", Shader::Type::Fragment}, fb_{static_cast<float>(width) / static_cast<float>(Viewport::width), static_cast<float>(height) / static_cast<float>(Viewport::height)} { } 

void Bloom::apply(GLuint texture) const {
    Texture::bind(texture);
    fb_.bind();
    shader_.enable();

    /* Rendering a mesh to both FBOs may cause issues with
     * down/upsampling. This is used to hide the mesh in the 
     * first render target */
    glGetFloatv(GL_COLOR_CLEAR_VALUE, &clear_color_[0]);
    shader_.upload_uniform(CLEAR_COLOR_UNIFORM_NAME, clear_color_);
}

std::array<GLuint, 2> Bloom::texture_ids() const {
    return fb_.texture_ids();
}

std::string const Bloom::CLEAR_COLOR_UNIFORM_NAME{"clear_col"};
