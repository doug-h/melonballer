/* Wrapped by raw string so it can be #included into the code*/
R"(#version 300 es

precision highp float;

uniform mat4 pvm;

layout(location = 0) in vec3 position;

out vec3 normal;

void main() {
  gl_Position = vec4(position, 1.0f);
  normal = position;
  }
)"
