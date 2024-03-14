#include "sdlgl_platform.h"

#include "types.h"

#define PI 3.141592653589793238
#define TWO_PI 6.283185307179586477

/*     ======  UV Sphere ======
 *  ndivs_u == number of horizontal divisors == number of faces per strip
 *  ndivs_v == number of vertical divisors == (number of strips-1)
 *
 * for ndivs_u = 6, ndivs_v = 2:
 *
 *   v=0        O
 *            / | \               O---O
 *   v=1     O--O--O             / \ / \
 *           |  |  |            O---O---O
 *   v=2     O--O--O             \ / \ /
 *            \ | /               O---O
 *   v=3        O
 *            From side          From above
 *           (squashed)
 *
 * num verts == 2 + ndivs_u*ndivs_v
 * num tris  == 2 * ndivs_u*ndivs_v
 */

void make_UV_sphere_verts(iZ ndivs_u, iZ ndivs_v, GLfloat *verts,
                          arena *mem_temp) {
  ASSERT(ndivs_u > 2);
  ASSERT(ndivs_v > 0);
  arena scratch = *mem_temp;
  float u_angle = TWO_PI / ndivs_u;
  float v_angle = PI / (ndivs_v + 1);
  // Work out sin/cos of vertical and horizontal segments
  GLfloat *usin, *ucos, *vsin, *vcos;
  usin = arena_push<GLfloat>(&scratch, ndivs_u);
  ucos = arena_push<GLfloat>(&scratch, ndivs_u);
  for (int i = 0; i < ndivs_u; ++i) {
    usin[i] = sin(i * u_angle);
    ucos[i] = cos(i * u_angle);
  }

  vsin = arena_push<GLfloat>(&scratch, ndivs_v + 2);
  vcos = arena_push<GLfloat>(&scratch, ndivs_v + 2);
  for (int i = 0; i < ndivs_v + 2; ++i) {
    vsin[i] = sin(i * v_angle);
    vcos[i] = cos(i * v_angle);
  }

  *verts++ = +0.0f;
  *verts++ = +0.0f;
  *verts++ = vcos[0];
  for (int v = 1; v < ndivs_v + 1; ++v) {
    for (int u = 0; u < ndivs_u; ++u) {
      *verts++ = -vsin[v] * ucos[u];
      *verts++ = +vsin[v] * usin[u];
      *verts++ = +vcos[v];
    }
  }
  *verts++ = +0.0f;
  *verts++ = +0.0f;
  *verts++ = vcos[ndivs_v + 1];
}

void make_UV_sphere_tris(iZ ndivs_u, iZ ndivs_v, GLfloat *tris,
                         arena *mem_temp) {
  arena scratch = *mem_temp;
  // TODO - Triangle strips? Not worth the effort?

  int num_strips = ndivs_v - 1;
  int num_verts  = 2 + ndivs_u * ndivs_v;
  int num_tris   = 2 * ndivs_u * ndivs_v;
  GLfloat *verts = arena_push<GLfloat>(&scratch, num_verts);
  make_UV_sphere_verts(ndivs_u, ndivs_v, verts, mem_temp);

  // Top fan
  for (int i = 0; i < ndivs_u; ++i) {
    *tris++ = verts[0];
    *tris++ = verts[1];
    *tris++ = verts[2];

    *tris++ = verts[3 * (i + 1) + 0];
    *tris++ = verts[3 * (i + 1) + 1];
    *tris++ = verts[3 * (i + 1) + 2];

    int j   = (i + 1) % ndivs_u;
    *tris++ = verts[3 * (j + 1) + 0];
    *tris++ = verts[3 * (j + 1) + 1];
    *tris++ = verts[3 * (j + 1) + 2];
  }
}

