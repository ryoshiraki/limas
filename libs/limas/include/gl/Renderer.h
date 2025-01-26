#pragma once
#include "gl/GL.h"
#include "graphics/BitmapFont.h"
#include "graphics/BitmapFontLarge.h"
#include "graphics/Color.h"
#include "math/MatrixStack.h"
#include "primitives/Primitives.h"
#include "system/Noncopyable.h"
#include "type/Stack.h"

namespace limas {
namespace gl {

struct DefaultUniforms {
  Color color = {1, 1, 1, 1};
  GLuint tex_id = 0;
  glm::mat4 tex_mat = glm::mat4(1.0f);
  bool has_color = false;
  bool has_normal = false;
  bool has_texcoord = false;
  bool b_instanced = false;
};

class Renderer : private Noncopyable {
  Shader def_shader_;
  ShaderBase* current_shader_;
  bool b_should_unbind_;

  gl::Vao vao_;

  VboMesh dot;
  VboMesh segment_;
  VboMesh rectangle_;
  VboMesh circle_;

  VboMesh box_;
  VboMesh wire_box_;
  VboMesh sphere_;
  VboMesh wire_sphere_;
  VboMesh cylinder_;
  VboMesh cone_;

  BitmapFont font_;
  BitmapFontLarge font_large_;

  Stack<DefaultUniforms> uniforms_stack_;

  MatrixStack matrix_stack_;

 public:
  using Ptr = std::shared_ptr<Renderer>;
  static Ptr create() {
    Ptr instance(new Renderer());
    return instance;
  }

  Renderer() {
    dot = prim::Dot(0, 0, 0);
    segment_ = prim::Segment(glm::vec3(0), glm::vec3(0.57735));
    rectangle_ = prim::Rectangle(glm::vec3(0), glm::vec3(1));
    circle_ = prim::Circle(glm::vec3(0), 1, 256);

    sphere_ = prim::Sphere(1, 64, 32);
    wire_sphere_ = prim::Sphere(1, 64, 32, true);
    box_ = prim::Box(1, 1, 1);
    wire_box_ = prim::Box(1, 1, 1, true);
    cylinder_ = prim::Cylinder(1, 1, 32, 2);
    cone_ = prim::Cone(1, 1, 32, 2);

    // box_wire
    font_.load();
    font_large_.load();

    def_shader_.load(fs::getCommonResourcePath("shaders/default.vert"),
                     fs::getCommonResourcePath("shaders/default.frag"));
    current_shader_ = &def_shader_;
    b_should_unbind_ = true;

    uniforms_stack_.push(DefaultUniforms());
  }

  void drawArrays(GLenum type, GLsizei first, GLsizei count) {
    vao_.drawArrays(type, first, count);
  }

  void drawArrays(const Vao& vao, GLenum mode, GLint count) {
    auto& uniforms = uniforms_stack_.push().get();
    uniforms.has_color = vao.isAttributeEnabled(COLOR_ATTRIBUTE);
    uniforms.has_normal = vao.isAttributeEnabled(NORMAL_ATTRIBUTE);
    uniforms.has_texcoord = vao.isAttributeEnabled(TEXCOORD_ATTRIBUTE);
    bindShader();
    vao.drawArrays(mode, 0, count);
    unbindShader();
    uniforms_stack_.pop();
  }

  void drawElements(const Vao& vao, GLenum mode, GLint count) {
    auto& uniforms = uniforms_stack_.push().get();
    uniforms.has_color = vao.isAttributeEnabled(COLOR_ATTRIBUTE);
    uniforms.has_normal = vao.isAttributeEnabled(NORMAL_ATTRIBUTE);
    uniforms.has_texcoord = vao.isAttributeEnabled(TEXCOORD_ATTRIBUTE);
    bindShader();
    vao.drawElements(mode, count);
    unbindShader();
    uniforms_stack_.pop();
  }

  void drawArraysInstanced(const Vao& vao, GLenum mode, GLint count,
                           GLint instance_count) {
    auto& uniforms = uniforms_stack_.push().get();
    uniforms.has_color = vao.isAttributeEnabled(COLOR_ATTRIBUTE);
    uniforms.has_normal = vao.isAttributeEnabled(NORMAL_ATTRIBUTE);
    uniforms.has_texcoord = vao.isAttributeEnabled(TEXCOORD_ATTRIBUTE);
    uniforms.b_instanced = true;
    bindShader();
    vao.drawArraysInstanced(mode, 0, count, instance_count);
    unbindShader();
    uniforms_stack_.pop();
  }

