#include <GL/glew.h>
#include <GLFW/glfw3.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

using namespace std;

#include <algorithm>
#include <any>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <exception>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <thread>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include "boost/algorithm/string.hpp"
#include "boost/array.hpp"
#include "boost/asio.hpp"
#include "boost/compute.hpp"
#include "boost/compute/interop/opengl.hpp"
#include "boost/compute/utility/dim.hpp"
#include "boost/noncopyable.hpp"
#include "boost/signals2.hpp"
#include "boost/uuid/uuid.hpp"
#include "boost/uuid/uuid_generators.hpp"
#include "boost/uuid/uuid_io.hpp"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "glm/gtx/string_cast.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtx/vector_angle.hpp"
