#ifndef TRANSLATION_H_
#define TRANSLATION_H_

#include "vector.h"
#include "tokenizer.h"
#include "trie.h"

// LL(2) parser state
typedef struct {
  const char* p; // (Readonly) Current string pointer
  Token next;    // (Readonly) Next token
  Token next2;   // (Readonly) Next next token
} Parser;

void ParserInit(Parser* this, const char* p); // Constructs a parser that points to a given string
void ParserFree(Parser* this); // Does nothing, actually
void ParserAdvance(Parser* this); // Advances the parser state by one token ("再吃进来一个词/符号")

// Address of a branching instruction with unresolved label
typedef struct {
  size_t address; // Instruction address
  Trie* label;    // Label name stored in the trie
} Branch;

void BranchInit(Branch* this);
void BranchCopyFrom(Branch* this, const Branch* src);
void BranchFree(Branch* this);

// Address of a constant "load" instruction with undetermined target
typedef struct {
  size_t address; // Instructions address
  size_t target;  // Target address (relative to the beginning of the reserved memory block)
} Load;

void LoadInit(Load* this);
void LoadCopyFrom(Load* this, const Load* src);
void LoadFree(Load* this);

VECTOR_DECLARE(Branch, BranchVector)
VECTOR_DECLARE(Load, LoadVector)

// Context of translation
typedef struct {
  TrieBlocks pool;       // Memory allocator for `Trie` nodes
  Trie* symbols;         // Symbol table
  ByteVector assembled;  // Assembled instructions
  ByteVector reserved;   // Reserved memory
  BranchVector branches; // Addresses of branching instructions with unresolved labels
  LoadVector loads;      // Addresses of constant "load" instructions with undetermined targets
} Context;

void ContextInit(Context* this);
void ContextFree(Context* this);

// Parse and register the next label into `ctx->symbols`
// Returns `false` on error
bool label(Parser* parser, Context* ctx);

// Parse and assemble the next instruction into `ctx->assembled`
// Returns `false` on error
bool instruction(Parser* parser, Context* ctx);

// Link branch labels and reserved memory pointers
// Returns `false` on error
bool link(const Context* ctx, ByteVector* out);

#endif // TRANSLATION_H_
