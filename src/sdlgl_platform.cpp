#include "sdlgl_platform.h"

#include "types.h"

/*     ======  UV Sphere ======
 *  ndivs_u == number of horizontal divisors == number of faces per strip
 *  ndivs_v == number of vertical divisors == (number of strips-1)
 *
 * e.g. for ndivs_u = 6, ndivs_v = 2:
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

void make_UV_sphere_mesh_verts(iZ ndivs_u, iZ ndivs_v, vec3 *verts,
                               arena *mem_temp) {
  ASSERT(ndivs_u > 2);
  ASSERT(ndivs_v > 0);
  arena scratch = *mem_temp;
  float u_angle = TWO_PI / (float)ndivs_u;
  float v_angle = PI / (float)(ndivs_v + 1);
  // Work out sin/cos of vertical and horizontal segments
  float *usin, *ucos, *vsin, *vcos;
  usin = arena_push<float>(&scratch, ndivs_u);
  ucos = arena_push<float>(&scratch, ndivs_u);
  for (int i = 0; i < ndivs_u; ++i) {
    usin[i] = sin(i * u_angle);
    ucos[i] = cos(i * u_angle);
  }

  vsin = arena_push<float>(&scratch, ndivs_v + 2);
  vcos = arena_push<float>(&scratch, ndivs_v + 2);
  for (int i = 0; i < ndivs_v + 2; ++i) {
    vsin[i] = sin(i * v_angle);
    vcos[i] = cos(i * v_angle);
  }

  *verts++ = {0.0f, 0.0f, vcos[0]};
  for (int v = 1; v < ndivs_v + 1; ++v) {
    for (int u = 0; u < ndivs_u; ++u) {
      *verts++ = {-vsin[v] * ucos[u], +vsin[v] * usin[u], +vcos[v]};
    }
  }
  *verts++ = {0.0f, 0.0f, vcos[ndivs_v + 1]};
}

void make_UV_sphere_tris(iZ ndivs_u, iZ ndivs_v, vec3 *tris, arena *mem_temp) {
  arena scratch = *mem_temp;
  // TODO - Triangle strips? Not worth the effort?

  int num_verts = 2 + ndivs_u * ndivs_v;
  vec3 *verts = arena_push<vec3>(&scratch, num_verts);
  make_UV_sphere_mesh_verts(ndivs_u, ndivs_v, verts, &scratch);

  // Top fan
  for (int u = 0; u < ndivs_u; ++u) {
    int i0 = 1;
    *tris++ = verts[0];
    *tris++ = verts[i0 + (u + 1) % ndivs_u];
    *tris++ = verts[i0 + u];
  }

  // Strips
  for (int v = 0; v < ndivs_v - 1; ++v) {
    for (int u = 0; u < ndivs_u; ++u) {
      int u0 = 1 + v * ndivs_u;
      int u1 = 1 + (v + 1) * ndivs_u;

      int c0 = u0 + u;
      int c1 = u0 + (u + 1) % ndivs_u;
      int c2 = u1 + u;
      int c3 = u1 + (u + 1) % ndivs_u;
      *tris++ = verts[c0];
      *tris++ = verts[c1];
      *tris++ = verts[c2];
      *tris++ = verts[c2];
      *tris++ = verts[c1];
      *tris++ = verts[c3];
    }
  }

  // Bot fan
  for (int u = 0; u < ndivs_u; ++u) {
    int i0 = num_verts - 1 - ndivs_u;
    *tris++ = verts[i0 + u];
    *tris++ = verts[i0 + ((u + 1) % ndivs_u)];
    *tris++ = verts[num_verts - 1];
  }
}

void make_box_tris(float w, float d, float h, float t, vec3 *tris,
                   arena *mem_temp) {
  // clang-format off
  /*
   *        6 ------ 7
   *  z    /|       /|
   *  ^   4 ------ 5 |
   *  |   | |      | |     y
   *  |   | 2 -----|-3    7
   *  |   |/       |/    /
   *  |   0 ------ 1    /
   *  |                /
   *  +===========> x
   */

  vec3 a0(-w/2, -d/2, -h/2);
  vec3 a1(+w/2, -d/2, -h/2);
  vec3 a2(-w/2, +d/2, -h/2);
  vec3 a3(+w/2, +d/2, -h/2);
  vec3 a4(-w/2, -d/2, +h/2);
  vec3 a5(+w/2, -d/2, +h/2);
  vec3 a6(-w/2, +d/2, +h/2);
  vec3 a7(+w/2, +d/2, +h/2);

  *tris++ = a0; *tris++ = a1; *tris++ = a2;
  *tris++ = a1; *tris++ = a2; *tris++ = a3;

  *tris++ = a0; *tris++ = a1; *tris++ = a4;
  *tris++ = a1; *tris++ = a4; *tris++ = a5;

  *tris++ = a1; *tris++ = a3; *tris++ = a5;
  *tris++ = a3; *tris++ = a5; *tris++ = a7;
  
  *tris++ = a3; *tris++ = a2; *tris++ = a7;
  *tris++ = a2; *tris++ = a7; *tris++ = a6;
  
  *tris++ = a2; *tris++ = a0; *tris++ = a6;
  *tris++ = a0; *tris++ = a6; *tris++ = a4;

  // Normals
  vec3 x(1,0,0); vec3 y(0,1,0); vec3 z(0,0,1);

  *tris++ = -z; *tris++ = -z; *tris++ = -z;
  *tris++ = -z; *tris++ = -z; *tris++ = -z;

  *tris++ = -y; *tris++ = -y; *tris++ = -y;
  *tris++ = -y; *tris++ = -y; *tris++ = -y;

  *tris++ = +x; *tris++ = +x; *tris++ = +x;
  *tris++ = +x; *tris++ = +x; *tris++ = +x;

  *tris++ = +y; *tris++ = +y; *tris++ = +y;
  *tris++ = +y; *tris++ = +y; *tris++ = +y;

  *tris++ = -x; *tris++ = -x; *tris++ = -x;
  *tris++ = -x; *tris++ = -x; *tris++ = -x;
  // clang-format on
}

