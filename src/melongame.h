#pragma once

#include "physics.h"
#include "types.h"

#define MAX_FRUIT     (1 << 10)
#define FRUIT_DENSITY 0.11f

#define BOX_WIDTH 2
#define BOX_DEPTH 2
#define BOX_HEIGHT 2

struct fruit_type {
  const char *label;

  vec3 colour;
  vec3 radii;
  float density;

  // Derived from radii+density
  float volume;
  float inv_mass;
  mat3 inv_moi;
};

fruit_type apple = {
    .label = "apple",
    .colour = {1.0f, 0.0f, 0.0f},
    .radii = {0.1f, 0.1f, 0.1f},
    .density = FRUIT_DENSITY
};
fruit_type melon = {
    .label = "melon",
    .colour = { 0.0f, 1.0f,  0.0f},
    .radii = {0.14f, 0.2f, 0.14f},
    .density = FRUIT_DENSITY
};

fruit_type TABLE_fruit_type[2] = {apple, melon};


struct renderer_input {
  fruit_body *fruit;

  iZ num_fruit;
  bool needs_reupload;
};

struct melon_state {
  array<fruit_body> fruit;
  array<body_dynamics> fruit_dynamics;
};

void melon_init(melon_state *, arena *);
void melon_tick(melon_state *, renderer_input *, arena *);

void melon_mousemotion(melon_state *);
void melon_mousedown(melon_state *);
void melon_mouseup(melon_state *);
