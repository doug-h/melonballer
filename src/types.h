#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cassert>

#define ASSERT(c) assert((c))

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t uZ;

typedef unsigned long long uLL;

typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef ptrdiff_t iZ;

typedef float f32;
typedef double f64;
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

uLL operator""_KB(uLL s) { return s << 10; }
uLL operator""_MB(uLL s) { return s << 20; }
uLL operator""_GB(uLL s) { return s << 30; }

struct arena {
  u8 *head;
  u8 *tail;
};
arena new_arena(iZ size);
void free_arena(arena *a);

template <typename T>
T *arena_push(arena *a, iZ num_T);
u8 *arena_push_bytes(arena *a, iZ num_bytes, uZ align = 1);

//

arena new_arena(iZ size) {
  ASSERT(size > 0);
  u8 *head = (u8 *)calloc((uZ)size, 1);
  ASSERT(head);
  return {.head = head, .tail = head + size};
}

void free_arena(arena *a) {
  ASSERT(a->head);
  free(a->head);
  a->head = a->tail = 0;
}

u8 *arena_push_bytes(arena *a, iZ num_bytes, uZ align) {
  u8 *aligned_tail = (u8 *)((uintptr_t)a->tail & ~(align - 1));

  iZ free = aligned_tail - a->head;
  if (free < num_bytes) {
    ASSERT(0); // OOM
  }
  a->tail = aligned_tail - num_bytes;
  return a->tail;
}

template <typename T>
T *arena_push(arena *a, iZ num_T) {
  ASSERT(num_T < PTRDIFF_MAX / sizeof(T));
  return (T *)arena_push_bytes(a, num_T * (iZ)sizeof(T), alignof(T));
}
