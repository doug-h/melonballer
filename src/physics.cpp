#include "physics.h"
#include "melongame.h"

void renormalise(mat3 &M); // Re-normalizes nearly orthonormal matrix

// finds the point on ellip furthest in the direction dir
// returns the vector in world space, relative to the ellip origin
vec3 support_ellip(fruit_body *ellip, vec3 dir) {
  /*
   * Ellipsoid = R * A * Unit Sphere
   *   where R = orientation matrix of body
   *             (r1, 0,  0)
   *     and A = (0, r2,  0) contains it's radii
   *             (0,  0, r3)
   *
   * Furthest point on ellip in direction n is
   *   p_local = A * R^T * n
   *   where R^T is the transpose (inverse rotation) of R
   */
  vec3 A = TABLE_fruit_type[ellip->id].radii;
  mat3 R = ellip->body.orientation;
  mat3 RT = glm::transpose(R);
  return R * (A * (RT * dir));
}

struct collision_manifold {
  float gap;

  vec3 r_pa;
  vec3 r_pb;
  vec3 n_ba;
};

collision_manifold collision_ellip_plane(fruit_body *ellip, vec3 plane_origin,
                                         vec3 plane_normal) {
  vec3 r_pa = support_ellip(ellip, -plane_normal);
  vec3 r_pb = r_pa + ellip->body.position - plane_origin;
  float gap = glm::dot(r_pb, plane_normal);

  collision_manifold result;
  result.gap = gap;
  result.r_pa = r_pa;
  result.r_pb = r_pb;
  result.n_ba = -plane_normal;

  return result;
}

#if 0
// TODO - Think about consistent labelling, required for warm starting
collision_manifold collision_ellip_ellip(fruit_body *ellip_a,
                                         fruit_body *ellip_b) {
  vec3 n_ba
  vec3 r_pa = support_ellip(ellip_a, 
}
#endif

void physics_step(fruit_body *fruit, body_dynamics *dynamics, iZ num_bodies,
                  float dt) {
  float gravity = -10.0f;

  // Integrate velocities
  for (int i = 0; i < num_bodies; ++i) {
    const fruit_type &type = TABLE_fruit_type[fruit[i].id];

    dynamics[i].linear_velocity +=
        (type.inv_mass) ? dt * vec3(0.0f, 0.0f, gravity) : vec3(0.0f);

    // No active forces or torques yet (other than gravity), so this is useless.
    // dynamics[i].linear_velocity += dt * type.inv_mass * dynamics[i].force;
    // dynamics[i].angular_velocity += dt * inv_moi_world * dynamics[i].torque;

    dynamics[i].linear_velocity.z += (type.inv_mass) ? dt * gravity : 0.0f;
  }

  // Solve velocity constraints
  for (int i = 0; i < num_bodies; ++i) {
    collision_manifold floor_test =
        collision_ellip_plane(&(fruit[i]), vec3(0.0f), vec3(0.0f, 0.0f, 1.0f));

    if (floor_test.gap <= 0.0f) {
      const fruit_type &type = TABLE_fruit_type[fruit[i].id];

      mat3 inv_moi_world = fruit[i].body.orientation * type.inv_moi *
                           glm::transpose(fruit[i].body.orientation);

      vec3 n = -floor_test.n_ba;
      vec3 r = floor_test.r_pa;
        
      printf("%.2f\n", floor_test.gap);
      //printf("%.2f,%.2f,%.2f\n",  r.x, r.y, r.z);

      float inv_mass =
          type.inv_mass +
          glm::dot(n, glm::cross(inv_moi_world * glm::cross(r, n), r));

      vec3 dv = dynamics[i].linear_velocity +
                glm::cross(dynamics[i].angular_velocity, r);

      float vn = glm::dot(dv, n);
      float impulse = vn / inv_mass;


      dynamics[i].linear_velocity -= type.inv_mass * impulse * n;
      dynamics[i].angular_velocity -=
          inv_moi_world * impulse * glm::cross(r, n);
    }
  }

  // Integrate positions
  for (int i = 0; i < num_bodies; ++i) {
    fruit[i].body.position += dt * dynamics[i].linear_velocity;

    float w1, w2, w3;
    w1 = dynamics[i].angular_velocity[0];
    w2 = dynamics[i].angular_velocity[1];
    w3 = dynamics[i].angular_velocity[2];
    mat3 ang_screw(0.0f, -w3, w2, w3, 0.0f, -w1, -w2, w1, 0.0);
    mat3 R = fruit[i].body.orientation;

    R += dt * ang_screw * R;

    renormalise(R);
    fruit[i].body.orientation = R;
  }

  // Solve position constraints
  for (int i = 0; i < num_bodies; ++i) {
    collision_manifold floor_test =
        collision_ellip_plane(&(fruit[i]), vec3(0.0f), vec3(0.0f, 0.0f, 1.0f));

    if (floor_test.gap <= 0.0f) {
      fruit[i].body.position += floor_test.gap * floor_test.n_ba;
    }
  }
}

void renormalise(mat3 &M) // Re-normalizes nearly orthonormal matrix
{
  double alpha = M[0][0] * M[0][0] + M[1][0] * M[1][0] +
                 M[2][0] * M[2][0]; // First column's norm squared
  double beta = M[0][1] * M[0][1] + M[1][1] * M[1][1] +
                M[2][1] * M[2][1]; // Second column's norm squared
  double gamma = M[0][2] * M[0][2] + M[1][2] * M[1][2] +
                 M[2][2] * M[2][2];  // Third column's norm squared
  alpha = 1.0 - 0.5 * (alpha - 1.0); // Get mult. factor
  beta = 1.0 - 0.5 * (beta - 1.0);
  gamma = 1.0 - 0.5 * (gamma - 1.0);
  M[0][0] *= alpha;
  M[1][0] *= alpha;
  M[2][0] *= alpha;
  M[0][1] *= beta;
  M[1][1] *= beta;
  M[2][1] *= beta;
  M[0][2] *= gamma;
  M[1][2] *= gamma;
  M[2][2] *= gamma;
  alpha = M[0][0] * M[0][1] + M[1][0] * M[1][1] +
          M[2][0] * M[2][1]; // First and second column dot product
  beta = M[0][0] * M[0][2] + M[1][0] * M[1][2] +
         M[2][0] * M[2][2]; // First and third column dot product
  gamma = M[0][1] * M[0][2] + M[1][1] * M[1][2] +
          M[2][1] * M[2][2]; // Second and third column dot product
  alpha *= 0.5;
  beta *= 0.5;
  gamma *= 0.5;
  double temp1, temp2;
  temp1 = M[0][0] - alpha * M[0][1] - beta * M[0][2]; // Update row1
  temp2 = M[0][1] - alpha * M[0][0] - gamma * M[0][2];
  M[0][2] -= beta * M[0][0] + gamma * M[0][1];
  M[0][0] = temp1;
  M[0][1] = temp2;
  temp1 = M[1][0] - alpha * M[1][1] - beta * M[1][2]; // Update row2
  temp2 = M[1][1] - alpha * M[1][0] - gamma * M[1][2];
  M[1][2] -= beta * M[1][0] + gamma * M[1][1];
  M[1][0] = temp1;
  M[1][1] = temp2;
  temp1 = M[2][0] - alpha * M[2][1] - beta * M[2][2]; // Update row3
  temp2 = M[2][1] - alpha * M[2][0] - gamma * M[2][2];
  M[2][2] -= beta * M[2][0] + gamma * M[2][1];
  M[2][0] = temp1;
  M[2][1] = temp2;
}
