#pragma once

namespace limas {
namespace geom {

template <class V, class N, class C, class T, class I>
class BaseMesh {
 public:
  BaseMesh() {}

  void clear() {
    vertices_.clear();
    normals_.clear();
    colors_.clear();
    texcoords_.clear();
    indices_.clear();
  }

  void addVertex(const V& v) { vertices_.push_back(v); }
  void addNormal(const N& n) { normals_.push_back(n); }
  void addColor(const C& c) { colors_.push_back(c); }
  void addTexCoord(const T& t) { texcoords_.push_back(t); }
  void addIndex(const I& i) { indices_.push_back(i); }

  void addVertices(const std::vector<V>& v) {
    copy(v.begin(), v.end(), std::back_inserter(vertices_));
  }
  void addNormals(const std::vector<N>& v) {
    copy(v.begin(), v.end(), std::back_inserter(normals_));
  }
  void addColors(const std::vector<C>& v) {
    copy(v.begin(), v.end(), std::back_inserter(colors_));
  }
  void addTexCoords(const std::vector<T>& v) {
    copy(v.begin(), v.end(), std::back_inserter(texcoords_));
  }
  void addIndices(const std::vector<I>& v) {
    copy(v.begin(), v.end(), std::back_inserter(indices_));
  }

  void setVertices(const std::vector<V>& vertices) { vertices_ = vertices; }
  void setNormals(const std::vector<N>& normals) { normals_ = normals; }
  void setColors(const std::vector<C>& colors) { colors_ = colors; }
  void setTexCoords(const std::vector<T>& texcoords) { texcoords_ = texcoords; }
  void setIndices(const std::vector<I>& indices) { indices_ = indices; }

  const std::vector<V>& getVertices() const { return vertices_; }
  const std::vector<N>& getNormals() const { return normals_; }
  const std::vector<C>& getColors() const { return colors_; }
  const std::vector<T>& getTexCoords() const { return texcoords_; }
  const std::vector<I>& getIndices() const { return indices_; }
  std::vector<V>& getVertices() { return vertices_; }
  std::vector<N>& getNormals() { return normals_; }
  std::vector<C>& getColors() { return colors_; }
  std::vector<T>& getTexCoords() { return texcoords_; }
  std::vector<I>& getIndices() { return indices_; }

  V& getIndexedVertex(int i) { return vertices_[indices_[i]]; }
  N& getIndexedNormal(int i) { return normals_[indices_[i]]; }
  C& getIndexedColor(int i) { return colors_[indices_[i]]; }
  T& getIndexedTexCoord(int i) { return texcoords_[indices_[i]]; }

  size_t getNumVertices() const { return vertices_.size(); }
  size_t getNumNormals() const { return normals_.size(); }
  size_t getNumColors() const { return colors_.size(); }
  size_t getNumTexCoords() const { return texcoords_.size(); }
  size_t getNumIndices() const { return indices_.size(); }

  void clearVertices() { vertices_.clear(); }
  void clearNormals() { normals_.clear(); }
  void clearColors() { colors_.clear(); }
  void clearTexCoords() { texcoords_.clear(); }
  void clearIndices() { indices_.clear(); }

  void fillColor(const C& fill_color) {
    std::vector<C> data(vertices_.size(), fill_color);
    colors_ = data;
  }

  void computeNormals() {
    if (indices_.empty() || vertices_.empty()) return;

    std::vector<N> face_normals(indices_.size());
    for (size_t i = 0; i < indices_.size(); i += 3) {
      auto& v0 = getIndexedVertex(i + 0);
      auto& v1 = getIndexedVertex(i + 1);
      auto& v2 = getIndexedVertex(i + 2);
      face_normals[i] = cross(v1 - v0, v2 - v0);
      normalize(face_normals[i]);
    }

    std::vector<N> vertex_normals(vertices_.size(), N(0, 0, 0));
    for (size_t i = 0; i < indices_.size(); i += 3) {
      vertex_normals[indices_[i + 0]] += face_normals[i];
      vertex_normals[indices_[i + 1]] += face_normals[i];
      vertex_normals[indices_[i + 2]] += face_normals[i];
    }

    for (auto& normal : vertex_normals) {
      normalize(normal);
    }

    normals_ = vertex_normals;
  }

 protected:
  std::vector<V> vertices_;
  std::vector<N> normals_;
  std::vector<C> colors_;
  std::vector<T> texcoords_;
  std::vector<I> indices_;

  template <typename Type>
  Type cross(const Type& a, const Type& b) {
    return Type(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2],
                a[0] * b[1] - a[1] * b[0]);
  }

  template <typename Type>
  float length(const Type& a) const {
    return std::sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
  }

  template <typename Type>
  void normalize(Type& a) {
    float len = length(a);
    a[0] /= len;
    a[1] /= len;
    a[2] /= len;
  }
};

template <class V, class N, class C, class T, class I>
inline std::ostream& operator<<(std::ostream& os,
                                const BaseMesh<V, N, C, T, I>& mesh) {
  os << "V:";
  os << mesh.getNumVertices();
  os << " N:";
  os << mesh.getNumNormals();
  os << " C:";
  os << mesh.getNumColors();
  os << " T:";
  os << mesh.getNumTexCoords();
  os << " I:";
  os << mesh.getNumIndices();
  return os;
}

using Mesh = BaseMesh<glm::vec3, glm::vec3, glm::vec4, glm::vec2, int>;

}  // namespace geom
}  // namespace limas