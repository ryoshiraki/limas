#pragma once
#include "system/Logger.h"

namespace limas {
namespace gl {

inline void checkError(const std::string& name = "GL") {
  GLenum error = glGetError();
  while (error != GL_NO_ERROR) {
    std::string message;
    switch (error) {
      case GL_INVALID_ENUM:
        message = "GL_INVALID_ENUM";
        break;
      case GL_INVALID_VALUE:
        message = "GL_INVALID_VALUE";
        break;
      case GL_INVALID_OPERATION:
        message = "GL_INVALID_OPERATION";
        break;
      case GL_STACK_OVERFLOW:
        message = "GL_STACK_OVERFLOW";
        break;
      case GL_STACK_UNDERFLOW:
        message = "GL_STACK_UNDERFLOW";
        break;
      case GL_OUT_OF_MEMORY:
        message = "GL_OUT_OF_MEMORY";
        break;
      default:
        message = "UNKNOWN ERROR";
        break;
    }
    log::error(name) << message << log::end();
    error = glGetError();
  }
}

inline void clearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a) {
  glClearColor(r, g, b, a);
  glClear(GL_COLOR_BUFFER_BIT);
}

inline void clearDepth(GLdouble depth = 1) {
  glClearDepth(depth);
  glClear(GL_DEPTH_BUFFER_BIT);
}

inline void clearStencil(GLint stencil = 0) {
  glClearStencil(stencil);
  glClear(GL_STENCIL_BUFFER_BIT);
}

inline void enableDepth() { glEnable(GL_DEPTH_TEST); }
inline void disableDepth() { glDisable(GL_DEPTH_TEST); }

inline void enableStencil() { glEnable(GL_STENCIL_TEST); }
inline void disableStencil() { glDisable(GL_STENCIL_TEST); }

inline void enableFaceCulling() { glEnable(GL_CULL_FACE); }
inline void disableFaceCulling() { glDisable(GL_CULL_FACE); }

inline void setCullFaceBack() { glCullFace(GL_BACK); }
inline void setCullFaceFront() { glCullFace(GL_FRONT); }

inline void enableBlend() { glEnable(GL_BLEND); }
inline void disableBlend() { glDisable(GL_BLEND); }
inline void setBlendModeAlpha() {
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
inline void setBlendModeAdd() { glBlendFunc(GL_SRC_ALPHA, GL_ONE); }
inline void setBlendModeMulti() { glBlendFunc(GL_ZERO, GL_SRC_COLOR); }
inline void setBlendModeScreen() {
  glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE);
}
inline void setBlendModeReverse() {
  glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
}
inline void setBlendModeReverse2() {
  glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ONE_MINUS_SRC_COLOR);
}
inline void setBlendModeSeparatedAlpha() {
  glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_SRC_ALPHA,
                      GL_ONE);
}

enum BlendMode {
  BLEND_NONE = 0,
  BLEND_ALPHA,
  BLEND_ADD,
  BLEND_MULTI,
  BLEND_SCREEN,
  BLEND_REVERSE,
  BLEND_REVERSE2,
  BLEND_ALPHA_SEPARATED
};

inline void setBlendMode(BlendMode mode) {
  if (mode == BLEND_ALPHA)
    setBlendModeAlpha();
  else if (mode == BLEND_ADD)
    setBlendModeAdd();
  else if (mode == BLEND_MULTI)
    setBlendModeMulti();
  else if (mode == BLEND_SCREEN)
    setBlendModeScreen();
  else if (mode == BLEND_REVERSE)
    setBlendModeReverse();
  else if (mode == BLEND_REVERSE2)
    setBlendModeReverse2();
  else if (mode == BLEND_ALPHA_SEPARATED)
    setBlendModeSeparatedAlpha();
}

inline void setViewport(GLsizei x, GLsizei y, GLsizei width, GLsizei height) {
  glViewport(x, y, width, height);
}

inline std::vector<GLint> getViewport() {
  std::vector<GLint> viewport(4);
  glGetIntegerv(GL_VIEWPORT, viewport.data());
  return viewport;
}

