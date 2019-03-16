
template <std::size_t N, std::size_t T>
Framebuffer<N, T>::Framebuffer(float width_ratio, float height_ratio) : width_ratio_{width_ratio}, height_ratio_{height_ratio} {
    static_assert(N > 0u, "Cannot create framebuffer without attached texture");
    static_assert(T == TexType::Color ||
                  T == TexType::Depth ||
                  T == (TexType::Color | TexType::Depth),
                  "Texture type must be either Color, Depth or both");
    init();
}

template <std::size_t N, std::size_t T>
Framebuffer<N, T>::~Framebuffer() {
    glDeleteFramebuffers(1, &fbo_);
    if constexpr(has_color_attachment()) {
        glDeleteTextures(N, &textures_[0]);
        glDeleteRenderbuffers(N, &rbos_[0]);
    }

    if constexpr(has_depth_attachment())
        glDeleteTextures(N, &depth_textures_[0]);

	instances_.erase(
		std::remove_if(std::begin(instances_), 
					   std::end(instances_), 
					   [this](auto ref_wrapper) {
			return std::addressof(ref_wrapper.get()) == this;
		}));
}

template <std::size_t N, std::size_t T>
template <std::size_t, typename>
GLuint Framebuffer<N, T>::texture() const {
    return textures_[0];
}

template <std::size_t N, std::size_t T>
template <std::size_t, typename>
std::array<GLuint, N> Framebuffer<N, T>::textures() const {
    return textures_;
}
template <std::size_t N, std::size_t T>
template <std::size_t, typename>
GLuint Framebuffer<N,T>::depth_texture() const {
    return depth_textures_[0];
}

template <std::size_t N, std::size_t T>
template <std::size_t, typename>
std::array<GLuint, N> Framebuffer<N,T>::depth_textures() const{
    return depth_textures_;
}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glDrawBuffers(N, &color_attachments_[0]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::reallocate() {
    glBindTexture(GL_TEXTURE_2D, 0);
    for(auto fb : instances_)
        fb.get().resize();
}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::resize() {
    for(auto [texture, rbo] : zip(textures_, rbos_)) {
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA16F,
                     Viewport::width * width_ratio_,
                     Viewport::height * height_ratio_,
                     0,
                     GL_RGB,
                     GL_FLOAT,
                     nullptr);

        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER,
                              GL_DEPTH_COMPONENT,
                              Viewport::width * width_ratio_,
                              Viewport::height * height_ratio_);
    
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::init() {
    if(instances_.size() == 0u) {
        for(auto i = 0u; i < N; i++)
            color_attachments_[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    instances_.push_back(std::ref(*this));

    glGenFramebuffers(1, &fbo_);
    setup_texture_environment();
}

template <std::size_t N, std::size_t T>
void Framebuffer<N, T>::setup_texture_environment() {
    bind();

    if constexpr(has_color_attachment()) {
        glGenTextures(N, &textures_[0]);
        glGenRenderbuffers(N, &rbos_[0]);

        for(auto i = 0u; i < N; i++) {
            glBindTexture(GL_TEXTURE_2D, textures_[i]);

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_RGBA16F,
                         Viewport::width * width_ratio_,
                         Viewport::height * height_ratio_,
                         0,
                         GL_RGB,
                         GL_FLOAT,
                         nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   color_attachments_[i],
                                   GL_TEXTURE_2D,
                                   textures_[i],
                                   0);

            glBindRenderbuffer(GL_RENDERBUFFER, rbos_[i]);
            glRenderbufferStorage(GL_RENDERBUFFER,
                                  GL_DEPTH_COMPONENT,
                                  Viewport::width * width_ratio_,
                                  Viewport::height * height_ratio_);
        
            glBindRenderbuffer(GL_RENDERBUFFER, 0);

            glFramebufferRenderbuffer(GL_FRAMEBUFFER,
                                      GL_DEPTH_ATTACHMENT,
                                      GL_RENDERBUFFER,
                                      rbos_[i]);
        }
    }
    if constexpr(has_depth_attachment()) {
        glGenTextures(N, &depth_textures_[0]);
        for(auto i = 0u; i < N; i++) {
            glBindTexture(GL_TEXTURE_2D, depth_textures_[i]);

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         GL_DEPTH_COMPONENT32,
                         Viewport::width * width_ratio_,
                         Viewport::height * height_ratio_,
                         0,
                         GL_DEPTH_COMPONENT,
                         GL_FLOAT,
                         nullptr);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


            glFramebufferTexture(GL_FRAMEBUFFER,
                                 GL_DEPTH_ATTACHMENT,
                                 depth_textures_[i],
                                 0);

        }
    }

    glBindTexture(GL_TEXTURE_2D, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};
    unbind();
}

template <std::size_t N, std::size_t T>
bool constexpr Framebuffer<N,T>::has_color_attachment() {
    return has_color_attachment_v<T>;
}

template <std::size_t N, std::size_t T>
bool constexpr Framebuffer<N,T>::has_depth_attachment() {
    return has_depth_attachment_v<T>;
}

template <std::size_t N, std::size_t T>
std::vector<std::reference_wrapper<Framebuffer<N, T>>> Framebuffer<N, T>::instances_{};

template <std::size_t N, std::size_t T>
std::array<GLuint, N> Framebuffer<N, T>::color_attachments_{};