  void drawElementsInstanced(const Vao& vao, GLenum mode, GLint count,
                             GLint instance_count) {
    auto& uniforms = uniforms_stack_.push().get();
    uniforms.has_color = vao.isAttributeEnabled(COLOR_ATTRIBUTE);
    uniforms.has_normal = vao.isAttributeEnabled(NORMAL_ATTRIBUTE);
    uniforms.has_texcoord = vao.isAttributeEnabled(TEXCOORD_ATTRIBUTE);
    uniforms.b_instanced = true;
    bindShader();
    vao.drawElementsInstanced(mode, count, instance_count);
    unbindShader();
    uniforms_stack_.pop();
  }

  void draw(const Drawable& drawable, GLenum mode, GLint count) {
    auto& vao = drawable.getVao();
    if (vao.isIboEnabled()) {
      drawElements(vao, mode, count);
    } else {
      drawArrays(vao, mode, count);
    }
  }

  void drawInstanced(const Drawable& drawable, GLenum mode, GLint count,
                     GLint instance_count) {
    auto& vao = drawable.getVao();
    if (vao.isIboEnabled()) {
      drawElementsInstanced(vao, mode, count, instance_count);
    } else {
      drawArraysInstanced(vao, mode, count, instance_count);
    }
  }

  template <class V, class N, class C, class T>
  void drawMesh(const BaseVboMesh<V, N, C, T>& mesh, GLenum mode) {
    GLint count =
        mesh.isIndexEnabled() ? mesh.getNumIndices() : mesh.getNumVertices();
    draw(mesh, mode, count);
  }

  template <class V, class N, class C, class T>
  void drawMeshInstanced(const BaseInstancedVboMesh<V, N, C, T>& mesh,
                         GLenum mode, size_t instance_count) {
    GLint count =
        mesh.isIndexEnabled() ? mesh.getNumIndices() : mesh.getNumVertices();
    drawInstanced(mesh, mode, count, instance_count);
  }

  template <class V>
  void drawPolyline(const BaseVboPolyline<V>& poly, GLenum mode) {
    draw(poly, mode, poly.getNumVertices());
  }

  void drawBitmapString(const std::string& text, const glm::vec3& p) {
    auto meshes = font_.getMeshes(text);
    bindTexture(font_.getTexture().getId());
    for (int i = 0; i < meshes.size(); i++) {
      pushMatrix();
      translate(
          p + glm::vec3(i * (font_.getCharacterWidth() + font_.getSpaceWidth()),
                        0, 0));
      drawMesh(*meshes[i], GL_TRIANGLE_FAN);
      popMatrix();
    }
    unbindTexture();
  }

  void drawLargeBitmapString(const std::string& text, const glm::vec3& p) {
    auto meshes = font_large_.getMeshes(text);
    bindTexture(font_large_.getTexture().getId());
    for (int i = 0; i < meshes.size(); i++) {
      pushMatrix();
      translate(p + glm::vec3(i * font_large_.getCharacterWidth(), 0, 0));
      drawMesh(*meshes[i], GL_TRIANGLE_FAN);
      popMatrix();
    }
    unbindTexture();
  }

  size_t getBitmapStringWidth(const std::string& text) const {
    return font_.getSize(text);
  }

  size_t getBitmapStringHeight() const { return font_.getCharacterWidth(); }

  void drawDot(const glm::vec3& p) {
    pushMatrix();
    translate(p);
    drawMesh(dot, GL_POINTS);
    popMatrix();
  }

  void drawSegment(const glm::vec3& p0, const glm::vec3& p1) {
    glm::vec3 src_dir = glm::vec3(0.57735);
    glm::vec3 dst_dir = glm::normalize(p1 - p0);
    float cos_theta = glm::dot(src_dir, dst_dir);
    float radian = std::acos(std::clamp(cos_theta, -1.0f, 1.0f));
    glm::vec3 axis = glm::normalize(glm::cross(src_dir, dst_dir));
    float scale_factor = glm::length(p1 - p0);

    pushMatrix();
    translate(p0);
    rotateRadians(radian, axis);
    scale(glm::vec3(scale_factor));
    drawMesh(segment_, GL_LINES);
    popMatrix();
  }

