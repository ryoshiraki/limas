#pragma once
#include "geom/Mesh.h"

namespace rs {
namespace prim {

class Grid : public geom::Mesh {
 public:
  Grid(int width, int height, int step_x, int step_y) {
    for (float x = 0; x <= width; x += step_x) {
      vertices_.push_back(glm::vec3(x == 0 ? 1 : x, 0, 0));
      vertices_.push_back(glm::vec3(x == 0 ? 1 : x, height, 0));
    }
    for (float y = 0; y <= height; y += step_y) {
      vertices_.push_back(glm::vec3(0, y == 0 ? 1 : y, 0));
      vertices_.push_back(glm::vec3(width, y == 0 ? 1 : y, 0));
    }
  }

 protected:
  Grid(){};
};

}  // namespace prim
}  // namespace rs
