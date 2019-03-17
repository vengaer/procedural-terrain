template <typename ShaderPolicy>
Water<ShaderPolicy>::Water(std::shared_ptr<Shader> const& shader,
                           std::shared_ptr<Camera> const& cam,
                           float terrain_height,
                           ShaderPolicy policy)
: plane_t{policy}, shader_{shader}, camera_{cam}, 
  refl_fb_{ReflFb::FULL_WIDTH, ReflFb::FULL_HEIGHT},
  refr_fb_{RefrFb::FULL_WIDTH, RefrFb::FULL_HEIGHT},
  dudv_{DUDV_MAP}, normal_{NORMAL_MAP},
  terrain_height_{terrain_height},
  refr_clip_{0.f, -1.f, 0.f, 1.f},
  refl_clip_{0.f, 1.f, 0.f, -1.f} {
    init();
}

template <typename ShaderPolicy>
template <typename SceneRenderer, typename... Shaders>
void Water<ShaderPolicy>::prepare(SceneRenderer renderer, Shaders const&... shaders) {
    static_assert(is_trivially_callable_v<SceneRenderer>, "SceneRenderer must be a functor");
    static_assert((std::is_same_v<remove_cvref_t<Shaders>, std::shared_ptr<Shader>> && ...), 
                  "Parameter pack may only contain variablers of type shared_ptr<Shader>");

    glEnable(GL_CLIP_DISTANCE0);

    dudv_offset_ += WAVE_SPEED * frametime::delta();
    dudv_offset_ = std::fmod(dudv_offset_, 1.f);

    shader_->upload_uniform("ufrm_dudv_offset", dudv_offset_);
    shader_->upload_uniform("ufrm_camera_pos", camera_->position());

    /* Reflection pass */
    refl_fb_.bind();
    
    upload_to_shaders("ufrm_clipping_plane",
                      refl_clip_,
                      std::forward_as_tuple(shaders...),
                      std::index_sequence_for<Shaders...>{});

    auto const pos = camera_->position();
    float const dist = 2.f * (pos.y - this->position().y);
    
    camera_->invert_pitch();
    camera_->set_position(glm::vec3{pos.x, pos.y - dist, pos.z});

    renderer();

    camera_->set_position(pos);
    camera_->invert_pitch();

    /* Refraction pass */
    refr_fb_.bind();

    upload_to_shaders("ufrm_clipping_plane",
                      refr_clip_,
                      std::forward_as_tuple(shaders...),
                      std::index_sequence_for<Shaders...>{});

    renderer();

    /* In case of driver issues */
    upload_to_shaders("ufrm_clipping_plane",
                      no_clip_,
                      std::forward_as_tuple(shaders...),
                      std::index_sequence_for<Shaders...>{});

    glDisable(GL_CLIP_DISTANCE0);
}

template <typename ShaderPolicy>
void Water<ShaderPolicy>::render() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, refl_fb_.texture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, refr_fb_.texture());
    dudv_.bind(2);
    normal_.bind(3);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, refr_fb_.depth_texture());
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    plane_t::render();
    glDisable(GL_BLEND);
}

template <typename ShaderPolicy>
void Water<ShaderPolicy>::translate(glm::vec3 direction) {
    plane_t::translate(direction);
    clip_height_ = this->position().y - terrain_height_;
    refr_clip_.w = clip_height_ + 1.f;
    refl_clip_.w = -clip_height_;
}

template <typename ShaderPolicy>
void Water<ShaderPolicy>::init() {
    shader_->upload_uniform("refl_texture", 0);
    shader_->upload_uniform("refr_texture", 1);
    shader_->upload_uniform("dudv_map", 2);
    shader_->upload_uniform("normal_map", 3);
    shader_->upload_uniform("depth_map", 4);
    
    clip_height_ = this->position().y - terrain_height_;
    refr_clip_.w = clip_height_ + 1.f;
    refl_clip_.w = -clip_height_;
}

template <typename ShaderPolicy>
template <typename Uniform, typename Tuple, std::size_t... Is>
void Water<ShaderPolicy>::upload_to_shaders(std::string const& name, Uniform const& ufrm, Tuple const& shaders, std::index_sequence<Is...>) {
    (std::get<Is>(shaders)->upload_uniform(name, ufrm), ...);
}

template <typename ShaderPolicy>
std::string const Water<ShaderPolicy>::DUDV_MAP{"assets/textures/waterDUDV.png"};

template <typename ShaderPolicy>
std::string const Water<ShaderPolicy>::NORMAL_MAP{"assets/textures/normal.png"};

template <typename ShaderPolicy>
GLfloat const Water<ShaderPolicy>::WAVE_SPEED{0.03f};

template <typename ShaderPolicy>
glm::vec4 const Water<ShaderPolicy>::no_clip_{0.f, -1.f, 0.f, 10'000};
