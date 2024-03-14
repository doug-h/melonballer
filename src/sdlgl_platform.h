#pragma once

#include "melongame.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_config.h>
#include <GLES3/gl3.h>
#include <GLES2/gl2ext.h>
#include <GLES3/gl3platform.h>

struct sdlgl_state {
  int width, height;

  SDL_Window *window;
  const Uint8 *keyboard;

  GLuint prog_sphere;
  GLuint vao_sphere, vbo_sphere;

  melon_state game;
};

void sdlgl_init(sdlgl_state *, int w, int h, arena *p, arena *t);
void sdlgl_loop(sdlgl_state *);
