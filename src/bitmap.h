#ifndef BITMAP_H_
#define BITMAP_H_

#include <stdlib.h>
#include <stdbool.h>

// Class invariant: width * 3 <= pitch && (p points to a memory block of length == height * pitch)
typedef struct {
  size_t width, height, pitch; // Readonly
  unsigned char* p;            // Private
} Bitmap;

void BitmapInit(Bitmap* this, size_t width, size_t height);
bool BitmapLoadFrom(Bitmap* this, const char* filename);
void BitmapFree(Bitmap* this);

bool BitmapGetPixel(const Bitmap* this, size_t x, size_t y, int* r, int* g, int* b);
bool BitmapSetPixel(Bitmap* this, size_t x, size_t y, int r, int g, int b);

void BitmapInvertSwap(Bitmap* this);
bool BitmapSave(Bitmap* this, const char* filename);

void BitmapFill(Bitmap* this, int r, int g, int b);
void BitmapToGreyscale(Bitmap* this);
void BitmapWirenize(Bitmap* this, int radius);

#endif // BITMAP_H_
