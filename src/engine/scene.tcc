template <typename ShaderPolicy>
Scene<ShaderPolicy>::Scene(Color clear_color) : color_{clear_color} { }

template <typename ShaderPolicy>
Scene<ShaderPolicy>::Scene(ShaderPolicy policy, Color clear_color) : canvas_{policy}, color_{clear_color} { }


template <typename ShaderPolicy>
void Scene<ShaderPolicy>::render() const {
    if(!PostProcessing::enabled())
        Shader::bind_scene_texture();
	Shader::bind_default_framebuffer();
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

    canvas_.render();

	Shader::bind_main_framebuffer();
	glClearColor(color_.r, color_.g, color_.b, color_.a);
	glEnable(GL_DEPTH_TEST);
}
