#ifndef TRIE_H_
#define TRIE_H_

#include <stdlib.h>
#include <stdint.h>
#include "blocks.h"

// Bit trie
typedef struct TrieT {
  struct TrieT* next[2];
  bool hasVal;
  size_t val;
} Trie;

BLOCKS_DECLARE(Trie, TrieBlocks)

// Constructor
void TrieInit(Trie* this);

// Destructor
void TrieFree(Trie* this);

// Gets the Trie node corresponding to key
// If blocks is not null (write mode), creates subtree along traversal
// Otherwise (read mode), abort upon null descendant
Trie* TrieNode(Trie* this, TrieBlocks* blocks, const char* key, size_t keylen);

bool TriePut(Trie* this, TrieBlocks* blocks, const char* key, size_t keylen, size_t val);
bool TrieGet(Trie* this, const char* key, size_t keylen, size_t* res);

#endif // TRIE_H_
