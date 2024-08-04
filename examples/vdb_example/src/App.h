#pragma once
#include <openvdb/tools/Statistics.h>
#include <openvdb/tree/LeafManager.h>

#include "3d/FPSCamera.h"
#include "IndirectionTexture.h"
#include "SparseVolume/Grid.h"
#include "SparseVolume/GridVectorIterators.h"
#include "TextureAtlas.h"
#include "app/BaseApp.h"
#include "primitives/Primitives.h"

namespace rs {

using namespace std;

inline std::vector<openvdb::GridBase::Ptr> loadGrids(
    const std::string &filename) {
  std::vector<openvdb::GridBase::Ptr> grids;

  openvdb::initialize();

  openvdb::io::File file(filename);
  if (!file.open()) {
    throw rs::Exception("cannot open " + filename);
  }

  if (file.beginName() == file.endName()) {
    throw rs::Exception("No grids found in file");
  }

  for (openvdb::io::File::NameIterator nameIterator = file.beginName();
       nameIterator != file.endName(); ++nameIterator) {
    std::string grid_name = *nameIterator;
    openvdb::GridBase::Ptr grid = file.readGrid(grid_name);
    grid->print(std::cout, 4);
    grids.push_back(grid);
  }

  file.close();

  return grids;
}

static std::vector<int> calcAtlasInternalNodeCounts(const Grid &grid,
                                                    int maxLeavesPerAtlas) {
  std::vector<int> result;
  InternalNodeIterator::Ptr i = grid.createInternalNodeIterator();

  while (i->hasNext()) {
    // calculate InternalNode count for this atlas
    int internalNodeCount = 0;
    int leafCount = 0;
    while (i->hasNext())  // iterate InternalNodes
    {
      InternalNodeIterator::Ptr prevIt = i->clone();
      const InternalNode::Ptr node = i->next();

      LeafIterator::Ptr leafIt = node->createLeafIterator();
      while (leafIt->next()) {
        leafCount++;
      }

      if (leafCount >= maxLeavesPerAtlas) {
        i = prevIt;
        break;
      }

      internalNodeCount++;
    }

    if (internalNodeCount == 0) {
      throw std::runtime_error(
          "Too many leaves in InternalNode to fit in a single texture atlas");
    }

    log::info() << result.size() << ": " << internalNodeCount << log::end();

    result.push_back(internalNodeCount);
  }

  return result;
}

static gl::IndirectionTexture<int> builIndirectionTexture(
    const Grid &grid, std::vector<int> atlasInternalNodeCounts) {
  InternalNodeIterator::Ptr node_iter = grid.createInternalNodeIterator();
  size_t maxLeafCountPerInternalNode = grid.getMaxLeafCountPerInternalNode();

  int internal_node_index = 0;
  gl::IndirectionTexture<int> tex(grid.getInternalNodeCount() *
                                  maxLeafCountPerInternalNode);

  for (int atlasIndex = 0; atlasIndex < atlasInternalNodeCounts.size();
       ++atlasIndex) {
    int leaf_index_in_atalas = 0;
    int internal_node_index_in_atlas = 0;
    while (internal_node_index_in_atlas < atlasInternalNodeCounts[atlasIndex]) {
      const InternalNode::Ptr &internalNode = node_iter->next();

      LeafIterator::Ptr leaf_iter = internalNode->createLeafIterator();
      while (const Leaf::Ptr &leaf = leaf_iter->next()) {
        int tbo_index = internal_node_index * maxLeafCountPerInternalNode +
                        leaf->getIndexInInternalNode();

        log::info("indirect")
            << tbo_index << ": " << leaf_index_in_atalas << log::end();

        tex.setIndirection(tbo_index, leaf_index_in_atalas);
        ++leaf_index_in_atalas;
      }
      ++internal_node_index;
      ++internal_node_index_in_atlas;
    }
  }

  tex.update();
  return tex;
}

using GridTexturesMap std::map<Grid::Ptr, gl::Texture3D::Ptr>;
struct MaterialConfig {
  glm::vec3 box_size;

  //! Texture atlas for each grid
  GridTexturesMap leaf_atlases;

