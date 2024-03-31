#pragma once

#include "types.h"

struct rigidbody {
  vec3 position;
  mat3 orientation;
};

struct body_dynamics {
  vec3 linear_velocity;
  vec3 angular_velocity;
};

struct fruit_body {
  rigidbody body;

  u32 id;
};

#define PHYSICS_SLOP (1e-3)

void physics_step(fruit_body *, body_dynamics *, iZ num_bodies, float dt);
