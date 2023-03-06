#ifndef VECTOR_H_
#define VECTOR_H_

#include "common.h"
#include <string.h>
#include <assert.h>

// Template for a dynamically allocated array of `T`s
// Requirements: type T has its constructor (T##Init), copy constructor (T##CopyFrom) and
// destructor (T##Free) defined
#define VECTOR_DECLARE(T, NAME) \
  /* Dynamically allocated array of `T`s */ \
  /* Class invariant: size <= capacity && (p points to a memory block of length == capacity) */ \
  typedef struct { \
    T* p;            /* Pointer readonly, content writable */ \
    size_t size;     /* Readonly */ \
    size_t capacity; /* Readonly */ \
  } NAME; \
  \
  /* Constructor */ \
  void NAME##Init(NAME* this); \
  \
  /* Copy constructor */ \
  /* Pre: this != src */ \
  void NAME##CopyFrom(NAME* this, const NAME* src); \
  \
  /* Destructor (frees memory and invalidates object) */ \
  void NAME##Free(NAME* this); \
  \
  /* Reserve (makes `this->capacity >= n`, reallocating if necessary) */ \
  void NAME##Reserve(NAME* this, size_t n); \
  \
  /* Resize */ \
  /* If `n <= this->size`, tail elements will be deleted */ \
  /* If `n > this->size`, new elements will be zero-initialised */ \
  void NAME##Resize(NAME* this, size_t n); \
  \
  /* Append an element */ \
  void NAME##PushBack(NAME* this, const T* elem);

// For use in implementation (`.c`) files
#define VECTOR_INITIAL_CAPACITY 10
#define VECTOR_DEFINE(T, NAME) \
  \
  void NAME##Init(NAME* this) { \
    this->size = 0; \
    this->capacity = VECTOR_INITIAL_CAPACITY; \
    this->p = malloc(VECTOR_INITIAL_CAPACITY * sizeof(T)); \
    assert(this->p != NULL); \
  } \
  \
  void NAME##CopyFrom(NAME* this, const NAME* src) { \
    this->size = src->size; \
    this->capacity = src->capacity; \
    this->p = malloc(src->capacity * sizeof(T)); \
    assert(this->p != NULL); \
    for (size_t i = 0; i < src->size; i++) T##CopyFrom(&this->p[i], &src->p[i]); \
  } \
  \
  void NAME##Free(NAME* this) { \
    for (size_t i = 0; i < this->size; i++) T##Free(&this->p[i]); \
    free(this->p); \
  } \
  \
  void NAME##Reserve(NAME* this, size_t n) { \
    if (this->capacity >= n) return; \
    while (this->capacity < n) this->capacity *= 2; \
    this->p = realloc(this->p, this->capacity * sizeof(T)); \
    assert(this->p != NULL); \
  } \
  \
  void NAME##Resize(NAME* this, size_t n) { \
    if (this->size >= n) { \
      for (size_t i = n; i < this->size; i++) T##Free(&this->p[i]); \
      this->size = n; \
      return; \
    } \
    /* Mid: this->size < n */ \
    NAME##Reserve(this, n); \
    for (size_t i = this->size; i < n; i++) T##Init(&this->p[i]); \
    this->size = n; \
  } \
  \
  void NAME##PushBack(NAME* this, const T* elem) { \
    NAME##Reserve(this, this->size + 1); \
    T##CopyFrom(&this->p[this->size], elem); \
    this->size++; \
  }

// Commonly-used template instantiations
VECTOR_DECLARE(byte, ByteVector)
VECTOR_DECLARE(char, String)
VECTOR_DECLARE(int, IntVector)
VECTOR_DECLARE(size_t, SizeVector)
VECTOR_DECLARE(String, StringVector)

// Template specialisation for vectors of `unsigned char`
// 32-bit access (little endian, assuming `unsigned char` is 8-bit)
// Pre: i + 4 <= this->size
uint32_t ByteVectorGetU32LE(const ByteVector* this, size_t i);
uint32_t ByteVectorGetU32BE(const ByteVector* this, size_t i);
void ByteVectorSetU32LE(ByteVector* this, size_t i, uint32_t val);
void ByteVectorSetU32BE(ByteVector* this, size_t i, uint32_t val);

#endif // VECTOR_H_
