#pragma once

#include "melongame.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_config.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>

struct sdlgl_state {
  int width;
  int height;

  SDL_Window  *window;
  const Uint8 *keyb;

  GLuint prog_fruit;
  GLuint vao_fruit;
  GLuint vbo_sphere;
  GLint  sphere_num_verts;
  GLuint vbo_fruit_instances;

  GLuint prog_box;
  GLuint vao_box;
  GLuint vbo_box;
  GLint  box_num_verts;

  arena memory;

  vec3 camera_pos;

  melon_state game;
};

void sdlgl_init(sdlgl_state *, int w, int h, arena *p, arena *t);
void sdlgl_loop(sdlgl_state *);
