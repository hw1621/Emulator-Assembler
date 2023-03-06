#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "common.h"
#include "vector.h"
#include "state.h"
#include "bitmap.h"

#define DEFAULT_MEM_SIZE 65536

// Reads whole file into `ByteVector`
bool readFile(const char* filename, ByteVector* a) {
  FILE* in = NULL;
  long size; // `ftell` only returns `long`... could be a problem with large files

  if ((in = fopen(filename, "rb")) == NULL)      goto fail;
  if (fseek(in, 0, SEEK_END) != 0)               goto fail;
  if ((size = ftell(in)) == -1)                  goto fail;
  ByteVectorResize(a, size);
  if (fseek(in, 0, SEEK_SET) != 0)               goto fail;
  if (fread(a->p, 1, size, in) != (size_t) size) goto fail;
  if (fclose(in) != 0)                           goto fail;
  return true;

fail:
  if (in != NULL) fclose(in);
  return false;
}

/*
// Saves framebuffer into a PPM (text-based bitmap) file
// Pre: `p` points to a memory block of length >= width * height * 4
bool savePPM(const char* filename, const unsigned char* p, size_t width, size_t height) {
  FILE* out = NULL;
  if ((out = fopen(filename, "w")) == NULL) goto fail;
  if (fprintf(out, "P3\n%zu %zu\n%d\n", width, height, 255) < 0) goto fail;
  for (size_t i = 0; i < width * height * 4; i += 4) {
    unsigned char r = p[i];
    unsigned char g = p[i + 1];
    unsigned char b = p[i + 2];
    // unsigned char a = p[i + 3];
    if (fprintf(out, "%hhu %hhu %hhu ", r, g, b) < 0) goto fail;
  }
  if (fclose(out) != 0) goto fail;
  return true;

fail:
  if (out != NULL) fclose(out);
  return false;
}
*/

// Saves framebuffer into a bitmap file
// Pre: `p` points to a memory block of length >= width * height * 4
bool saveBMP(const char* filename, const unsigned char* p, size_t width, size_t height, bool wirenize) {
  Bitmap bitmap;
  BitmapInit(&bitmap, width, height);
  for (size_t i = 0; i < height; i++)
    for (size_t j = 0; j < width; j++) {
      size_t ind = (i * width + j) * 4;
      BitmapSetPixel(&bitmap, j, i, p[ind], p[ind + 1], p[ind + 2]);
    }
  if (wirenize) BitmapWirenize(&bitmap, bitmap.height * 11 / 24);
  bool res = BitmapSave(&bitmap, filename);
  BitmapFree(&bitmap);
  return res;
}

// Decode operand2 as shifted register
// Modifies the 2nd and 3rd arguments as its output (when applicable)
void decodeShiftedRegister(const State* state, uint32_t* op2, bool* carry) {
  // Shift amount
  uint32_t val = bits(*op2, 4, 4) ?
    state->reg[bits(*op2, 8, 11)] : // Shift specified by register
    bits(*op2, 7, 11);              // Shift by a constant amount
  uint32_t rm = state->reg[bits(*op2, 0, 3)];
  // Shifts only has effect when val > 0; check performed to avoid overflow of (val-1)
  if (val) {
    uint32_t type = bits(*op2, 5, 6);
    switch (type) {
      case 0: *op2 = rm << val; *carry = (rm >> (32 - val)) & 1;          break; // lsl
      case 1: *op2 = rm >> val; *carry = (rm >> (val - 1)) & 1;           break; // lsr
      case 2: *op2 = (rm >> val) | ((rm >> 31) * (0xFFFFFFFF << (32 - val)));
              *carry = (rm >> (val - 1)) & 1;                             break; // asr
      case 3: *op2 = (rm >> val) | (rm << (32 - val)); *carry = rm >> 31; break; // ror
    }
  } else { *op2 = rm; }
  // ARM spec p55~56 says `lsr #0`, `asr #0` and `ror #0` have special meanings;
  // But currently we choose not to implement them.
}

