#pragma once

#include "types.h"

#define MAX_FRUIT (1 << 10)
#define FRUIT_DENSITY 0.01f

struct fruit_type {
  const char *label;

  vec3f colour;
  vec3f radii;
  float density;
};

fruit_type apple = {.label   = "apple",
                    .colour  = {1.0f, 0.0f, 0.0f},
                    .radii   = {0.1f, 0.1f, 0.1f},
                    .density = FRUIT_DENSITY};
fruit_type melon = {.label   = "melon",
                    .colour  = {0.0f, 1.0f, 0.0f},
                    .radii   = {0.14f, 0.2f, 0.14f},
                    .density = FRUIT_DENSITY};

fruit_type TABLE_fruit_type[2] = {apple, melon};

struct fruit {
  vec3f pos;
  float id;

  vec3f vel;
  float inv_mass;

  vec3f ang_vel;
  vec3f inv_moi;

  vec3f acc_imp;
};

struct renderer_input {
  fruit *fruit;

  iZ   num_fruit;
  bool needs_reupload;
};

struct melon_state {
  array<fruit> fruit;

  bool render_state_changed;
};

void melon_mousemotion(melon_state *);
void melon_mousedown(melon_state *);
void melon_mouseup(melon_state *);

void melon_init(melon_state *, arena *);
void melon_tick(melon_state *, renderer_input *, arena *);
