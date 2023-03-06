#include "translation.h"
#include <stdio.h>
#include "state.h"

void ParserInit(Parser* this, const char* p) {
  this->p = p;
  ParserAdvance(this);
  ParserAdvance(this);  
}
void ParserFree(Parser* this) { MAYBE_UNUSED(this); }
void ParserAdvance(Parser* this) {
  this->next = this->next2;
  this->next2 = nextToken(&this->p);
}

// `BranchInit` will never be used (all Branches are copy-constructed)
void BranchInit(Branch* this) { MAYBE_UNUSED(this); assert(false); }
void BranchCopyFrom(Branch* this, const Branch* src) { memcpy(this, src, sizeof(Branch)); }
void BranchFree(Branch* this) { MAYBE_UNUSED(this); }

// `LoadInit` will never be used (all Loads are copy-constructed)
void LoadInit(Load* this) { MAYBE_UNUSED(this); assert(false); }
void LoadCopyFrom(Load* this, const Load* src) { memcpy(this, src, sizeof(Load)); }
void LoadFree(Load* this) { MAYBE_UNUSED(this); }

VECTOR_DEFINE(Branch, BranchVector)
VECTOR_DEFINE(Load, LoadVector)

void ContextInit(Context* this) {
  TrieBlocksInit(&this->pool);
  this->symbols = TrieBlocksNew(&this->pool);
  ByteVectorInit(&this->assembled);
  ByteVectorInit(&this->reserved);
  BranchVectorInit(&this->branches);
  LoadVectorInit(&this->loads);
}
void ContextFree(Context* this) {
  TrieBlocksFree(&this->pool);
  ByteVectorFree(&this->assembled);
  ByteVectorFree(&this->reserved);
  BranchVectorFree(&this->branches);
  LoadVectorFree(&this->loads);
}

// Prints a fixed-length (sub)string
void print(const char* start, const char* end) {
  for (const char* p = start; p != end; p++) printf("%c", *p);
}

// Pre: `s` is null-terminated
bool hasPrefix(const char* start, const char* end, const char* s) {
  size_t len = strlen(s);
  if (start + len > end) return false;
  for (size_t i = 0; i < len; i++) if (start[i] != s[i]) return false;
  return true;
}

// Pre: `s` is null-terminated
bool same(const char* start, const char* end, const char* s) {
  size_t len = strlen(s);
  if (start + len != end) return false;
  for (size_t i = 0; i < len; i++) if (start[i] != s[i]) return false;
  return true;
}

// Get the corresponding condition code of a given fixed-length (sub)string
bool condFromString(const char* start, const char* end, uint32_t* res) {
  // Empty string (no suffix) means "always"
  if (start == end) { *res = 14; return true; }
  // TODO: optimise with `Trie`?
  if (same(start, end, "eq")) { *res = 0; return true; }
  if (same(start, end, "ne")) { *res = 1; return true; }
  if (same(start, end, "cs")) { *res = 2; return true; }
  if (same(start, end, "cc")) { *res = 3; return true; }
  if (same(start, end, "mi")) { *res = 4; return true; }
  if (same(start, end, "pl")) { *res = 5; return true; }
  if (same(start, end, "vs")) { *res = 6; return true; }
  if (same(start, end, "vc")) { *res = 7; return true; }
  if (same(start, end, "hi")) { *res = 8; return true; }
  if (same(start, end, "ls")) { *res = 9; return true; }
  if (same(start, end, "ge")) { *res = 10; return true; }
  if (same(start, end, "lt")) { *res = 11; return true; }
  if (same(start, end, "gt")) { *res = 12; return true; }
  if (same(start, end, "le")) { *res = 13; return true; }
  if (same(start, end, "al")) { *res = 14; return true; }
  printf("Parsing error: unknown condition suffix "); print(start, end); printf("\n");
  return false;
}

