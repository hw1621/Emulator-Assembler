gcc -Wall -Wextra -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -pedantic -o example \
  example.c \
  ../../src/bitmap.c \
  -lm

[ $? -eq 0 ] && ./example
