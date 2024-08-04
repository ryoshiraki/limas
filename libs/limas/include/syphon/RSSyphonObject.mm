
#include "RSSyphonObject.h"

RSSyphonObject::RSSyphonObject()
: mObject(nil)
{
    
}

RSSyphonObject::~RSSyphonObject()
{
    // This can be called from C++ with no autorelease pool in place, so we need one
    @autoreleasepool {
        // Do this explicitely here to force ARC release
        mObject = nil;
    }
}

RSSyphonObject::RSSyphonObject(const RSSyphonObject &o)
: mObject(o.mObject)
{
    
}

RSSyphonObject & RSSyphonObject::operator=(const RSSyphonObject &o)
{
    if (&o != this)
    {
        // This can be called from C++ with no autorelease pool in place, so we need one
        @autoreleasepool {
            mObject = o.mObject;
        }
    }
    return *this;
}

RSSyphonObject::operator bool() const
{
    return mObject ? true : false;
}

RSSyphonObject SNOMake(RSSyphonObject::ObjectType obj)
{
    RSSyphonObject dst;
    dst.mObject = obj;
    return dst;
}

void SNOSet(RSSyphonObject &dst, RSSyphonObject::ObjectType obj)
{
    dst.mObject = obj;
}

RSSyphonObject::ObjectType SNOGet(const RSSyphonObject &o)
{
    return o.mObject;
}