// Macros that automatically returns `false` on failure.
// Note that they implicitly refer to a local variable named `parser` (for the sake of brevity),
// and some create a new local variable with a given name.
// (Haskell's `Maybe` monad would make this look much better...)
#define PARSE_TOKEN(NAME, KIND) Token NAME; if (!parseToken(parser, KIND, &NAME)) return false;
#define SKIP_TOKEN(KIND)        if (!parseToken(parser, KIND, NULL)) return false;
#define PARSE_REGISTER(NAME)    uint32_t NAME; if (!parseRegister(parser, &NAME)) return false;
#define PARSE_NUMBER(NAME)      uint32_t NAME; if (!parseNumber(parser, &NAME)) return false;
#define PARSE_SHIFT(NAME)       uint32_t NAME; if (!parseShift(parser, &NAME)) return false;
#define PARSE_IMMEDIATE(NAME)   uint32_t NAME; if (!parseImmediate(parser, &NAME)) return false;

// Parses (and "eats") the next token of some expected kind
// `res` is nullable (if it is null, result is discarded)
bool parseToken(Parser* parser, TokenKind kind, Token* res) {
  // Check that the next token is of the expected kind
  if (parser->next.kind != kind) {
    printf("Parsing error: expected %s, got %s\n", tokenName(kind), tokenName(parser->next.kind));
    return false;
  }
  // Set return value
  if (res != NULL) *res = parser->next;
  // Eat the token
  ParserAdvance(parser);
  return true;
}

// Parses (and "eats") the next token as register name
bool parseRegister(Parser* parser, uint32_t* res) {
  // Uses `parseToken` to get and eat the next token
  PARSE_TOKEN(name, TOK_IDENTIFIER);
  // Check if the name starts with "r"
  size_t len = name.end - name.start;
  if (len == 0 || name.start[0] != 'r') {
    // Check if the name is one of the special register names
    if (same(name.start, name.end, "sp")) { *res = REG_SP; return true; }
    if (same(name.start, name.end, "lr")) { *res = REG_LR; return true; }
    if (same(name.start, name.end, "pc")) { *res = REG_PC; return true; }
    // Name is unknown
    printf("Parsing error: unknown register name "); print(name.start, name.end); printf("\n");
    return false;
  }
  // Manually convert string to integer
  *res = 0;
  for (size_t i = 1; i < len; i++) *res = (*res) * 10 + (name.start[i] - '0');
  // Check that the number is a valid register index
  if (*res >= NUM_REGS) {
    printf("Parsing error: unknown register name "); print(name.start, name.end); printf("\n");
    return false;
  }
  return true;
}

// Parses (and "eats") a number
bool parseNumber(Parser* parser, uint32_t* res) {
  // Uses `parseToken` to get and eat the next token
  PARSE_TOKEN(num, TOK_NUMBER);
  // Put the fixed-length string of `num` into a buffer so that we can terminate it with a '\0'
  size_t len = num.end - num.start;
  char* buf = malloc(len + 1);
  assert(buf != NULL);
  for (size_t i = 0; i < len; i++) buf[i] = num.start[i];
  buf[len] = '\0';
  // Use `sscanf` to convert string to integer
  if (buf[0] == '0' && (buf[1] == 'x' || buf[1] == 'X')) sscanf(buf, "%x", res); // Hexadecimal
  else sscanf(buf, "%u", res); // Decimal
  free(buf);
  return true;
}

// Tries encoding `num` as an immediate value, returns false if not possible
bool encodeImmediate(uint32_t num, uint32_t* res) {
  for (uint32_t rot = 0; rot < 16; rot++) {
    // Rotate `num` to the left (by 2*rot bits) to see if it fits in 8 bits
    uint32_t imm = (num << (2 * rot)) | (num >> (32 - 2 * rot));
    if (imm < (1 << 8)) {
      *res = (rot << 8) | imm;
      return true;
    }
  }
  return false;
}

// Parses (and "eats") a number, and tries to encode it as an immediate value
bool parseImmediate(Parser* parser, uint32_t* res) {
  PARSE_NUMBER(num);
  if (!encodeImmediate(num, res)) {
    printf("Error: unsupported immediate value %u\n", num);
    return false;
  }
  return true;
}

