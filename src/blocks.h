#ifndef BLOCKS_H_
#define BLOCKS_H_

#include <stdbool.h>
#include "vector.h"

// Template for a region-based allocator of `T`s
// Requirements: type T has its constructor (T##Init) and destructor (T##Free) defined
#define BLOCKS_DECLARE(T, NAME) \
  \
  typedef T* NAME##Block; \
  void NAME##BlockInit(NAME##Block* this); \
  void NAME##BlockCopyFrom(NAME##Block* this, const NAME##Block* rhs); \
  void NAME##BlockFree(NAME##Block* this); \
  \
  VECTOR_DECLARE(NAME##Block, NAME##BlockVector)\
  \
  /* Class invariant: blocks.size >= 1 && 0 <= next < BLOCKS_BLOCKSIZE */ \
  typedef struct { \
    NAME##BlockVector blocks; /* Private */ \
    size_t next;              /* Private */ \
  } NAME; \
  \
  /* Constructor */ \
  void NAME##Init(NAME* this); \
  \
  /* Destructor (frees memory and invalidates object) */ \
  void NAME##Free(NAME* this); \
  \
  /* Allocate and initialise new object */ \
  T* NAME##New(NAME* this);

// For use in implementation (`.c`) files
#define BLOCKS_BLOCKSIZE 4096
#define BLOCKS_DEFINE(T, NAME) \
  \
  void NAME##BlockInit(NAME##Block* this) { \
    *this = malloc(BLOCKS_BLOCKSIZE * sizeof(T)); \
    assert(*this != NULL); \
  } \
  \
  /* This is never used (since NAME##BlockVector is never copy constructed) */ \
  void NAME##BlockCopyFrom(NAME##Block* this, const NAME##Block* rhs) { \
    MAYBE_UNUSED(this); MAYBE_UNUSED(rhs); \
    assert(false); \
  } \
  \
  void NAME##BlockFree(NAME##Block* this) { \
    free(*this); \
  } \
  \
  VECTOR_DEFINE(NAME##Block, NAME##BlockVector)\
  \
  void NAME##Init(NAME* this) { \
    this->next = 0; \
    NAME##BlockVectorInit(&this->blocks); \
    NAME##BlockVectorResize(&this->blocks, 1); \
  } \
  \
  void NAME##Free(NAME* this) { \
    /* Destroy all objects */ \
    for (size_t i = 0; i < this->blocks.size - 1; i++) { \
      for (size_t j = 0; j < BLOCKS_BLOCKSIZE; j++) T##Free(&this->blocks.p[i][j]); \
    } \
    for (size_t j = 0; j < this->next; j++) T##Free(&this->blocks.p[this->blocks.size - 1][j]); \
    /* Destroy all blocks (automatically done by the vector) */ \
    NAME##BlockVectorFree(&this->blocks); \
  } \
  \
  T* NAME##New(NAME* this) { \
    /* Initialise a new object */ \
    T* res = &this->blocks.p[this->blocks.size - 1][this->next]; \
    T##Init(res); \
    this->next++; \
    if (this->next == BLOCKS_BLOCKSIZE) { \
      /* Create a new block */ \
      this->next = 0; \
      NAME##BlockVectorResize(&this->blocks, this->blocks.size + 1); \
    } \
    return res; \
  }

#endif // BLOCKS_H_