inline int getNumMonitors() {
  int count;
  glfwGetMonitors(&count);
  return count;
}

inline glm::vec2 getScreenSize(int index = 0) {
  int count;
  GLFWmonitor** monitors = glfwGetMonitors(&count);
  if (count > index) {
    GLFWmonitor* monitor = monitors[index];
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (mode) {
      float xscale, yscale;
      glfwGetMonitorContentScale(monitor, &xscale, &yscale);
      return {mode->width * xscale, mode->height * yscale};
    }
  }
  return glm::vec2();
}

static std::map<int, int> glfwKeyToAsciiMap = {{GLFW_KEY_SPACE, ' '},
                                               {GLFW_KEY_APOSTROPHE, '\''},
                                               {GLFW_KEY_COMMA, ','},
                                               {GLFW_KEY_MINUS, '-'},
                                               {GLFW_KEY_PERIOD, '.'},
                                               {GLFW_KEY_SLASH, '/'},
                                               {GLFW_KEY_1, '1'},
                                               {GLFW_KEY_2, '2'},
                                               {GLFW_KEY_3, '3'},
                                               {GLFW_KEY_4, '4'},
                                               {GLFW_KEY_5, '5'},
                                               {GLFW_KEY_6, '6'},
                                               {GLFW_KEY_7, '7'},
                                               {GLFW_KEY_8, '8'},
                                               {GLFW_KEY_9, '9'},
                                               {GLFW_KEY_SEMICOLON, ';'},
                                               {GLFW_KEY_EQUAL, '='},
                                               {GLFW_KEY_0, '0'},
                                               {GLFW_KEY_A, 'a'},
                                               {GLFW_KEY_B, 'b'},
                                               {GLFW_KEY_C, 'c'},
                                               {GLFW_KEY_D, 'd'},
                                               {GLFW_KEY_E, 'e'},
                                               {GLFW_KEY_F, 'f'},
                                               {GLFW_KEY_G, 'g'},
                                               {GLFW_KEY_H, 'h'},
                                               {GLFW_KEY_I, 'i'},
                                               {GLFW_KEY_J, 'j'},
                                               {GLFW_KEY_K, 'k'},
                                               {GLFW_KEY_L, 'l'},
                                               {GLFW_KEY_M, 'm'},
                                               {GLFW_KEY_N, 'n'},
                                               {GLFW_KEY_O, 'o'},
                                               {GLFW_KEY_P, 'p'},
                                               {GLFW_KEY_Q, 'q'},
                                               {GLFW_KEY_R, 'r'},
                                               {GLFW_KEY_S, 's'},
                                               {GLFW_KEY_T, 't'},
                                               {GLFW_KEY_U, 'u'},
                                               {GLFW_KEY_V, 'v'},
                                               {GLFW_KEY_W, 'w'},
                                               {GLFW_KEY_X, 'x'},
                                               {GLFW_KEY_Y, 'y'},
                                               {GLFW_KEY_Z, 'z'},
                                               {GLFW_KEY_LEFT_BRACKET, '['},
                                               {GLFW_KEY_BACKSLASH, '\\'},
                                               {GLFW_KEY_RIGHT_BRACKET, ']'},
                                               {GLFW_KEY_GRAVE_ACCENT, '/'}};

template <typename PixelType>
static GLenum getGLInternalFormat(size_t channels) {
  size_t size_in_bytes = sizeof(PixelType);
  if (std::is_integral<PixelType>::value) {
    switch (channels) {
      case 1:
        switch (size_in_bytes) {
          case 1:
            return GL_R8;
          case 2:
            return GL_R16I;
          case 4:
            return GL_R32I;
        }
      case 2:
        switch (size_in_bytes) {
          case 1:
            return GL_RG8;
          case 2:
            return GL_RG16I;
          case 4:
            return GL_RG32I;
        }
      case 3:
        switch (size_in_bytes) {
          case 1:
            return GL_RGB8;
          // There is no GL_RGB16I
          case 4:
            return GL_RGB32I;
        }
      case 4:
        switch (size_in_bytes) {
          case 1:
            return GL_RGBA8;
          case 2:
            return GL_RGBA16I;
          case 4:
            return GL_RGBA32I;
        }
    }
  }

  // Check for float type
  else if (std::is_floating_point<PixelType>::value) {
    switch (channels) {
      case 1:
        return GL_R32F;
      case 2:
        return GL_RG32F;
      case 3:
        return GL_RGB32F;
      case 4:
        return GL_RGBA32F;
    }
  }

  log::warn("getGLInternalFormat()") << "unsupported format " << log::end();
  return 0;  // Error case: unsupported number of channels
}