// Parses (and "eats") an `Rm {, <shift>}`
// Returns the coded shift (only bits 0~11 can be nonzero) to the second argument
bool parseShift(Parser* parser, uint32_t* res) {
  *res = 0;
  PARSE_REGISTER(rm);
  setBits(res, 4, 11, 0); // Default: no shift applied to Rm
  setBits(res, 0, 3, rm);
  if (parser->next.kind == TOK_COMMA) {
    SKIP_TOKEN(TOK_COMMA); PARSE_TOKEN(name, TOK_IDENTIFIER);
    if (same(name.start, name.end, "lsl")) setBits(res, 5, 6, 0);
    else if (same(name.start, name.end, "lsr")) setBits(res, 5, 6, 1);
    else if (same(name.start, name.end, "asr")) setBits(res, 5, 6, 2);
    else if (same(name.start, name.end, "ror")) setBits(res, 5, 6, 3);
    else {
      printf("Error: unsupported shift operation "); print(name.start, name.end); printf("\n");
      return false;
    }
    if (parser->next.kind == TOK_HASHSIGN) { // Shift by a constant amount
      SKIP_TOKEN(TOK_HASHSIGN); PARSE_NUMBER(expr);
      if (expr >= 32) {
        printf("Error: shift amount must be less than 32, got %u\n", expr);
        return false;
      }
      setBits(res, 7, 11, expr);
    } else { // Shift specified by a register
      PARSE_REGISTER(rs);
      setBits(res, 4, 4, 1);
      setBits(res, 8, 11, rs);
    }
  }
  return true;
}

// Append a new instruction to `ctx->assembled`
// Returns its address
size_t addInstruction(Context* ctx, uint32_t instr) {
  size_t address = ctx->assembled.size;
  ByteVectorResize(&ctx->assembled, address + 4);
  ByteVectorSetU32LE(&ctx->assembled, address, instr);
  return address;
}

// Data Processing
bool dataProcessing(uint32_t cond, uint32_t opcode, uint32_t s, uint32_t rd, uint32_t rn, Parser* parser, Context* ctx) {
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 21, 24, opcode);
  setBits(&res, 20, 20, s);
  setBits(&res, 16, 19, rn);
  setBits(&res, 12, 15, rd);
  if (parser->next.kind == TOK_HASHSIGN) { // op2 is a <#expression>
    SKIP_TOKEN(TOK_HASHSIGN); PARSE_IMMEDIATE(imm);
    setBits(&res, 25, 25, 1);
    setBits(&res, 0, 11, imm);
  } else { // op2 is a <shift>
    PARSE_SHIFT(shift);
    setBits(&res, 0, 11, shift);
  }
  addInstruction(ctx, res);
  return true;
}

bool dataCompute(uint32_t cond, uint32_t opcode, Parser* parser, Context* ctx) { 
  PARSE_REGISTER(rd); SKIP_TOKEN(TOK_COMMA);
  PARSE_REGISTER(rn); SKIP_TOKEN(TOK_COMMA);
  return dataProcessing(cond, opcode, 0, rd, rn, parser, ctx);
}

bool nonCompute(uint32_t cond, uint32_t opcode, Parser* parser, Context* ctx) { 
  PARSE_REGISTER(rn); SKIP_TOKEN(TOK_COMMA);
  return dataProcessing(cond, opcode, 1, 0, rn, parser, ctx);
}

bool asmMov(uint32_t cond, Parser* parser, Context* ctx) { 
  PARSE_REGISTER(rd); SKIP_TOKEN(TOK_COMMA);
  return dataProcessing(cond, 13, 0, rd, 0, parser, ctx);
}

bool asmAnd(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 0, parser, ctx); }
bool asmEor(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 1, parser, ctx); }
bool asmSub(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 2, parser, ctx); }
bool asmRsb(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 3, parser, ctx); }
bool asmAdd(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 4, parser, ctx); }
bool asmOrr(uint32_t cond, Parser* parser, Context* ctx) { return dataCompute(cond, 12, parser, ctx); }
bool asmTst(uint32_t cond, Parser* parser, Context* ctx) { return nonCompute(cond, 8, parser, ctx); }
bool asmTeq(uint32_t cond, Parser* parser, Context* ctx) { return nonCompute(cond, 9, parser, ctx); }
bool asmCmp(uint32_t cond, Parser* parser, Context* ctx) { return nonCompute(cond, 10, parser, ctx); }

