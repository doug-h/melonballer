#pragma once

#include <cstdint>
#include <cstdlib>
#include <cstddef>
#include <cassert>
#include <cstdio>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>

typedef glm::vec3 vec3f;
typedef glm::mat4 mat4f;

#define ASSERT(c) assert((c))

#define PI 3.141592653589793238
#define TWO_PI 6.283185307179586477


typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef size_t   uZ;

typedef unsigned long long uLL;

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef ptrdiff_t iZ;

typedef float  f32;
typedef double f64;
static_assert(sizeof(float) == 4);
static_assert(sizeof(double) == 8);

uLL operator""_KB(uLL s) { return s << 10; }
uLL operator""_MB(uLL s) { return s << 20; }
uLL operator""_GB(uLL s) { return s << 30; }

// Bump down allocator
struct arena {
  u8 *head;
  u8 *tail;
};
arena new_arena(iZ size);
void  free_arena(arena *a);

template <class T>
T  *arena_push(arena *, iZ num_T);
u8 *arena_push_bytes(arena *, iZ num_bytes, uZ align = 1);

arena arena_split(arena *a, iZ num_bytes);
void  arena_rejoin(arena *parent, arena *split);

// Contiguous fixed size array, with swap&pop erase
template <class T>
struct array {
  T *base;
  T *tail;
  iZ cap;

  void push(T v) { *tail++ = v; }
  T    pop() { return *--tail; }
  void clear() { tail = base; }
  void erase(iZ i) { base[i] = *--tail; }

  iZ   size() const { return tail - base; }
  iZ   free() const { return cap - size(); }
  bool isempty() const { return size() == 0; }
  bool isfull() const { return free() == 0; }
};

template <class T>
array<T> new_array(iZ cap);
template <class T>
void free_array(array<T> *);

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

template <class T>
T *arena_push(arena *a, iZ num_T) {
  ASSERT(num_T < PTRDIFF_MAX / sizeof(T));
  return (T *)arena_push_bytes(a, num_T * (iZ)sizeof(T), alignof(T));
}

arena arena_split(arena *a, iZ num_bytes) {
  arena result;
  result.head = a->head;
  result.tail = a->head + num_bytes;
  ASSERT(result.tail <= a->tail);
  a->head = result.tail;
  return result;
}
void arena_rejoin(arena *parent, arena *split) {
  parent->head = split->head;
  // split no longer valid
  split->tail  = split->head;
}

template <class T>
array<T> new_array(arena *a, iZ cap) {
  T *store = arena_push<T>(a, cap);
  ASSERT(store);
  return {.base = store, .tail = store, .cap = cap};
}
template <class T>
void free_array(array<T> *a) {
  ASSERT(a->base);
  free(a->base);
  a->cap = 0;
}