static GLenum getGLFormatFromInternal(GLenum internal_format) {
  switch (internal_format) {
    case GL_R8:
    case GL_R16:
    case GL_R16F:
    case GL_R32F:
      return GL_RED;

    case GL_R8I:
    case GL_R8UI:
    case GL_R16I:
    case GL_R16UI:
    case GL_R32I:
    case GL_R32UI:
      return GL_RED_INTEGER;

    case GL_RG8:
    case GL_RG16:
    case GL_RG16F:
    case GL_RG32F:
      return GL_RG;

    case GL_RG8I:
    case GL_RG8UI:
    case GL_RG16I:
    case GL_RG16UI:
    case GL_RG32I:
    case GL_RG32UI:
      return GL_RG_INTEGER;

    case GL_RGB8:
    case GL_RGB16:
    case GL_RGB16F:
    case GL_RGB32F:
      return GL_RGB;

    case GL_RGB8I:
    case GL_RGB8UI:
    case GL_RGB16I:
    case GL_RGB16UI:
    case GL_RGB32I:
    case GL_RGB32UI:
      return GL_RGB_INTEGER;

    case GL_RGBA8:
    case GL_RGBA16:
    case GL_RGBA16F:
    case GL_RGBA32F:
      return GL_RGBA;

    case GL_RGBA8I:
    case GL_RGBA8UI:
    case GL_RGBA16I:
    case GL_RGBA16UI:
    case GL_RGBA32I:
    case GL_RGBA32UI:
      return GL_RGBA_INTEGER;

    case GL_DEPTH_STENCIL:
    case GL_DEPTH24_STENCIL8:
      return GL_DEPTH_STENCIL;

    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32:
      return GL_DEPTH_COMPONENT;

    case GL_STENCIL_INDEX:
    case GL_STENCIL_INDEX8:
      return GL_STENCIL_INDEX;

    default:
      log::warn("GetGLFormatFromInternal()")
          << "unknown internal format " << internal_format
          << ", returning GL_RGBA" << log::end();
      return GL_RGBA8;
  }
}

static GLenum getGLTypeFromInternal(GLenum internal_format) {
  switch (internal_format) {
    case GL_R8:
    case GL_RG8:
    case GL_RGB8:
    case GL_RGBA8:
      return GL_UNSIGNED_BYTE;

      // normalized to 0-1
    case GL_R16:
    case GL_RG16:
    case GL_RGB16:
    case GL_RGBA16:
      return GL_UNSIGNED_SHORT;

    case GL_R16UI:
    case GL_RG16UI:
    case GL_RGB16UI:
    case GL_RGBA16UI:
      return GL_UNSIGNED_SHORT;

    case GL_R16I:
    case GL_RG16I:
    case GL_RGB16I:
    case GL_RGBA16I:
      return GL_SHORT;

    case GL_R32I:
    case GL_RG32I:
    case GL_RGB32I:
    case GL_RGBA32I:
      return GL_INT;

    case GL_R16F:
    case GL_RG16F:
    case GL_RGB16F:
    case GL_RGBA16F:
      return GL_HALF_FLOAT;

    case GL_R32F:
    case GL_RG32F:
    case GL_RGB32F:
    case GL_RGBA32F:
      return GL_FLOAT;

    case GL_DEPTH_STENCIL:
    case GL_DEPTH24_STENCIL8:
      return GL_UNSIGNED_INT_24_8;

    case GL_DEPTH_COMPONENT16:
      return GL_UNSIGNED_SHORT;

    case GL_DEPTH_COMPONENT:
    case GL_DEPTH_COMPONENT24:
      return GL_UNSIGNED_INT;  // GL_FLOAT;

    case GL_DEPTH_COMPONENT32:
      return GL_UNSIGNED_INT;

    case GL_DEPTH_COMPONENT32F:
      return GL_FLOAT;

    case GL_STENCIL_INDEX:
    case GL_STENCIL_INDEX8:
      return GL_UNSIGNED_BYTE;

    default:
      log::warn("GetGlTypeFromInternal()")
          << "unknown internal format " << internal_format
          << ", returning GL_UNSIGNED_BYTE" << log::end();
      return GL_UNSIGNED_BYTE;
  }
}

