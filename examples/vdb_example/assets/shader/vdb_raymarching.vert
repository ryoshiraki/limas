#version 400

//default attributes
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 texcoord;

//default uniforms
uniform mat4 u_mvp_mat;

uniform int firstInternalNodeIndex;
uniform int maxLeafCountPerInternalNode;

out vec3 v_position;
out vec3 v_texcoord;
out flat int nodeIndirectionBaseIndex;

const int kVerticesPerInternalNode = 8;

void main() {
  v_texcoord = texcoord;
  v_position = position;
  gl_Position = u_mvp_mat * vec4(position, 1);

  int internalNodeIndex = firstInternalNodeIndex + (gl_VertexID / kVerticesPerInternalNode);
	nodeIndirectionBaseIndex = internalNodeIndex * maxLeafCountPerInternalNode;	

}