bool execDataProcessing(State* state, uint32_t cmd) {
  if (bits(cmd, 26, 27) != 0) { printf("Not a data processing command: %#.8x\n", cmd); return false; }
  uint32_t i   = bits(cmd, 25, 25);
  uint32_t op  = bits(cmd, 21, 24);
  uint32_t s   = bits(cmd, 20, 20);
  uint32_t rn  = bits(cmd, 16, 19); // First operand register
  uint32_t rd  = bits(cmd, 12, 15); // Destination register
  uint32_t op2 = bits(cmd, 0, 11);
  bool carry = StateGetFlag(state, FLAG_C);

  if (i) {
    // Immediate constant
    uint32_t rot = bits(op2, 8, 11) * 2; // Rotation amount
    uint32_t imm = bits(op2, 0, 7);
    op2 = (imm >> rot) | (imm << (32 - rot));
    carry = op2 >> 31;
  } else {
    // Shifted register
    decodeShiftedRegister(state, &op2, &carry);
  }

  // Perform operation
  uint32_t op1 = state->reg[rn];
  uint32_t res;
  bool write;
  switch (op) {
    case 0:  res = op1 & op2;                                   write = true;  break; // and
    case 1:  res = op1 ^ op2;                                   write = true;  break; // eor
    case 2:  res = op1 - op2; carry = op1 >= op2;               write = true;  break; // sub
    case 3:  res = op2 - op1; carry = op2 >= op1;               write = true;  break; // rsb
    case 4:  res = op1 + op2; carry = (0xFFFFFFFF - op1) < op2; write = true;  break; // add
    case 8:  res = op1 & op2;                                   write = false; break; // tst
    case 9:  res = op1 ^ op2;                                   write = false; break; // teq
    case 10: res = op1 - op2; carry = op1 >= op2;               write = false; break; // cmp
    case 12: res = op1 | op2;                                   write = true;  break; // orr
    case 13: res = op2;                                         write = true;  break; // mov
    default: printf("Unsupported operation %#.4x\n", op);       return false;
  }
  if (write) StateSetReg(state, rd, res);
  if (s) {
    StateSetFlag(state, FLAG_C, carry);
    StateSetFlag(state, FLAG_Z, res == 0);
    StateSetFlag(state, FLAG_N, res >> 31);
  }
  return true;
}

bool execMultiply(State* state, uint32_t cmd) {
  if (bits(cmd, 22, 27) != 0) { printf("Not a multiply command %#.8x\n", cmd); return false; }
  uint32_t a  = bits(cmd, 21, 21);
  uint32_t s  = bits(cmd, 20, 20);
  uint32_t rd = bits(cmd, 16, 19);
  uint32_t rn = bits(cmd, 12, 15);
  uint32_t rs = bits(cmd, 8, 11);
  if (bits(cmd, 4, 7) != 9) { printf("Not a multiply command %#.8x\n", cmd); return false; }
  uint32_t rm = bits(cmd, 0, 3);

  // Perform operation
  uint32_t res = state->reg[rm] * state->reg[rs] + (a ? state->reg[rn] : 0);
  StateSetReg(state, rd, res);
  if (s) {
    StateSetFlag(state, FLAG_Z, res == 0);
    StateSetFlag(state, FLAG_N, res >> 31);
  }
  return true;
}

bool execSingleDataTransfer(State* state, uint32_t cmd) {
  if (bits(cmd, 26, 27) != 1) { printf("Not a single data transfer command %#.8x\n", cmd); return false; }
  uint32_t i   = bits(cmd, 25, 25);
  uint32_t p   = bits(cmd, 24, 24);
  uint32_t u   = bits(cmd, 23, 23);
  uint32_t b   = bits(cmd, 22, 22); MAYBE_UNUSED(b);
  uint32_t w   = bits(cmd, 21, 21);
  uint32_t l   = bits(cmd, 20, 20);
  uint32_t rn  = bits(cmd, 16, 19); // Base register
  uint32_t rd  = bits(cmd, 12, 15); // Source or destination register
  uint32_t off = bits(cmd, 0, 11);

  if (i) {
    // Shifted register (carry output is discarded)
    bool carry;
    decodeShiftedRegister(state, &off, &carry);
  }

  // Perform operation
  uint32_t rnv = state->reg[rn];
  uint32_t address = p ? (u ? rnv + off : rnv - off) : rnv;
  // To pass the tests `ldr05`, `ldr06`, `opt_ldr11`, `opt_ldr12` and `opt_ldr13`,
  // we need to continue execution despite out-of-bounds memory access...
  if ((size_t) address + 4 > state->mem.size) {
    printf("Error: Out of bounds memory access at address 0x%.8x\n", address);
  } else {
    if (l) StateSetReg(state, rd, ByteVectorGetU32LE(&state->mem, address)); // Load
    else ByteVectorSetU32LE(&state->mem, address, state->reg[rd]);           // Store
  }
  // Write-back or post-indexing: change the content of the base register
  if (w || !p) StateSetReg(state, rn, u ? rnv + off : rnv - off);

  return true;
}

