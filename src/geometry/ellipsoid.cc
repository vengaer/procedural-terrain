#include "ellipsoid.h"

Ellipsoid::Ellipsoid(GLuint vertical_segments, GLuint horizontal_segments, GLfloat x_scale, GLfloat y_scale, GLfloat z_scale) : Renderer{}, vertices_{}, indices_{} {
	Renderer<Ellipsoid>::init(vertical_segments, horizontal_segments, x_scale, y_scale, z_scale);
}

void Ellipsoid::init(GLuint vertical_segments, GLuint horizontal_segments, GLfloat x_scale, GLfloat y_scale, GLfloat z_scale) {
	/* Prevent potential overflow */
	glm::clamp(vertical_segments, 5u, 30u);
	glm::clamp(horizontal_segments, 5u, 30u);
	glm::clamp(x_scale, 0.1f, 2.f);
	glm::clamp(y_scale, 0.1f, 2.f);
	glm::clamp(z_scale, 0.1f, 2.f);

	/* Top, bottom, vertical_segments - 1 for remaining vertical vertices, 
 	 * horizontal_segments + 1 for horizontal vertices*/
	auto const NUMBER_OF_VERTICES = 2 + (vertical_segments - 1) * horizontal_segments;

	vertices_.reserve(VERTEX_SIZE * NUMBER_OF_VERTICES);
	
	{
		using std::sin;
		using std::cos;
		using std::abs;

		std::array<GLfloat, VERTEX_SIZE> vertex;
		GLfloat x,y,z;
		GLfloat s,t;
		GLfloat theta, phi;
		GLfloat sin_theta;
		
		/* Top vertex */	
		vertex[0] = 0.f;     /* Position */
		vertex[1] = y_scale;
		vertex[2] = 0.f;
		vertex[3] = 0.f;	 /* Normal */
		vertex[4] = 1.f;
		vertex[5] = 0.f;
		vertex[6] = .5f;    /* Texture */
		vertex[7] = 1.f;
	
		vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));

		for(auto i = 0u; i < vertical_segments - 1; i++){
			t = 1.f - interpolation::linear(static_cast<GLfloat>(i+1) / static_cast<GLfloat>(vertical_segments));
			theta = PI * static_cast<GLfloat>(i+1) / static_cast<GLfloat>(vertical_segments);
			y = cos(theta);
			sin_theta = sin(theta);

			for(auto j = 0u; j < horizontal_segments ; j++){
				s = interpolation::linear(static_cast<GLfloat>(j) / static_cast<GLfloat>(horizontal_segments));
				
				phi = 2.f * PI * static_cast<GLfloat>(j) / static_cast<GLfloat>(horizontal_segments);
				
				x = sin_theta * sin(phi);
				z = sin_theta * cos(phi);
			
				vertex[0] = x_scale * x; /* Position */
				vertex[1] = y_scale * y;
				vertex[2] = z_scale * z;
				vertex[3] = x;  /* Normal */
				vertex[4] = y;
				vertex[5] = z;
				vertex[6] = s;			 /* Texture */
				vertex[7] = t;
	
				vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
			}
		}
		
		/* Bottom vertex */
		vertex[0] =  0.f;		/* Position */
		vertex[1] = -y_scale;
		vertex[2] =  0.f;
		vertex[3] =  0.f;       /* Normal */
		vertex[4] = -1.f;
		vertex[5] =  0.f;
		vertex[6] =  .5f;		/* Texture */
		vertex[7] =  0.f;
		
		vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
	}
	/* 2 * horizontal_segments for triangles connected to top and bottom vertices,
 	 * vertical_segments - 2 for remaining vertical segments and horizontal_segments
 	 * for remainig horizontal vertices. *2 for 2 triangles / quad */
	indices_.reserve(3 * 2 * (horizontal_segments + (vertical_segments - 2) * horizontal_segments));

	std::array<GLuint, 3> triangle_indices;

	/* Top vertex */
	for(auto i = 0u; i < horizontal_segments; i++){
		triangle_indices[0] = 0u;
		triangle_indices[1] = i+1;	
		triangle_indices[2] = i+2;
		
		/* Last vertex in final triangle */
		if(triangle_indices[2] > horizontal_segments)
			triangle_indices[2] -= horizontal_segments;

		indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
	}

	auto idx = 1u;
	for(auto i = 0u; i < vertical_segments-2; i++) {
		for(auto j = 0u; j < 2*horizontal_segments; j++) {
			triangle_indices[0] = idx;
			if(j % 2){
				triangle_indices[1] = idx - 1;
				triangle_indices[2] = idx + horizontal_segments;
			}
			else {
				triangle_indices[1] = idx + horizontal_segments;
				triangle_indices[2] = idx + horizontal_segments + 1;
				idx++;
			}

			/* Triangles for last quad in each horizontal belt */
			if((triangle_indices[2] % horizontal_segments) == 1){
				triangle_indices[2] -= horizontal_segments;
				if((triangle_indices[0] % horizontal_segments) == 1)
					triangle_indices[0] -= horizontal_segments;
			}

			indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
		}
	}

	/* Bottom vertex */
	for(auto i = 0u; i < horizontal_segments; i++){	
		triangle_indices[0] = NUMBER_OF_VERTICES - 1;
		triangle_indices[1] = NUMBER_OF_VERTICES - 2 - i;
		triangle_indices[2] = NUMBER_OF_VERTICES - 3 - i;

		/* Last vertex in final triangle */
		if(triangle_indices[2] < NUMBER_OF_VERTICES - horizontal_segments - 1)
			triangle_indices[2] += horizontal_segments;

		indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
	}
}

std::vector<GLfloat> const& Ellipsoid::vertices() const {
	return vertices_;
}

std::vector<GLuint> const& Ellipsoid::indices() const {
	return indices_;
}
