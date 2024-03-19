/* Wrapped by raw string so it can be #included into the code*/
R"(#version 300 es

precision highp float;

uniform mat4 pvm;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;

out vec3 normal;

void main() {
  gl_Position = pvm*vec4(in_position, 1.0f);
  normal = in_normal;
}
)"
