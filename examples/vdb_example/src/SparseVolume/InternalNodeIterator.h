#pragma once
#include "SparseVolume/InternalNode.h"

namespace rs {

class InternalNodeIterator {
 public:
  using Ptr = std::shared_ptr<InternalNodeIterator>;
  ~InternalNodeIterator() {}
  virtual InternalNode::Ptr next() = 0;
  virtual bool hasNext() const = 0;
  virtual InternalNodeIterator::Ptr clone() const = 0;
};

template <typename GridType>
class VdbInternalNodeIterator : public InternalNodeIterator {
 public:
  VdbInternalNodeIterator(const TreeType::NodeCIter &it) : it_(it) {}

  InternalNode::Ptr next() {
    for (; it_; ++it_) {
      switch (it_.getDepth()) {
        case 2: {
          const Int2Type *node = 0;
          it_.getNode(node);
          assert(node);

          ++it_;
          return std::make_shared<VdbInternalNode<Int2Type>>(node);
        }
      }
    }
    return InternalNode::Ptr();
  }

  bool hasNext() const {
    auto it = *this;
    return (it.next() != 0);
  }

  InternalNodeIterator::Ptr clone() const {
    return std::make_shared<VdbInternalNodeIterator<GridType>>(it_);
  }

 private:
  TreeType::NodeCIter it_;
};

}  // namespace rs