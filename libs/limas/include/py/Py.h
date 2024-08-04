#pragma once

#include "py/PyObject.h"
#include "system/Future.h"
#include "system/Logger.h"
#include "system/Observable.h"
#include "system/Singleton.h"

namespace rs {

class Py {
  class LibraryManager {
    friend class Singleton<LibraryManager>;

   public:
    ~LibraryManager() { Py_Finalize(); }

   private:
    LibraryManager() {
      Py_Initialize();
      np::initialize();
      log::info("Python") << Py_GetVersion() << log::end();
    }
  };

  LibraryManager& initialize() {
    return Singleton<LibraryManager>::getInstance();
  }

 public:
  bool load(const std::string& script_path) {
    initialize();

    script_path_ = script_path;
    try {
      main_module_ = py::import("__main__");
      main_namespace_ = main_module_.attr("__dict__");

      std::ifstream ifs(script_path_);
      std::stringstream buffer;
      buffer << "import os\n";
      buffer << "os.chdir(\"" << fs::getParent(script_path) << "\")\n";
      buffer << ifs.rdbuf();

      py::exec(buffer.str().c_str(), main_namespace_);
    } catch (py::error_already_set const&) {
      PyErr_Print();
      return false;
    }
    return true;
  }

  bool reload() { return load(script_path_); }

  template <typename... Args>
  PyObject call(const std::string& function_name, Args... args) const {
    py::object py_func = main_namespace_[function_name];
    auto obj = Function<Args...>::call(py_func, args...);
    return obj;
  }

  // template <typename... Args>
  // Future<PyObject>::Ptr& callAsync(const std::string& function_name,
  //                                  Args... args) {
  //   auto promise = std::make_shared<std::promise<PyObject>>();
  //   wrapper = std::make_shared<Future<PyObject>>(promise->get_future());

  //   std::thread([=]() mutable {
  //     PyGILState_STATE state = PyGILState_Ensure();
  //     auto obj = call(function_name, args...);
  //     promise->set_value(obj);
  //     PyGILState_Release(state);
  //   }).detach();

  //   return wrapper;
  // }

  std::string getScriptPath() const { return script_path_; }

 private:
  template <typename... Args>
  struct Function {
    static PyObject call(const py::object& func, Args... args) {
      py::object result;
      try {
        result = func(args...);
        return result;
      } catch (py::error_already_set const&) {
        PyErr_Print();
      }
      return PyObject(result);
    }
  };

  template <typename T>
  struct Function<std::vector<T>> {
    static PyObject call(const py::object& func, void*) {
      py::object result;
      try {
        py::object py_res = func();
      } catch (py::error_already_set const&) {
        PyErr_Print();
      }
      return PyObject(result);
    }
  };

  template <>
  struct Function<void> {
    static PyObject call(const py::object& func, void*) {
      py::object result;
      try {
        py::object py_res = func();
      } catch (py::error_already_set const&) {
        PyErr_Print();
      }
      return PyObject(result);
    }
  };

  std::string script_path_;
  py::object main_module_;
  py::object main_namespace_;
  Observable<const PyObject> done_;
};

}  // namespace rs