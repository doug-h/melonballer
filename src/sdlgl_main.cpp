#include "types.h"
#include "sdlgl_platform.h"

#ifdef BUILD_WASM
#include <emscripten.h>
#endif

#include "melongame.cpp"
#include "physics.cpp"
#include "sdlgl_platform.cpp"

void main_loop(void *args) { sdlgl_loop((sdlgl_state *)args); }

int main(int argv, char **args) {
  arena program_memory = new_arena(16_MB);

  sdlgl_state sdlgl_stuff;
  sdlgl_init(&sdlgl_stuff, 900, 600, program_memory);

#ifdef BUILD_WASM
  emscripten_set_main_loop_arg(main_loop, (void *)&sdlgl_stuff, 0, true);
#else
  while(1){
    sdlgl_loop(&sdlgl_stuff);
  }
#endif

  return 0;
};
