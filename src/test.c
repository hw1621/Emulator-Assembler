#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "vector.h"
#include "blocks.h"
#include "trie.h"
#include "tokenizer.h"

void byteVectorTestSimple(void) {
  ByteVector a;
  ByteVectorInit(&a); assert(a.size == 0);
  ByteVectorReserve(&a, 15); assert(a.size == 0);
  ByteVectorResize(&a, 15); assert(a.size == 15);
  ByteVectorResize(&a, 233);
  for (size_t i = 0; i < a.size; i++) {
    assert(a.p[i] == 0);
    a.p[i] = i;
  }
  ByteVectorResize(&a, 233333);
  for (size_t i = 0; i < a.size; i++) {
    if (i < 233) assert(a.p[i] == i);
    else assert(a.p[i] == 0);
  }
  assert(ByteVectorGetU32LE(&a, 0) == (1u << 8u) + (2u << 16u) + (3u << 24u));
  assert(ByteVectorGetU32LE(&a, 3) == 3u + (4u << 8u) + (5u << 16u) + (6u << 24u));
  ByteVectorSetU32LE(&a, 1, 233u);
  assert(ByteVectorGetU32LE(&a, 1) == 233u);
  ByteVectorFree(&a);
}

void stringVectorTestSimple(void) {
  StringVector a;
  StringVectorInit(&a);
  StringVectorResize(&a, 10);
  for (size_t i = 0; i < a.size; i++) StringResize(&a.p[i], 233);
  StringVectorFree(&a);
}

int counter;
typedef struct { int data; } TestNode;
void TestNodeInit(TestNode* this) { this->data = counter++; }
void TestNodeFree(TestNode* this) { MAYBE_UNUSED(this); counter--; }

BLOCKS_DECLARE(TestNode, TestNodeBlocks)
BLOCKS_DEFINE(TestNode, TestNodeBlocks)

void blocksTestSimple(void) {
  counter = 0;
  TestNodeBlocks blocks;
  TestNodeBlocksInit(&blocks);
  for (size_t i = 0; i < 233333; i++) TestNodeBlocksNew(&blocks);
  TestNodeBlocksFree(&blocks);
  assert(counter == 0);
}

char* randString(size_t length, char* dest) {
  static const char* set = "1234567890qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM";
  for (size_t i = 0; i < length; i++) dest[i] = set[rand()%62];
  return dest;
}

#define MAGIC 233
void trieTestSimple(void) {
  static char key[MAGIC][MAGIC];
  static size_t val[MAGIC];
  TrieBlocks blocks;
  TrieBlocksInit(&blocks);
  Trie* trie = TrieBlocksNew(&blocks);
  for (size_t i = 0; i < MAGIC; i++) TriePut(trie, &blocks, randString(MAGIC, key[i]), MAGIC, val[i] = rand());
  for (size_t i = 0; i < MAGIC; i++) {
    size_t res;
    assert(TrieGet(trie, key[i], MAGIC, &res) && res == val[i]);
    MAYBE_UNUSED(res);
  }
  TrieBlocksFree(&blocks);
}

void tokenizerTestSimple(void) {
  const char* str = "  ldr r0, =0x555555 ;This is a comment \n\r ldr r0,[r1,r2,lsl #2]\r\n";
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_COMMA);
  assert(nextToken(&str).kind == TOK_EQUALS);
  assert(nextToken(&str).kind == TOK_NUMBER);
  // assert(nextToken(&str).kind == TOK_LINE_BREAK);
  // assert(nextToken(&str).kind == TOK_LINE_BREAK);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_COMMA);
  assert(nextToken(&str).kind == TOK_LBRACKET);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_COMMA);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_COMMA);
  assert(nextToken(&str).kind == TOK_IDENTIFIER);
  assert(nextToken(&str).kind == TOK_HASHSIGN);
  assert(nextToken(&str).kind == TOK_NUMBER);
  assert(nextToken(&str).kind == TOK_RBRACKET);
  // assert(nextToken(&str).kind == TOK_LINE_BREAK);
  assert(nextToken(&str).kind == TOK_EOF);
  assert(nextToken(&str).kind == TOK_EOF);
  assert(nextToken(&str).kind == TOK_EOF);
  MAYBE_UNUSED(str);
}

int main(void) {
  for (int i = 0; i < 100; i++) byteVectorTestSimple();
  for (int i = 0; i < 100; i++) stringVectorTestSimple();
  for (int i = 0; i < 100; i++) blocksTestSimple();
  for (int i = 0; i < MAGIC; i++) trieTestSimple();
  tokenizerTestSimple();
  printf("All tests passed.\nPress any key to continue...\n");
  getchar();
  return EXIT_SUCCESS;
}
