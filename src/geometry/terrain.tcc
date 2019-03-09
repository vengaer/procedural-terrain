template <typename ShaderPolicy>
Terrain<ShaderPolicy>::Terrain(ShaderPolicy policy, GLfloat amplitude, GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz) : renderer_t{policy}, generator_{amplitude} {
    if constexpr(renderer_t::template policy_is_automatic<ShaderPolicy>(renderer_t::OVERLOAD_RESOLVER))
        policy.shader()->upload_uniform("ufrm_terrain_amplitude", amplitude);

    renderer_t::init(x_len, dx, z_len, dz);
}

template <typename ShaderPolicy>
void Terrain<ShaderPolicy>::init(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz) {
	x_len = glm::clamp(x_len, 0.05f, 10.f);
	z_len = glm::clamp(z_len, 0.05f, 10.f);
	dx    = glm::clamp(dx, .01f, x_len);
	dz    = glm::clamp(dz, .01f, z_len);

	GLuint x_iters = static_cast<GLuint>(x_len / dx) + 1u;
	GLuint z_iters = static_cast<GLuint>(z_len / dz) + 1u;

	auto constexpr VERTEX_SIZE = renderer_t::VERTEX_SIZE;

	vertices_.reserve(VERTEX_SIZE*x_iters*z_iters);
	
	{
		GLfloat x_start = -static_cast<GLfloat>(x_len/2);
		GLfloat x = x_start;
		GLfloat z = -static_cast<GLfloat>(z_len/2);
		GLfloat s ,t;
		std::array<GLfloat, VERTEX_SIZE> vertex;
		for(auto i = 0u; i < z_iters; i++, z += dz, x = x_start){
			t = interpolation::linear(static_cast<GLfloat>(i)/static_cast<GLfloat>(z_iters-1));

			for(auto j = 0u; j < x_iters; j++, x += dx){
				s = interpolation::linear(static_cast<GLfloat>(j)/static_cast<GLfloat>(x_iters-1));
                auto normal = calculate_normal(j,i);

				vertex[0] = x;   /* Position */
				vertex[1] = generator_.generate(j,i);
				vertex[2] = z;
				vertex[3] = normal.x; /* Normal */
				vertex[4] = normal.y;	
				vertex[5] = normal.z;
				vertex[6] = s;   /* Texture */
				vertex[7] = t;
			
				vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
			}
		} 
	}

	indices_.reserve(3*2*(x_iters-1)*(z_iters-1));
	std::array<GLuint, 3> triangle_indices;
	
	auto idx = 0u;
	for(auto i = 0u; i < z_iters-1; idx++, i++){
		for(auto j = 0u; j < 2*(x_iters-1); j++){
			triangle_indices[0] = idx;
			if(j % 2){
				triangle_indices[1] = idx + x_iters;
				triangle_indices[2] = idx + x_iters - 1;
			}
			else{
				triangle_indices[1] = idx + 1;
				triangle_indices[2] = idx + x_iters;
                idx++;
			}
			
			indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
		}
	}
}

template <typename ShaderPolicy>
std::vector<GLfloat> const& Terrain<ShaderPolicy>::vertices() const {
    return vertices_;
}

template <typename ShaderPolicy>
std::vector<GLuint> const& Terrain<ShaderPolicy>::indices() const {
    return indices_;
}

template <typename ShaderPolicy>
glm::vec3 Terrain<ShaderPolicy>::calculate_normal(int x, int z) {
    float height_left  = generator_.generate(x-1, z);
    float height_right = generator_.generate(x+1, z);
    float height_up    = generator_.generate(x, z+1);
    float height_down  = generator_.generate(x, z-1);
    glm::vec3 normal = {height_left - height_right, 
                        2.f, 
                        height_down - height_up};

    return glm::normalize(normal);
}