// Multiply
bool multiply(uint32_t cond, bool mla, Parser* parser, Context* ctx) {
  PARSE_REGISTER(rd); SKIP_TOKEN(TOK_COMMA);
  PARSE_REGISTER(rm); SKIP_TOKEN(TOK_COMMA);
  PARSE_REGISTER(rs);
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 16, 19, rd);
  setBits(&res, 8, 11, rs);
  setBits(&res, 4, 7, 9);
  setBits(&res, 0, 3, rm);
  if (mla) {
    SKIP_TOKEN(TOK_COMMA);
    PARSE_REGISTER(rn);
    setBits(&res, 21, 21, 1);
    setBits(&res, 12, 15, rn);
  }
  addInstruction(ctx, res);
  return true;
}
bool asmMul(uint32_t cond, Parser* parser, Context* ctx) { return multiply(cond, false, parser, ctx); }
bool asmMla(uint32_t cond, Parser* parser, Context* ctx) { return multiply(cond, true, parser, ctx); }

// Single Data Transfer
// Parse and encode an optional "+/-" sign
bool optSign(Parser* parser, uint32_t* res) {
  if (parser->next.kind == TOK_PLUS) { SKIP_TOKEN(TOK_PLUS); setBits(res, 23, 23, 1); }
  else if (parser->next.kind == TOK_MINUS) { SKIP_TOKEN(TOK_MINUS); }
  else setBits(res, 23, 23, 1);
  return true;
}

// Parse and encode the "offset" part
bool offset(Parser* parser, uint32_t* res) {
  if (parser->next.kind == TOK_HASHSIGN) { // Immediate offset
    SKIP_TOKEN(TOK_HASHSIGN);
    if (!optSign(parser, res)) return false;
    PARSE_IMMEDIATE(offset);
    setBits(res, 0, 11, offset);
  } else { // Shifted register
    setBits(res, 25, 25, 1);
    if (!optSign(parser, res)) return false;
    PARSE_SHIFT(shift);
    setBits(res, 0, 11, shift);
  }
  return true;
}

bool transfer(uint32_t cond, bool load, Parser* parser, Context* ctx) {
  PARSE_REGISTER(rd); SKIP_TOKEN(TOK_COMMA);
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 26, 27, 1);
  setBits(&res, 20, 20, load ? 1 : 0);
  setBits(&res, 12, 15, rd);
  if (load && parser->next.kind == TOK_EQUALS) { // Load numeric constant
    SKIP_TOKEN(TOK_EQUALS); PARSE_NUMBER(expr);
    uint32_t imm;
    if (encodeImmediate(expr, &imm)) { // Assemble as `mov`
      res = 0;
      setBits(&res, 28, 31, cond);
      setBits(&res, 25, 25, 1);
      setBits(&res, 21, 24, 13);
      setBits(&res, 12, 15, rd);
      setBits(&res, 0, 11, imm);
      addInstruction(ctx, res);
    } else { // Put the constant into reserved memory
      setBits(&res, 24, 24, 1);
      setBits(&res, 23, 23, 1);
      setBits(&res, 16, 19, REG_PC);
      size_t target = ctx->reserved.size;
      ByteVectorResize(&ctx->reserved, target + 4);
      ByteVectorSetU32LE(&ctx->reserved, target, expr);
      size_t address = addInstruction(ctx, res);
      LoadVectorPushBack(&ctx->loads, &(Load) { address, target });
    }
    return true;
  }
  SKIP_TOKEN(TOK_LBRACKET); PARSE_REGISTER(rn);
  setBits(&res, 16, 19, rn);
  if (parser->next.kind == TOK_COMMA) { // Pre-indexing
    SKIP_TOKEN(TOK_COMMA);
    setBits(&res, 24, 24, 1);
    if (!offset(parser, &res)) return false;
    SKIP_TOKEN(TOK_RBRACKET);
  } else {
    SKIP_TOKEN(TOK_RBRACKET);
    if (parser->next.kind == TOK_COMMA) { // Post-indexing
      SKIP_TOKEN(TOK_COMMA);
      if (!offset(parser, &res)) return false;
    } else { // Zero offset
      setBits(&res, 24, 24, 1);
      setBits(&res, 23, 23, 1); // Up bit is set, as required by the tests
    }
  }
  addInstruction(ctx, res);
  return true;
}

