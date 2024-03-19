#include "types.h"
#include "sdlgl_platform.h"

#include <emscripten.h>

#include "melongame.cpp"
#include "sdlgl_platform.cpp"

void main_loop(void *args) { sdlgl_loop((sdlgl_state *)args); }

int main(int argv, char **args) {
  arena program_memory = new_arena(16_MB);

  sdlgl_state sdlgl_stuff;
  sdlgl_init(&sdlgl_stuff, 900, 600, program_memory);

  emscripten_set_main_loop_arg(main_loop, (void *)&sdlgl_stuff, 0, true);

  return 0;
};
