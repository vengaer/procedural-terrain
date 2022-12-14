template <typename ShaderPolicy>
Plane<ShaderPolicy>::Plane(ShaderPolicy policy, GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz) : renderer_t{policy}, Transform{}, vertices_{}, indices_{} {
	/* Initialize Renderer, pass parameters */
	renderer_t::init(x_len, dx, z_len, dz);
}

template <typename ShaderPolicy>
void Plane<ShaderPolicy>::init(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz){
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

				vertex[0] = x;   /* Position */
				vertex[1] = 0.f;
				vertex[2] = z;
				vertex[3] = 0.f; /* Normal */
				vertex[4] = 1.f;	
				vertex[5] = 0.f;
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
std::vector<GLfloat> const& Plane<ShaderPolicy>::vertices() const{
	return vertices_;
}

template <typename ShaderPolicy>
std::vector<GLuint> const& Plane<ShaderPolicy>::indices() const {
	return indices_;
}
