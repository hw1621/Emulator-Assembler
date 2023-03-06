#ifndef COMMON_H_
#define COMMON_H_

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// Bounds are inclusive
// Pre: 0 <= lsbi <= msbi < 32
static inline uint32_t bits(uint32_t n, uint32_t lsbi, uint32_t msbi) {
  assert(lsbi <= msbi && msbi < 32);
  return (n >> lsbi) & ((1u << (msbi - lsbi + 1u)) - 1u);
}

static inline void setBits(uint32_t* n, uint32_t lsbi, uint32_t msbi, uint32_t val) {
  assert(lsbi <= msbi && msbi < 32);
  uint32_t m = (1u << (msbi - lsbi + 1u)) - 1u;
  *n = (*n & ~(m << lsbi)) | ((val & m) << lsbi);
}

// See: https://stackoverflow.com/questions/3599160/how-can-i-suppress-unused-parameter-warnings-in-c
#define MAYBE_UNUSED(x) (void)(x)

// Constructors and copy constructors for primitive types
// (For use in e.g. template specialisation of vectors)
typedef unsigned char byte;

static inline void byteInit(byte* this) { *this = 0; }
static inline void byteCopyFrom(byte* this, const byte* rhs) { *this = *rhs; }
static inline void byteFree(byte* this) { MAYBE_UNUSED(this); }

static inline void charInit(char* this) { *this = 0; }
static inline void charCopyFrom(char* this, const char* rhs) { *this = *rhs; }
static inline void charFree(char* this) { MAYBE_UNUSED(this); }

static inline void intInit(int* this) { *this = 0; }
static inline void intCopyFrom(int* this, const int* rhs) { *this = *rhs; }
static inline void intFree(int* this) { MAYBE_UNUSED(this); }

static inline void size_tInit(size_t* this) { *this = 0; }
static inline void size_tCopyFrom(size_t* this, const size_t* rhs) { *this = *rhs; }
static inline void size_tFree(size_t* this) { MAYBE_UNUSED(this); }

#endif // COMMON_H_
