#pragma once
#include "SparseVolume/Grid.h"

namespace rs {

using InternalNodes = std::vector<InternalNode::Ptr>;
using Leaves = std::vector<Leaf::Ptr>;

class InternalNodesIterator {
 public:
  InternalNodesIterator(const std::vector<Grid::Ptr>& grids) {
    internal_nodes_.resize(grids.size());
    internal_node_iterators_.resize(grids.size());
    for (size_t i = 0; i < grids.size(); ++i) {
      internal_node_iterators_[i] = grids[i]->createInternalNodeIterator();
    }
  }

  const InternalNodes* next() {
    for (size_t i = 0; i < internal_node_iterators_.size(); ++i) {
      internal_nodes_[i] = internal_node_iterators_[i]->next();
    }

    if (internal_nodes_[0] != 0) {
      return &internal_nodes_;
    }
    return 0;
  }

  bool hasNext() const {
    if (!internal_node_iterators_.empty()) {
      return internal_node_iterators_.back()->hasNext();
    }
    return false;
  }

  std::vector<InternalNodeIterator::Ptr> getIterators() const {
    return internal_node_iterators_;
  }

 private:
  InternalNodes internal_nodes_;
  std::vector<InternalNodeIterator::Ptr> internal_node_iterators_;
};

class LeavesIterator {
 public:
  LeavesIterator(const InternalNodes& internal_nodes) {
    leaves_.resize(internal_nodes.size());
    for (size_t i = 0; i < internal_nodes.size(); ++i) {
      leaf_iterators_.push_back(internal_nodes[i]->createLeafIterator());
    }
  }

  const Leaves* next() {
    for (size_t i = 0; i < leaf_iterators_.size(); ++i) {
      leaves_[i] = leaf_iterators_[i]->next();
    }

    if (leaves_[0] != 0) {
      return &leaves_;
    }
    return 0;
  }

  bool hasNext() const {
    if (!leaf_iterators_.empty()) {
      return leaf_iterators_.back()->hasNext();
    }
    return false;
  }

 private:
  Leaves leaves_;
  std::vector<LeafIterator::Ptr> leaf_iterators_;
};

}