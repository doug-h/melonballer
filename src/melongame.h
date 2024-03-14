#pragma once

#include "types.h"

struct sphere {
  float x, y, z;
  float r;
};

struct renderer_input {
  sphere *spheres;
  iZ num_spheres;
};

struct melon_state {
  bool initialised;
};

void melon_mousemotion(melon_state *);
void melon_mousedown(melon_state *);
void melon_mouseup(melon_state *);

void melon_init(melon_state *, arena *mem_perm, arena *mem_temp);
void melon_tick(melon_state *, renderer_input*);
