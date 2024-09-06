#pragma once
#include "type/Parameter.h"

namespace limas {

class ParameterGroup {
  struct Data {
    std::string name_;
    std::vector<std::shared_ptr<AbstractParameter>> params_;
    std::vector<std::shared_ptr<Data>> children_;
  };

  auto findParam(const std::string& name) const {
    return std::find_if(
        data_->params_.begin(), data_->params_.end(),
        [&name](const std::shared_ptr<AbstractParameter>& param) {
          return param->getName() == name;
        });
  }

  auto findChild(const std::string& name) const {
    return std::find_if(data_->children_.begin(), data_->children_.end(),
                        [&name](const std::shared_ptr<Data>& child) {
                          return child->name_ == name;
                        });
  }

 public:
  ParameterGroup() { data_ = std::make_shared<Data>(); }
  ParameterGroup(const ParameterGroup& group) : data_(group.data_) {}
  ParameterGroup(const std::shared_ptr<Data>& data) : data_(data) {}

  void setName(const std::string& name) { data_->name_ = name; }
  const std::string& getName() const {
    if (data_->name_.empty()) {
      static std::string name =
          "##unnamed_" + std::to_string(reinterpret_cast<uintptr_t>(this));
      return name;
    }
    return data_->name_;
  }

  ParameterGroup addChild(const ParameterGroup& child) {
    if (hasChild(child.getName())) {
      log::warn("ParameterGroup")
          << "Adding another child with same name '" << child.getName()
          << "' to group '" << getName() << "'" << log::end();
    }

    data_->children_.push_back(child.data_);
    return child;
  }

  ParameterGroup addChild(const std::string& name) {
    auto child = ParameterGroup();
    child.setName(name);
    return addChild(child);
  }

  ParameterGroup getChild(const std::string& name) {
    auto it = findChild(name);
    if (it == data_->children_.end())
      throw limas::Exception("Child '" + name + "'not found");
    return ParameterGroup(*it);
  }

  std::vector<ParameterGroup> getChildren() const {
    std::vector<ParameterGroup> children;
    for (auto& child : data_->children_) {
      children.emplace_back(ParameterGroup(child));
    }
    return children;
  }

  template <typename T>
  Parameter<T>& addParam(const Parameter<T>& param) {
    if (hasParam(param.getName())) {
      log::warn("ParameterGroup")
          << "Adding another parameter with same name '" << param.getName()
          << "' to group '" << getName() << "'" << log::end();
    }

    auto shared_param = std::make_shared<Parameter<T>>(param);
    data_->params_.push_back(shared_param);
    return *shared_param;
  }

  template <typename T>
  Parameter<T>& addParam(const std::string& name,
                         const std::optional<T>& value = {},
                         const std::optional<T>& min = {},
                         const std::optional<T>& max = {}) {
    return addParam(Parameter<T>(name, value, min, max));
  }

  bool removeParam(const std::string& name) {
    // erase first one
    auto it = findParam(name);
    if (it != data_->params_.end()) {
      it = data_->params_.erase(it);
      return true;
    }
    return false;
  }

  template <typename T>
  Parameter<T>& getParam(const std::string& name) {
    auto it = findParam(name);
    if (it == data_->params_.end())
      throw limas::Exception("Parameter'" + name + "' not found");
    return (*it)->template as<T>();
  }

  std::vector<std::shared_ptr<AbstractParameter>>& getParams() {
    return data_->params_;
  }

  template <typename T>
  void setValue(const std::string& name, const T& value) {
    getParam<T>(name).setValue(value);
  }

  template <typename T>
  T getValue(const std::string& name) {
    return getParam<T>(name).getValue();
  }

  std::type_index getType(const std::string& name) const {
    auto it = findParam(name);
    if (it == data_->params_.end())
      throw limas::Exception("Parameter'" + name + "' not found");
    return (*it)->getType();
  }

  bool hasChild(const std::string& name) const {
    return findChild(name) != data_->children_.end();
  }

  bool hasParam(const std::string& name) const {
    return findParam(name) != data_->params_.end();
  }

  json serialize() const {
    json j;

    for (const auto& param : data_->params_) {
      j["parameters"][param->getName()] = param->serialize();
    }

    for (const auto& child : data_->children_) {
      j["children"][child->name_] = ParameterGroup(child).serialize();
    }

    return j;
  }

  void deserialize(const json& j) {
    if (j.contains("parameters")) {
      for (const auto& [name, data] : j["parameters"].items()) {
        auto type = data["type"].get<std::string>();

        if (type == typeid(bool).name()) {
          auto& param = addParam<bool>(name);
          param.deserialize(data);
        } else if (type == typeid(char).name()) {
          auto& param = addParam<char>(name);
          param.deserialize(data);
        } else if (type == typeid(int).name()) {
          auto& param = addParam<int>(name);
          param.deserialize(data);
        } else if (type == typeid(float).name()) {
          auto& param = addParam<float>(name);
          param.deserialize(data);
        } else if (type == typeid(std::string).name()) {
          auto& param = addParam<std::string>(name);
          param.deserialize(data);
        } else if (type == typeid(glm::vec2).name()) {
          auto& param = addParam<glm::vec2>(name);
          param.deserialize(data);
        } else if (type == typeid(glm::vec3).name()) {
          auto& param = addParam<glm::vec3>(name);
          param.deserialize(data);
        } else if (type == typeid(glm::vec4).name()) {
          auto& param = addParam<glm::vec4>(name);
          param.deserialize(data);
        } else {
          log::warn("ParameterGroup")
              << "Type '" << type << "' is not supported to deserialize"
              << log::end();
        }
      }
    }

    if (j.contains("children")) {
      for (const auto& [name, data] : j["children"].items()) {
        auto group = addChild(name);
        group.deserialize(data);
      }
    }
  }

 protected:
  std::shared_ptr<Data> data_;
};

inline std::ostream& operator<<(std::ostream& os, const ParameterGroup& group) {
  os << group.serialize().dump(2);
  return os;
}

}  // namespace limas