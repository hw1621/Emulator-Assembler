#include "vector.h"

VECTOR_DEFINE(byte, ByteVector)
VECTOR_DEFINE(char, String)
VECTOR_DEFINE(int, IntVector)
VECTOR_DEFINE(size_t, SizeVector)
VECTOR_DEFINE(String, StringVector)

uint32_t ByteVectorGetU32LE(const ByteVector* this, size_t i) {
  return
    (uint32_t) this->p[i + 0] +
    ((uint32_t) this->p[i + 1] << 8) +
    ((uint32_t) this->p[i + 2] << 16) +
    ((uint32_t) this->p[i + 3] << 24);
}

uint32_t ByteVectorGetU32BE(const ByteVector* this, size_t i) {
  return
    ((uint32_t) this->p[i + 0] << 24) +
    ((uint32_t) this->p[i + 1] << 16) +
    ((uint32_t) this->p[i + 2] << 8) +
    (uint32_t) this->p[i + 3];
}

void ByteVectorSetU32LE(ByteVector* this, size_t i, uint32_t val) {
  this->p[i + 0] = (unsigned char) (val & 0xff);
  this->p[i + 1] = (unsigned char) ((val >> 8) & 0xff);
  this->p[i + 2] = (unsigned char) ((val >> 16) & 0xff);
  this->p[i + 3] = (unsigned char) ((val >> 24) & 0xff);
}

void ByteVectorSetU32BE(ByteVector* this, size_t i, uint32_t val) {
  this->p[i + 0] = (unsigned char) ((val >> 24) & 0xff);
  this->p[i + 1] = (unsigned char) ((val >> 16) & 0xff);
  this->p[i + 2] = (unsigned char) ((val >> 8) & 0xff);
  this->p[i + 3] = (unsigned char) (val & 0xff);
}
