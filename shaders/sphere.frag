/* Wrapped by raw string so it can be #included into the code*/
R"(#version 300 es

precision highp float;

in vec3 texCoord;

out vec4 colour_out;

void main() {
  colour_out = vec4(1.0f, 1.0f, 0.0f, 1.0f);
}
)"
