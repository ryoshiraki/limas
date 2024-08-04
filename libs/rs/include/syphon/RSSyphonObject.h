#pragma once

#ifdef __OBJC__
#include <Foundation/Foundation.h>
#endif

class RSSyphonObject {
 public:
#ifdef __OBJC__
  using ObjectType = id<NSObject>;
#else
  using ObjectType = void *;
#endif
  RSSyphonObject();
  ~RSSyphonObject();
  RSSyphonObject(const RSSyphonObject &o);
  RSSyphonObject &operator=(const RSSyphonObject &o);
  operator bool() const;
  // There is no Objective-C functionality on the class
  // so it is logically impossible to cause undefined behaviour from pure C++
  // code
  friend RSSyphonObject SNOMake(RSSyphonObject::ObjectType obj);
  friend void SNOSet(RSSyphonObject &dst, RSSyphonObject::ObjectType obj);
  friend RSSyphonObject::ObjectType SNOGet(const RSSyphonObject &o);

 private:
  ObjectType mObject;
};

#ifdef __OBJC__
RSSyphonObject SNOMake(RSSyphonObject::ObjectType obj);
void SNOSet(RSSyphonObject &dst, RSSyphonObject::ObjectType obj);
RSSyphonObject::ObjectType ofxSNOGet(const RSSyphonObject &o);
#endif