GLuint compile_shader(const char *vertex_shader, const char *fragment_shader) {
  GLuint vertex, fragment;
  vertex = glCreateShader(GL_VERTEX_SHADER);
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

void upload_fruit_instances(sdlgl_state *s, fruit_body *f, int num_fruit) {
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo_fruit_instances);
  glBufferData(GL_ARRAY_BUFFER, num_fruit * (iZ)sizeof(fruit_body), f,
               GL_STATIC_DRAW);
}

void draw_fruit(sdlgl_state *s, int num_fruit) {
  mat4 proj_mat = glm::perspective(
      glm::radians(69.0f), (float)s->width / (float)s->height, 0.1f, 1000.0f);
  mat4 view_mat = glm::lookAt(s->camera_pos, vec3(0, 0, 1), vec3(0, 0, 1));

  mat4 pv = proj_mat * view_mat;

  glUseProgram(s->prog_fruit);
  GLint loc_pvm = glGetUniformLocation(s->prog_fruit, "pv");
  glUniformMatrix4fv(loc_pvm, 1, GL_FALSE, glm::value_ptr(pv));

  glBindVertexArray(s->vao_fruit);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo_fruit_instances);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo_sphere);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glEnable(GL_CULL_FACE);
  GLint loc_outline = glGetUniformLocation(s->prog_fruit, "outline");

  glUniform1f(loc_outline, 1);
  glFrontFace(GL_CW);
  glDrawArraysInstanced(GL_TRIANGLES, 0, s->sphere_num_verts, num_fruit);

  glUniform1f(loc_outline, 0);
  glFrontFace(GL_CCW);
  glDrawArraysInstanced(GL_TRIANGLES, 0, s->sphere_num_verts, num_fruit);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void draw_box(sdlgl_state *s) {
  mat4 proj_mat = glm::perspective(
      glm::radians(69.0f), (float)s->width / (float)s->height, 0.1f, 1000.0f);
  mat4 view_mat = glm::lookAt(s->camera_pos, vec3(0, 0, 1), vec3(0, 0, 1));
  mat4 vt = glm::translate(mat4(1.0f), vec3(0, 0, 1));

  mat4 model_mat = glm::mat4(1.0f);

  mat4 pvm = proj_mat * view_mat * vt * model_mat;

  glUseProgram(s->prog_box);
  GLint loc_pvm = glGetUniformLocation(s->prog_box, "pvm");
  glUniformMatrix4fv(loc_pvm, 1, GL_FALSE, glm::value_ptr(pvm));

  glBindVertexArray(s->vao_box);
  glBindBuffer(GL_ARRAY_BUFFER, s->vbo_box);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glDisable(GL_CULL_FACE);
  glDrawArrays(GL_TRIANGLES, 0, s->box_num_verts);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void sdlgl_init(sdlgl_state *s, int width, int height, arena memory) {
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

  const char *fruit_vert_code =
#include "../shaders/fruit.vert"
      ;
  const char *fruit_frag_code =
#include "../shaders/fruit.frag"
      ;

  GLuint fruit_program = compile_shader(fruit_vert_code, fruit_frag_code);
  glUseProgram(fruit_program);

  int ndu = 32, ndv = 16;
  int sphere_num_tris = 2 * ndu * ndv;

  GLuint sphere_mesh_vao;
  GLuint sphere_mesh_vbo, fruit_instance_vbo;
  glGenVertexArrays(1, &sphere_mesh_vao);
  glBindVertexArray(sphere_mesh_vao);
  {
    glGenBuffers(1, &sphere_mesh_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_vbo);
    {
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void *)0);
      glEnableVertexAttribArray(0);
      glVertexAttribDivisor(0, 0);
    }
    glGenBuffers(1, &fruit_instance_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, fruit_instance_vbo);
    {
      uZ pos_offset =
          offsetof(fruit_body, body) + offsetof(rigidbody, position);
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(fruit_body),
                            (void *)pos_offset);
      glEnableVertexAttribArray(1);
      glVertexAttribDivisor(1, 1);

      uZ ori_offset =
          offsetof(fruit_body, body) + offsetof(rigidbody, orientation);
      for (u32 i = 0; i < 3; ++i) {
        glVertexAttribPointer(2 + i, 3, GL_FLOAT, GL_FALSE, sizeof(fruit_body),
                              (void *)(ori_offset + i * sizeof(vec3)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1);
      }

      glVertexAttribIPointer(5, 1, GL_INT, sizeof(fruit_body),
                             (void *)offsetof(fruit_body, id));
      glEnableVertexAttribArray(5);
      glVertexAttribDivisor(5, 1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glBindVertexArray(0);

  const char *box_vert_code =
#include "../shaders/box.vert"
      ;
  const char *box_frag_code =
#include "../shaders/box.frag"
      ;

  GLuint box_program = compile_shader(box_vert_code, box_frag_code);
  glUseProgram(box_program);

  int box_num_tris = 10;
  GLuint box_mesh_vao;
  GLuint box_mesh_vbo;
  glGenVertexArrays(1, &box_mesh_vao);
  glBindVertexArray(box_mesh_vao);
  {
    glGenBuffers(1, &box_mesh_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, box_mesh_vbo);
    {
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                            (void *)0);
      glEnableVertexAttribArray(0);
      glVertexAttribPointer(
          1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
          (void *)(box_num_tris * 3 * 3 * (iZ)sizeof(GLfloat)));
      glEnableVertexAttribArray(1);
    }
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glBindVertexArray(0);

  {
    arena scratch = memory;
    vec3 *tris = arena_push<vec3>(&scratch, 3 * sphere_num_tris);
    make_UV_sphere_tris(ndu, ndv, tris, &scratch);

    printf("Generating fruit with %i tris,%i verts\n", sphere_num_tris,
           3 * sphere_num_tris);
    glBindVertexArray(sphere_mesh_vao);
    glBindBuffer(GL_ARRAY_BUFFER, sphere_mesh_vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * sphere_num_tris * 3 * (iZ)sizeof(GLfloat),
                 tris, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  {
    arena scratch = memory;
    vec3 *tris = arena_push<vec3>(&scratch, 6 * box_num_tris);
    make_box_tris(BOX_WIDTH, BOX_DEPTH, BOX_HEIGHT, 0.3f, tris, &scratch);

    glBindVertexArray(box_mesh_vao);
    glBindBuffer(GL_ARRAY_BUFFER, box_mesh_vbo);
    glBufferData(GL_ARRAY_BUFFER, 6 * box_num_tris * 3 * (iZ)sizeof(GLfloat),
                 tris, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
  }

  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  SDL_GL_SwapWindow(window);

  melon_state game{};
  melon_init(&game, &memory);

  s->width = width;
  s->height = height;

  s->window = window;
  s->keyb = SDL_GetKeyboardState(0);

  s->prog_fruit = fruit_program;
  s->vao_fruit = sphere_mesh_vao;
  s->vbo_sphere = sphere_mesh_vbo;
  s->sphere_num_verts = sphere_num_tris * 3;
  s->vbo_fruit_instances = fruit_instance_vbo;

  s->prog_box = box_program;
  s->vao_box = box_mesh_vao;
  s->vbo_box = box_mesh_vbo;
  s->box_num_verts = box_num_tris * 3;

  s->memory = memory;

  s->camera_pos = vec3(0, -2, 1);

  s->game = game;
}

void process_event_queue(sdlgl_state *s, arena *mem) {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    switch (e.type) {
    case SDL_QUIT: {
      exit(0);
    } break;
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

  float axisLR = s->keyb[SDL_SCANCODE_D] - s->keyb[SDL_SCANCODE_A];
  float axisFB = s->keyb[SDL_SCANCODE_W] - s->keyb[SDL_SCANCODE_S];
  float axisUD = s->keyb[SDL_SCANCODE_SPACE] - s->keyb[SDL_SCANCODE_LSHIFT];

  float L = glm::length(s->camera_pos);

  float target_L = L - axisFB / 10.0f;

  vec3 tang = vec3(-s->camera_pos.y, s->camera_pos.x, 0) / L;
  s->camera_pos += (axisLR * tang + axisUD * vec3(0, 0, 1)) * 0.1f;
  s->camera_pos *= target_L / glm::length(s->camera_pos);
}

void sdlgl_loop(sdlgl_state *s) {
  arena frame_memory = arena_split(&s->memory, 8_MB);
  process_event_queue(s, &frame_memory);

  renderer_input stuff_to_upload;
  melon_tick(&s->game, &stuff_to_upload, &frame_memory);

  upload_fruit_instances(s, stuff_to_upload.fruit, stuff_to_upload.num_fruit);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_fruit(s, stuff_to_upload.num_fruit);
  draw_box(s);
  SDL_GL_SwapWindow(s->window);

  arena_rejoin(&s->memory, &frame_memory);
}
