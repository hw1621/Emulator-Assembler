#include "trie.h"
#include "common.h"
#include <stdio.h>
#include <string.h>
#include <assert.h>

BLOCKS_DEFINE(Trie, TrieBlocks)

void TrieInit(Trie* this) {
  this->next[0] = NULL;
  this->next[1] = NULL;
  this->hasVal = false;
  this->val = 0;
}

void TrieFree(Trie* this) {
  MAYBE_UNUSED(this);
}

// Returns true if a descendant has to be created
bool TrieNext(Trie** p, TrieBlocks* blocks, size_t bit) {
  bool res = (*p)->next[bit] == NULL;
  if (res && blocks != NULL)
    (*p)->next[bit] = TrieBlocksNew(blocks); // Create descendant if not present
  *p = (*p)->next[bit]; // Set pointer to next Trie node
  return res;
}

Trie* TrieNode(Trie* this, TrieBlocks* blocks, const char* key, size_t keylen) {
  for (size_t i = 0; i < keylen; i++)
    for (size_t j = 0; j < 8; j++)
      if (TrieNext(&this, blocks, ((size_t) key[i] >> j) & 1) && blocks == NULL)
        return NULL;
  return this;
}

bool TriePut(Trie* this, TrieBlocks* blocks, const char* key, size_t keylen, size_t val) {
  Trie* node = TrieNode(this, blocks, key, keylen);
  if (node->hasVal) return false;
  node->hasVal = true;
  node->val = val;
  return true;
}

bool TrieGet(Trie* this, const char* key, size_t keylen, size_t* res) {
  Trie* node = TrieNode(this, NULL, key, keylen);
  if (node == NULL || !node->hasVal) return false;
  *res = node->val;
  return true;
}
