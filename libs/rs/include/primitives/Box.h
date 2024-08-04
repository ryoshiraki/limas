#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Box : public geom::Mesh {
 public:
  Box(const glm::vec3& p0, const glm::vec3& p1, bool b_wireframe = false)
      : p0_(p0), p1_(p1) {
    // Front face
    vertices_.insert(vertices_.end(), {{p0.x, p0.y, p1.z},
                                       {p1.x, p0.y, p1.z},
                                       {p1.x, p1.y, p1.z},
                                       {p0.x, p1.y, p1.z}});
    normals_.insert(normals_.end(), {glm::vec3(0, 0, 1), glm::vec3(0, 0, 1),
                                     glm::vec3(0, 0, 1), glm::vec3(0, 0, 1)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    // Right face
    vertices_.insert(vertices_.end(), {{p1.x, p0.y, p1.z},
                                       {p1.x, p0.y, p0.z},
                                       {p1.x, p1.y, p0.z},
                                       {p1.x, p1.y, p1.z}});
    normals_.insert(normals_.end(), {glm::vec3(1, 0, 0), glm::vec3(1, 0, 0),
                                     glm::vec3(1, 0, 0), glm::vec3(1, 0, 0)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    // Back face
    vertices_.insert(vertices_.end(), {{p1.x, p0.y, p0.z},
                                       {p0.x, p0.y, p0.z},
                                       {p0.x, p1.y, p0.z},
                                       {p1.x, p1.y, p0.z}});
    normals_.insert(normals_.end(), {glm::vec3(0, 0, -1), glm::vec3(0, 0, -1),
                                     glm::vec3(0, 0, -1), glm::vec3(0, 0, -1)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    // Left face
    vertices_.insert(vertices_.end(), {{p0.x, p0.y, p0.z},
                                       {p0.x, p0.y, p1.z},
                                       {p0.x, p1.y, p1.z},
                                       {p0.x, p1.y, p0.z}});
    normals_.insert(normals_.end(), {glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0),
                                     glm::vec3(-1, 0, 0), glm::vec3(-1, 0, 0)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    // Bottom face
    vertices_.insert(vertices_.end(), {{p0.x, p0.y, p0.z},
                                       {p1.x, p0.y, p0.z},
                                       {p1.x, p0.y, p1.z},
                                       {p0.x, p0.y, p1.z}});
    normals_.insert(normals_.end(), {glm::vec3(0, -1, 0), glm::vec3(0, -1, 0),
                                     glm::vec3(0, -1, 0), glm::vec3(0, -1, 0)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    // Top face
    vertices_.insert(vertices_.end(), {{p0.x, p1.y, p1.z},
                                       {p1.x, p1.y, p1.z},
                                       {p1.x, p1.y, p0.z},
                                       {p0.x, p1.y, p0.z}});
    normals_.insert(normals_.end(), {glm::vec3(0, 1, 0), glm::vec3(0, 1, 0),
                                     glm::vec3(0, 1, 0), glm::vec3(0, 1, 0)});
    texcoords_.insert(texcoords_.end(),
                      {{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}});

    if (b_wireframe) {
      indices_ = {
          0,  1,  1,  2,  2,  3,  3,  0,   // Front face
          4,  5,  5,  6,  6,  7,  7,  4,   // Right face
          8,  9,  9,  10, 10, 11, 11, 8,   // Back face
          12, 13, 13, 14, 14, 15, 15, 12,  // Left face
          16, 17, 17, 18, 18, 19, 19, 16,  // Bottom face
          20, 21, 21, 22, 22, 23, 23, 20   // Top face
      };

    } else {
      indices_ = {
          0,  1,  2,  0,  2,  3,   // Front face
          4,  5,  6,  4,  6,  7,   // Right face
          8,  9,  10, 8,  10, 11,  // Back face
          12, 13, 14, 12, 14, 15,  // Left face
          16, 17, 18, 16, 18, 19,  // Bottom face
          20, 21, 22, 20, 22, 23   // Top face
      };
    }
  }

  Box(float width, float height, float depth, bool b_wireframe = false)
      : Box(glm::vec3(-width * 0.5, -height * 0.5, -depth * 0.5),
            glm::vec3(width * 0.5, height * 0.5, depth * 0.5), b_wireframe) {}

 protected:
  glm::vec3 p0_, p1_;
};

}  // namespace prim
}  // namespace rs