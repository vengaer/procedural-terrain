#include "cuboid.h"

Cuboid::Cuboid(GLfloat x_scale, GLfloat y_scale, GLfloat z_scale) : Renderer{}, vertices_{}, indices_{} {
	Renderer<Cuboid>::init(x_scale, y_scale, z_scale);
}

void Cuboid::init(GLfloat x_scale, GLfloat y_scale, GLfloat z_scale) {
	glm::clamp(x_scale, 0.1f, 2.f);
	glm::clamp(y_scale, 0.1f, 2.f);
	glm::clamp(z_scale, 0.1f, 2.f);

	vertices_.reserve(VERTEX_SIZE * 4u * 6u);

	{
		std::array<GLfloat, 3> offsets { x_scale / 2.f, y_scale / 2.f, z_scale / 2.f };
		std::array<std::pair<GLuint, GLuint>, 4> texture_coords { std::make_pair(0u, 0u), std::make_pair(1u, 0u), std::make_pair(1u, 1u), std::make_pair(0u, 1u) };

		enum class Axis { NONE = -1, X, Y, Z };
		enum class Sign { POS, NEG };

		std::array<GLfloat, VERTEX_SIZE> vertex;
		
		auto face = 0u;
		Axis fixed = Axis::NONE;
		Sign fixed_sign = Sign::POS;
		GLfloat sign;
		for(auto i = 0u; i < 6u; i++, face++) {
			if((face % 2u) == 0)
				fixed = static_cast<Axis>(static_cast<int>(fixed) + 1); /* Set fixed axis */
			
			for(auto j = 0u; j < 4u; j++) {
				sign = std::pow(-1, static_cast<int>(fixed_sign));

				vertex[static_cast<int>(fixed)] = sign * offsets[static_cast<int>(fixed)];			/* Fixed axis */
				vertex[static_cast<int>(fixed) + 3] = sign * 1.f;						  			/* Normal along fixed axis */
				vertex[6] = texture_coords[j].first;									  			/* Texture s coord */
				vertex[7] = texture_coords[j].second;									  			/* Texture t coord */

				auto non_fixed_idx1 = (static_cast<int>(fixed) % 2) +1;								/* Index for first non-fixed axis */
				auto non_fixed_idx2 = static_cast<bool>(fixed) ? 0 : static_cast<int>(fixed) + 2;	/* Index for second non-fixed axis */
				vertex[non_fixed_idx1] = (j < 2 ? -1.f : 1.f) * offsets[non_fixed_idx1];			/* Set first vertex value */
				vertex[non_fixed_idx2] = (!(j%3) ? -1.f : 1.f) * offsets[non_fixed_idx2];			/* Set second vertex value */

				/* Multiply by sign of fixed axis if needed */
				if(static_cast<bool>(fixed))
					vertex[(static_cast<int>(fixed) + 1) % 3] *= sign;
				else
					vertex[static_cast<int>(fixed) + 2] *= sign;

				/* If Z is not the fixed axis, its sign must be flipped */
				if(fixed != Axis::Z)
					vertex[static_cast<int>(Axis::Z)] *= -1.f;
		
				/* Normals along non-fixed axes */
				vertex[non_fixed_idx1 + 3] = 0.f;
				vertex[non_fixed_idx2 + 3] = 0.f;

				vertices_.insert(std::end(vertices_), std::begin(vertex), std::end(vertex));
			}
			
			fixed_sign = static_cast<Sign>((static_cast<int>(fixed_sign) + 1) % 2); /* Sign alternates between POS and NEG */
		}	
	}

	indices_.reserve(3u * 4u * 6u);
	std::array<GLuint, 3> triangle_indices;
	
	auto idx = 0u;
	for(auto i = 0u; i < 12u; i++, idx += 2) {
		triangle_indices[0] = idx;
		triangle_indices[1] = idx + 1;
		triangle_indices[2] = idx + (i%2 ? -2 : 2);
		
		indices_.insert(std::end(indices_), std::begin(triangle_indices), std::end(triangle_indices));
	}
}

std::vector<GLfloat> const& Cuboid::vertices() const {
	return vertices_;
}

std::vector<GLuint> const& Cuboid::indices() const {
	return indices_;
}
