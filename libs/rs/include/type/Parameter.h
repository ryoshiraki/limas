#pragma once
#include <typeindex>

#include "system/Exception.h"
#include "system/Noncopyable.h"
#include "system/Observable.h"
#include "type/BoundedValue.h"
#include "utils/Json.h"
#include "utils/Utils.h"

namespace rs {

template <typename T>
class Parameter;

class AbstractParameter {
 public:
  virtual ~AbstractParameter() {}

  template <typename T>
  Parameter<T>& as() {
    if (getType() != typeid(T)) {
      throw rs::Exception("Type mismatch during casting " + getName());
    }
    return static_cast<Parameter<T>&>(*this);
  }

  template <typename T>
  Parameter<T>* asPtr() {
    if (getType() != typeid(T)) {
      return nullptr;
    }
    return static_cast<Parameter<T>*>(this);
  }

  virtual void setName(const std::string& name) = 0;
  virtual const std::string& getName() const = 0;
  virtual std::type_index getType() const = 0;
  virtual json serialize() const = 0;
  virtual void deserialize(const json& j) = 0;

 protected:
  AbstractParameter() {}
};

template <class T>
class Parameter : public AbstractParameter {
  struct Data {
    std::string name_;
    BoundedValue<T> value_;
    Observable<const T> updated_;
    Connection conn_;
  };

 public:
  Parameter() { data_ = std::make_shared<Data>(); }
  Parameter(const Parameter<T>& param) : data_(param.data_) {}
  Parameter(const std::shared_ptr<Data>& data) : data_(data) {}

  Parameter(const std::string& name, const std::optional<T>& value = {},
            const std::optional<T>& min = {},
            const std::optional<T>& max = {}) {
    data_ = std::make_shared<Data>();
    data_->name_ = name;
    if (value) data_->value_.setValue(*value);
    if (min) data_->value_.setMin(*min);
    if (max) data_->value_.setMax(*max);
  }

  void setName(const std::string& name) override { data_->name_ = name; };
  const std::string& getName() const override { return data_->name_; }

  json serialize() const override {
    json j;
    j["name"] = getName();
    j["value"] = getValue();
    j["type"] = typeid(T).name();
    if (hasMin()) j["min"] = getMin();
    if (hasMax()) j["max"] = getMax();
    return j;
  }

  void deserialize(const json& j) override {
    if (j.contains("type")) {
      if (j["type"].get<std::string>() != typeid(T).name()) {
        throw rs::Exception("Type mismatch during deserialization " +
                            getName());
      }
    } else {
      throw rs::Exception("Serialized data missing 'type' field.");
    }

    if (j.contains("name")) setName(j["name"].get<std::string>());
    if (j.contains("value")) setValue(j["value"].get<T>());
    if (j.contains("min")) setMin(j["min"].get<T>());
    if (j.contains("max")) setMax(j["max"].get<T>());
  }

  std::type_index getType() const override { return typeid(T); }

  Parameter<T>& setValue(const T& value) {
    applyChange([&]() { data_->value_.setValue(value); });
    return *this;
  }

  Parameter<T>& setMin(const T& value) {
    applyChange([&]() { data_->value_.setMin(value); });
    return *this;
  }

  Parameter<T>& setMax(const T& value) {
    applyChange([&]() { data_->value_.setMax(value); });
    return *this;
  }

  const T& getValue() const { return data_->value_.getValue(); }
  const T& getMin() const { return data_->value_.getMin(); }
  const T& getMax() const { return data_->value_.getMax(); }

  bool hasMin() const { return data_->value_.hasMin(); }
  bool hasMax() const { return data_->value_.hasMax(); }

  Parameter<T>& addListener(const std::function<void(const T&)>& callback) {
    removeListener();
    data_->conn_ = data_->updated_.addObserver(callback);
    return *this;
  }

  template <class ObserverClass>
  Parameter<T>& addListener(ObserverClass* observer,
                            void (ObserverClass::*callback)(const T&)) {
    return addListener(std::bind(callback, observer, std::placeholders::_1));
  }

  void removeListener() { data_->conn_.disconnect(); }

  void callListener() {
    if (data_->conn_.isConnected()) {
      T v = data_->value_.getValue();
      data_->updated_.notify(v);
    }
  }

  Parameter<T>& operator=(const T& value) {
    setValue(value);
    return *this;
  }

 protected:
  std::shared_ptr<Data> data_;

  void applyChange(std::function<void()> action) {
    T cur_val = data_->value_.getValue();
    action();
    T next_val = data_->value_.getValue();
    if (cur_val != next_val) {
      if (data_->conn_.isConnected()) data_->updated_.notify(next_val);
    }
  }
};

template <typename U>
inline std::ostream& operator<<(std::ostream& os, const Parameter<U>& param) {
  os << param.serialize().dump(2);
  return os;
}

}  // namespace rs