#include "melongame.h"
#include "types.h"

float gravity = 10;

fruit make_fruit(int fruit_id) {
  float r1, r2, r3;
  r1 = TABLE_fruit_type[fruit_id].radii.x;
  r2 = TABLE_fruit_type[fruit_id].radii.y;
  r3 = TABLE_fruit_type[fruit_id].radii.z;

  float mass, density, volume;
  density = TABLE_fruit_type[fruit_id].density;
  volume  = 4.0f * PI * r1 * r2 * r3 / 3.0f;
  mass    = density * volume;

  float moi_x, moi_y, moi_z;
  moi_x = mass * (r2 * r2 + r3 * r3) / 5.0f;
  moi_y = mass * (r1 * r1 + r3 * r3) / 5.0f;
  moi_z = mass * (r1 * r1 + r2 * r2) / 5.0f;

  fruit f;
  f.id       = (float)fruit_id;
  f.pos      = vec3f(0);
  f.vel      = vec3f(0);
  f.ang_vel  = vec3f(0);
  f.acc_imp  = vec3f(0);
  f.inv_mass = 1.0f / mass;
  f.inv_moi  = vec3f(1.0f / moi_x, 1.0f / moi_y, 1.0f / moi_z);

  return f;
}

void melon_init(melon_state *m, arena *mem_perm) {

  m->render_state_changed = true;

  m->fruit = new_array<fruit>(mem_perm, MAX_FRUIT);
}

void melon_tick(melon_state *m, renderer_input *ri, arena *frame_mem) {
  for (int i = 0; i < m->fruit.size(); ++i) {
    m->fruit.base[i].acc_imp = vec3f(0, 0, -gravity);
  }
  // Solve collisions
  for (int i = 0; i < m->fruit.size(); ++i) {
    if (m->fruit.base[i].pos.z -
            TABLE_fruit_type[(int)m->fruit.base[i].id].radii.z <=
        0.0f) {
      m->fruit.base[i].acc_imp.z = 0;
      m->fruit.base[i].vel.z     = 0;
    }
  }

  for (int i = 0; i < m->fruit.size(); ++i) {
    m->fruit.base[i].vel += m->fruit.base[i].acc_imp / 60.0f;
    m->fruit.base[i].pos += m->fruit.base[i].vel / 60.0f;
  }
  ri->needs_reupload = m->render_state_changed;
  ri->fruit          = m->fruit.base;
  ri->num_fruit      = m->fruit.size();

  m->render_state_changed = true;
}

void melon_mousemotion(melon_state *m) {}
void melon_mousedown(melon_state *m) {
  puts("New fruit");
  fruit s = make_fruit(m->fruit.size() % 2);
  s.pos = vec3f(0,0,1);
  m->fruit.push(s);
  m->render_state_changed = true;
}
void melon_mouseup(melon_state *m) {}
