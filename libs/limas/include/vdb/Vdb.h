#pragma once
#include <openvdb/io/Stream.h>
#include <openvdb/openvdb.h>
#include <openvdb/tools/Dense.h>
#include <openvdb/tools/MeshToVolume.h>
#include <openvdb/tools/VolumeToMesh.h>

#include "geom/AABB.h"
#include "gl/Texture3D.h"
#include "gl/VboMesh.h"
#include "graphics/Pixels.h"

namespace limas {
namespace vdb {

using FloatGridType = openvdb::FloatGrid;
using VdbMesh = gl::BaseVboMesh<openvdb::Vec3s, openvdb::Vec3s, openvdb::Vec4s,
                                openvdb::Vec2s, int>;

template <typename T>
inline std::ostream& operator<<(std::ostream& os,
                                typename openvdb::Grid<T>::Ptr& grid) {
  os << "Creator: " << grid->getCreator() << "\n";
  os << "GridClass: " << grid->getGridClass() << "\n";
  os << "VectorType: " << grid->getVectorType() << "\n";
  return os;
}
template <typename T>
class GridData {
 public:
  GridData() {}
  GridData(int width, int height, int depth)
      : width_(width), height_(height), depth_(depth) {
    data_.resize(width_ * height_ * depth_);
  }

  void setup(int width, int height, int depth) {
    width_ = width;
    height_ = height;
    depth_ = depth;
    data_.resize(width_ * height_ * depth_);
  }

  int getIndex(int x, int y, int z) const {
    return z * width_ * height_ + y * width_ + x;
  }

  void setValue(int x, int y, int z, T d) { data_[getIndex(x, y, z)] = d; }
  T getValue(int x, int y, int z) const { return data_[getIndex(x, y, z)]; }

  int getWidth() const { return width_; }
  int getHeight() const { return height_; }
  int getDepth() const { return depth_; }

  T* data() { return &data_[0]; }

