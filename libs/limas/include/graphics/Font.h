#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
// #include FT_GLYPH_H
// #include FT_OUTLINE_H
// #include FT_TRIGONOMETRY_H

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
        log::error() << "couldn't exit Freetype lib" << log::end();
      }
    }

    FT_Library& getLibrary() { return lib_; }

   private:
    LibraryManager() {
      if (FT_Init_FreeType(&lib_)) {
        log::error() << "couldn't initialize Freetype lib" << log::end();
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
    Pixels2D pixels;
  };

  Font() {}
  virtual ~Font() { FT_Done_Face(face_); }

  bool load(const string& path, float pixel_size, bool b_antialiased) {
    if (FT_New_Face(getLibrary(), path.c_str(), 0, &face_)) {
      log::error("Font") << "failed to load font from " << path << log::end();
      return false;
    }

    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    FT_Set_Pixel_Sizes(face_, 0, pixel_size);

    unit_size_.x_max = 0;
    unit_size_.y_max = 0;
    unit_size_.x_min = INT_MAX;
    unit_size_.y_min = INT_MAX;

    for (unsigned char c = CHAR_CODE_START; c < CHAR_CODE_END; ++c) {
      if (FT_Load_Char(face_, c,
                       b_antialiased ? FT_LOAD_RENDER
                                     : FT_LOAD_RENDER | FT_LOAD_MONOCHROME)) {
        log::warn("Font") << "FT_Load_Char failed for utf8 code " << c
                          << log::end();
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
      ch.y_max = ch.y_min + ch.height - 5;  // madic number...

      ch.advance_x = slot->advance.x >> 6;
      ch.advance_y = slot->advance.y >> 6;

      if (ch.x_min < unit_size_.x_min) {
        unit_size_.x_min = ch.x_min;
      }

      if (ch.y_min < unit_size_.y_min) {
        unit_size_.y_min = ch.y_min;
      }

      if (ch.x_max > unit_size_.x_max) {
        unit_size_.x_max = ch.x_max;
      }

      if (ch.y_max > unit_size_.y_max) {
        unit_size_.y_max = ch.y_max;
      }

      if (b_antialiased) {
        unsigned char* buf = bitmap.buffer;
        ch.pixels.allocate(ch.width, ch.height, 1);
        ch.pixels.loadData(buf);
      } else {
        unsigned char* src = bitmap.buffer;
        ch.pixels.allocate(ch.width, ch.height, 1);
        for (int y = 0; y < ch.height; y++) {
          unsigned char b = 0;
          unsigned char* b_ptr = src;
          for (int x = 0; x < ch.width; x++) {
            if (x % 8 == 0) b = (*b_ptr++);
            unsigned char col = b & 0x80 ? 255 : 0;
            ch.pixels.loadData(&col, 1, 1, x, y);
            b <<= 1;
          }
          src += bitmap.pitch;
        }
      }
    }

    unit_size_.width = unit_size_.x_max - unit_size_.x_min;
    unit_size_.height = unit_size_.y_max - unit_size_.y_min;

    int tex_width = unit_size_.width * NUM_CHARS;
    int tex_height = unit_size_.width;
    Pixels2D pixels(tex_width, tex_height, 1);

    for (unsigned char c = CHAR_CODE_START; c < CHAR_CODE_END; ++c) {
      auto& ch = characters_[c];
      if (ch.width == 0 || ch.height == 0) continue;
      pixels.loadData(&ch.pixels.getData()[0], ch.width, ch.height,
                      -unit_size_.x_min + ch.x_min +
                          (int)(c - CHAR_CODE_START) * unit_size_.width,
                      -unit_size_.y_min + ch.y_min);
    }

    auto remapped = pixels.remapChannels({0, 0, 0, 0});

    texture_.allocate(tex_width, tex_height, GL_RGBA8);
    texture_.loadData(&remapped.getData()[0]);
    texture_.setMinFilter(GL_LINEAR);
    texture_.setMagFilter(GL_LINEAR);

    // glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return true;
  }

  geom::Mesh getMesh(const std::string& text, float x, float y,
                     float spacing = 0) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texcoords;
    std::vector<int> indices;

    auto it = text.begin();
    for (; it != text.end(); ++it) {
      auto ch_pair = characters_.find(*it);
      if (ch_pair == characters_.end()) {
        log::warn() << "font doesn't contain " << *it << log::end();
        continue;
      }

      int ch_code = ch_pair->first;
      auto& ch = ch_pair->second;
      int index_offset = vertices.size();

      float dx = x + ch.x_min;
      float dy = y + ch.y_min;
      vertices.emplace_back(glm::vec3{dx, dy, 0});
      vertices.emplace_back(glm::vec3{dx + ch.width, dy, 0});
      vertices.emplace_back(glm::vec3{dx + ch.width, dy + ch.height, 0});
      vertices.emplace_back(glm::vec3{dx, dy + ch.height, 0});

      float uw = 1.0 / (float)NUM_CHARS;

      float tl = uw * (ch_code - CHAR_CODE_START);
      tl +=
          uw * ((float)(ch.x_min - unit_size_.x_min) / (float)unit_size_.width);
      float tw = uw * (((float)ch.width / (float)unit_size_.width));
      float tr = tl + tw;
      // tr += uw * ((float)(ch.x_min - unit_size_.x_min) /
      // (float)unit_size_.width);

      float tt = 0;
      tt += (float)(ch.y_min - unit_size_.y_min) / (float)unit_size_.height;
      float th = ((float)ch.height / (float)unit_size_.height);
      float tb = tt + th;
      // tb += (float)(ch.y_min - unit_size_.y_min) /
      // (float)unit_size_.height;

      texcoords.emplace_back(glm::vec2{tl, tt});
      texcoords.emplace_back(glm::vec2{tr, tt});
      texcoords.emplace_back(glm::vec2{tr, tb});
      texcoords.emplace_back(glm::vec2{tl, tb});

      indices.push_back(index_offset + 0);
      indices.push_back(index_offset + 1);
      indices.push_back(index_offset + 2);

      indices.push_back(index_offset + 2);
      indices.push_back(index_offset + 3);
      indices.push_back(index_offset + 0);

      x += ch.advance_x + spacing;
      y += ch.advance_y;
    }

    geom::Mesh mesh;
    mesh.setVertices(vertices);
    mesh.setTexCoords(texcoords);
    mesh.setIndices(indices);
    mesh.fillColor(glm::vec4(1));
    return mesh;
  }

  prim::Rectangle getBoundingBox(const std::string& text, float x, float y,
                                 int spacing = 0) {
    glm::vec2 min_p(INT_MAX, INT_MAX);
    glm::vec2 max_p(-INT_MAX, -INT_MAX);

    auto it = text.begin();
    for (; it != text.end(); ++it) {
      auto ch_pair = characters_.find(*it);
      if (ch_pair == characters_.end()) {
        log::warn() << "font doesn't contain " << *it << log::end();
        continue;
      }

      auto& ch = ch_pair->second;

      if ((x + ch.x_min) < min_p.x) {
        min_p[0] = x + ch.x_min;
      }

      if ((x + ch.x_min + ch.width) > max_p.x) {
        max_p[0] = x + ch.x_min + ch.width;
      }

      if ((y + ch.y_min) < min_p.y) {
        min_p[1] = y + ch.y_min;
      }

      if ((y + ch.y_min + ch.height) > max_p.y) {
        max_p[1] = y + ch.y_min + ch.height;
      }

      x += ch.advance_x + spacing;
      y += ch.advance_y;
    }

    return prim::Rectangle(min_p, max_p);
  }

  gl::Texture2D& getTexture() { return texture_; }

 protected:
  FT_Face face_;
  std::map<unsigned char, Character> characters_;
  gl::Texture2D texture_;
  UnitSize unit_size_;
};

}  // namespace limas
