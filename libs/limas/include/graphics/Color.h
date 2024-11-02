#pragma once

namespace limas {

struct Hsv {
  float data_[4];
  float& operator[](int i) { return data_[i]; }
  const float& operator[](int i) const { return data_[i]; }
};

struct Cmyk {
  float data_[4];
  float& operator[](int i) { return data_[i]; }
  const float& operator[](int i) const { return data_[i]; }
};

struct Yuv {
  float data_[3];
  float& operator[](int i) { return data_[i]; }
  const float& operator[](int i) const { return data_[i]; }
};

struct Brcosa {
  float data_[3];
  float& operator[](int i) { return data_[i]; }
  const float& operator[](int i) const { return data_[i]; }
};

template <typename T>
class BaseColor;

template <typename T>
class BaseColor {
 public:
  BaseColor() {
    data_[0] = 0;
    data_[1] = 0;
    data_[2] = 0;
    data_[3] = 0;
  }
  BaseColor(T r, T g, T b, T a) {
    data_[0] = r;
    data_[1] = g;
    data_[2] = b;
    data_[3] = a;
  }

  T getGrayScale() const {
    return (data_[0] * 0.299f + data_[1] * 0.587f + data_[2] * 0.114f);
  }

  BaseColor toGray() const {
    T gray = getGrayScale();
    return BaseColor(gray, gray, gray, 1);
  }

  glm::vec4 toVec() const {
    return glm::vec4(data_[0], data_[1], data_[2], data_[3]);
  }

  T r() const { return data_[0]; }
  T g() const { return data_[1]; }
  T b() const { return data_[2]; }
  T a() const { return data_[3]; }

  T* getData() { return data_; }

  T& operator[](int i) { return data_[i]; }
  const T& operator[](int i) const { return data_[i]; }

  BaseColor& operator+=(const BaseColor& other) {
    for (int i = 0; i < 4; ++i) {
      this->data_[i] += other[i];
    }
    return *this;
  }

  BaseColor& operator*=(float value) {
    for (int i = 0; i < 4; ++i) {
      this->data_[i] *= value;
    }
    return *this;
  }

  BaseColor operator*(float value) const {
    return BaseColor(this->data_[0] * value, this->data_[1] * value,
                     this->data_[2] * value, this->data_[3] * value);
  }

  static BaseColor lerp(const BaseColor& a, const BaseColor& b, float t) {
    return a * (1 - t) + b * t;
  }

 protected:
  T data_[4];
};

template <typename T>
BaseColor<T> operator+(BaseColor<T> lhs, const BaseColor<T>& rhs) {
  lhs += rhs;
  return lhs;
}

template <typename T>
BaseColor<T> operator*(BaseColor<T> lhs, float rhs) {
  lhs *= rhs;
  return lhs;
}

template <typename T>
BaseColor<T> operator*(float lhs, BaseColor<T> rhs) {
  rhs *= lhs;
  return rhs;
}

class FloatColor;
class CharColor;

class FloatColor : public BaseColor<float> {
 public:
  using BaseColor::BaseColor;

  FloatColor(const BaseColor<unsigned char>& other) {
    for (int i = 0; i < 4; ++i) {
      data_[i] = other[i] / 255.0f;
    }
  }

  FloatColor(const glm::vec4& vec) {
    data_[0] = vec.r;
    data_[1] = vec.g;
    data_[2] = vec.b;
    data_[3] = vec.a;
  }

  inline FloatColor& operator=(const BaseColor<unsigned char>& other) {
    for (int i = 0; i < 4; ++i) {
      data_[i] = other[i] / 255.0f;
    }
    return *this;
  }

  Hsv toHsv() const {
    float r = data_[0];
    float g = data_[1];
    float b = data_[2];
    float a = data_[3];

    float max = std::max({r, g, b});
    float min = std::min({r, g, b});
    float h = 0, s = 0, v = max;

    if (max != min) {
      float diff = max - min;
      s = diff / max;

      if (max == r) {
        h = (g - b) / diff * 60 + ((g >= b) ? 0 : 360);
      } else if (max == g) {
        h = (b - r) / diff * 60 + 120;
      } else if (max == b) {
        h = (r - g) / diff * 60 + 240;
      }
    }

    return Hsv{h / 360.0f, s, v, a};
  }

  Cmyk toCmyk() const {
    float r = data_[0];
    float g = data_[1];
    float b = data_[2];

    float c = 1 - r;
    float m = 1 - g;
    float y = 1 - b;
    float k = std::min({c, m, y});

    c = (c - k) / (1 - k);
    m = (m - k) / (1 - k);
    y = (y - k) / (1 - k);

    return Cmyk{c, m, y, k};
  }

  Yuv toYuv() const {
    float r = data_[0];
    float g = data_[1];
    float b = data_[2];

    float y = 0.299 * r + 0.587 * g + 0.114 * b;
    float u = -0.14713 * r - 0.28886 * g + 0.436 * b;
    float v = 0.615 * r - 0.51498 * g - 0.10001 * b;
    return Yuv{y, u, v};
  }

  FloatColor applyBrcosa(float brightness, float contrast,
                         float saturatoin) const {
    float gray = getGrayScale();
    FloatColor col = *this;
    for (int i = 0; i < 3; ++i) {
      col[i] = std::clamp(gray + saturatoin * (col[i] - gray), 0.0f, 1.0f);
      col[i] = std::clamp(((col[i] - 0.5f) * contrast) + 0.5f, 0.0f, 1.0f);
      col[i] = std::clamp(col[i] * brightness, 0.0f, 1.0f);
    }
    return col;
  }

