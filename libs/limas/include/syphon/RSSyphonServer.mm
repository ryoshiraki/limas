#include "RSSyphonServer.h"
#import <Syphon/Syphon.h>

using namespace limas;

RSSyphonServer::RSSyphonServer() {}

void RSSyphonServer::setName(const std::string &name) {
  @autoreleasepool {
    NSString *title =
        [NSString stringWithCString:name.c_str()
                           encoding:[NSString defaultCStringEncoding]];

    if (!syphon_) {
      SNOSet(syphon_,
                [[SyphonOpenGLServer alloc] initWithName:title
                                                 context:CGLGetCurrentContext()
                                                 options:nil]);
    } else {
      [(SyphonOpenGLServer *)SNOGet(syphon_) setName:title];
    }
  }
}

void RSSyphonServer::publish(GLuint id, GLenum target, GLsizei width,
                             GLsizei height, bool b_flipped) {
  @autoreleasepool {
    if (!syphon_) {
      SNOSet(syphon_,
                [[SyphonOpenGLServer alloc] initWithName:@"Untitled"
                                                 context:CGLGetCurrentContext()
                                                 options:nil]);
    }

    [(SyphonOpenGLServer *)SNOGet(syphon_)
        publishFrameTexture:id
              textureTarget:target
                imageRegion:NSMakeRect(0, 0, width, height)
          textureDimensions:NSMakeSize(width, height)
                    flipped:!b_flipped];
  }
}