bool execBlockDataTransfer(State* state, uint32_t cmd) {
  if (bits(cmd, 25, 27) != 4) { printf("Not a block data transfer command %#.8x\n", cmd); return false; }
  uint32_t p    = bits(cmd, 24, 24);
  uint32_t u    = bits(cmd, 23, 23);
  uint32_t s    = bits(cmd, 22, 22); MAYBE_UNUSED(s);
  uint32_t w    = bits(cmd, 21, 21);
  uint32_t l    = bits(cmd, 20, 20);
  uint32_t rn   = bits(cmd, 16, 19); // Base register
  uint32_t regs = bits(cmd, 0, 15);  // Register list
  if (regs == 0) { printf("At least one register must be transferred"); return false; }

  // Perform operation
  uint32_t address = state->reg[rn];
  if (u) {
    for (uint32_t i = 0; i < 16; i++) if ((regs >> i) & 1) {
      if (p) address += 4; // Pre-increment
      if ((size_t) address + 4 > state->mem.size) {
        printf("Error: Out of bounds memory access at address 0x%.8x\n", address);
      } else {
        if (l) StateSetReg(state, i, ByteVectorGetU32LE(&state->mem, address)); // Load
        else ByteVectorSetU32LE(&state->mem, address, state->reg[i]);           // Store
      }
      if (!p) address += 4; // Post-increment
    }
  } else {
    for (uint32_t i = 16; i --> 0;) if ((regs >> i) & 1) {
      if (p) address -= 4; // Pre-decrement
      if ((size_t) address + 4 > state->mem.size) {
        printf("Error: Out of bounds memory access at address 0x%.8x\n", address);
      } else {
        if (l) StateSetReg(state, i, ByteVectorGetU32LE(&state->mem, address)); // Load
        else ByteVectorSetU32LE(&state->mem, address, state->reg[i]);           // Store
      }
      if (!p) address -= 4; // Post-decrement
    }
  }
  // Write-back: change the content of the base register
  if (w) StateSetReg(state, rn, address);

  return true;
}

bool execBranchAndExchange(State* state, uint32_t cmd) {
  if (bits(cmd, 4, 27) != 0x12FFF1) { printf("Not a branch and exchange command %#.8x\n", cmd); return false; }
  uint32_t rn = bits(cmd, 0, 3);
  StateSetReg(state, REG_PC, state->reg[rn]);
  return true;
}

bool execBranch(State* state, uint32_t cmd) {
  if (bits(cmd, 25, 27) != 5) { printf("Not a branch command %#.8x\n", cmd); return false; }
  uint32_t l   = bits(cmd, 24, 24);
  uint32_t off = bits(cmd, 0, 23);
  if (l) StateSetReg(state, REG_LR, state->reg[REG_PC] - 4); // Branch with link
  off = ((bits(off, 23, 23) * 0xFF000000u) | off) << 2; // Sign extend and shift left
  StateSetReg(state, REG_PC, state->reg[REG_PC] + off); // Unsigned overflow is ok in C (will wrap around)
  return true;
}

