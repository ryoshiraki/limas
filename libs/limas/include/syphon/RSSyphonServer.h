#include "RSSyphonObject.h"

namespace limas {
class RSSyphonServer {
 public:
  RSSyphonServer();
  void setName(const std::string &name);
  void publish(GLuint id, GLenum target, GLsizei width, GLsizei height,
               bool b_flippe);

 protected:
  RSSyphonObject syphon_;
};
}  // namespace limas