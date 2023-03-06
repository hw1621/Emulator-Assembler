#include "state.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

void StateInit(State* this) {
  memset(this->reg, 0, sizeof(this->reg));
  ByteVectorInit(&this->mem);
  this->pcDirty = false;
}

void StateFree(State* this) {
  ByteVectorFree(&this->mem);
}

void StateLoadMem(State* this, const ByteVector* src) {
  assert(this->mem.p != src->p && this->mem.size >= src->size);
  memset(this->mem.p, 0, this->mem.size);
  memcpy(this->mem.p, src->p, src->size);
}

void StateSetReg(State* this, uint32_t i, uint32_t val) {
  assert(i < NUM_REGS);
  this->reg[i] = val;
  if (i == REG_PC) this->pcDirty = true;
}

bool StateGetFlag(const State* this, uint32_t i) {
  return (this->reg[REG_CPSR] >> i) & 1;
}

void StateSetFlag(State* this, uint32_t i, bool val) {
  if (val) this->reg[REG_CPSR] |= (1 << i);
  else this->reg[REG_CPSR] &= (0xFFFFFFFFu - (1 << i));
}

bool StateCheckCond(const State* this, uint32_t cond) {
  // See: imperial spec p4, ARM spec p47
  switch (cond) {
    case 0:  return  StateGetFlag(this, FLAG_Z);
    case 1:  return !StateGetFlag(this, FLAG_Z);
    case 2:  return  StateGetFlag(this, FLAG_C);
    case 3:  return !StateGetFlag(this, FLAG_C);
    case 4:  return  StateGetFlag(this, FLAG_N);
    case 5:  return !StateGetFlag(this, FLAG_N);
    case 6:  return  StateGetFlag(this, FLAG_V);
    case 7:  return !StateGetFlag(this, FLAG_V);
    case 8:  return  StateGetFlag(this, FLAG_C) && !StateGetFlag(this, FLAG_Z);
    case 9:  return !StateGetFlag(this, FLAG_C) ||  StateGetFlag(this, FLAG_Z);
    case 10: return  StateGetFlag(this, FLAG_N) ==  StateGetFlag(this, FLAG_V);
    case 11: return  StateGetFlag(this, FLAG_N) !=  StateGetFlag(this, FLAG_V);
    case 12: return !StateGetFlag(this, FLAG_Z) && (StateGetFlag(this, FLAG_N) == StateGetFlag(this, FLAG_V));
    case 13: return  StateGetFlag(this, FLAG_Z) || (StateGetFlag(this, FLAG_N) != StateGetFlag(this, FLAG_V));
    case 14: return  true;
  }
  // Mid: unknown condition code
  assert(false);
  return false;
}

void StatePrint(const State* this) {
  printf("Registers:\n");
  for (size_t i = 0; i < NUM_GP_REGS; i++) {
    printf("$%-3zu: %10d (0x%.8x)\n", i, (int)this->reg[i], this->reg[i]);
  }
  printf("PC  : %10d (0x%.8x)\n", (int)this->reg[REG_PC], this->reg[REG_PC]);
  printf("CPSR: %10d (0x%.8x)\n", (int)this->reg[REG_CPSR], this->reg[REG_CPSR]);
  printf("Non-zero memory:\n");
  for (size_t i = 0; i + 4 <= this->mem.size; i += 4) {
    uint32_t val = ByteVectorGetU32BE(&this->mem, i);
    if (val != 0) printf("0x%.8zx: 0x%.8x\n", i, val);
  }
}
