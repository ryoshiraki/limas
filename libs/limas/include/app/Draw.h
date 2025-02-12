
#pragma once
#include "app/AppUtils.h"
#include "gl/Context.h"
#include "gl/GLUtils.h"
#include "gl/Renderer.h"
#include "graphics/Font.h"

namespace limas {

class FloatColor;
using Color = FloatColor;

namespace gl {

class Vao;
class Shader;
class TextureBase;

template <class V, class N, class C, class T>
class BaseVboMesh;

template <class V>
class BaseVboPolyline;

template <class V, class N, class C, class T>
class BaseInstancedVboMesh;

inline void setShaderDefault() { app::getRenderer()->setShaderDefault(); }
inline void setShader(gl::Shader* shader) {
  app::getRenderer()->setShader(shader);
}

#pragma mark DEFAULT UNIFORMS
inline void setColor(const Color& c) { app::getRenderer()->setColor(c); }
inline void setColor(float r, float g, float b, float a) {
  setColor(Color{r, g, b, a});
}
inline void setColorHsv(float h, float s, float v, float a) {
  setColor(Color::fromHsv(h, s, v, a));
}

inline void bindTexture(const TextureBase& tex) {
  app::getRenderer()->bindTexture(tex.getId());
}
inline void unbindTexture() { app::getRenderer()->unbindTexture(); }

#pragma mark MATRIX STACK
inline MatrixStack& pushMatrix() { return app::getRenderer()->pushMatrix(); }
inline MatrixStack& popMatrix() { return app::getRenderer()->popMatrix(); }
inline MatrixStack& loadIdentity() {
  return app::getRenderer()->loadIdentity();
}
inline MatrixStack& multMatrix(const glm::mat4& m) {
  return app::getRenderer()->multMatrix(m);
}
inline MatrixStack& translate(const glm::vec3& translation) {
  return app::getRenderer()->translate(translation);
}
inline MatrixStack& translate(float x, float y, float z) {
  return translate(glm::vec3(x, y, z));
}
inline MatrixStack& scale(const glm::vec3& scale) {
  return app::getRenderer()->scale(scale);
}
inline MatrixStack& scale(float x, float y, float z) {
  return scale(glm::vec3(x, y, z));
}
inline MatrixStack& flip(bool x, bool y) {
  return app::getRenderer()->flip(x, y);
}
inline MatrixStack& rotate(const glm::mat3& rotation) {
  return app::getRenderer()->rotate(rotation);
}
inline MatrixStack& rotate(const glm::quat& quat) {
  return app::getRenderer()->rotate(quat);
}
inline MatrixStack& rotateRadians(float theta, const glm::vec3& axis) {
  return app::getRenderer()->rotateRadians(theta, axis);
}
inline MatrixStack& rotateRadians(float theta, float x, float y, float z) {
  return rotateRadians(theta, glm::vec3(x, y, z));
}
inline MatrixStack& rotateDegree(float angle, const glm::vec3& axis) {
  return app::getRenderer()->rotateDegree(angle, axis);
}
inline MatrixStack& rotateDegree(float angle, float x, float y, float z) {
  return rotateDegree(angle, glm::vec3(x, y, z));
}

inline glm::mat4 getCurrentMatrix() {
  return app::getRenderer()->getCurrentMatrix();
}

inline MatrixStack& setOrthoView(float left, float right, float bottom,
                                 float top, float near, float far) {
  app::getContext()->setViewport(0, 0, abs(left - right), abs(bottom - top));
  auto proj_mat = glm::ortho(left, right, bottom, top, near, far);
  return app::getRenderer()->multMatrix(proj_mat);
}

inline MatrixStack& setOrthoView(float width, float height,
                                 float near = -INT_MAX, float far = INT_MAX) {
  app::getContext()->setViewport(0, 0, width, height);
  auto proj_mat = glm::ortho(0.0f, width, height, 0.0f, near, far);
  return app::getRenderer()->multMatrix(proj_mat);
}

#pragma mark DRAW VAOs
inline void drawArrays(GLenum type, GLsizei first, GLsizei count) {
  app::getRenderer()->drawArrays(type, first, count);
}

inline void drawArrays(const Vao& vao, GLenum mode, GLint count) {
  app::getRenderer()->drawArrays(vao, mode, count);
}

inline void drawElements(const Vao& vao, GLenum mode, GLint count) {
  app::getRenderer()->drawElements(vao, mode, count);
}

inline void draw(const Drawable& drawable, GLenum mode, GLint count) {
  app::getRenderer()->draw(drawable, mode, count);
}

inline void drawInstanced(const Drawable& drawable, GLenum mode, GLint count,
                          GLint instance_count) {
  app::getRenderer()->drawInstanced(drawable, mode, count, instance_count);
}

template <class V, class N, class C, class T>
inline void drawMesh(const BaseVboMesh<V, N, C, T>& mesh, GLenum mode) {
  app::getRenderer()->drawMesh(mesh, mode);
}

template <class V, class N, class C, class T>
inline void drawMeshInstanced(const BaseInstancedVboMesh<V, N, C, T>& mesh,
                              GLenum mode, size_t count) {
  app::getRenderer()->drawMeshInstanced(mesh, mode, count);
}

template <class V>
inline void drawPolyline(const BaseVboPolyline<V>& poly, GLenum mode) {
  app::getRenderer()->drawPolyline(poly, mode);
}

#pragma mark DRAW PRIMITIVES 2D
inline void drawDot(const glm::vec3& p) { app::getRenderer()->drawDot(p); }
inline void drawDot(float x, float y, float z) { drawDot(glm::vec3(x, y, z)); }

inline void drawSegment(const glm::vec3& p0, const glm::vec3& p1) {
  app::getRenderer()->drawSegment(p0, p1);
}
inline void drawSegment(float x0, float y0, float z0, float x1, float y1,
                        float z1) {
  drawSegment(glm::vec3(x0, y0, z0), glm::vec3(x1, y1, z1));
}

inline void drawSegment(float x0, float y0, float x1, float y1) {
  drawSegment(glm::vec3(x0, y0, 0), glm::vec3(x1, y1, 0));
}

inline void drawSegment(const glm::vec2& p0, const glm::vec2& p1) {
  drawSegment(glm::vec3(p0.x, p0.y, 0), glm::vec3(p1.x, p1.y, 0));
}

inline void drawCross(const glm::vec3& p, float size) {
  drawSegment(p.x - size, p.y, 0, p.x + size, p.y, 0);
  drawSegment(p.x, p.y - size, 0, p.x, p.y + size, 0);
}

inline void drawCross(float x, float y, float size) {
  drawCross(glm::vec3(x, y, 0), size);
}

inline void drawRectangle(const glm::vec2& p, float w, float h) {
  app::getRenderer()->drawRectangle(glm::vec3(p, 0), w, h);
}

inline void drawRectangle(const glm::vec3& p, float w, float h) {
  app::getRenderer()->drawRectangle(p, w, h);
}

inline void drawRectangle(float x, float y, float w, float h) {
  drawRectangle(glm::vec3(x, y, 0), w, h);
}

inline void drawWireRectangle(const glm::vec3& p, float w, float h) {
  app::getRenderer()->drawWireRectangle(p, w, h);
}

inline void drawWireRectangle(float x, float y, float w, float h) {
  drawWireRectangle(glm::vec3(x, y, 0), w, h);
}

#pragma mark DRAW ELLIPSE
inline void drawEllipse(const glm::vec2& p, float a, float b) {
  app::getRenderer()->drawEllipse(glm::vec3(p, 0), a, b);
}

inline void drawEllipse(const glm::vec3& p, float a, float b) {
  app::getRenderer()->drawEllipse(p, a, b);
}

inline void drawEllipse(float x, float y, float a, float b) {
  drawEllipse(glm::vec3(x, y, 0), a, b);
}

inline void drawWireEllipse(const glm::vec2& p, float a, float b) {
  app::getRenderer()->drawWireEllipse(glm::vec3(p, 0), a, b);
}

inline void drawWireCircle(const glm::vec3& p, float a, float b) {
  app::getRenderer()->drawWireEllipse(p, a, b);
}

inline void drawWireEllipse(float x, float y, float a, float b) {
  drawWireEllipse(glm::vec3(x, y, 0), a, b);
}

#pragma mark DRAW CIRCLE
inline void drawCircle(const glm::vec2& p, float r) {
  app::getRenderer()->drawCircle(glm::vec3(p, 0), r);
}

inline void drawCircle(const glm::vec3& p, float r) {
  app::getRenderer()->drawCircle(p, r);
}

inline void drawCircle(float x, float y, float r) {
  drawCircle(glm::vec3(x, y, 0), r);
}

inline void drawWireCircle(const glm::vec2& p, float r) {
  app::getRenderer()->drawWireCircle(glm::vec3(p, 0), r);
}

inline void drawWireCircle(const glm::vec3& p, float r) {
  app::getRenderer()->drawWireCircle(p, r);
}

inline void drawWireCircle(float x, float y, float r) {
  drawWireCircle(glm::vec3(x, y, 0), r);
}

#pragma mark DRAW PRIMITIVES 3D
inline void drawSphere(const glm::vec3& p, float r) {
  app::getRenderer()->drawSphere(p, r);
}

inline void drawSphere(float x, float y, float z, float r) {
  drawSphere(glm::vec3(x, y, z), r);
}

inline void drawBox(const glm::vec3& p, float w, float h, float d) {
  app::getRenderer()->drawBox(p, w, h, d);
}

inline void drawBox(float x, float y, float z, float w, float h, float d) {
  drawBox(glm::vec3(x, y, z), w, h, d);
}

inline void drawWireBox(const glm::vec3& p, float w, float h, float d) {
  app::getRenderer()->drawWireBox(p, w, h, d);
}

inline void drawWireBox(float x, float y, float z, float w, float h, float d) {
  drawWireBox(glm::vec3(x, y, z), w, h, d);
}

inline void drawCylinder(const glm::vec3& p, float radius, float height) {
  app::getRenderer()->drawCylinder(p, radius, height);
}

inline void drawCylinder(float x, float y, float z, float radius,
                         float height) {
  drawCylinder(glm::vec3(x, y, z), radius, height);
}

inline void drawCone(const glm::vec3& p, float radius, float height) {
  app::getRenderer()->drawCone(p, radius, height);
}

inline void drawCone(float x, float y, float z, float radius, float height) {
  drawCone(glm::vec3(x, y, z), radius, height);
}

inline void drawAxis(float size) {
  setColor(1, 0, 0, 1);
  drawSegment(glm::vec3(0, 0, 0), glm::vec3(size, 0, 0));
  setColor(0, 1, 0, 1);
  drawSegment(glm::vec3(0, 0, 0), glm::vec3(0, size, 0));
  setColor(0, 0, 1, 1);
  drawSegment(glm::vec3(0, 0, 0), glm::vec3(0, 0, size));
}

inline void drawGrid(float width, float height, float step_x, float step_y) {
  for (float x = 0; x <= width; x += step_x) {
    drawSegment(glm::vec3(x == 0 ? 1 : x, 0, 0),
                glm::vec3(x == 0 ? 1 : x, height, 0));
  }
  for (float y = 0; y <= height; y += step_y) {
    drawSegment(glm::vec3(0, y == 0 ? 1 : y, 0),
                glm::vec3(width, y == 0 ? 1 : y, 0));
  }
}

#pragma mark DRAW TEXTURES
inline void drawTextureSubsection(const TextureBase& tex, float x, float y,
                                  float w, float h, float s, float t, float sw,
                                  float th) {
  app::getRenderer()->transformTexture(s / tex.getWidth(), t / tex.getHeight(),
                                       sw / tex.getWidth(),
                                       th / tex.getHeight());
  app::getRenderer()->drawTexture(tex.getId(), glm::vec3(x, y, 0), w, h);
  app::getRenderer()->transformTexture(0, 0, 1, 1);
}

inline void drawTexture(const TextureBase& tex, float x, float y, float w = 0,
                        float h = 0, bool b_flip_y = false) {
  if (w == 0) w = tex.getWidth();
  if (h == 0) h = tex.getHeight();

  if (b_flip_y) {
    drawTextureSubsection(tex, x, y + h, w, -h, 0, 0, tex.getWidth(),
                          tex.getHeight());
  } else {
    drawTextureSubsection(tex, x, y, w, h, 0, 0, tex.getWidth(),
                          tex.getHeight());
  }
}

inline void drawFontString(const Font& font, const std::string& text, float x,
                           float y) {
  bindTexture(font.getTexture());
  gl::VboMesh mesh = font.getMesh(text, x, y);
  drawMesh(mesh, GL_TRIANGLES);
  unbindTexture();
}

inline void drawBitmapString(const std::string& text, float x, float y) {
  app::getRenderer()->drawBitmapString(text, glm::vec3(x, y, 0));
}

inline void drawLargeBitmapString(const std::string& text, float x, float y) {
  app::getRenderer()->drawLargeBitmapString(text, glm::vec3(x, y, 0));
}

inline size_t getBitmapStringWidth(const std::string& text) {
  return app::getRenderer()->getBitmapStringWidth(text);
}

inline size_t getBitmapStringHeight() {
  return app::getRenderer()->getBitmapStringHeight();
}

#pragma mark LEGACY STYLE
namespace legacy {
inline void begin(GLenum mode) { app::getRenderer()->begin(mode); }
inline void end() { app::getRenderer()->end(); }
inline void addVertex(const glm::vec3& v) { app::getRenderer()->addVertex(v); }
inline void addVertex(float x, float y, float z) {
  addVertex(glm::vec3(x, y, z));
}
inline void addNormal(const glm::vec3& n) { app::getRenderer()->addNormal(n); }
inline void addNormal(float x, float y, float z) {
  addNormal(glm::vec3(x, y, z));
}
inline void addColor(const glm::vec4& c) { app::getRenderer()->addColor(c); }
inline void addColor(float r, float g, float b, float a) {
  addColor(glm::vec4(r, g, b, a));
}
inline void addTexCoord(const glm::vec2& t) {
  app::getRenderer()->addTexCoord(t);
}
inline void addTexCoord(float s, float t) { addTexCoord(glm::vec2(s, t)); }
inline void addIndex(int i) { app::getRenderer()->addIndex(i); }
}  // namespace legacy

}  // namespace gl
}  // namespace limas