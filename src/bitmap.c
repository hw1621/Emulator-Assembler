#include "bitmap.h"
#include "common.h"
#include "vector.h"
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#define PI 3.1415926535898
#define NUM_POINTS 200
#define NUM_LINES 5000
#define LINE_OPACITY 25
#define END_THRESHOLD 200

size_t align(size_t x, size_t a) {
  return x % a == 0 ? x : x + a - x % a;
}

#pragma pack(push)
#pragma pack(1) // Avoid structure padding

typedef struct {
  int16_t magic;
  int32_t size;
  int16_t reserved1, reserved2;
  int32_t offset;
} FileHeader;

typedef struct {
  int32_t size, width, height;
  int16_t planes, depth;
  int32_t compression, sizeImage, xPixelsPerMetre, yPixelsPerMetre, clrUsed, clrImportant;
} InfoHeader;

#pragma pack(pop)

void FileHeaderInit(FileHeader* this) {
  this->magic = 0x4D42;
  this->size = 0;
  this->reserved1 = this->reserved2 = 0;
  this->offset = 54;
}

void InfoHeaderInit(InfoHeader* this) {
  this->size = 40;
  this->width = this->height = 0;
  this->planes = 1;
  this->depth = 24;
  this->compression = this->sizeImage = 0;
  this->xPixelsPerMetre = this->yPixelsPerMetre = 3780;
  this->clrUsed = this->clrImportant = 0;
}

void BitmapInit(Bitmap* this, size_t width, size_t height) {
  this->width = width;
  this->height = height;
  this->pitch = align(width * 3, 4);
  this->p = malloc(this->height * this->pitch);
  assert(this->p != NULL);
  memset(this->p, 0, this->height * this->pitch);
}

bool BitmapLoadFrom(Bitmap* this, const char* filename) {
  FileHeader bfh;
  FileHeaderInit(&bfh);
  InfoHeader bih;
  InfoHeaderInit(&bih);
  FILE* in = NULL;
  this->p = NULL;

  if ((in = fopen(filename, "rb")) == NULL) goto fail;
  if (fread(&bfh, sizeof(FileHeader), 1, in) != 1) goto fail;
  if (fread(&bih, sizeof(InfoHeader), 1, in) != 1) goto fail;

  this->width = bih.width;
  this->height = bih.height;
  this->pitch = align(bih.width * 3, 4);
  size_t count = this->height * this->pitch;
  this->p = malloc(count);
  assert(this->p != NULL);

  if (fread(this->p, 1, count, in) != count) goto fail;
  if (fclose(in) != 0) goto fail;
  BitmapInvertSwap(this);
  return true;

fail:
  if (in != NULL) fclose(in);
  if (this->p != NULL) free(this->p);
  BitmapInit(this, 1, 1); // Returns black 1x1 bitmap on failure
  return false;
}

void BitmapFree(Bitmap* this) {
  free(this->p);
}

bool BitmapGetPixel(const Bitmap* this, size_t x, size_t y, int* r, int* g, int* b) {
  if (x >= this->width || y >= this->height) return false;
  size_t i = y * this->pitch + x * 3;
  *r = this->p[i];
  *g = this->p[i + 1];
  *b = this->p[i + 2];
  return true;
}

bool BitmapSetPixel(Bitmap* this, size_t x, size_t y, int r, int g, int b) {
  if (x >= this->width || y >= this->height) return false;
  if (r < 0) r = 0;
  if (r > 255) r = 255;
  if (g < 0) g = 0;
  if (g > 255) g = 255;
  if (b < 0) b = 0;
  if (b > 255) b = 255;
  size_t i = y * this->pitch + x * 3;
  this->p[i] = r;
  this->p[i + 1] = g;
  this->p[i + 2] = b;
  return true;
}

void BitmapInvertSwap(Bitmap* this) {
  // Upside down
  for (size_t i = 0; i < this->height / 2; i++) {
    for (size_t j = 0; j < this->pitch; j++) {
      size_t ind = i * this->pitch + j;
      size_t rind = (this->height - 1 - i) * this->pitch + j;
      unsigned char t = this->p[ind];
      this->p[ind] = this->p[rind];
      this->p[rind] = t;
    }
  }
  // Swap red and blue channels
  for (size_t i = 0; i < this->height; i++) {
    for (size_t j = 0; j < this->width; j++) {
      size_t ind = i * this->pitch + j * 3;
      unsigned char t = this->p[ind];
      this->p[ind] = this->p[ind + 2];
      this->p[ind + 2] = t;
    }
  }
}

bool BitmapSave(Bitmap* this, const char* filename) {
  FileHeader bfh;
  FileHeaderInit(&bfh);
  InfoHeader bih;
  InfoHeaderInit(&bih);
  bfh.size = (int32_t) (this->height * this->pitch + 54);
  bih.width = (int32_t) this->width;
  bih.height = (int32_t) this->height;
  FILE* out = NULL;

  BitmapInvertSwap(this);
  if ((out = fopen(filename, "wb")) == NULL) goto fail;
  if (fwrite(&bfh, sizeof(FileHeader), 1, out) != 1) goto fail;
  if (fwrite(&bih, sizeof(InfoHeader), 1, out) != 1) goto fail;
  size_t count = this->height * this->pitch;
  if (fwrite(this->p, 1, count, out) != count) goto fail;
  if (fclose(out) != 0) goto fail;
  BitmapInvertSwap(this);
  return true;

fail:
  if (out != NULL) fclose(out);
  BitmapInvertSwap(this);
  return false;
}

