#ifndef TERRAIN_H
#define TERRAIN_H

#pragma once
#include "height_generator.h"
#include "renderer.h"
#include "transform.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>

template <typename ShaderPolicy>
class Terrain : public Renderer<Terrain<ShaderPolicy>, ShaderPolicy>, public Transform {
    using renderer_t = Renderer<Terrain<ShaderPolicy>, ShaderPolicy>;
    public:
        Terrain(ShaderPolicy policy = {}, GLfloat amplitude = 10.f, GLfloat x_len = 1.f, GLfloat dx = .5f, GLfloat z_len = 1.f, GLfloat dz = .5f);

        std::vector<GLfloat> const& vertices() const;
        std::vector<GLuint> const& indices() const;
        
        void init(GLfloat x_len, GLfloat dx, GLfloat z_len, GLfloat dz);

    private:
        HeightGenerator generator_;
        std::vector<GLfloat> vertices_{};
        std::vector<GLuint> indices_{};

        glm::vec3 calculate_normal(int x, int z);
};

#include "terrain.tcc"
#endif
