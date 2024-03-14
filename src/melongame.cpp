#include "melongame.h"

void melon_init(melon_state *m, arena *mem_perm, arena *mem_temp) {
  ASSERT(!m->initialised);

  m->initialised = true;
}

void melon_tick(melon_state *m, renderer_input *ri) {
  ri->spheres     = nullptr;
  ri->num_spheres = 0;
}

void melon_mousemotion(melon_state* m){
}
void melon_mousedown(melon_state* m){
}
void melon_mouseup(melon_state* m){
}
