#pragma once
#include "py/NdArray.h"

namespace limas {

namespace py = boost::python;
namespace np = boost::python::numpy;

class PyObject {
  template <typename T>
  struct Conv {
    static T conv(const py::object &obj) { return py::extract<T>(obj); }
  };

#pragma mark Convert to PyObject
  template <>
  struct Conv<PyObject> {
    static PyObject conv(const py::object &obj) { return PyObject(obj); }
  };

#pragma mark Convert to C++ Tuple
  // Base case for tuple recursion
  template <typename Tuple, std::size_t I = std::tuple_size<Tuple>::value,
            typename = void>
  struct TupleConv {
    static void conv(const py::list &pyList, Tuple &tuple) {}
  };

  // Recursive case for tuple recursion
  template <typename Tuple, std::size_t I>
  struct TupleConv<Tuple, I, std::enable_if_t<(I > 0)>> {
    static void conv(const py::list &list, Tuple &tuple) {
      std::get<I - 1>(tuple) =
          Conv<typename std::tuple_element<I - 1, Tuple>::type>::conv(
              list[I - 1]);
      TupleConv<Tuple, I - 1>::conv(list, tuple);
    }
  };

  template <typename... Types>
  struct Conv<std::tuple<Types...>> {
    static std::tuple<Types...> conv(const py::object &obj) {
      py::list list = py::extract<py::list>(obj);
      if (py::len(list) != sizeof...(Types)) {
        throw rs::Exception("Invalid length");
      }
      std::tuple<Types...> result;
      TupleConv<std::tuple<Types...>>::conv(list, result);
      return result;
    }
  };

#pragma mark Convert to GLM Vector
  template <typename T, glm::length_t L, glm::qualifier Q>
  struct Conv<glm::vec<L, T, Q>> {
    static glm::vec<L, T, Q> conv(const py::object &obj) {
      glm::vec<L, T, Q> result;
      py::list list = py::extract<py::list>(obj);
      if (py::len(list) != L) {
        throw rs::Exception("Invalid length");
      }
      for (glm::length_t i = 0; i < L; ++i) {
        result[i] = py::extract<T>(list[i]);
      }
      return result;
    }
  };

#pragma mark Convert to std::vector
  template <typename T>
  struct Conv<std::vector<T>> {
    static std::vector<T> conv(const py::object &obj) {
      std::vector<T> result;
      py::list list = py::extract<py::list>(obj);
      result.resize(py::len(list));
      for (int i = 0; i < result.size(); i++) {
        result[i] = Conv<T>::conv(list[i]);
      }
      return result;
    }
  };

#pragma mark Convert to 2d std::vector
  template <typename T>
  struct Conv<std::vector<std::vector<T>>> {
    static std::vector<std::vector<T>> conv(const py::object &obj) {
      std::vector<std::vector<T>> result;
      py::list list = py::extract<py::list>(obj);
      result.resize(py::len(list));
      for (int i = 0; i < result.size(); i++) {
        result[i] = Conv<std::vector<T>>::conv(list[i]);
      }
      return result;
    }
  };

#pragma mark Convert to 3d std::vector
  template <typename T>
  struct Conv<std::vector<std::vector<std::vector<T>>>> {
    static std::vector<std::vector<std::vector<T>>> conv(
        const py::object &obj) {
      std::vector<std::vector<std::vector<T>>> result;
      py::list list = py::extract<py::list>(obj);
      result.resize(py::len(list));
      for (int i = 0; i < result.size(); i++) {
        result[i] = Conv<std::vector<std::vector<T>>>::conv(list[i]);
      }
      return result;
    }
  };

#pragma mark Convert to std::map
  template <typename KeyType, typename ValueType>
  struct Conv<std::map<KeyType, ValueType>> {
    static std::map<KeyType, ValueType> conv(const py::object &obj) {
      py::dict dict = py::extract<py::dict>(obj);
      py::list keys = dict.keys();
      std::map<KeyType, ValueType> result;
      for (int i = 0; i < py::len(keys); ++i) {
        KeyType key = Conv<KeyType>::conv(keys[i]);
        ValueType value = Conv<ValueType>::conv(dict[keys[i]]);
        result.insert(std::make_pair(key, value));
      }
      return result;
    }
  };

 public:
  PyObject() {}  //= delete;
  PyObject(const py::object &obj) : obj_(obj) {}

  template <typename T>
  T convert() {
    T result;
    try {
      result = Conv<T>::conv(obj_);
      return result;
    } catch (const rs::Exception &e) {
      log::warn(e.what());
    } catch (py::error_already_set const &) {
      PyErr_Print();
    }
    return result;
  }

  template <>
  NdArray convert() {
    py::tuple shape = py::make_tuple(0);
    np::dtype dtype = np::dtype::get_builtin<double>();
    np::ndarray result = np::empty(shape, dtype);
    try {
      result = Conv<np::ndarray>::conv(obj_);
      return NdArray(result);
    } catch (const rs::Exception &e) {
      log::warn(e.what());
    } catch (py::error_already_set const &) {
      PyErr_Print();
    }
    return NdArray(result);
  }

 private:
  py::object obj_;
};

}  // namespace limas