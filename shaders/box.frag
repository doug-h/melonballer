/* Wrapped by raw string so it can be #included into the code*/
R"(#version 300 es
precision highp float;

in vec3 normal;

out vec4 colour_out;

void main() {
  vec3 diffuse_colour = vec3(1.0f, 1.0f, 0.0f);
  
  vec3 light1_colour = vec3(1.0f, 0.0f, 0.0f);
  vec3 light2_colour = vec3(0.0f, 0.0f, 1.0f);

  vec3 n = normalize(normal);
  vec3 ambient = diffuse_colour * vec3(0.1f, 0.1f, 0.1f);
  vec3 diffuse = diffuse_colour * max(dot(n, vec3(0.0f, 0.4f, 0.98f)), 0.0f);

  colour_out = vec4(diffuse + ambient, 0.3f);
}
)"
