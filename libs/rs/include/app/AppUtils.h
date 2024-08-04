#pragma once

namespace rs {

namespace gl {
class Renderer;
class Context;
}  // namespace gl

class BaseApp;
class Window;
class EventArgs;
class KeyEventArgs;
class Connection;

namespace app {

std::shared_ptr<BaseApp>& getPtr();
std::shared_ptr<gl::Renderer>& getRenderer();
std::shared_ptr<Window>& getMainWindow();

gl::Context* getContext();

int getFrameNumber();
double getElapsedTimeInSeconds();
int getWidth();
int getHeight();
glm::vec2 getMousePos();
float getMouseX();
float getMouseY();

template <class ObserverClass>
Connection setOnLoop(ObserverClass* observer,
                     void (ObserverClass::*callback)(const EventArgs&));

template <class ObserverClass>
Connection setOnMousePressed(ObserverClass* observer,
                             void (ObserverClass::*callback)(const EventArgs&));

template <class ObserverClass>
Connection setOnMouseMoved(ObserverClass* observer,
                           void (ObserverClass::*callback)(const EventArgs&));

template <class ObserverClass>
Connection setOnMouseReleased(
    ObserverClass* observer, void (ObserverClass::*callback)(const EventArgs&));

template <class ObserverClass>
Connection setOnKeyPressed(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const KeyEventArgs&));

template <class ObserverClass>
Connection setOnKeyReleased(
    ObserverClass* observer,
    void (ObserverClass::*callback)(const KeyEventArgs&));

void exit();

}  // namespace app
}  // namespace rs