bool emulate(State* state) {
  // To simulate the "three stage pipeline" we simply store the commands going through each stage
  // at a certain time point. Better approaches exist (e.g. pretending there is no pipeline and
  // offset every branching by +8 bytes), but this should be easier to understand.
  uint32_t executing = 0, decoding = 0, fetching = 0;  // The commands at each stage (if available)
  bool ef = false, df = false, ff = false; // Whether there are commands available at each stage

  // Inv: state.reg[REG_PC] denotes the address of the next command to be fetched;
  //      if `ef` is true then `executing` contains valid data, and similarly for `df` and `ff`.
  while (true) {
    // Advance pipeline (fetching is delayed until execution completes, since branching changes PC)
    executing = decoding; ef = df;
    decoding = fetching; df = ff;
    ff = false;
    // Execute command if `executing` is valid
    if (ef) {
      if (executing == 0) break; // The "halt" command
      if (StateCheckCond(state, bits(executing, 28, 31))) { // Condition code is satisfied
        // In general, we check patterns with the greatest number of fixed bits first
        if ((executing & 0x0FFFFFF0) == 0x012FFF10) {
          if (!execBranchAndExchange(state, executing)) goto fail;
        } else if ((executing & 0x0FC000F0) == 0x00000090) {
          if (!execMultiply(state, executing)) goto fail;
        } else if ((executing & 0x0C000000) == 0x00000000) {
          if (!execDataProcessing(state, executing)) goto fail;
        } else if ((executing & 0x0E000000) == 0x0A000000) {
          if (!execBranch(state, executing)) goto fail;
        } else if ((executing & 0x0E000000) == 0x08000000) {
          if (!execBlockDataTransfer(state, executing)) goto fail;
        } else if ((executing & 0x0C000000) == 0x04000000) {
          if (!execSingleDataTransfer(state, executing)) goto fail;
        } else {
          printf("Unsupported command %#.8x\n", executing);
          goto fail;
        }
      }
    }
    // Clears pipeline if PC has been written to
    if (state->pcDirty) {
      state->pcDirty = false;
      ef = df = ff = false;
    }
    // Fetch and advance PC (without updating `pcDirty` flag)
    if (state->reg[REG_PC] + 4 > state->mem.size) {
      printf("Program counter overflow\n");
      goto fail;
    }
    fetching = ByteVectorGetU32LE(&state->mem, state->reg[REG_PC]); ff = true;
    state->reg[REG_PC] += 4;
  }
  return true;

fail:
  printf("Executing: "); if (ef) printf("%#.8x\n", executing); else printf("n/a\n");
  printf("Decoding: "); if (df) printf("%#.8x\n", decoding); else printf("n/a\n");
  printf("Fetching: "); if (ff) printf("%#.8x\n", fetching); else printf("n/a\n");
  return false;
}

int main(int argc, char** argv) {
  ByteVector a;
  ByteVectorInit(&a);
  State state;
  StateInit(&state);

  // Check arguments
  if (argc != 2 && argc != 3 && argc != 7 && argc != 8) {
    printf("Usage: ./emulate <filename> [<memory-size> [<program-stack-size> "
           "<framebuffer-width> <framebuffer-height> <output-filename> [--wirenize]]]\n");
    goto fail;
  }

  // Get arguments
  size_t memSize = DEFAULT_MEM_SIZE;
  bool graphics = false, wirenize = false;
  size_t stack = memSize, width, height;

  if (argc >= 3) {
    sscanf(argv[2], "%zu", &memSize);
    stack = memSize;
  }
  if (argc >= 7) {
    sscanf(argv[3], "%zu", &stack);
    sscanf(argv[4], "%zu", &width);
    sscanf(argv[5], "%zu", &height);
    size_t end = stack + width * height * 4;
    if (end > memSize) printf("Framebuffer end position %zu exceeds memory size %zu, graphics mode disabled\n", end, memSize);
    else graphics = true;
  }
  if (argc >= 8) wirenize = true;

  // Read file
  if (!readFile(argv[1], &a)) {
    printf("Error: could not read file \"%s\"\n", argv[1]);
    goto fail;
  }
  if (a.size > stack) {
    printf("Error: file too large, file size is %zu bytes but (program + stack) size limit is %zu bytes\n", a.size, stack);
    goto fail;
  }

  // Emulate
  ByteVectorResize(&state.mem, memSize);
  StateLoadMem(&state, &a);
  StateSetReg(&state, REG_SP, (uint32_t) stack);
  if (graphics) {
    // Push arguments onto stack
    ByteVectorSetU32LE(&state.mem, stack - 12, (uint32_t) stack);
    ByteVectorSetU32LE(&state.mem, stack - 8, (uint32_t) width);
    ByteVectorSetU32LE(&state.mem, stack - 4, (uint32_t) height);
    StateSetReg(&state, REG_SP, (uint32_t) (stack - 12));
  }
  if (!emulate(&state)) {
    printf("Error: error(s) occurred in emulation\n");
    goto fail;
  }

  // Output results
  if (graphics) {
    // Mid: memSize >= stack + width * height * 4
    if (!saveBMP(argv[6], state.mem.p + stack, width, height, wirenize)) {
      printf("Error: could not save framebuffer to bitmap file \"%s\"\n", argv[6]);
      goto fail;
    }
  } else {
    // Only print state when it is not in "graphics mode"
    StatePrint(&state);
  }

  ByteVectorFree(&a);
  StateFree(&state);
  return EXIT_SUCCESS;

fail:
  ByteVectorFree(&a);
  StateFree(&state);
  return EXIT_FAILURE;
}
