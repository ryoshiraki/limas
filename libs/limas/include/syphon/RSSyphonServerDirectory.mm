#include "RSSyphonServerDirectory.h"
#import <Syphon/Syphon.h>

// CFNotificationCallback implementation

using namespace limas;

static void notificationHandler(CFNotificationCenterRef center, void *observer, CFStringRef name, const void *object, CFDictionaryRef user_info) {

    (static_cast<RSSyphonServerDirectory *>(observer))->handleNotification(name, user_info);
}

// RSSyphonServerDirectory implementation

RSSyphonServerDirectory::RSSyphonServerDirectory()
{
    addObservers();
    refresh(true);
}

RSSyphonServerDirectory::~RSSyphonServerDirectory()
{
    removeObservers();
}

// Our workaround for the incomplete CFNotification.  There's just no love for Core Foundation anymore.
void RSSyphonServerDirectory::refresh(bool is_announce){
    vector<RSSyphonServerDescription> event_args;

    @autoreleasepool {
        for(NSDictionary* serverDescription in [[SyphonServerDirectory sharedDirectory] servers])
        {
            NSString* name = [serverDescription objectForKey:SyphonServerDescriptionNameKey];
            NSString* app_name = [serverDescription objectForKey:SyphonServerDescriptionAppNameKey];
            NSString *uuid = [serverDescription objectForKey:SyphonServerDescriptionUUIDKey];
            NSImage* app_image = [serverDescription objectForKey:SyphonServerDescriptionIconKey];
            
            NSString *title = [NSString stringWithString:app_name];
            
            if(is_announce){
                bool exists = serverExists([name UTF8String], [app_name UTF8String]);
                if(!exists){
                    RSSyphonServerDescription sy = RSSyphonServerDescription(std::string([name UTF8String]),std::string([app_name UTF8String]));
                    servers_.push_back(sy);
                    event_args.push_back(sy);
                }
            } else {
                event_args.push_back(RSSyphonServerDescription(std::string([name UTF8String]),std::string([app_name UTF8String])));
            }
        }
    }
    
    if(!is_announce){
        vector<RSSyphonServerDescription> found_servers = event_args;
        event_args.clear();
        for(vector<RSSyphonServerDescription>::iterator it = servers_.begin(); it != servers_.end(); ++it){
            if(std::find(found_servers.begin(), found_servers.end(), RSSyphonServerDescription(it->server_name_, it->app_name_)) == found_servers.end()){
                event_args.push_back(RSSyphonServerDescription(it->server_name_, it->app_name_));
                //cout<<"Removing server: "<<it->serverName<<" app_name: "<<it->app_name<<"\n";
            }
        }
        servers_ = found_servers;
    }
    
    RSSyphonServerDirectoryEventArgs args;
    args.servers_ = event_args;

    if(is_announce){
        // ofNotifyEvent(events_.server_announced_, args, this);
    } else {        
        // ofNotifyEvent(events_.server_retired_, args, this);
    }

}

bool RSSyphonServerDirectory::serverExists(const RSSyphonServerDescription &_server){
    for(auto& s: servers_){
        if(s == _server)
            return true;
    }
    
    return false;
}

bool RSSyphonServerDirectory::serverExists(const std::string &server_name, const std::string &app_name){
    return serverExists(RSSyphonServerDescription(server_name, app_name));
}

const RSSyphonServerDescription& RSSyphonServerDirectory::getDescription(int _idx){
    return servers_.at(_idx);
}

const vector<RSSyphonServerDescription>& RSSyphonServerDirectory::getServerList(){
    return servers_;
}

// Unfortunately userInfo is null when dealing with CFNotifications from a Darwin notification center.  This is one of the few non-toll-free bridges between CF and NS.  Otherwise this class would be far less complicated.
void RSSyphonServerDirectory::handleNotification(CFStringRef name, CFDictionaryRef user_info){
    if((__bridge NSString*)name == SyphonServerAnnounceNotification){
        serverAnnounced();
    } else if((__bridge NSString*)name == SyphonServerUpdateNotification){
        serverUpdated();
    } else if((__bridge NSString*)name == SyphonServerRetireNotification){
        serverRetired();
    }
}

void RSSyphonServerDirectory::serverAnnounced(){
    refresh(true);
}

void RSSyphonServerDirectory::serverUpdated(){
    //
}

void RSSyphonServerDirectory::serverRetired(){
    refresh(false);
}

void RSSyphonServerDirectory::addObservers(){
    CFNotificationCenterAddObserver
    (
     CFNotificationCenterGetLocalCenter(),
     this,
     (CFNotificationCallback)&notificationHandler,
     (CFStringRef)SyphonServerAnnounceNotification,
     NULL,
     CFNotificationSuspensionBehaviorDeliverImmediately
     );
    
    CFNotificationCenterAddObserver
    (
     CFNotificationCenterGetLocalCenter(),
     this,
     (CFNotificationCallback)&notificationHandler,
     (CFStringRef)SyphonServerUpdateNotification,
     NULL,
     CFNotificationSuspensionBehaviorDeliverImmediately
     );
    
    CFNotificationCenterAddObserver
    (
     CFNotificationCenterGetLocalCenter(),
     this,
     (CFNotificationCallback)&notificationHandler,
     (CFStringRef)SyphonServerRetireNotification,
     NULL,
     CFNotificationSuspensionBehaviorDeliverImmediately
     );
     
    // events_.server_announced_.addObserver(this, args);
    // events_.server_retired_.addObserver(this, args);
}

void RSSyphonServerDirectory::removeObservers(){
    CFNotificationCenterRemoveEveryObserver(CFNotificationCenterGetLocalCenter(), this);
}

bool RSSyphonServerDirectory::CFStringRefToString(CFStringRef src, std::string &dest){
    const char *cstr = CFStringGetCStringPtr(src, CFStringGetSystemEncoding());
    if (!cstr)
    {
        CFIndex strLen = CFStringGetMaximumSizeForEncoding(CFStringGetLength(src) + 1,
                                                           CFStringGetSystemEncoding());
        char *allocStr = (char*)malloc(strLen);
        
        if(!allocStr)
            return false;
        
        if(!CFStringGetCString(src, allocStr, strLen, CFStringGetSystemEncoding()))
        {
            free((void*)allocStr);
            return false;
        }
        
        dest = allocStr;
        free((void*)allocStr);
        
        return true;
    }
    
    dest = cstr;
    return true;
}