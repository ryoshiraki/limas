#pragma once
#include "assimp/Exporter.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "geom/Mesh.h"
#include "geom/Polyline.h"
#include "system/Exception.h"
#include "system/Logger.h"
#include "utils/FileSystem.h"

namespace limas {
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

  static void save(const std::string& path, const geom::Mesh& mesh) {
    aiScene* scene = new aiScene();

    scene->mNumMeshes = 1;
    scene->mMeshes = new aiMesh*[scene->mNumMeshes];

    aiMesh* m = new aiMesh();
    m->mNumVertices = mesh.getNumVertices();
    m->mVertices = new aiVector3D[m->mNumVertices];
    for (int i = 0; i < mesh.getNumVertices(); i++) {
      const auto& v = mesh.getVertices()[i];
      m->mVertices[i] = aiVector3D(v.x, v.y, v.z);
    }

    scene->mMeshes[0] = m;

    Assimp::Exporter exporter;
    std::string format_id = fs::getExtension(path);
    format_id = format_id.substr(1);
    exporter.Export(scene, format_id.c_str(), path);

    delete scene;
  }

  static bool saveObj(const std::string& path, const geom::Mesh& mesh) {
    std::ofstream file(path);
    if (!file.is_open()) {
      logger::error() << "Could not open file for writing: " << path
                      << logger::end();
      return false;
    }

    // 頂点を書き出し
    for (const auto& v : mesh.getVertices()) {
      file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    // テクスチャ座標を書き出し
    for (size_t i = 0; i < mesh.getTexCoords().size(); ++i) {
      const glm::vec2& uv = mesh.getTexCoords()[i];
      file << "vt " << uv.x << " " << uv.y << "\n";
    }

    // 法線を書き出し
    for (size_t i = 0; i < mesh.getNormals().size(); ++i) {
      const glm::vec3& normal = mesh.getNormals()[i];
      file << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
    }

    // カラー情報を書き出し
    for (size_t i = 0; i < mesh.getColors().size(); ++i) {
      const glm::vec4& color = mesh.getColors()[i];
      file << "vc " << color.x << " " << color.y << " " << color.z << " "
           << color.w << "\n";
    }

    // 面情報を書き出し (頂点インデックス)
    const auto& indices = mesh.getIndices();
    for (size_t i = 0; i < indices.size(); i += 3) {
      file << "f " << (indices[i] + 1) << " " << (indices[i + 1] + 1) << " "
           << (indices[i + 2] + 1) << "\n";
    }

    file.close();
    return true;
  }

  static bool saveObj(const std::string& path, const geom::Polyline& polyline) {
    std::ofstream file(path);
    if (!file.is_open()) {
      logger::error() << "Could not open file for writing: " << path
                      << logger::end();
      return false;
    }

    // 頂点を書き出し
    for (const auto& v : polyline.getVertices()) {
      file << "v " << v.x << " " << v.y << " " << v.z << "\n";
    }

    file.close();
    return true;
  }

  static bool loadObj(const std::string& path, geom::Mesh* mesh) {
    FILE* file = fopen(path.c_str(), "r");
    if (file == NULL) {
      logger::error() << path << " is impossible to open" << logger::end();
      return false;
    }

    while (1) {
      char lineHeader[128];
      // read the first word of the line
      int res = fscanf(file, "%s", lineHeader);
      if (res == EOF) break;  // EOF = End Of File. Quit the loop.

      // else : parse lineHeader

      if (strcmp(lineHeader, "v") == 0) {
        glm::vec3 vertex;
        fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
        mesh->addVertex(vertex);
      } else if (strcmp(lineHeader, "vt") == 0) {
        glm::vec2 uv;
        fscanf(file, "%f %f\n", &uv.x, &uv.y);
        mesh->addTexCoord(uv);
      } else if (strcmp(lineHeader, "vn") == 0) {
        glm::vec3 normal;
        fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
        mesh->addNormal(normal);
      } else if (strcmp(lineHeader, "vc") == 0) {
        glm::vec4 color;
        fscanf(file, "%f %f %f %f\n", &color.x, &color.y, &color.z, &color.w);
        mesh->addColor(color);
      } else if (strcmp(lineHeader, "f") == 0) {
        unsigned int vertex_index[4];
        int matches =
            fscanf(file, "%d %d %d \n", &vertex_index[0], &vertex_index[1],
                   &vertex_index[2]);  // 4 vertices

        mesh->addIndex(vertex_index[0] - 1);
        mesh->addIndex(vertex_index[1] - 1);

        mesh->addIndex(vertex_index[1] - 1);
        mesh->addIndex(vertex_index[2] - 1);

        mesh->addIndex(vertex_index[2] - 1);
        mesh->addIndex(vertex_index[0] - 1);
      } else {
        // Probably a comment, eat up the rest of the line
        char stupidBuffer[1000];
        fgets(stupidBuffer, 1000, file);
      }
    }

    return true;
  }
};

}  // namespace geom
}  // namespace limas
