template <typename ShaderPolicy>
Water<ShaderPolicy>::Water(std::shared_ptr<Shader> const& shader,
                           std::shared_ptr<Camera> const& cam,
                           float terrain_height,
                           ShaderPolicy policy)
: plane_t{policy}, shader_{shader}, camera_{cam}, 
  refl_fb_{ReflFb::FULL_WIDTH, ReflFb::FULL_HEIGHT},
  refr_fb_{RefrFb::FULL_WIDTH, RefrFb::FULL_HEIGHT},
  dudv_{dudv_map()}, normal_{normal_map()},
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

    using namespace math;

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

    auto const cam_pos = camera_->position();
    float const vertical_dist = 2.f * (cam_pos.y - this->position().y);
    
    camera_->invert_pitch();
    camera_->set_position(glm::vec3{cam_pos.x, cam_pos.y - vertical_dist, cam_pos.z});

    renderer();

    camera_->set_position(cam_pos);
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
                      NO_CLIP,
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
typename Water<ShaderPolicy>::image_t Water<ShaderPolicy>::generate_map_data() {
    std::size_t constexpr period = 128;
    std::size_t constexpr size = 3u * TEXTURE_SIZE;
    image_t data;

    for(auto i = 0u; i < size; i++) {
        for(auto j = 0u; j < size; j += 3u) {
            float fi = static_cast<float>(i) * NOISE_FREQUENCY;
            float fj = static_cast<float>(j) * NOISE_FREQUENCY;

            data[i][j]   = static_cast<unsigned char>((tileable_noise::generate(4.f*fj, 4.f*fi, period) + 1.f) * 0.5f * 255);
            data[i][j+1] = static_cast<unsigned char>((tileable_noise::generate(2.f*fj, 8.f*fi, period) + 1.f) * 0.5f * 255);
            data[i][j+2] = 0u;
        }
    }

    return data;
}

template <typename ShaderPolicy>
Texture Water<ShaderPolicy>::dudv_map() {
    return Texture{&map_data_[0][0], TEXTURE_SIZE, TEXTURE_SIZE};
}

template <typename ShaderPolicy>
Texture Water<ShaderPolicy>::normal_map() {
    std::size_t constexpr size = 3u * TEXTURE_SIZE;
    image_t data;

    /* Invert dudv map and maximize brightness */
    for(auto i = 0u; i < size; i++) {
        for(auto j = 0u; j < size; j += 3u) {
            auto r = 255 - map_data_[i][j];
            auto g = 255 - map_data_[i][j+1];
            auto b = 255 - map_data_[i][j+2];

            auto max = math::max(r, g, b);
            float scale_factor = 255.f / static_cast<float>(max);

            data[i][j]   = static_cast<unsigned char>(static_cast<float>(r) * scale_factor);
            data[i][j+1] = static_cast<unsigned char>(static_cast<float>(g) * scale_factor);
            data[i][j+2] = static_cast<unsigned char>(static_cast<float>(b) * scale_factor);
        }
    }

    return Texture{&data[0][0], TEXTURE_SIZE, TEXTURE_SIZE};
}

template <typename ShaderPolicy>
glm::vec4 const Water<ShaderPolicy>::NO_CLIP{0.f, -1.f, 0.f, 10'000};

template <typename ShaderPolicy>
typename Water<ShaderPolicy>::image_t Water<ShaderPolicy>::map_data_ = generate_map_data();
