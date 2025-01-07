#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H

#include "gl/GLUtils.h"
#include "gl/Renderer.h"
#include "gl/Texture2D.h"
#include "graphics/Pixels.h"
#include "primitives/Primitives.h"
#include "system/Logger.h"
#include "system/Singleton.h"

namespace limas {

using namespace std;

class Font {
  static const int CHAR_CODE_START = 32;
  static const int CHAR_CODE_END = 128;
  static const int NUM_CHARS = CHAR_CODE_END - CHAR_CODE_START;

  class LibraryManager {
    friend class Singleton<LibraryManager>;

   public:
    ~LibraryManager() {
      if (FT_Done_FreeType(lib_)) {
        logger::error() << "Couldn't exit FreeType library" << logger::end();
      }
    }

    FT_Library& getLibrary() { return lib_; }

   private:
    LibraryManager() {
      if (FT_Init_FreeType(&lib_)) {
        logger::error() << "Couldn't initialize FreeType library"
                        << logger::end();
      }
    }
    FT_Library lib_;
  };

  FT_Library& getLibrary() {
    return Singleton<LibraryManager>::getInstance().getLibrary();
  }

 public:
  struct UnitSize {
    int x_min, y_min;
    int x_max, y_max;
    int width, height;
  };

  struct Character {
    Character()
        : x_min(0),
          y_min(0),
          x_max(0),
          y_max(0),
          width(0),
          height(0),
          advance_x(0),
          advance_y(0) {}
    int x_min, y_min;
    int x_max, y_max;
    int width, height;
    int advance_x, advance_y;
    int x_offset;
    Pixels2D pixels;
  };

  Font() : face_(nullptr) {}
  virtual ~Font() {
    if (face_) {
      FT_Done_Face(face_);
    }
  }

  bool load(const string& path, float point_size, float dpi,
            bool b_antialiased) {
    if (FT_New_Face(getLibrary(), path.c_str(), 0, &face_)) {
      logger::error("Font")
          << "Failed to load font from " << path << logger::end();
      return false;
    }

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    float pixel_size =
        (point_size * dpi) / 72.0f;  // Convert point size to pixel size
    FT_Set_Pixel_Sizes(face_, 0, static_cast<FT_UInt>(pixel_size));

    unit_size_.x_max = 0;
    unit_size_.y_max = 0;
    unit_size_.x_min = INT_MAX;
    unit_size_.y_min = INT_MAX;

    // Precompute character data and determine texture dimensions
    int total_advance_x = 0;
    for (unsigned char c = CHAR_CODE_START; c < CHAR_CODE_END; ++c) {
      if (FT_Load_Char(face_, c,
                       b_antialiased ? FT_LOAD_RENDER
                                     : FT_LOAD_RENDER | FT_LOAD_MONOCHROME)) {
        logger::warn("Font")
            << "FT_Load_Char failed for UTF-8 code " << (int)c << logger::end();
        continue;
      }

      FT_GlyphSlot& slot = face_->glyph;
      FT_Bitmap& bitmap = slot->bitmap;
      auto& ch = characters_[c];

      ch.x_min = slot->bitmap_left;
      ch.y_min = -slot->bitmap_top;
      ch.width = bitmap.width;
      ch.height = bitmap.rows;
      ch.x_max = ch.x_min + ch.width;
      ch.y_max = ch.y_min + ch.height;

      ch.advance_x = slot->advance.x >> 6;
      ch.advance_y = slot->advance.y >> 6;

      unit_size_.x_min = std::min(unit_size_.x_min, ch.x_min);
      unit_size_.y_min = std::min(unit_size_.y_min, ch.y_min);
      unit_size_.x_max = std::max(unit_size_.x_max, ch.x_max);
      unit_size_.y_max = std::max(unit_size_.y_max, ch.y_max);

      if (b_antialiased) {
        ch.pixels.allocate(ch.width, ch.height, 1);
        ch.pixels.loadData(bitmap.buffer);
      } else {
        ch.pixels.allocate(ch.width, ch.height, 1);
        unsigned char* src = bitmap.buffer;
        for (int y = 0; y < ch.height; y++) {
          unsigned char b = 0;
          unsigned char* b_ptr = src;
          for (int x = 0; x < ch.width; x++) {
            if (x % 8 == 0) b = (*b_ptr++);
            unsigned char col = (b & 0x80) ? 255 : 0;
            ch.pixels.loadData(&col, 1, 1, x, y);
            b <<= 1;
          }
          src += bitmap.pitch;
        }
      }

      total_advance_x += ch.advance_x;
    }

    unit_size_.width = unit_size_.x_max - unit_size_.x_min;
    unit_size_.height = unit_size_.y_max - unit_size_.y_min;

    // Texture dimensions
    int tex_width = std::max(total_advance_x, unit_size_.width * NUM_CHARS);
    int tex_height = unit_size_.height;
    Pixels2D pixels(tex_width, tex_height, 1);

    // Copy character pixels into the texture
    int x_offset = 0;
    for (unsigned char c = CHAR_CODE_START; c < CHAR_CODE_END; ++c) {
      auto& ch = characters_[c];
      if (ch.width == 0 || ch.height == 0) continue;

      pixels.loadData(&ch.pixels.getData()[0], ch.width, ch.height, x_offset,
                      -unit_size_.y_min + ch.y_min);
      ch.x_offset = x_offset;
      x_offset += ch.advance_x;
    }

    auto remapped = pixels.remapChannels({0, 0, 0, 0});

    texture_.allocate(tex_width, tex_height, GL_RGBA8);
    texture_.loadData(&remapped.getData()[0]);
    texture_.setMinFilter(GL_LINEAR);
    texture_.setMagFilter(GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return true;
  }

  geom::Mesh getMesh(const std::string& text, float x, float y,
                     float spacing = 0) const {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    std::vector<GLuint> indices;

    float x_offset = 0;

    for (char c : text) {
      auto it = characters_.find(c);
      if (it == characters_.end()) continue;

      const auto& ch = it->second;
      int index_offset = vertices.size();

      float dx = x + x_offset + ch.x_min;
      float dy = y + ch.y_min;
      vertices.emplace_back(dx, dy, 0);
      vertices.emplace_back(dx + ch.width, dy, 0);
      vertices.emplace_back(dx + ch.width, dy + ch.height, 0);
      vertices.emplace_back(dx, dy + ch.height, 0);

      float tl = (float)ch.x_offset / texture_.getWidth();
      float tr = (float)(ch.x_offset + ch.width) / texture_.getWidth();

      float tt = (float)(ch.y_min - unit_size_.y_min) / unit_size_.height;
      float tb = tt + ((float)ch.height / unit_size_.height);

      texcoords.emplace_back(tl, tt);
      texcoords.emplace_back(tr, tt);
      texcoords.emplace_back(tr, tb);
      texcoords.emplace_back(tl, tb);

      indices.push_back(index_offset + 0);
      indices.push_back(index_offset + 1);
      indices.push_back(index_offset + 2);
      indices.push_back(index_offset + 2);
      indices.push_back(index_offset + 3);
      indices.push_back(index_offset + 0);

      x_offset += ch.advance_x + spacing;
    }

    geom::Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setTexCoords(texcoords);
    mesh.setIndices(indices);
    mesh.fillColor(glm::vec4(1));

    return mesh;
  }

  const gl::Texture2D& getTexture() const { return texture_; }
  gl::Texture2D& getTexture() { return texture_; }

 protected:
  FT_Face face_;
  std::map<unsigned char, Character> characters_;
  gl::Texture2D texture_;
  UnitSize unit_size_;
};

}  // namespace limas
