
template <std::size_t N>
Framebuffer<N>::Framebuffer(float width_ratio, float height_ratio) : width_ratio_{width_ratio}, height_ratio_{height_ratio} {
    static_assert(N > 0u, "Must request at least one texture");
    init();
}

template <std::size_t N>
Framebuffer<N>::~Framebuffer() {
    glDeleteFramebuffers(1, &fbo_);
    delete_textures();

	instances_.erase(
		std::remove_if(std::begin(instances_), 
					   std::end(instances_), 
					   [this](auto ref_wrapper) {
			return std::addressof(ref_wrapper.get()) == this;
		}));
}

template <std::size_t N>
template <std::size_t M, typename>
GLuint Framebuffer<N>::texture() const {
    return textures_[0];
}

template <std::size_t N>
template <std::size_t M, typename>
std::array<GLuint, N> Framebuffer<N>::textures() const {
    return textures_;
}

template <std::size_t N>
void Framebuffer<N>::bind() const {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    glDrawBuffers(N, &color_attachments_[0]);
}

template <std::size_t N>
void Framebuffer<N>::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

template <std::size_t N>
void Framebuffer<N>::reallocate() {
    glBindTexture(GL_TEXTURE_2D, 0);
    for(auto fb : instances_) {
        fb.get().delete_textures();
        fb.get().setup_texture_environment();
    }
}

template <std::size_t N>
void Framebuffer<N>::init() {
    if(instances_.size() == 0u) {
        for(auto i = 0u; i < N; i++)
            color_attachments_[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    instances_.push_back(std::ref(*this));

    glGenFramebuffers(1, &fbo_);
    setup_texture_environment();
}

template <std::size_t N>
void Framebuffer<N>::setup_texture_environment() {
    bind();

    glGenTextures(N, &textures_[0]);
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

    }
    glBindTexture(GL_TEXTURE_2D, 0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};
    unbind();
}

template <std::size_t N>
void Framebuffer<N>::delete_textures() {
    glDeleteTextures(N, &textures_[0]);
}

template <std::size_t N>
std::vector<std::reference_wrapper<Framebuffer<N>>> Framebuffer<N>::instances_{};

template <std::size_t N>
std::array<GLuint, N> Framebuffer<N>::color_attachments_{};
