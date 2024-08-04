#include "app/AppUtils.h"

#include "app/BaseApp.h"
#include "gl/Context.h"
#include "gl/Renderer.h"
#include "system/Observable.h"
#include "system/Singleton.h"

namespace limas {
namespace app {

std::shared_ptr<BaseApp>& getPtr() {
  return Singleton<std::shared_ptr<BaseApp>>::getInstance();
}

std::shared_ptr<Window>& getMainWindow() { return getPtr()->getMainWindow(); }

std::shared_ptr<gl::Renderer>& getRenderer() { return getPtr()->getRenderer(); }
gl::Context* getContext() { return getPtr()->getContext(); }

int getFrameNumber() { return getPtr()->getFrameNumber(); }
double getElapsedTimeInSeconds() { return getPtr()->getElapsedTimeInSeconds(); }

int getWidth() { return getPtr()->getWidth(); }
int getHeight() { return getPtr()->getHeight(); }
glm::vec2 getWindowSize() { return getPtr()->getWindowSize(); }

float getMouseX() { return getPtr()->getMouseX(); }
float getMouseY() { return getPtr()->getMouseY(); }
glm::vec2 getMousePos() { return getPtr()->getMousePos(); }

template <class ObserverClass>
Connection setOnLoopBegin(ObserverClass* observer,
                          void (ObserverClass::*callback)(const EventArgs&)) {
  return getPtr()->getWindows()[0]->setOnLoopBegin(observer, callback);
}

template <class ObserverClass>
Connection setOnMousePressed(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const EventArgs&)) {
  return getPtr()->getWindows()[0]->setOnMousePressed(observer, callback);
}

template <class ObserverClass>
Connection setOnMouseMoved(ObserverClass* observer,
                           void (ObserverClass::*callback)(const EventArgs&)) {
  return getPtr()->getWindows()[0]->setOnMouseMoved(observer, callback);
}

template <class ObserverClass>
Connection setOnMouseReleased(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const EventArgs&)) {
  return getPtr()->getWindows()[0]->setOnMouseReleased(observer, callback);
}

template <class ObserverClass>
Connection setOnKeyPressed(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const KeyEventArgs&)) {
  return getPtr()->getWindows()[0]->setOnKeyPressed(observer, callback);
}

template <class ObserverClass>
Connection setOnKeyReleased(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const KeyEventArgs&)) {
  return getPtr()->getWindows()[0]->setOnKeyReleased(observer, callback);
}

void exit() { return getPtr()->exit(); }

}  // namespace app
}  // namespace limas
