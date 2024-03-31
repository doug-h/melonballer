#include "melongame.h"
#include "physics.h"
#include "types.h"

float gravity = 10;

void add_fruit(melon_state *m, vec3 pos, int fruit_id) {
  rigidbody body;
  body.position = pos;
  body.orientation = mat3(1.0f);

  body_dynamics dynamics;
  dynamics.linear_velocity = vec3(0.0f);
  dynamics.angular_velocity = vec3(0.0f);

  fruit_body f;
  f.id = (u32)fruit_id;
  f.body = body;

  m->fruit.push(f);
  m->fruit_dynamics.push(dynamics);
}

void melon_init(melon_state *m, arena *mem_perm) {

  // Calculate fruit_type derived properties
  int num_types = sizeof(TABLE_fruit_type) / sizeof(TABLE_fruit_type[0]);
  for (int i = 0; i < num_types; ++i) {
    const fruit_type &f = TABLE_fruit_type[i];

    float r1, r2, r3;
    r1 = f.radii.x;
    r2 = f.radii.y;
    r3 = f.radii.z;

    float mass, density, volume;
    density = f.density;
    volume = 4.0f * PI * r1 * r2 * r3 / 3.0f;
    mass = density * volume;

    float inv_moi_x, inv_moi_y, inv_moi_z;
    inv_moi_x = 5.0f / (mass * (r2 * r2 + r3 * r3));
    inv_moi_y = 5.0f / (mass * (r1 * r1 + r3 * r3));
    inv_moi_z = 5.0f / (mass * (r1 * r1 + r2 * r2));

    TABLE_fruit_type[i].volume = volume;
    TABLE_fruit_type[i].inv_mass = 1.0f / mass;
    TABLE_fruit_type[i].inv_moi = mat3( //
        inv_moi_x, 0.0f, 0.0f,          //
        0.0f, inv_moi_y, 0.0f,          //
        0.0f, 0.0f, inv_moi_z);
  }

  m->fruit = new_array<fruit_body>(mem_perm, MAX_FRUIT);
  m->fruit_dynamics = new_array<body_dynamics>(mem_perm, MAX_FRUIT);
}

void melon_tick(melon_state *m, renderer_input *ri, arena *frame_mem) {
  int physics_substeps = 10;
  for (int i = 0; i < physics_substeps; ++i) {
    physics_step(m->fruit.base, m->fruit_dynamics.base, m->fruit.size(),
                 1.0f / 60 / physics_substeps);
  }

  ri->fruit = m->fruit.base;
  ri->num_fruit = m->fruit.size();
}

void melon_mousemotion(melon_state *m) {}
void melon_mousedown(melon_state *m) {
  puts("New fruit");
  add_fruit(m, vec3(0.0f, 0.0f, (float)BOX_HEIGHT), 1);
  m->fruit.tail[-1].body.orientation =
      mat3(glm::rotate(glm::mat4(1.0f), (float)TWO_PI / 4.0f, vec3(1.0f)));
}
void melon_mouseup(melon_state *m) {}
