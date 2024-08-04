#pragma once
#include "SparseVolume/LeafIterator.h"

namespace rs {

inline glm::vec3 toVec3(const openvdb::Vec3d &v) {
  return glm::vec3(v.x(), v.y(), v.z());
}

class InternalNode {
 public:
  using Ptr = std::shared_ptr<InternalNode>;
  virtual ~InternalNode() {}
  virtual LeafIterator::Ptr createLeafIterator() const = 0;
  virtual glm::vec3 getBoundingBoxSize() const = 0;
  virtual glm::vec3 getBoundingBoxCenter() const = 0;
  virtual geom::Mesh toMesh() const = 0;
};

template <typename NodeType>
class VdbInternalNode : public InternalNode {
 public:
  VdbInternalNode(const NodeType *node) : node_(node) {}

  LeafIterator::Ptr createLeafIterator() const {
    return LeafIterator::Ptr(
        new VdbLeafIterator<typename NodeType::ChildOnCIter>(
            node_->cbeginChildOn()));
  }

  glm::vec3 getBoundingBoxSize() const {
    return toVec3(node_->getNodeBoundingBox().dim().asVec3d());
  }

  glm::vec3 getBoundingBoxCenter() const {
    return toVec3(node_->getNodeBoundingBox().getCenter());
  }

  NodeType *getWrappedNode() const { return node_; }

  geom::Mesh toMesh() const {}

 private:
  const NodeType *node_;
};

}  // namespace rs