bool asmLdr(uint32_t cond, Parser* parser, Context* ctx) { return transfer(cond, true, parser, ctx); }
bool asmStr(uint32_t cond, Parser* parser, Context* ctx) { return transfer(cond, false, parser, ctx); }

// Branch
bool branch(uint32_t cond, bool link, Parser* parser, Context* ctx) {
  PARSE_TOKEN(label, TOK_IDENTIFIER);
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 25, 27, 5);
  setBits(&res, 24, 24, link ? 1 : 0);
  uint32_t address = addInstruction(ctx, res);
  Trie* node = TrieNode(ctx->symbols, &ctx->pool, label.start, label.end - label.start);
  // See: https://en.cppreference.com/w/c/language/compound_literal
  // "The value category of a compound literal is lvalue (its address can be taken)."
  BranchVectorPushBack(&ctx->branches, &(Branch) { address, node });
  return true;
}

bool asmB(uint32_t cond, Parser* parser, Context* ctx) { return branch(cond, false, parser, ctx); }
bool asmBl(uint32_t cond, Parser* parser, Context* ctx) { return branch(cond, true, parser, ctx); }
bool asmBx(uint32_t cond, Parser* parser, Context* ctx) {
  PARSE_REGISTER(rn);
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 4, 27, 0x12FFF1);
  setBits(&res, 0, 3, rn);
  addInstruction(ctx, res);
  return true;
}

// Special
// Parses a list of registers
bool regList(Parser* parser, uint32_t* res) {
  SKIP_TOKEN(TOK_LBRACE);
  PARSE_REGISTER(first);
  *res = 1 << first;
  while (parser->next.kind == TOK_COMMA) {
    SKIP_TOKEN(TOK_COMMA); PARSE_REGISTER(reg);
    *res |= 1 << reg;
  }
  SKIP_TOKEN(TOK_RBRACE);
  return true;
}

bool asmLsl(uint32_t cond, Parser* parser, Context* ctx) {
  PARSE_REGISTER(rn); SKIP_TOKEN(TOK_COMMA); SKIP_TOKEN(TOK_HASHSIGN); PARSE_NUMBER(shift);
  if (shift >= 32) {
    printf("Error: unsupported shift amount %u\n", shift);
    return false;
  }
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 21, 24, 13);
  setBits(&res, 12, 15, rn);
  setBits(&res, 7, 11, shift);
  setBits(&res, 0, 3, rn);
  addInstruction(ctx, res);
  return true;
}

bool asmPush(uint32_t cond, Parser* parser, Context* ctx) {
  uint32_t regs;
  if (!regList(parser, &regs)) return false;
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 25, 27, 4);
  setBits(&res, 24, 24, 1); // Pre
  setBits(&res, 23, 23, 0); // Decrement
  setBits(&res, 21, 21, 1); // Writeback
  setBits(&res, 20, 20, 0); // Store
  setBits(&res, 16, 19, REG_SP);
  setBits(&res, 0, 15, regs);
  addInstruction(ctx, res);
  return true;
}

bool asmPop(uint32_t cond, Parser* parser, Context* ctx) {
  uint32_t regs;
  if (!regList(parser, &regs)) return false;
  uint32_t res = 0;
  setBits(&res, 28, 31, cond);
  setBits(&res, 25, 27, 4);
  setBits(&res, 24, 24, 0); // Post
  setBits(&res, 23, 23, 1); // Increment
  setBits(&res, 21, 21, 1); // Writeback
  setBits(&res, 20, 20, 1); // Load
  setBits(&res, 16, 19, REG_SP);
  setBits(&res, 0, 15, regs);
  addInstruction(ctx, res);
  return true;
}

// Main label handler
bool label(Parser* parser, Context* ctx) {
  PARSE_TOKEN(label, TOK_IDENTIFIER); SKIP_TOKEN(TOK_COLON);
  TriePut(ctx->symbols, &ctx->pool, label.start, label.end - label.start, ctx->assembled.size);
  return true;
}

