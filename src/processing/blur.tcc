
template <impl::Direction dir>
impl::Blur<dir>::Blur(int width, int height) : shader_{"assets/shaders/blur.vert", Shader::Type::Vertex, "assets/shaders/blur.frag", Shader::Type::Fragment}, width_{width}, height_{height}, fb_{static_cast<float>(width) / static_cast<float>(Viewport::width), static_cast<float>(height) / static_cast<float>(Viewport::height)} { 
    init();
}

template <impl::Direction dir>
void impl::Blur<dir>::apply(GLuint texture) const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    fb_.bind();
    shader_.enable();
}

template <impl::Direction dir>
GLuint impl::Blur<dir>::texture() const {
    return fb_.texture();
}

template <impl::Direction dir>
void impl::Blur<dir>::init() {

    if constexpr(dir == Direction::Vertical) {
        shader_.upload_uniform("ufrm_direction", VERTICAL);
        shader_.upload_uniform("ufrm_dim", static_cast<float>(height_));
    }
    else {
        shader_.upload_uniform("ufrm_direction", HORIZONTAL);
        shader_.upload_uniform("ufrm_dim", static_cast<float>(width_));
    }
    float x_scale = static_cast<float>(Viewport::width) / static_cast<float>(width_);
    float y_scale = static_cast<float>(Viewport::height) / static_cast<float>(height_);
    shader_.upload_uniform("ufrm_x_scale", x_scale);
    shader_.upload_uniform("ufrm_y_scale", y_scale);
}

