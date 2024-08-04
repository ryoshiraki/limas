#pragma once
#include "3d/Camera.h"
#include "gl/Shader.h"
#include "gl/Renderer.h"

namespace rs {

static const string projectorVertexShader = gl::vertexShaderHeader +
STRINGIFY(
          layout (location = 0) IN vec3 position;
          layout (location = 1) IN vec4 color;
          layout (location = 2) IN vec2 texcoord;
          layout (location = 3) IN vec3 normal;
          
          OUT vec4 v_color;
          OUT vec4 v_texcoord;
          
          uniform mat4 u_view_mat;
          uniform mat4 u_proj_mat;
          uniform mat4 u_tex_mat;
          
          void main() {
            v_color = color;
            v_texcoord = u_tex_mat * vec4(position, 1);
            gl_Position = u_proj_mat * u_view_mat * vec4(position, 1);
          }
          );

static const string projectorFragmentShader = gl::fragmentShaderHeader +
STRINGIFY (
           IN vec4 v_color;
           IN vec4 v_texcoord;
           
           OUT vec4 frag_color;
           uniform sampler2D u_tex;
           uniform mat4 u_tex_mat;
           
           void main() {
             
             if (v_texcoord.w < 0) {
               frag_color = v_color;
             } else {
               vec2 texcoord = v_texcoord.xy / v_texcoord.w;
               texcoord = texcoord;
               if (texcoord.x > 0.0 && texcoord.y > 0.0 &&
                   texcoord.x < 1.0 && texcoord.y < 1.0) {
                 frag_color = texture(u_tex, texcoord);
               } else {
                 frag_color = v_color;
               }
             }
             
           }
           );

class Projector : public Camera {
  
public:
  Projector() : Camera() {}
  
  void loadDefault() {
    shader.loadFromSource(projectorVertexShader, projectorFragmentShader);
  }
  
  void load(const std::string& vertex_path, const std::string& fragmant_path) {
    shader.load(vertex_path, fragmant_path);
  }
  
  void bind(const gl::TextureBase& tex,
            const glm::mat4& view,
            const glm::mat4& proj) {
    
    static const glm::mat4 bias_mat = glm::mat4(0.5f, 0.0f, 0.0f, 0.0f,
                                                0.0f, -0.5f, 0.0f, 0.0f,
                                                0.0f, 0.0f, 1.0f, 0.0f,
                                                0.5f, 0.5f, 0.0f, 1.0f);
    
    glm::mat4 tex_mat = bias_mat * getProjectionMatrix(0, 0, tex.getWidth(), tex.getHeight()) * getModelViewMatrix();;
    
    shader.bind();
    shader.setTexture("u_tex", tex, 0);
    shader.setUniform("u_tex_mat", tex_mat);
    shader.setUniform("u_view_mat", view);
    shader.setUniform("u_proj_mat", proj);
  }
  
  void unbind() {
    shader.unbind();
  }
  
  gl::Shader shader;
};

}