typedef struct { int x, y; } Point;

void PointInit(Point* p) { p->x = p->y = 0; }
void PointCopyFrom(Point* p, const Point* src) { p->x = src->x; p->y = src->y; }
void PointFree(Point* p) { MAYBE_UNUSED(p); }

VECTOR_DECLARE(Point, PointVector)
VECTOR_DEFINE(Point, PointVector)

void BitmapFill(Bitmap* this, int r, int g, int b) {
  for (size_t y = 0; y < this->height; y++) {
    for (size_t x = 0; x < this->width; x++) {
      BitmapSetPixel(this, x, y, r, g, b);
    }
  }
}

void BitmapToGreyscale(Bitmap* this) {
  for (size_t y = 0; y < this->height; y++) {
    for (size_t x = 0; x < this->width; x++) {
      int r, g, b, grey;
      BitmapGetPixel(this, x, y, &r, &g, &b);
      grey = (int) (0.299 * r + 0.587 * g + 0.114 * b);
      BitmapSetPixel(this, x, y, grey, grey, grey);
    }
  }
}

// Returns all points on the line
void linePoints(Point s, Point t, PointVector* res) {
  int xdist = abs(t.x - s.x);
  int ydist = abs(t.y - s.y);
  if (xdist == 0 && ydist == 0) return;
  if (ydist <= xdist) { // Slope <= 1
    // Mid: s.x != t.x
    if (s.x > t.x) { Point u = s; s = t; t = u; }
    // Mid: s.x < t.x
    for (int x = s.x; x <= t.x; x++) {
      Point curr = { .x = x, .y = (t.y - s.y) * (x - s.x) / (t.x - s.x) + s.y };
      PointVectorPushBack(res, &curr);
    }
  } else { // Slope > 1
    // Mid: s.y != t.y
    if (s.y > t.y) { Point u = s; s = t; t = u; }
    // Mid: s.y < t.y
    for (int y = s.y; y <= t.y; y++) {
      Point curr = { .x = (t.x - s.x) * (y - s.y) / (t.y - s.y) + s.x, .y = y };
      PointVectorPushBack(res, &curr);
    }
  }
}

// Builds a circle
void buildCircle(const Bitmap* this, int radius, size_t count, PointVector* res) {
  Point centre = (Point) { .x = ((int) this->width - 1) / 2, .y = ((int) this->height - 1) / 2 };
  for (size_t i = 0; i < count; i++) {
    double rad = 2 * PI * i / count;
    Point curr = {
      .x = centre.x + (int) (cos(rad) * radius + 0.5),
      .y = centre.y + (int) (sin(rad) * radius + 0.5)
    };
    PointVectorPushBack(res, &curr);
  }
}

// Pre: input image is greyscale (the RGB values of each pixel are the same)
double getAvgBrightness(const Bitmap* this, Point s, Point t) {
  double res = 0.0;
  PointVector points;
  PointVectorInit(&points);
  linePoints(s, t, &points);
  size_t count = 0;
  for (size_t i = 0; i < points.size; i++) {
    int r = 0, g = 0, b = 0;
    if (!BitmapGetPixel(this, points.p[i].x, points.p[i].y, &r, &g, &b)) continue;
    assert(r == g && g == b);
    res += r;
    count++;
  }
  res /= count;
  PointVectorFree(&points);
  return res;
}

void brightenLine(Bitmap* this, Point s, Point t, int amount) {
  PointVector points;
  PointVectorInit(&points);
  linePoints(s, t, &points);
  for (size_t i = 0; i < points.size; i++) {
    int r = 0, g = 0, b = 0;
    if (!BitmapGetPixel(this, points.p[i].x, points.p[i].y, &r, &g, &b)) continue;
    BitmapSetPixel(this, points.p[i].x, points.p[i].y, r + amount, g + amount, b + amount);
  }
  PointVectorFree(&points);
}

void BitmapWirenize(Bitmap* this, int radius) {
  PointVector points;
  PointVectorInit(&points);
  SizeVector starts, ends;
  SizeVectorInit(&starts);
  SizeVectorInit(&ends);

  BitmapToGreyscale(this);
  buildCircle(this, radius, NUM_POINTS, &points);

  size_t last = 0;
  for (size_t l = 0; l < NUM_LINES; l++) {
    double best = 256.0;
    size_t next = 0;
    for (size_t i = 0; i < points.size; i++) if (i != last) {
      double curr = getAvgBrightness(this, points.p[last], points.p[i]);
      if (curr < best) {
        best = curr;
        next = i;
      }
    }
    SizeVectorPushBack(&starts, &last);
    SizeVectorPushBack(&ends, &next);
    brightenLine(this, points.p[last], points.p[next], LINE_OPACITY);
    last = next;
    if (best > END_THRESHOLD) break;
  }

  BitmapFill(this, 255, 255, 255);
  for (size_t i = 0; i < starts.size; i++) {
    size_t start = starts.p[i], end = ends.p[i];
    brightenLine(this, points.p[start], points.p[end], -LINE_OPACITY);
  }

  PointVectorFree(&points);
  SizeVectorFree(&starts);
  SizeVectorFree(&ends);
}
