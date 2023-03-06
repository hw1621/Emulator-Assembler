#include <stdlib.h>
#include <stdio.h>
#include "bitmap.h"

int main(int argc, char** argv) {
  if (argc != 3) {
    printf("Usage: ./wirenizer <input-filename> <output-filename>\n");
    return EXIT_FAILURE;
  }

  Bitmap bitmap;
  if (!BitmapLoadFrom(&bitmap, argv[1])) {
    printf("Error: could not read bitmap file \"%s\"\n", argv[1]);
    goto fail;
  }
  BitmapWirenize(&bitmap, bitmap.height * 11 / 24);
  if (!BitmapSave(&bitmap, argv[2])) {
    printf("Error: could not save bitmap file \"%s\"\n", argv[2]);
    goto fail;
  }
  BitmapFree(&bitmap);
  return EXIT_SUCCESS;

fail:
  BitmapFree(&bitmap);
  return EXIT_FAILURE;
}
