#pragma once
#include "SparseVolume/InternalNodeIterator.h"

namespace rs {

class Grid {
 public:
  using Ptr = std::shared_ptr<Grid>;

  virtual ~Grid() {}
  virtual size_t getMaxLeafCountPerInternalNode() const = 0;
  virtual size_t getMaxLeafCountPerInternalNodeDimension() const = 0;
  virtual size_t getVoxelCountPerLeafDimension() const = 0;
  virtual size_t getInternalNodeCount() const = 0;
  virtual InternalNodeIterator::Ptr createInternalNodeIterator() const = 0;
  virtual int getChannelCount() const = 0;
};

template <typename GridType>
class VdbGrid : public Grid {
 public:
  VdbGrid(const typename GridType::Ptr& grid, int channel_count)
      : grid_(grid), channel_count_(channel_count) {
    // count number of internal nodes
    internal_node_count_ = 0;
    InternalNodeIterator::Ptr i = createInternalNodeIterator();
    while (i->next()) {
      internal_node_count_++;
    }
  }

  size_t getMaxLeafCountPerInternalNode() const { return Int2Type::NUM_VALUES; }
  size_t getMaxLeafCountPerInternalNodeDimension() const {
    return 1 << Int2Type::LOG2DIM;
  }
  size_t getVoxelCountPerLeafDimension() const { return LeafType::DIM; }
  size_t getInternalNodeCount() const { return internal_node_count_; }

  InternalNodeIterator::Ptr createInternalNodeIterator() const {
    return InternalNodeIterator::Ptr(
        new VdbInternalNodeIterator<GridType>(grid_->constTree().beginNode()));
  }

  int getChannelCount() const { return channel_count_; }

 private:
  typename GridType::Ptr grid_;
  int channel_count_;
  size_t internal_node_count_;
};

}  // namespace rs