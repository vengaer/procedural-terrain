#include "cylinder.h"

Cylinder::Cylinder(GLfloat height, GLuint horizontal_segments, GLuint vertical_segments, Axis main_axis, GLfloat off_axis1_scale, GLfloat off_axis2_scale) : Renderer{}, Transform{}, vertices_{}, indices_{} {
	Renderer<Cylinder>::init(height, horizontal_segments, vertical_segments, main_axis, off_axis1_scale, off_axis2_scale);
}

Cylinder::Cylinder(std::shared_ptr<Shader> const& shader, GLfloat height, GLuint horizontal_segments, GLuint vertical_segments, Axis main_axis, GLfloat off_axis1_scale, GLfloat off_axis2_scale) : Renderer{shader}, Transform{shader}, vertices_{}, indices_{} {
	Renderer<Cylinder>::init(height, horizontal_segments, vertical_segments, main_axis, off_axis1_scale, off_axis2_scale);
}

void Cylinder::init(GLfloat height, GLuint horizontal_segments, GLuint vertical_segments, Axis main_axis, GLfloat off_axis1_scale, GLfloat off_axis2_scale) {
	height 				= glm::clamp(height, 0.1f, 4.f);
	horizontal_segments = glm::clamp(horizontal_segments, 3u, 60u);
	vertical_segments   = glm::clamp(vertical_segments, 1u, 20u);
	off_axis1_scale		= glm::clamp(off_axis1_scale, 0.1f, 2.0f);
	off_axis2_scale		= glm::clamp(off_axis2_scale, 0.1f, 2.0f);

	GLfloat height_offset = height / 2.f;
	GLfloat height_increment = height / vertical_segments;

	/* 2 additional rings for 2 sets of normals for top / bottom faces */

	auto const NUMBER_OF_VERTICES = 2 + (1 + 2 + vertical_segments) * horizontal_segments;
	vertices_.reserve(VERTEX_SIZE * NUMBER_OF_VERTICES);

	{
		/* off_axes.first == x and off_axes.second == y when viewed in xy coord system */
		auto off_axes = get_off_axes(main_axis);
		using std::sin;
		using std::cos;

		std::array<GLfloat, VERTEX_SIZE> vertex;
		
		/* Bottom center */
		vertex[enum_value(off_axes.first)] = 0.f;
		vertex[enum_value(off_axes.second)] = 0.f;
		vertex[enum_value(main_axis)] = -height_offset;
		vertex[enum_value(off_axes.first) + 3] = 0.f;
		vertex[enum_value(off_axes.second) + 3] = 0.f;
		vertex[enum_value(main_axis) + 3] = -1.f;
		vertex[6] = 0.5f;
		vertex[7] = 0.5f;
	
		vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));

		GLfloat phi;

		/* Bottom edges */
		for(auto i = 0u; i < horizontal_segments; i++) {
			phi = 2 * PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(horizontal_segments);
			interpolation::Parameters texture = interpolation::polar(phi);

			vertex[enum_value(off_axes.first)] = off_axis1_scale * cos(phi);
			vertex[enum_value(off_axes.second)] = off_axis2_scale * sin(phi);
			vertex[enum_value(main_axis)] = -height_offset;
			vertex[enum_value(off_axes.first)+3] = 0.f;
			vertex[enum_value(off_axes.second)+3] = 0.f;
			vertex[enum_value(main_axis) + 3] = -1.f;
			vertex[6] = (texture.x + 1.f) * 0.5f; /* Transform from [-1, 1] to [0, 1] */
			vertex[7] = (texture.y + 1.f) * 0.5f;

			vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
		}

		GLfloat current_height = -height_offset;
		GLfloat u, v;
		/* Middle */
		for(auto i = 0u; i < vertical_segments + 1; i++, current_height += height_increment) {
			for(auto j = 0u; j < horizontal_segments; j++) {
				phi = 2 * PI * static_cast<GLfloat>(j) / static_cast<GLfloat>(horizontal_segments);
				u = cos(phi);
				v = sin(phi);

				vertex[enum_value(off_axes.first)] = off_axis1_scale * u;
				vertex[enum_value(off_axes.second)] = off_axis2_scale * v;
				vertex[enum_value(main_axis)] = current_height;
				vertex[enum_value(off_axes.first) + 3] = u;
				vertex[enum_value(off_axes.second) + 3] = v;
				vertex[enum_value(main_axis) + 3] = 0.f;
				vertex[6] = interpolation::linear(static_cast<GLfloat>(j) / static_cast<GLfloat>(horizontal_segments));
				vertex[7] = interpolation::linear(static_cast<GLfloat>(i) / static_cast<GLfloat>(vertical_segments));
		
				vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
			}
		}

		/* Top edges */
		for(auto i = 0u; i < horizontal_segments; i++) {
			phi = 2 * PI * static_cast<GLfloat>(i) / static_cast<GLfloat>(horizontal_segments);
			interpolation::Parameters texture = interpolation::polar(phi);
			
			vertex[enum_value(off_axes.first)] = off_axis1_scale * cos(phi);
			vertex[enum_value(off_axes.second)] = off_axis2_scale * sin(phi);
			vertex[enum_value(main_axis)] = height_offset;
			vertex[enum_value(off_axes.first)+3] = 0.f;
			vertex[enum_value(off_axes.second)+3] = 0.f;
			vertex[enum_value(main_axis) + 3] = 1.f;
			vertex[6] = (texture.x + 1.f) * 0.5f; /* Transform form [-1, 1] to [0, 1] */
			vertex[7] = (texture.y + 1.f) * 0.5f;

			vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
		}
		
		/* Top center */
		vertex[enum_value(off_axes.first)] = 0.f;
		vertex[enum_value(off_axes.second)] = 0.f;
		vertex[enum_value(main_axis)] = height_offset;
		vertex[enum_value(off_axes.first) + 3] = 0.f;
		vertex[enum_value(off_axes.second) + 3] = 0.f;
		vertex[enum_value(main_axis) + 3] = 1.f;
		vertex[6] = 0.5f;
		vertex[7] = 0.5f;
	
		vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));

	}

	indices_.reserve(3u * (2u * horizontal_segments + 2u * vertical_segments * horizontal_segments));

	std::array<GLuint, 3> triangle_indices;
	
	auto idx = 1u;
	/* Bottom */
	for(auto i = 0u; i < horizontal_segments; i++, idx++){
		triangle_indices[0] = 0u;
		triangle_indices[1] = idx;
		triangle_indices[2] = (idx + 1u) % horizontal_segments;
		
		indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
	}

	/* Middle */
	for(auto i = 0u; i < vertical_segments; i++) {
		for(auto j = 0u; j < 2*horizontal_segments; j++) {
			triangle_indices[0] = idx;
			if(j % 2) {	
				triangle_indices[1] = idx + horizontal_segments;
				triangle_indices[2] = idx + horizontal_segments - 1;
			}
			else{
				triangle_indices[1] = idx + 1;
				triangle_indices[2] = idx + horizontal_segments;
				idx++;
			}

			if((triangle_indices[1] % horizontal_segments) == 1) {
				triangle_indices[1] -= horizontal_segments;
				if((triangle_indices[0] % horizontal_segments) == 1)
					triangle_indices[0] -= horizontal_segments;
			}

			indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
		}
	}
	
	/* Top */
	for(auto i = 0u; i < horizontal_segments; i++) {
		triangle_indices[0] = NUMBER_OF_VERTICES - 1;
		triangle_indices[1] = NUMBER_OF_VERTICES - 3 - i;
		triangle_indices[2] = NUMBER_OF_VERTICES - 2 - i;
		
		if(i == horizontal_segments - 1)
			triangle_indices[1] -= horizontal_segments;

		indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
	}
}

std::vector<GLfloat> const& Cylinder::vertices() const {
	return vertices_;
}

std::vector<GLuint> const& Cylinder::indices() const {
	return indices_;
}

std::pair<Cylinder::Axis, Cylinder::Axis> Cylinder::get_off_axes(Axis main_axis) {
	std::pair<Axis, Axis> axes;
	switch(main_axis) {
		case Axis::X:
			axes.first = Axis::Y;
			axes.second = Axis::Z;
			break;
		case Axis::Y:
			axes.first = Axis::Z;
			axes.second = Axis::X;
			break;
		default:
			axes.first = Axis::X;
			axes.second = Axis::Y;
			break;
	}
	return axes;
}
			
