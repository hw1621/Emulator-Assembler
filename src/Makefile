CC      = gcc
CFLAGS  = -Wall -Wextra -g -D_POSIX_SOURCE -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
LDFLAGS = -lm

.SUFFIXES: .c .o

.PHONY: all release clean

all: assemble emulate test wirenizer
release: CFLAGS += -DNDEBUG -O3
release: assemble emulate test wirenizer

# Object dependencies
assemble: assemble.o vector.o state.o tokenizer.o trie.o translation.o
	$(CC) $(CLFAGS) $^ -o $@ $(LDFLAGS)
emulate: emulate.o vector.o state.o bitmap.o
	$(CC) $(CLFAGS) $^ -o $@ $(LDFLAGS)
test: test.o vector.o state.o trie.o tokenizer.o
	$(CC) $(CLFAGS) $^ -o $@ $(LDFLAGS)
wirenizer: wirenizer.o vector.o bitmap.o
	$(CC) $(CLFAGS) $^ -o $@ $(LDFLAGS)

# Header dependencies
# (TODO: automatically generate)
vector.o: vector.h
state.o: state.h
bitmap.o: common.h vector.h
trie.o: blocks.h
translation.o: common.h vector.h blocks.h state.h trie.h tokenizer.h translation.h
assemble.o: common.h vector.h blocks.h state.h trie.h tokenizer.h translation.h
emulate.o: common.h vector.h state.h bitmap.h
test.o: common.h vector.h blocks.h state.h trie.h tokenizer.h
wirenizer.o: bitmap.h

clean:
	rm -f $(wildcard *.o)
	rm -f assemble
	rm -f emulate
	rm -f test
	rm -f wirenizer