 protected:
  std::vector<T> data_;
  int width_, height_, depth_;
};

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const GridData<T>& data) {
  os << "(" << data.getWidth() << ", " << data.getHeight() << ", "
     << data.getDepth() << ")";
  return os;
}

using ShortGridData = GridData<short>;
using IntGridData = GridData<int>;
using FloatGridData = GridData<float>;

inline void initialize() { openvdb::initialize(); }

template <typename T>
inline openvdb::FloatGrid::Ptr toVdb(GridData<T>& data, float tolerance) {
  openvdb::Coord bmin(0, 0, 0);
  openvdb::Coord bmax(data.getWidth(), data.getHeight(), data.getDepth());
  const openvdb::CoordBBox bbox(bmin, bmax);
  openvdb::tools::Dense<float> dense(bbox);

  openvdb::Coord xyz;
  int &x = xyz[0], &y = xyz[1], &z = xyz[2];
  float density_max = -INT_MAX;
  float density_min = INT_MAX;

  for (x = bmin.x(); x < bmax.x(); ++x) {
    for (y = bmin.y(); y < bmax.y(); ++y) {
      for (z = bmin.z(); z < bmax.z(); ++z) {
        float density = data.getValue(x, y, z);
        density_max = std::max<float>(density_max, density);
        density_min = std::min<float>(density_min, density);
      }
    }
  }

  for (x = bmin.x(); x < bmax.x(); ++x) {
    for (y = bmin.y(); y < bmax.y(); ++y) {
      for (z = bmin.z(); z < bmax.z(); ++z) {
        float density = (float)data.getValue(x, y, z) / (density_max);
        density = std::max(density, 0.0f);
        dense.setValue(xyz, density);
      }
    }
  }

  openvdb::FloatGrid::Ptr grid = openvdb::FloatGrid::create(0.0f);
  openvdb::tools::copyFromDense(dense, *grid, tolerance);
  // grid->setName("density");
  // grid->setGridClass(openvdb::GRID_FOG_VOLUME);
  return grid;
}

inline openvdb::GridPtrVecPtr load(const std::string& file_path) {
  // openvdb::io::File file(file_path);
  // file.open();
  // openvdb::GridBase::Ptr grid = file.readGrid(grid_name);
  // file.close();
  // return grid;
  std::ifstream ifile(file_path, std::ios_base::binary);
  return openvdb::io::Stream(ifile).getGrids();
}

template <typename T>
inline glm::tvec3<T> toGlm(const openvdb::math::Vec3<T>& v) {
  return glm::tvec3<T>(v.x(), v.y(), v.z());
}

template <typename T>
inline openvdb::math::Vec3<T> toVdb(const glm::tvec3<T>& v) {
  return openvdb::math::Vec3<T>(v.x, v.y, v.z);
}

template <typename T>
inline glm::tvec4<T> toGl(const openvdb::math::Vec4<T>& v) {
  return glm::tvec3<T>(v.x(), v.y(), v.z(), v.w());
}

template <typename T>
inline openvdb::math::Vec4<T> toVdb(const glm::tvec4<T>& v) {
  return openvdb::math::Vec4<T>(v.x, v.y, v.z, v.w);
}

std::vector<openvdb::Vec3I> getTriangles(const std::vector<int>& indices) {
  std::vector<openvdb::Vec3I> triangles(indices.size() / 3);
  for (int i = 0; i < triangles.size(); i++) {
    triangles[i][0] = indices[i * 3 + 0];
    triangles[i][1] = indices[i * 3 + 1];
    triangles[i][2] = indices[i * 3 + 2];
  }
  return triangles;
}

template <typename T>
inline gl::VboMesh toMesh(const typename openvdb::Grid<T>& grid) {
  std::vector<openvdb::Vec3s> points;
  std::vector<openvdb::Vec4I> quads;
  openvdb::tools::volumeToMesh(grid, points, quads);

  gl::VboMesh mesh;

  std::vector<glm::vec3> vertices(points.size());
  for (int i = 0; i < vertices.size(); i++) {
    vertices[i] = toGlm(points[i]);
  }

  std::vector<int> indices(quads.size() * 6);
  for (int i = 0; i < quads.size(); i++) {
    indices[i * 6 + 0] = quads[i].x();
    indices[i * 6 + 1] = quads[i].y();
    indices[i * 6 + 2] = quads[i].w();

    indices[i * 6 + 4] = quads[i].w();
    indices[i * 6 + 5] = quads[i].y();
    indices[i * 6 + 6] = quads[i].z();
  }

  mesh.setVertices(vertices);
  mesh.setIndices(indices);
  return mesh;
}

inline openvdb::FloatGrid::Ptr toSdf(gl::VboMesh& mesh, float voxel_size = 0.1,
                                     float exbw = 3, float inbw = 3) {
  const auto& vertices = mesh.getVertices();
  const auto& indices = mesh.getIndices();
  assert(indices.size() > 0);

  std::vector<openvdb::Vec3s> points(vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    points[i] = toVdb(vertices[i]);
  }
  std::vector<openvdb::Vec3I> triangles = getTriangles(indices);
  std::vector<openvdb::Vec4I> quads;

  openvdb::math::Transform::Ptr transform =
      openvdb::math::Transform::createLinearTransform(voxel_size);
  return openvdb::tools::meshToSignedDistanceField<openvdb::FloatGrid>(
      *transform, points, triangles, quads, exbw, inbw);
}

inline openvdb::FloatGrid::Ptr toLevelSet(
    gl::VboMesh& mesh, float voxel_size = 0.1,
    float half_width = float(openvdb::LEVEL_SET_HALF_WIDTH)) {
  const auto& vertices = mesh.getVertices();
  const auto& indices = mesh.getIndices();
  assert(indices.size() > 0);

  std::vector<openvdb::Vec3s> points(vertices.size());
  for (int i = 0; i < vertices.size(); i++) {
    points[i] = toVdb(vertices[i]);
  }
  std::vector<openvdb::Vec3I> triangles = getTriangles(indices);

  openvdb::math::Transform::Ptr transform =
      openvdb::math::Transform::createLinearTransform(voxel_size);

  return openvdb::tools::meshToLevelSet<openvdb::FloatGrid>(
      *transform, points, triangles, half_width);
}

template <typename T>
inline geom::AABB3D getAABB(const typename openvdb::Grid<T>& grid) {
  openvdb::CoordBBox bbox = grid.evalActiveVoxelBoundingBox();
  geom::AABB3D aabb;
  aabb.add(glm::vec3(bbox.min().x(), bbox.min().y(), bbox.min().z()));
  aabb.add(glm::vec3(bbox.max().x(), bbox.max().y(), bbox.max().z()));
  return aabb;
}

template <typename T>
inline FloatPixels3D toTextureData(const typename openvdb::Grid<T>& grid) {
  openvdb::CoordBBox bbox = grid.evalActiveVoxelBoundingBox();
  int width = bbox.dim().x();
  int height = bbox.dim().y();
  int depth = bbox.dim().z();
  FloatPixels3D data(width, height, depth, 1);
  for (auto iter = grid.beginValueOn(); iter; ++iter) {
    openvdb::Coord coord = iter.getCoord() - bbox.min();
    int index = coord.x() + coord.y() * width + coord.z() * width * height;
    data.getData()[index] = iter.getValue();
  }

  return data;
}

//   openvdb::tools::Dense<float, openvdb::tools::LayoutXYZ> dense(bbox);
//   openvdb::tools::copyFromGrid(dense, *grid);
//   gl::Texture3D tex;
//   tex.setup(bbox.dim().x(), bbox.dim().y(), bbox.dim().z(), GL_R32F,
//             dense.data());
//   tex.setWrapS(GL_CLAMP_TO_EDGE);
//   tex.setWrapT(GL_CLAMP_TO_EDGE);
//   tex.setWrapR(GL_CLAMP_TO_EDGE);
//   tex.setMinFilter(GL_LINEAR);
//   tex.setMagFilter(GL_LINEAR);
//   return tex;
// }

}  // namespace vdb
}  // namespace limas