#define DISPATCH(PREFIX, FUNC) \
  if (hasPrefix(mnemonic.start, mnemonic.end, PREFIX)) { \
    uint32_t cond; \
    if (!condFromString(mnemonic.start + strlen(PREFIX), mnemonic.end, &cond)) return false; \
    return FUNC(cond, parser, ctx); \
  }

#define DISPATCH_EXACT(PREFIX, N, FUNC) \
  if (same(mnemonic.start, mnemonic.end, PREFIX)) { \
    uint32_t cond; \
    if (!condFromString(mnemonic.start + N, mnemonic.end, &cond)) return false; \
    return FUNC(cond, parser, ctx); \
  }

// Main instruction handler
bool instruction(Parser* parser, Context* ctx) {
  PARSE_TOKEN(mnemonic, TOK_IDENTIFIER);
  // TODO: optimise with `Trie`?
  DISPATCH("add", asmAdd);
  DISPATCH("sub", asmSub);
  DISPATCH("rsb", asmRsb);
  DISPATCH("and", asmAnd);
  DISPATCH("eor", asmEor);
  DISPATCH("orr", asmOrr);
  DISPATCH("mov", asmMov);
  DISPATCH("tst", asmTst);
  DISPATCH("teq", asmTeq);
  DISPATCH("cmp", asmCmp);
  DISPATCH("mul", asmMul);
  DISPATCH("mla", asmMla);
  DISPATCH("ldr", asmLdr);
  DISPATCH("str", asmStr);
  DISPATCH("bx", asmBx);
  DISPATCH_EXACT("ble", 1, asmB);
  DISPATCH_EXACT("blt", 1, asmB);
  DISPATCH("bl", asmBl);
  DISPATCH("b", asmB);
  DISPATCH("lsl", asmLsl);
  DISPATCH("push", asmPush);
  DISPATCH("pop", asmPop);
  printf("Error: unknown mnemonic "); print(mnemonic.start, mnemonic.end); printf("\n");
  return false;
}

// Main linking function
bool link(const Context* ctx, ByteVector* out) {
  size_t reserveOffset = ctx->assembled.size;
  ByteVectorResize(out, reserveOffset + ctx->reserved.size);
  for (size_t i = 0; i < ctx->assembled.size; i++) out->p[i] = ctx->assembled.p[i];
  for (size_t i = 0; i < ctx->reserved.size; i++) out->p[reserveOffset + i] = ctx->reserved.p[i];
  // Link branching instructions with labels
  for (size_t i = 0; i < ctx->branches.size; i++) {
    size_t instructionAddress = ctx->branches.p[i].address;
    Trie* label = ctx->branches.p[i].label;
    if (!label->hasVal) {
      printf("Linking error: undefined label\n"); // TODO: how to print string for a trie node?
      return false;
    }
    size_t labelAddress = label->val;
    bool forward = labelAddress >= instructionAddress + 8;
    size_t offset = forward ?
      labelAddress - instructionAddress - 8 :
      instructionAddress + 8 - labelAddress;
    if (offset % 4 != 0) {
      printf("Linking error: unsupported branch offset %zu\n", offset);
      return false;
    }
    offset /= 4;
    if (offset >= (1 << 23)) {
      printf("Linking error: unsupported branch offset %zu\n", offset);
      return false;
    }
    uint32_t res = ByteVectorGetU32LE(out, instructionAddress);
    setBits(&res, 0, 23, bits(forward ? offset : -offset, 0, 23));
    ByteVectorSetU32LE(out, instructionAddress, res);
  }
  // Link constant load instructions with reserved memory
  for (size_t i = 0; i < ctx->loads.size; i++) {
    size_t instructionAddress = ctx->loads.p[i].address;
    size_t targetAddress = reserveOffset + ctx->loads.p[i].target;
    size_t offset = targetAddress - instructionAddress - 8;
    // Assuming the last instruction is `halt`, offset will always be non-negative (forward) despite
    // the +8 offset of PC.
    if (offset >= (1 << 12)) {
      printf("Linking error: unsupported load offset %zu\n", offset);
      return false;
    }
    uint32_t res = ByteVectorGetU32LE(out, instructionAddress);
    setBits(&res, 0, 11, (uint32_t) offset);
    ByteVectorSetU32LE(out, instructionAddress, res);
  }
  return true;
}
