/* Wrapped by raw string so it can be #included into the code*/
R"(#version 300 es

precision highp float;

uniform mat4 pvm;
uniform bool outline;

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 instance; // {x,y,z,id}

out vec3 normal;
out vec3 colour;

const vec3 fruit_dims[2] = vec3[2](
  vec3(0.1f, 0.1f, 0.1f),
  vec3(0.14f, 0.2f, 0.14f)
);
const vec3 fruit_colours[2] = vec3[2](
  vec3(1.0f, 0.0f, 0.0f),
  vec3(0.0f, 1.0f, 0.0f)
);

void main() {
  int id = int(instance.w);
  float border = (outline) ? 0.05f : 0.0f;
  vec3 pos = (position * fruit_dims[id] * (1.0f + border)) + instance.xyz;
  gl_Position = pvm*vec4(pos, 1.0f);
  normal = (outline) ? vec3(0.0f) : position;
  colour = (outline) ? fruit_colours[id] * 0.5f : fruit_colours[id];
}
)"
