// This is an example program waiting to be translated to run on the emulator...
#include <math.h>
#include "../../src/bitmap.h"

#define PI 3.1415926f

// Converts HSV colour representation (hue, saturation, value) to RGB (red, green, blue)
// Pre: (0 <= h <= 2 * PI) && (0 <= s <= 1) && (0 <= v <= 1)
// See: https://en.wikipedia.org/wiki/HSL_and_HSV#To_RGB
void hsvrgb(float h, float s, float v, float* r, float* g, float* b) {
  float c = s * v;
  float h1 = h / (PI / 3.0f);
  float x = c * (1.0f - fabsf(fmodf(h1, 2.0f) - 1.0f));
  switch ((int) h1) {
    case 0: *r = c; *g = x; *b = 0; break;
    case 1: *r = x; *g = c; *b = 0; break;
    case 2: *r = 0; *g = c; *b = x; break;
    case 3: *r = 0; *g = x; *b = c; break;
    case 4: *r = x; *g = 0; *b = c; break;
    case 5: *r = c; *g = 0; *b = x; break;
  }
  float m = v - c;
  *r += m; *g += m; *b += m;
}

int main(void) {
  const int width = 640;
  const int height = 480;
  Bitmap bitmap;
  BitmapInit(&bitmap, width, height);

  for (int y = 0; y < height; y++)
    for (int x = 0; x < width; x++) {
      // `xrel` and `yrel` are roughly in the range [-1, 1]
      // (Keeping aspect ratio)
      float xrel = (x - width / 2) / (float) (height / 2);
      float yrel = (height / 2 - y) / (float) (height / 2);
      // Calculate colours for the current pixel
      float hue = atan2f(yrel, xrel);
      if (hue < 0) hue += 2 * PI;
      float sat = 1.0f;
      float val = sqrtf(xrel * xrel + yrel * yrel);
      float r, g, b;
      hsvrgb(hue, sat, val, &r, &g, &b);
      // Set pixel colour
      BitmapSetPixel(&bitmap, x, y, (int) (r * 256), (int) (g * 256), (int) (b * 256));
    }

  BitmapSave(&bitmap, "output.bmp");
  BitmapFree(&bitmap);
  return 0;
}
