#pragma once
#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "geom/Mesh.h"
#include "system/Exception.h"
#include "system/Logger.h"
#include "utils/FileSystem.h"

namespace rs {
namespace geom {

class MeshIO {
 public:
  MeshIO() = delete;

  static std::vector<geom::Mesh> load(const std::string& filepath) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(
        filepath.c_str(), aiProcess_Triangulate |
                              aiProcess_JoinIdenticalVertices |
                              aiProcess_SortByPType);

    if (!scene) throw Exception(importer.GetErrorString());

    std::vector<geom::Mesh> meshes;
    for (int i = 0; i < scene->mNumMeshes; i++) {
      geom::Mesh mesh;

      aiMesh* m = scene->mMeshes[i];
      for (int j = 0; j < m->mNumVertices; j++) {
        if (m->HasPositions()) {
          aiVector3D v = m->mVertices[j];
          mesh.addVertex({v.x, v.y, v.z});
        }

        if (m->HasNormals()) {
          aiVector3D n = m->mNormals[j];
          mesh.addNormal({n.x, n.y, n.z});
        }

        if (m->HasTextureCoords(0)) {
          aiVector3D tc = m->mTextureCoords[0][j];
          mesh.addTexCoord({tc.x, tc.y});
        }

        if (m->HasVertexColors(0)) {
          aiColor4D c = m->mColors[0][j];
          mesh.addColor({c.r, c.g, c.b, c.a});
        }
      }

      if (m->HasFaces()) {
        for (int k = 0; k < m->mNumFaces; k++) {
          aiFace face = m->mFaces[k];
          for (int l = 0; l < face.mNumIndices; l++) {
            mesh.addIndex(face.mIndices[l]);
          }
        }
      }

      meshes.push_back(mesh);
    }

    return meshes;
  }

  static void save(const geom::Mesh& mesh, const std::string& path) {
    // aiMesh* m = new aiMesh();
    // m->mNumVertices = mesh.getNumVertices();
    // for (int i = 0; i < mesh.getNumVertices(); i++) {
    //   const auto& v = mesh.getVertices()[i];
    //   mesh->mVertices[i] = aiVector3D(v.x, v.y, v.z);
    // }

    // aiScene* scene = new aiScene();
    // scene->mNumMeshes = 1;
    // scene->mMeshes = new aiMesh*[scene->mNumMeshes];
    // scene->mMeshes[0] = m;

    // Assimp::Exporter exporter;
    // const char* format_id = fs::getExtension(path).c_str();
    // exporter.Export(scene, format_id, path);

    // delete m;
    // delete scene;
  }
};

}  // namespace geom
}  // namespace rs
