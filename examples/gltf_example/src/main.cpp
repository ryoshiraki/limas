#include "App.h"
#include "AppUtils.h"

int main() {
  rs::Window::Settings settings;
  settings.x = 0;
  settings.y = 0;
  settings.width = 1024;
  settings.height = 768;
  settings.major = 4;
  settings.minor = 1;
  settings.visible = true;
  settings.decorated = true;
  settings.floating = false;
  settings.transparent = false;
  settings.red_bits = 8;
  settings.green_bits = 8;
  settings.blue_bits = 8;
  settings.alpha_bits = 8;
  settings.depth_bits = 24;
  settings.samples = 0;
  settings.doublebuffering = true;
  settings.hide_cursor = false;

  auto& app = rs::app::getPtr();
  app = std::make_shared<rs::App>(settings);
  app->run();

  return 0;
}