  static FloatColor fromHex(const std::string& hex_code);

  static FloatColor fromHsv(const Hsv& hsv) {
    float h = fmod(hsv[0] * 360.0f, 360.0f);
    float s = hsv[1];
    float v = hsv[2];
    float a = hsv[3];

    int hi = static_cast<int>(h / 60) % 6;
    float f = h / 60 - hi;
    float p = v * (1 - s);
    float q = v * (1 - f * s);
    float t = v * (1 - (1 - f) * s);
    switch (hi) {
      case 0:
        return FloatColor{v, t, p, a};
      case 1:
        return FloatColor{q, v, p, a};
      case 2:
        return FloatColor{p, v, t, a};
      case 3:
        return FloatColor{p, q, v, a};
      case 4:
        return FloatColor{t, p, v, a};
      case 5:
        return FloatColor{v, p, q, a};
      default:
        return FloatColor{0, 0, 0, 0};  // this should never happen
    }
  }
  static FloatColor fromHsv(float h, float s, float v, float a = 1) {
    return fromHsv(Hsv{h, s, v, a});
  }

  static FloatColor fromCmyk(const Cmyk& cmyk) {
    float c = cmyk[0];
    float m = cmyk[1];
    float y = cmyk[2];
    float k = cmyk[3];
    float r = (1.0 - c) * (1.0 - k);
    float g = (1.0 - m) * (1.0 - k);
    float b = (1.0 - y) * (1.0 - k);
    return FloatColor{r, g, b, 1};
  }
  static FloatColor fromCmyk(float c, float m, float y, float k) {
    return fromCmyk(Cmyk{c, m, y, k});
  }

  static FloatColor fromYuv(const Yuv& yuv) {
    float y = yuv[0];
    float u = yuv[1];
    float v = yuv[2];

    float r = y + 1.140 * v;
    float g = y - 0.395 * u - 0.581 * v;
    float b = y + 2.032 * u;
    return FloatColor{r, g, b, 1};
  }
  static FloatColor fromYuv(float y, float u, float v) {
    return fromYuv(Yuv{y, u, v});
  }

  static FloatColor white() { return FloatColor{1, 1, 1, 1}; };
  static FloatColor silver() { return FloatColor{0.75, 0.75, 0.75, 1}; };
  static FloatColor gray() { return FloatColor{0.5, 0.5, 0.5, 1}; };
  static FloatColor black() { return FloatColor{0, 0, 0, 1}; };
  static FloatColor red() { return FloatColor{1, 0, 0, 1}; };
  static FloatColor maroon() { return FloatColor{0.5, 0, 0, 1}; };
  static FloatColor yellow() { return FloatColor{1, 1, 0, 1}; };
  static FloatColor olive() { return FloatColor{0.5, 0.5, 0, 1}; };
  static FloatColor lime() { return FloatColor{0, 1, 0, 1}; };
  static FloatColor green() { return FloatColor{0, 0.5, 0, 1}; };
  static FloatColor aqua() { return FloatColor{0, 1, 1, 1}; };
  static FloatColor blue() { return FloatColor{0, 0, 1, 1}; };
  static FloatColor navy() { return FloatColor{0.3125, 0, 0.5, 1}; };
  static FloatColor fuchsia() { return FloatColor{1, 0, 1, 1}; };
  static FloatColor purple() { return FloatColor{0.5, 0, 0.5, 1}; };
};

class CharColor : public BaseColor<unsigned char> {
 public:
  using BaseColor::BaseColor;

  CharColor(const BaseColor<float>& other) {
    for (int i = 0; i < 4; ++i) {
      data_[i] = static_cast<unsigned char>(other[i] * 255.0f);
    }
  }

  inline CharColor& operator=(const BaseColor<float>& other) {
    for (int i = 0; i < 4; ++i) {
      data_[i] = static_cast<unsigned char>(other[i] * 255.0f);
    }
    return *this;
  }
  void fromFloatColor(const BaseColor<float>& c) {
    this->data_[0] = c.r() * 255.0f;
    this->data_[1] = c.g() * 255.0f;
    this->data_[2] = c.b() * 255.0f;
    this->data_[3] = c.a() * 255.0f;
  }

  int32_t toHex() const {
    uint32_t r = data_[0] << 24;
    uint32_t g = data_[1] << 16;
    uint32_t b = data_[2] << 8;
    uint32_t a = data_[3];
    return r | g | b | a;
  }

  static CharColor fromHex(uint32_t hex) {
    unsigned char r = (hex >> 24) & 0xFF;
    unsigned char g = (hex >> 16) & 0xFF;
    unsigned char b = (hex >> 8) & 0xFF;
    unsigned char a = hex & 0xFF;
    return CharColor{r, g, b, a};
  }

  static CharColor fromHex(const std::string& hex_code) {
    std::string _hex_code = hex_code;

    // remove '#'
    if (!_hex_code.empty() && _hex_code[0] == '#') {
      _hex_code.erase(0, 1);
    }

    if (_hex_code.length() == 6) {
      _hex_code += "FF";
    }

    if (_hex_code.length() != 8) {
      throw std::invalid_argument("Hex code must be 6 or 8 characters long");
    }

    uint32_t hex_num = std::stoul(_hex_code, nullptr, 16);
    return CharColor::fromHex(hex_num);
  }
};

using Color = FloatColor;

template <typename T>
inline std::ostream& operator<<(std::ostream& os, const BaseColor<T>& c) {
  os << "{";
  os << c[0] << ",";
  os << c[0] << ",";
  os << c[1] << "}\n";
  return os;
}

}  // namespace limas
