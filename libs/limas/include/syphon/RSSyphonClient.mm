#include "RSSyphonClient.h"

#import <Syphon/Syphon.h>

#import "SyphonNameboundClient.h"

using namespace rs;

RSSyphonClient::RSSyphonClient() {}

// void RSSyphonClient::setup(const std::string &server_name,
//                            const std::string &app_name) {
//   @autoreleasepool {
//     client_ = SNOMake(
//         [[SyphonNameboundClient alloc]
//         initWithContext:CGLGetCurrentContext()]);

//   }
// }

void RSSyphonClient::setApplicationName(const std::string &app_name) {
  @autoreleasepool {
    client_ = SNOMake(
        [[SyphonNameboundClient alloc] initWithContext:CGLGetCurrentContext()]);

    /// not working
    // NSString *name = @"Simple Server";

    /// not working
    // NSString *name = [NSString stringWithCString: app_name.c_str()
    // encoding:NSUTF8StringEncoding];

    NSString *name =
        [[NSString alloc] initWithCString:app_name.c_str()
                                 encoding:NSMacOSRomanStringEncoding];

    [(SyphonNameboundClient *)SNOGet(client_) setAppName:name];
  }
}

void RSSyphonClient::setServerName(const std::string &server_name) {
  @autoreleasepool {
    client_ = SNOMake(
        [[SyphonNameboundClient alloc] initWithContext:CGLGetCurrentContext()]);

    NSString *name =
        [NSString stringWithCString:server_name.c_str()
                           encoding:[NSString defaultCStringEncoding]];

    if ([name length] == 0) name = nil;

    [(SyphonNameboundClient *)SNOGet(client_) setName:name];
  }
}

void RSSyphonClient::update() {
  @autoreleasepool {
    [(SyphonNameboundClient *)SNOGet(client_) lockClient];

    SyphonOpenGLClient *client =
        [(SyphonNameboundClient *)SNOGet(client_) client];
    SNOSet(latest_image_, [client newFrameImage]);
    NSSize tex_size = [(SyphonOpenGLImage *)SNOGet(latest_image_) textureSize];

    texture_.setExternalID(
        [(SyphonOpenGLImage *)SNOGet(latest_image_) textureName],
        GL_TEXTURE_RECTANGLE_ARB, tex_size.width, tex_size.height, 0, GL_RGBA8,
        GL_RGBA, GL_UNSIGNED_BYTE);

    [(SyphonNameboundClient *)SNOGet(client_) unlockClient];
    latest_image_ = RSSyphonObject();
  }
}
