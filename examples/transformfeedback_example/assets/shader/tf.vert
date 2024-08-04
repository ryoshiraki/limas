#version 400

layout (location = 0) in vec3 position;
out vec3 v_position;

void main() {
  v_position = position;
  v_position.x += 1;
  v_position.y += 1;
  gl_Position = vec4(v_position, 1.0);
}