GLuint compile_shader(const char *vertex_shader, const char *fragment_shader) {

  GLuint vertex, fragment;
  vertex   = glCreateShader(GL_VERTEX_SHADER);
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(vertex, 1, &vertex_shader, nullptr);
  glShaderSource(fragment, 1, &fragment_shader, nullptr);

  int success;
  char infoLog[512];
  glCompileShader(vertex);
  glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("Vertex shader compilation failed: \n %.*s\n", 512, infoLog);
  }
  glCompileShader(fragment);
  glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vertex, 512, NULL, infoLog);
    printf("Fragment shader compilation failed: \n %.*s\n", 512, infoLog);
  }

  GLuint ID = glCreateProgram();
  glAttachShader(ID, vertex);
  glAttachShader(ID, fragment);

  glLinkProgram(ID);
  // Catch linking errors
  glGetProgramiv(ID, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(ID, 512, NULL, infoLog);
    printf("Shader linking failed: \n %.*s\n", 512, infoLog);
  }

  glDeleteShader(vertex);
  glDeleteShader(fragment);

  return ID;
}

void draw_sphere(sdlgl_state *s, arena *mem_temp) {
  arena scratch = *mem_temp;
  GLfloat *tris = arena_push<GLfloat>(&scratch, 3 * 3 * 2 * 6 * 2);
  make_UV_sphere_tris(6, 2, tris, &scratch);

  glBindVertexArray(s->vao_sphere);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo_sphere);
  glBufferData(GL_ARRAY_BUFFER, 3 * 3 * 6 * sizeof(GLfloat), tris,
               GL_STATIC_DRAW);

  glClear(GL_COLOR_BUFFER_BIT);
  glDrawArrays(GL_TRIANGLES, 0, 3 * 6);
  SDL_GL_SwapWindow(s->window);
}

void sdlgl_init(sdlgl_state *s, int width, int height, arena *mem_perm,
                arena *mem_temp) {
  if (SDL_Init(SDL_INIT_VIDEO)) {
    printf("SDL could not initialize! SDL_Error:%s\n", SDL_GetError());
    ASSERT(0);
  }
  {
    SDL_version vers;
    SDL_GetVersion(&vers);
    printf("SDL Version: %d.%d.%d\n", vers.major, vers.minor, vers.patch);
  }

  SDL_Window *window;
  {
    int request_prof = SDL_GL_CONTEXT_PROFILE_ES;
    int request_majv = 3;
    int request_minv = 0;

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, request_prof);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, request_majv);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, request_minv);

    window = SDL_CreateWindow("Melon", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, width, height,
                              SDL_WINDOW_OPENGL);

    SDL_GL_CreateContext(window);

    int prof, majv, minv;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, &prof);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &majv);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minv);

    // TODO - Allow non-exact version matches
    if (prof != request_prof || majv != request_majv || minv != request_minv) {
      puts("Platform doesn't support requested OpenGLES version");
      ASSERT(0);
    }
  }

  SDL_GL_SetSwapInterval(1);

  const char *vert_code =
#include "../shaders/sphere.vert"
      ;
  const char *frag_code =
#include "../shaders/sphere.frag"
      ;

  GLuint sphere_program = compile_shader(vert_code, frag_code);
  glUseProgram(sphere_program);

  GLuint sphere_vao, sphere_vbo;
  glGenVertexArrays(1, &sphere_vao);
  glGenBuffers(1, &sphere_vbo);

  glBindVertexArray(sphere_vao);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                        (void *)0);
  glEnableVertexAttribArray(0);

  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  SDL_GL_SwapWindow(window);

  melon_state game{};
  melon_init(&game, mem_perm, mem_temp);

  s->width  = width;
  s->height = height;

  s->window   = window;
  s->keyboard = SDL_GetKeyboardState(0);

  s->prog_sphere = sphere_program;
  s->vao_sphere  = sphere_vao;
  s->vbo_sphere  = sphere_vbo;

  s->game = game;

  draw_sphere(s, mem_temp);
}

void process_event_queue(sdlgl_state *s) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_MOUSEMOTION: {
      melon_mousemotion(&s->game);
    } break;
    case SDL_MOUSEBUTTONDOWN: {
      melon_mousedown(&s->game);
    } break;
    case SDL_MOUSEBUTTONUP: {
      melon_mouseup(&s->game);
    } break;
    }
  }
}

void draw(sdlgl_state *s) {
  glClear(GL_COLOR_BUFFER_BIT);

  SDL_GL_SwapWindow(s->window);
}

void sdlgl_loop(sdlgl_state *s) {
  process_event_queue(s);

  // draw(s);
}