  void drawRectangle(const glm::vec3& p, float w, float h) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(w, h, 1));
    drawMesh(rectangle_, GL_TRIANGLE_FAN);
    popMatrix();
  }

  void drawWireRectangle(const glm::vec3& p, float w, float h) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(w, h, 1));
    drawMesh(rectangle_, GL_LINE_LOOP);
    popMatrix();
  }

  void drawEllipse(const glm::vec3& p, float a, float b) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(a, b, 1));
    drawMesh(circle_, GL_TRIANGLE_FAN);
    popMatrix();
  }

  void drawWireEllipse(const glm::vec3& p, float a, float b) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(a, b, 1));
    drawMesh(circle_, GL_LINE_LOOP);
    popMatrix();
  }

  void drawCircle(const glm::vec3& p, float r) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(r, r, 1));
    drawMesh(circle_, GL_TRIANGLE_FAN);
    popMatrix();
  }

  void drawWireCircle(const glm::vec3& p, float r) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(r, r, 1));
    drawMesh(circle_, GL_LINE_LOOP);
    popMatrix();
  }

  void drawSphere(const glm::vec3& p, float r) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(r, r, r));
    drawMesh(sphere_, GL_TRIANGLES);
    popMatrix();
  }

  void drawWireSphere(const glm::vec3& p, float r) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(r, r, r));
    drawMesh(wire_sphere_, GL_LINES);
    popMatrix();
  }

  void drawBox(const glm::vec3& p, float w, float h, float d) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(w, h, d));
    drawMesh(box_, GL_TRIANGLES);
    popMatrix();
  }

  void drawWireBox(const glm::vec3& p, float w, float h, float d) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(w, h, d));
    drawMesh(wire_box_, GL_LINES);
    popMatrix();
  }

  void drawCylinder(const glm::vec3& p, float radius, float height) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(radius, height, radius));
    drawMesh(cylinder_, GL_TRIANGLES);
    popMatrix();
  }

  void drawCone(const glm::vec3& p, float radius, float height) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(radius, height, radius));
    drawMesh(cone_, GL_TRIANGLES);
    popMatrix();
  }

  void drawTexture(GLuint tex, const glm::vec3& p, float w, float h) {
    pushMatrix();
    translate(p);
    scale(glm::vec3(w, h, 1));
    bindTexture(tex);
    drawMesh(rectangle_, GL_TRIANGLE_FAN);
    unbindTexture();
    popMatrix();
  }

  MatrixStack& pushMatrix() { return matrix_stack_.push(); }
  MatrixStack& popMatrix() { return matrix_stack_.pop(); }
  MatrixStack& multMatrix(const glm::mat4& m) { return matrix_stack_.mult(m); }
  MatrixStack& loadIdentity() { return matrix_stack_.identity(); }
  MatrixStack& translate(const glm::vec3& translation) {
    return matrix_stack_.translate(translation);
  }
  MatrixStack& rotate(const glm::mat3& rotation) {
    return matrix_stack_.rotate(rotation);
  }

  MatrixStack& rotate(const glm::quat& quat) {
    return matrix_stack_.rotate(quat);
  }

  MatrixStack& rotateRadians(float theta, const glm::vec3& axis) {
    return matrix_stack_.rotateRadians(theta, axis);
  }
  MatrixStack& rotateDegree(float angle, const glm::vec3& axis) {
    return matrix_stack_.rotateDegree(angle, axis);
  }
  MatrixStack& scale(const glm::vec3& s) { return matrix_stack_.scale(s); }

  MatrixStack& flip(bool x, bool y) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    scale(glm::vec3(x ? -1 : 1, y ? -1 : 1, 1));
    return translate(glm::vec3(x ? -viewport[2] : 0, y ? -viewport[3] : 0, 0));
  }

  glm::mat4& getCurrentMatrix() { return matrix_stack_.top(); }
  const glm::mat4& getCurrentMatrix() const { return matrix_stack_.top(); }

  void setColor(const Color& c) { uniforms_stack_.get().color = c; }

  void transformTexture(float s, float t, float sw, float th) {
    auto& uniforms = uniforms_stack_.get();
    uniforms.tex_mat = glm::mat4(1);
    uniforms.tex_mat = glm::translate(uniforms.tex_mat, glm::vec3(s, t, 0));
    uniforms.tex_mat = glm::scale(uniforms.tex_mat, glm::vec3(sw, th, 0));
  }

  void setShaderDefault() { current_shader_ = &def_shader_; }
  void setShader(Shader* shader) { current_shader_ = shader; }

  void bindShader() {
    if (ShaderBase::getCurrentShader() != nullptr) {
      current_shader_ = ShaderBase::getCurrentShader();
      b_should_unbind_ = false;
    } else {
      current_shader_->bind();
      b_should_unbind_ = true;
    }
    setUniforms();
  }

  void unbindShader() {
    if (b_should_unbind_) current_shader_->unbind();
  }

  void setUniforms() {
    auto& uniforms = uniforms_stack_.get();
    current_shader_->setUniformMatrix4f("MVP_MAT", matrix_stack_.top());
    current_shader_->setUniform4f("COLOR", uniforms.color);

    if (uniforms.tex_id > 0) {
      current_shader_->setUniform1i("USE_TEX", true);
      current_shader_->setUniformTexture("TEX", GL_TEXTURE_2D, uniforms.tex_id,
                                         0);

    } else {
      current_shader_->setUniform1i("USE_TEX", false);
      current_shader_->setUniformTexture("TEX", GL_TEXTURE_2D, 0, 0);
    }

    current_shader_->setUniformMatrix4f("TEX_MAT", uniforms.tex_mat);

    current_shader_->setUniform1i("HAS_COLOR", uniforms.has_color);
    current_shader_->setUniform1i("HAS_NORMAL", uniforms.has_normal);
    current_shader_->setUniform1i("HAS_TEXCOORD", uniforms.has_texcoord);
    if (uniforms.b_instanced) {
      current_shader_->setUniform1i("INSTANCED", true);
    } else {
      current_shader_->setUniform1i("INSTANCED", false);
    }
  }

  void bindTexture(GLuint tex_id) {
    uniforms_stack_.push().get().tex_id = tex_id;
  }

  void unbindTexture() { uniforms_stack_.pop(); }

