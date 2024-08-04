#pragma once
#include <memory>

#include "geom/Mesh.h"
#include "glm/glm.hpp"
#include "graphics/Pixels.h"
#include "openvdb/openvdb.h"

namespace rs {

using GridType = openvdb::FloatGrid;
using TreeType = GridType::TreeType;
using RootType = TreeType::RootNodeType;   // level 3 RootNode
using Int1Type = RootType::ChildNodeType;  // level 2 InternalNode
using Int2Type = Int1Type::ChildNodeType;  // level 1 InternalNode
using LeafType = TreeType::LeafNodeType;   // level 0 LeafNode

using Vec3UByte = openvdb::math::Vec3<unsigned char>;
using Vec3UByteTree = openvdb::tree::Tree4<Vec3UByte, 5, 4, 3>::Type;
using Vec3UByteGrid = openvdb::Grid<Vec3UByteTree>;

inline std::vector<unsigned char> getColor(
    const openvdb::FloatGrid::TreeType::LeafNodeType& leaf,
    const openvdb::Coord& coord, size_t channels) {
  float value;
  bool active = leaf.probeValue(coord, value);
  if (!active) {
    value = 0;
  } else {
    value *= 255.0f;
  }

  std::vector<unsigned char> data(channels);
  for (int i = 0; i < channels; ++i) {
    data[i] = (unsigned char)value;
  }
  return data;
}

// inline void leafToPixel(unsigned char* p, int channelCount,
//                         const openvdb::Coord& coord,
//                         const Vec3UByteGrid::TreeType::LeafNodeType& leaf) {
//   Vec3UByte value;
//   leaf.probeValue(coord, value);

//   channelCount = std::min(channelCount, 3);
//   for (int i = 0; i < channelCount; ++i) {
//     *p++ = value[i];
//   }
// }

class Leaf {
 public:
  using Ptr = std::shared_ptr<Leaf>;

  virtual ~Leaf() {}

  //! Returns index of this leaf's position within parent InternalNode.
  //! Indicies do not need to be contiguious, i.e Leaves can be sparsely
  //! distributed within the InternalNode.
  virtual size_t getIndexInInternalNode() const = 0;
  virtual int getVoxelCountPerDimension() const = 0;
  virtual Pixels3D getPixels(size_t channels) const = 0;
  virtual geom::Mesh toMesh() const = 0;
};

template <typename T>
class VdbLeaf : public Leaf {
 public:
  VdbLeaf(const T& leaf, size_t offset_in_internal_node)
      : leaf_(leaf), offset_in_internal_node_(offset_in_internal_node) {}

  virtual size_t getIndexInInternalNode() const {
    return offset_in_internal_node_;
  }

  virtual int getVoxelCountPerDimension() const { return leaf_.dim(); }

  virtual Pixels3D getPixels(size_t channels) const {
    int dim = leaf_.dim();
    Pixels3D pix(dim, dim, dim, channels);

    // We access openvdb data by Coord instead of linearly walking through
    // buffer by index because openvdb leaf buffer is packed in zyx order and
    // store our texture in xyz order

    std::vector<unsigned char> data;
    for (int z = 0; z < dim; ++z) {
      for (int y = 0; y < dim; ++y) {
        for (int x = 0; x < dim; ++x) {
          auto d = getColor(leaf_, openvdb::Coord(x, y, z), channels);
          std::copy(d.begin(), d.end(), std::back_inserter(data));
        }
      }
    }
    pix.loadData(&data[0], dim, dim, dim);
    return pix;
  }

  // virtual void loadToPixels(Pixels3D& pixels, const glm::ivec3& offset) const
  // {
  //   int dim = leaf_.dim();

  //   assert(offset.x + dim <= pixels.getWidth());
  //   assert(offset.y + dim <= pixels.getHeight());
  //   assert(offset.z + dim <= pixels.getDepth());

  //   // We access openvdb data by Coord instead of linearly walking through
  //   // buffer by index because openvdb leaf buffer is packed in zyx order and
  //   // store our texture in xyz order
  //   for (int z = 0; z < dim; ++z) {
  //     for (int y = 0; y < dim; ++y) {
  //       int i = pixels.getIndex(offset.x, offset.y + y, offset.z + z);
  //       unsigned char* p = &pixels.getData()[i];
  //       for (int x = 0; x < dim; ++x) {
  //         leafToPixel(p, pixels.getNumChannels(), openvdb::Coord(x, y, z),
  //                     leaf_);
  //         p += pixels.getNumChannels();
  //       }
  //     }
  //   }
  // }

  virtual geom::Mesh toMesh() const {}

 private:
  T leaf_;
  size_t offset_in_internal_node_;
};

}  // namespace rs