static uint getNumChannelsFromFormat(GLenum format) {
  uint num_channels = 0;
  switch (format) {
    case GL_RED:
    case GL_RED_INTEGER:
    case GL_DEPTH_COMPONENT:
    case GL_STENCIL_INDEX:
      num_channels = 1;
      break;
    case GL_RG:
    case GL_RG_INTEGER:
    case GL_DEPTH_STENCIL:
      num_channels = 2;
    case GL_RGB:
    case GL_RGB_INTEGER:
    case GL_BGR:
      num_channels = 3;
      break;
    case GL_RGBA:
    case GL_RGBA_INTEGER:
    case GL_BGRA:
      num_channels = 4;
      break;
    default:
      log::warn("getNumChannelsFromInternal()")
          << "format " << format << " is unsupported." << log::end();
  }
  return num_channels;
}

static uint getNumChannelsFromInternal(GLenum internal_format) {
  GLenum format = getGLFormatFromInternal(internal_format);
  return getNumChannelsFromFormat(format);
}

static size_t getByteOfType(GLenum internal_format) {
  size_t byte = 0;
  GLenum type = getGLTypeFromInternal(internal_format);
  switch (type) {
    case GL_BYTE:
      byte = sizeof(GLbyte);
      break;
    case GL_UNSIGNED_BYTE:
      byte = sizeof(GLubyte);
      break;
    case GL_SHORT:
      byte = sizeof(GLshort);
      break;
    case GL_UNSIGNED_SHORT:
      byte = sizeof(GLushort);
      break;
    case GL_INT:
      byte = sizeof(GLint);
      break;
    case GL_UNSIGNED_INT:
      byte = sizeof(GLuint);
      break;
    case GL_HALF_FLOAT:
      byte = sizeof(GLhalf);
      break;
    case GL_FLOAT:
      byte = sizeof(GLfloat);
      break;
    case GL_DOUBLE:
      byte = sizeof(GL_DOUBLE);
      break;
    default:
      log::warn("getByteOfType()")
          << "type " << type << " is unsupported." << log::end();
  }
  return byte;
}

static std::string getStringFromInternal(GLenum format) {
  static std::map<GLenum, std::string> formats_map = {
      {GL_RGBA8, "GL_RGBA8"},     {GL_RGBA16, "GL_RGBA16"},
      {GL_RGBA16F, "GL_RGBA16F"}, {GL_RGBA32F, "GL_RGBA32F"},
      {GL_RG8, "GL_RG8"},         {GL_RG16, "GL_RG16"},
      {GL_RG16F, "GL_RG16F"},     {GL_RG32F, "GL_RG32F"},
      {GL_RGB8, "GL_RGB8"},       {GL_RGB16, "GL_RGB16"},
      {GL_R8, "GL_R8"},           {GL_R16, "GL_R16"},
      {GL_R16F, "GL_R16F"},       {GL_R32F, "GL_R32F"},
      {GL_R8I, "GL_R8I"},         {GL_R16I, "GL_R16I"},
      {GL_R32I, "GL_R32I"}};

  auto iter = formats_map.find(format);
  if (iter == formats_map.end()) {
    return "Unknown format";
  }
  return iter->second;
}

static size_t getByteOfPixel(GLenum internal_format) {
  return getNumChannelsFromInternal(internal_format) *
         getByteOfType(internal_format);
}

}  // namespace gl
}  // namespace limas
