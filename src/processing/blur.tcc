
template <impl::Direction dir>
impl::Blur<dir>::Blur(int width, int height) : shader_{"assets/shaders/blur.vert", Shader::Type::Vertex, "assets/shaders/blur.frag", Shader::Type::Fragment}, width_ratio_{static_cast<float>(width) / static_cast<float>(Viewport::width)}, height_ratio_{static_cast<float>(height) / static_cast<float>(Viewport::height)}, fb_{static_cast<float>(width) / static_cast<float>(Viewport::width), static_cast<float>(height) / static_cast<float>(Viewport::height)} { 
    init();
}

template <impl::Direction dir>
impl::Blur<dir>::~Blur() {
    instances_.erase(
        std::remove_if(std::begin(instances_),
                       std::end(instances_),
                       [this](auto ref_wrapper) {
            return std::addressof(ref_wrapper.get()) == this;
        }));

}

template <impl::Direction dir>
void impl::Blur<dir>::apply(GLuint texture) const {
    Texture::bind(texture);
    fb_.bind();
    shader_.enable();
}

template <impl::Direction dir>
GLuint impl::Blur<dir>::texture_id() const {
    return fb_.texture_id();
}

template <impl::Direction dir>
void impl::Blur<dir>::update_size() {
    for(auto const& i : instances_)
        i.get().resize();
}

template <impl::Direction dir>
void impl::Blur<dir>::init() {
    instances_.push_back(std::ref(*this));

    if constexpr(dir == Direction::Vertical) {
        shader_.upload_uniform("ufrm_direction", VERTICAL);
        shader_.upload_uniform("ufrm_dim", height_ratio_ * static_cast<float>(Viewport::height));
    }
    else {
        shader_.upload_uniform("ufrm_direction", HORIZONTAL);
        shader_.upload_uniform("ufrm_dim", width_ratio_ * static_cast<float>(Viewport::width));
    }
    float x_scale = 1.f / width_ratio_;
    float y_scale = 1.f / height_ratio_;
    shader_.upload_uniform("ufrm_x_scale", x_scale);
    shader_.upload_uniform("ufrm_y_scale", y_scale);
}

template <impl::Direction dir>
void impl::Blur<dir>::resize() const {
    if constexpr(dir == Direction::Vertical) 
        shader_.upload_uniform("ufrm_dim", height_ratio_ * static_cast<float>(Viewport::height));
    else 
        shader_.upload_uniform("ufrm_dim", width_ratio_ * static_cast<float>(Viewport::width));
}

template <impl::Direction dir>
std::vector<std::reference_wrapper<impl::Blur<dir>>> impl::Blur<dir>::instances_{};