  //! 1D texture which maps {internalNodeIndex * maxLeafCountPerInternalNode +
  //! leafIndexInInternalNode} to {leafIndexInAtlas}
  gl::TextureBuffer::Ptr indirection_texture;

  //! Index of the first InternalNode in the geometry with this material
  //! assigned
  int firstInternalNodeIndex;

  int maxLeafCountPerInternalNodeDimension;
  glm::ivec3 maxLeafCountPerAtlasDimension;
};

class App : public BaseApp {
 public:
  using BaseApp::BaseApp;

  FPSCamera cam;
  vector<gl::VboMesh::Ptr> meshes;
  vector<glm::vec3> box_centers;
  std::map<Grid::Ptr, gl::Texture3d::Ptr> texture_map;

  void setup() {
    setFPS(60.0f);
    setVerticalSync(true);

    cam.setPosition({0, 0, 100});
    cam.lookAt({0, 0, 0}, {0, 1, 0});
    cam.setNearClip(50);
    cam.setFarClip(10000);
    cam.setFov(45);
    cam.setDistanceToPivot(100);
    cam.enableInput(getWindows()[0]);

    auto grids = loadGrids(fs::getResourcesPath() + "vdb/bunny.vdb");
    log::info() << grids.size() << " grids are loaded" << log::end();

    GridType::Ptr grid = openvdb::gridPtrCast<GridType>(grids[0]);

    std::vector<Grid::Ptr> sparse_grids;
    sparse_grids.emplace_back(
        std::make_shared<VdbGrid<openvdb::FloatGrid>>(grid, 1));

    log::info() << "getChannelCount(): " << sparse_grids[0]->getChannelCount()
                << log::end();
    log::info() << "getInternalNodeCount(): "
                << sparse_grids[0]->getInternalNodeCount() << log::end();
    log::info() << "getMaxLeafCountPerInternalNode(): "
                << sparse_grids[0]->getMaxLeafCountPerInternalNode()
                << log::end();
    log::info() << "getMaxLeafCountPerInternalNodeDimension(): "
                << sparse_grids[0]->getMaxLeafCountPerInternalNodeDimension()
                << log::end();
    log::info() << "getVoxelCountPerLeafDimension(): "
                << sparse_grids[0]->getVoxelCountPerLeafDimension()
                << log::end();

    // sparse_grids.push_back(Grid::Ptr(new VdbGrid<openvdb::FloatGrid>(grid,
    // 1)));

    // std::vector<int> result;
    // TreeType::NodeCIter iter = grid->constTree().beginNode();
    // for (; iter; ++iter) {
    //   int internal_node_count = 0;
    //   int leaf_count = 0;

    //   switch (iter.getDepth()) {
    //     case 0: {
    //       const RootType *node = nullptr;
    //       iter.getNode(node);
    //       if (node) log::info("Root") << iter.getBoundingBox() << log::end();
    //       break;
    //     }
    //     case 1: {
    //       const Int1Type *node = nullptr;
    //       iter.getNode(node);
    //       if (node) log::info("Int1") << iter.getBoundingBox() << log::end();
    //       break;
    //     }
    //     case 2: {
    //       const Int2Type *node = nullptr;
    //       iter.getNode(node);
    //       if (node) log::info("Int2") << iter.getBoundingBox() << log::end();
    //       break;
    //     }
    //     case 3: {
    //       const LeafType *node = nullptr;
    //       iter.getNode(node);
    //       if (node) log::info("Leaf") << iter.getBoundingBox() << log::end();
    //       break;
    //     }
    //   }

    //   result.push_back(internal_node_count);
    // }

    float scale = 1;

    std::vector<int> atlasInternalNodeCounts =
        calcAtlasInternalNodeCounts(*sparse_grids[0], 4096);

    gl::IndirectionTexture<int> indir_tex =
        builIndirectionTexture(*sparse_grids[0], atlasInternalNodeCounts);

    int current_internal_node_index = 0;
    InternalNodesIterator internalNodesIterator(sparse_grids);
    for (int atlasIndex = 0; atlasIndex < atlasInternalNodeCounts.size();
         ++atlasIndex) {
      std::vector<TextureAtlas::Ptr> texture_atlases;

      for (int i = 0; i < sparse_grids.size(); ++i) {
        // TODO: Optimise texture size. Currently we always create texture full
        // sized texture even if only a fraction of the space is used. In
        // practice only the final texture in a series of atlaes will benifit
        // from this optimisation, so may not be worth implementing.
        TextureAtlas::Ptr atlas(new TextureAtlas(
            4096, sparse_grids[i]->getVoxelCountPerLeafDimension(),
            sparse_grids[i]->getChannelCount()));
        texture_atlases.push_back(atlas);
      }

      int firstInternalNodeIndexInAtlas = current_internal_node_index;
      while (current_internal_node_index - firstInternalNodeIndexInAtlas <
             atlasInternalNodeCounts[atlasIndex]) {
        const InternalNodes *internalNodes = internalNodesIterator.next();

        glm::vec3 box_size =
            internalNodes->front()->getBoundingBoxSize() * scale;
        glm::vec3 box_center =
            internalNodes->front()->getBoundingBoxCenter() * scale;

        geom::Mesh mesh = prim::Box(box_center - box_size * 0.5f,
                                    box_center + box_size * 0.5f, true);
        meshes.emplace_back(std::make_shared<gl::VboMesh>(mesh));
        box_centers.push_back(box_center);

        // Add all leaves of this InternalNode to atlases
        LeavesIterator leavesIterator(*internalNodes);
        while (const Leaves *leaves = leavesIterator.next()) {
          for (int i = 0; i < leaves->size(); ++i) {
            texture_atlases[i]->addTexture(*(*leaves)[i]);
          }
        }
        current_internal_node_index++;
      }

      for (int i = 0; i < sparse_grids.size(); ++i) {
        texture_atlases[i]->update();
        texture_map[sparse_grids[i]] = texture_atlases[i]->getTexture();
      }
    }
  }

