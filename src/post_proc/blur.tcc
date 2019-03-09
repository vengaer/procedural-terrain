
template <impl::Direction dir>
impl::Blur<dir>::Blur(int width, int height) : shader_{"assets/shaders/blur.vert", Shader::Type::Vertex, "assets/shaders/blur.frag", Shader::Type::Fragment}, width_{width}, height_{height} { 
    init();
}

template <impl::Direction dir>
impl::Blur<dir>::~Blur() {
    glDeleteFramebuffers(1, &fbo_);
    glDeleteTextures(1, &texture_);
}

template <impl::Direction dir>
void impl::Blur<dir>::apply(GLuint texture) const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    shader_.enable();
    glClear(GL_COLOR_BUFFER_BIT);
}

template <impl::Direction dir>
void impl::Blur<dir>::bind() const {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_);
}

template <impl::Direction dir>
GLuint impl::Blur<dir>::texture() const {
    return texture_;
}

template <impl::Direction dir>
void impl::Blur<dir>::init() {
    setup_texture_environment();

    if constexpr(dir == Direction::Vertical) {
        shader_.upload_uniform("ufrm_direction", VERTICAL);
        shader_.upload_uniform("ufrm_dim", static_cast<float>(height_));
    }
    else {
        shader_.upload_uniform("ufrm_direction", HORIZONTAL);
        shader_.upload_uniform("ufrm_dim", static_cast<float>(width_));
    }
}

template <impl::Direction dir>
void impl::Blur<dir>::setup_texture_environment() {
    glGenFramebuffers(1, &fbo_);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
    
    glGenTextures(1, &texture_);
    glBindTexture(GL_TEXTURE_2D, texture_);

	glTexImage2D(GL_TEXTURE_2D, 
				 0, 
				 GL_RGBA16F, 
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
	
	glBindTexture(GL_TEXTURE_2D, 0);

	glFramebufferTexture2D(GL_FRAMEBUFFER, 
						   GL_COLOR_ATTACHMENT0,
						   GL_TEXTURE_2D, 
						   texture_,
						   0);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw FramebufferException{"Generated framebuffer not complete"};
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
