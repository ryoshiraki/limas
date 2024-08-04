#pragma once
#include "imgui.h"
// #include "imgui_impl_glfw.h"
// #include "imgui_impl_opengl3.h"
#include "app/Window.h"
#include "gl/Context.h"
#include "gl/Ibo.h"
#include "gl/Shader.h"
#include "gl/Texture2D.h"
#include "gl/Vao.h"
#include "gl/Vbo.h"
#include "system/Logger.h"
#include "system/Noncopyable.h"

namespace rs {
namespace gui {

class Renderer {
 public:
  Renderer() {
    //! initializes the vbo mesh
    vao_ = gl::Vao::create();
    vbo_ = gl::Vbo<ImDrawVert>::create();
    ibo_ = gl::Ibo<ImDrawIdx>::create();

    shader_ =
        gl::Shader::create(fs::getCommonResourcesPath("shaders/gui.vert"),
                           fs::getCommonResourcesPath("shaders/gui.frag"));

    //! initalizes the font texture
    unsigned char* pixels;
    int width, height;
    ImGui::GetIO().Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);
    tex_ = gl::Texture2D::create(width, height, GL_RGBA8);
    tex_->loadData(pixels);
    tex_->setMinFilter(GL_LINEAR);
    tex_->setMagFilter(GL_LINEAR);

    ImGui::GetIO().Fonts->ClearTexData();
    ImGui::GetIO().Fonts->TexID = (void*)(intptr_t)tex_->getID();
  }

  void draw(ImDrawData* draw_data) {
    ImGuiIO& io = ImGui::GetIO();
    const float width = io.DisplaySize.x;
    const float height = io.DisplaySize.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    const glm::mat4 mat = {
        {2.0f / width, 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f / -height, 0.0f, 0.0f},
        {0.0f, 0.0f, -1.0f, 0.0f},
        {-1.0f, 1.0f, 0.0f, 1.0f},
    };

    shader_->bind();
    shader_->setUniformTexture("u_tex", tex_, 0);
    shader_->setUniformMatrix4f("u_mvp", mat);

    context_.pushViewport();
    // context_.setViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
    context_.pushScissorTest();
    context_.setScissorTest(true);
    context_.pushScissorBox();

    for (int n = 0; n < draw_data->CmdListsCount; n++) {
      const ImDrawList* cmd_list = draw_data->CmdLists[n];
      const ImDrawIdx* idx_buffer_offset = 0;

      if (vbo_->getSize())
        vbo_->update(cmd_list->VtxBuffer.Data, 0, cmd_list->VtxBuffer.Size);
      else {
        vbo_->allocate(cmd_list->VtxBuffer.Data, cmd_list->VtxBuffer.Size);
        vao_->bindVbo(*vbo_, 0, 2, GL_FLOAT,
                      (const GLvoid*)offsetof(ImDrawVert, pos));
        vao_->bindVbo(*vbo_, 1, 2, GL_FLOAT,
                      (const GLvoid*)offsetof(ImDrawVert, uv));
        vao_->bindVbo(*vbo_, 2, 4, GL_FLOAT,
                      (const GLvoid*)offsetof(ImDrawVert, col));
      }

      if (ibo_->getSize())
        ibo_->update(cmd_list->IdxBuffer.Data, 0, cmd_list->IdxBuffer.Size);
      else {
        ibo_->allocate(cmd_list->IdxBuffer.Data, cmd_list->IdxBuffer.Size);
        vao_->bindIbo(*ibo_);
      }

      for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++) {
        const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
        if (pcmd->UserCallback) {
          pcmd->UserCallback(cmd_list, pcmd);
        } else {
          context_.setScissorBox((int)pcmd->ClipRect.x, (int)(pcmd->ClipRect.y),
                                 (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                                 (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));

          vao_->drawElements(
              GL_TRIANGLES, pcmd->ElemCount,
              sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT,
              idx_buffer_offset);
        }
        idx_buffer_offset += pcmd->ElemCount;
      }
    }

    context_.popScissorBox();
    context_.popScissorTest();
    context_.popViewport();

    shader_->unbind();
  }
  gl::Vao::Ptr vao_;
  gl::Vbo<ImDrawVert>::Ptr vbo_;
  gl::Ibo<ImDrawIdx>::Ptr ibo_;
  gl::Shader::Ptr shader_;

  gl::Texture2D::Ptr tex_;
  gl::Context context_;
};

}  // namespace gui

class Gui {
 public:
  Gui() {}

  ~Gui() { ImGui::DestroyContext(); }

  void setup(GLFWwindow* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    ImGui::StyleColorsDark();

    renderer = new gui::Renderer();

    this->window_ = window;
  }

  void begin() {
    ImGuiIO& io = ImGui::GetIO();

    int width, height;
    int display_width, display_height;
    glfwGetWindowSize(window_, &width, &height);
    glfwGetFramebufferSize(window_, &display_width, &display_height);
    io.DisplaySize = ImVec2((float)width, (float)height);
    io.DisplayFramebufferScale =
        ImVec2(width > 0 ? ((float)display_width / width) : 0,
               height > 0 ? ((float)display_height / height) : 0);

    ImGui::NewFrame();
  }

  void end() {
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    if (draw_data) {
      renderer->draw(draw_data);
    }
  }

  void addButton(const std::string& label) {
    if (ImGui::Button(label.c_str())) {
      std::cout << label << " button pressed!" << std::endl;
    }
  }

  void addSliderFloat(const std::string& label, float* value, float min,
                      float max) {
    ImGui::SliderFloat(label.c_str(), value, min, max);
  }

  GLFWwindow* window_;
  gui::Renderer* renderer;
};
}  // namespace rs