#pragma mark LEGACY STYLE
 private:
  struct LegacyParams {
    VboMesh mesh;
    GLenum mode;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec4> colors;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> texcoords;
    std::vector<GLuint> indices;
  };
  LegacyParams legacy_params_;

 public:
  void begin(GLenum mode) { legacy_params_.mode = mode; }
  void end() {
    if (legacy_params_.vertices.size())
      legacy_params_.mesh.setVertices(legacy_params_.vertices);
    if (legacy_params_.colors.size())
      legacy_params_.mesh.setColors(legacy_params_.colors);
    if (legacy_params_.normals.size())
      legacy_params_.mesh.setNormals(legacy_params_.normals);
    if (legacy_params_.texcoords.size())
      legacy_params_.mesh.setTexCoords(legacy_params_.texcoords);
    if (legacy_params_.indices.size())
      legacy_params_.mesh.setIndices(legacy_params_.indices);

    legacy_params_.mesh.update();
    drawMesh(legacy_params_.mesh, legacy_params_.mode);

    // legacy_params_.mesh.clear();
    legacy_params_.vertices.clear();
    legacy_params_.colors.clear();
    legacy_params_.normals.clear();
    legacy_params_.texcoords.clear();
    legacy_params_.indices.clear();
  }

  void addVertex(const glm::vec3& v) { legacy_params_.vertices.push_back(v); }
  void addNormal(const glm::vec3& n) { legacy_params_.normals.push_back(n); }
  void addColor(const glm::vec4& c) { legacy_params_.colors.push_back(c); }
  void addTexCoord(const glm::vec2& t) {
    legacy_params_.texcoords.push_back(t);
  }
  void addIndex(int i) { legacy_params_.indices.push_back(i); }

 private:
};

}  // namespace gl
}  // namespace limas