  void draw() {
    gl::clearColor(0, 0, 0, 0);
    gl::clearDepth();
    gl::enableDepth();
    gl::pushMatrix();
    auto view = cam.getModelViewMatrix();
    auto proj = cam.getProjectionMatrix();
    gl::loadIdentity();
    gl::multMatrix(proj * view);
    gl::setColor(1, 1, 1, 1);
    for (auto &m : meshes) {
      gl::drawMesh(*m, GL_LINES);
    }
    gl::popMatrix();

    gl::pushMatrix();
    gl::setOrthoView(getWidth(), getHeight());
    gl::disableDepth();
    gl::setColor(1, 1, 1, 1);
    gl::drawBitmapString("FPS:" + util::toString(getFPS(), 2, 6, '0'), 5, 5);
    gl::drawBitmapString(
        "MEM:" + util::toString(getMemoryUsage(), 2, 6, '0') + " MB", 5, 15);
    gl::drawBitmapString(
        "USR:" + util::toString(getUseCpuTime(), 2, 6, '0') + " MS", 5, 25);
    gl::drawBitmapString(
        "SYS:" + util::toString(getSystemCpuTime(), 2, 6, '0') + " MS", 5, 35);
    gl::drawBitmapString(
        "WAL:" + util::toString(getWallTime(), 2, 6, '0') + " MS", 5, 45);
    gl::drawBitmapString(
        "CPU:" + util::toString(getCpuUsage() * 100.0, 2, 6, '0') + " PC", 5,
        55);
    gl::popMatrix();
  }

  void keyPressed(const rs::KeyEventArgs &e) {
    if (e.key == 'f') getMainWindow()->toggleFullscreen();
  }

  void keyReleased(const rs::KeyEventArgs &e) {}

  void mouseMoved(const rs::MouseEventArgs &e) {}

  void mousePressed(const rs::MouseEventArgs &e) {}

  void mouseReleased(const rs::MouseEventArgs &e) {}

  void mouseScrolled(const rs::ScrollEventArgs &e) {}

  void windowResized(const rs::ResizeEventArgs &e) {}

  void windowRefreshed(const rs::EventArgs &e) {}

  void windowClosed(const rs::EventArgs &e) {}

  void fileDropped(const rs::FileDropEventArgs &e) {}
};

}  // namespace rs
