#pragma once

#include "SparseVolume/Leaf.h"

namespace rs {

class LeafIterator {
 public:
  using Ptr = std::shared_ptr<LeafIterator>;
  virtual ~LeafIterator() {}

  //! returns null pointer at end
  virtual Leaf::Ptr next() = 0;
  virtual bool hasNext() const = 0;
};

template <typename ChildIter>
class VdbLeafIterator : public LeafIterator {
  using LeafType = typename ChildIter::ValueType;

 public:
  VdbLeafIterator(const ChildIter &it) : it_(it) {}

  virtual Leaf::Ptr next() {
    Leaf::Ptr leaf;
    if (it_) {
      leaf.reset(new VdbLeaf<LeafType>(*it_, it_.offset()));
      ++it_;
    }
    return leaf;
  }

  virtual bool hasNext() const { return it_; }

 private:
  ChildIter it_;
};

}  // namespace rs