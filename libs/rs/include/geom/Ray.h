#pragma once

namespace rs {
namespace geom {

class Ray {
 public:
  Ray() {}
  Ray(const glm::vec3& orig, const glm::vec3& dir) : orig_(orig) {
    setDirection(dir);
  }

  void setOrigin(const glm::vec3& orig) { orig_ = orig; }
  const glm::vec3& getOrigin() const { return orig_; }

  void setDirection(const glm::vec3& dir) {
    dir_ = dir;
    inv_dir_ = glm::vec3(1.0f / dir_.x, 1.0f / dir_.y, 1.0f / dir_.z);
    sign_x_ = dir_.x < 0.0f ? 1 : 0;
    sign_y_ = dir_.y < 0.0f ? 1 : 0;
    sign_z_ = dir_.z < 0.0f ? 1 : 0;
  }

  char getSignX() const { return sign_x_; }
  char getSignY() const { return sign_y_; }
  char getSignZ() const { return sign_z_; }

  const glm::vec3& getDirection() const { return dir_; }
  const glm::vec3& getInvDirection() const { return inv_dir_; }

  void transform(const glm::mat4& matrix) {
    orig_ = glm::vec3(matrix * vec4(orig_, 1));
    setDirection(glm::mat3(matrix) * dir_);
  }

  Ray transformed(const glm::mat4& matrix) const {
    Ray result;
    result.setOrigin(glm::vec3(matrix * vec4(orig_, 1)));
    result.setDirection(glm::mat3(matrix) * dir_);
    return result;
  }

  glm::vec3 calcPosition(float t) const { return orig_ + dir_ * t; }

  bool getTriangleIntersection(const glm::vec3& v0, const glm::vec3& v2,
                               const glm::vec3& v3, float* result) const {
    glm::vec3 edge1, edge2, tvec, pvec, qvec;
    float det;
    float u, v;
    const float epsilon = 0.000001f;

    edge1 = vert1 - vert0;
    edge2 = vert2 - vert0;

    pvec = glm::cross(getDirection(), edge2);
    det = glm::dot(edge1, pvec);

#if 0  // we don't want to backface cull
    if (det < epsilon)
      return false;
    tvec = getOrigin() - vert0;
    
    u = glm::dot(tvec, pvec);
    if ( ( u < 0.0f ) || ( u > det ) )
      return false;
    
    qvec = glm::cross(tvec, edge1);
    v = glm::dot(getDirection(), qvec);
    if ( v < 0.0f || u + v > det )
      return false;
    
    *result = glm::dot(edge2, qvec) / det;
    return true;
#else

    if (det > -epsilon && det < epsilon) return false;

    float inv_det = 1.0f / det;
    tvec = getOrigin() - vert0;
    u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0.0f || u > 1.0f) return false;

    qvec = glm::cross(tvec, edge1);

    v = glm::dot(getDirection(), qvec) * inv_det;
    if (v < 0.0f || u + v > 1.0f) return 0;

    *result = glm::dot(edge2, qvec) * inv_det;
    return true;

#endif
  }

  bool getPlaneIntersection(const glm::vec3& plane_point,
                            const glm::vec3& plane_normal,
                            float* result) const {
    float denom = glm::dot(plane_normal, getDirection());

    if (denom != 0.0f) {
      *result = glm::dot(plane_normal, plane_point - getOrigin()) / denom;
      return true;
    }
    return false;
  }

 protected:
  glm::vec3 orig_;
  glm::vec3 dir_;
  // these are helpful to certain ray intersection algorithms
  char sign_x_, sign_y_, sign_z_;
  glm::vec3 inv_dir_;
};

template <typename T>
std::ostream& operator<<(std::ostream& o, const Ray& r) {
  return o << "(" << r.getOrigin() << ", " << p.getDirection() << ")";
}

}  // namespace geom
}  // namespace rs