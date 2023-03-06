#ifndef STATE_H_
#define STATE_H_

#include <stdint.h>
#include <stdbool.h>
#include "vector.h"

#define REG_SP 13
#define REG_LR 14
#define REG_PC 15
#define REG_CPSR 16

#define NUM_GP_REGS 13
#define NUM_REGS 17

#define FLAG_N 31
#define FLAG_Z 30
#define FLAG_C 29
#define FLAG_V 28

// State of an ARM machine
// Memory is heap-allocated to avoid stack overflow
typedef struct {
  uint32_t reg[NUM_REGS]; // Readonly
  ByteVector mem;         // Read-write
  bool pcDirty;           // Read-write
} State;

// Constructor
void StateInit(State* this);

// Destructor
void StateFree(State* this);

// Loads memory from `ByteVector src` (clearing existing memory content)
// Pre: this->mem.p != src->p && this->mem.size >= src->size
void StateLoadMem(State* this, const ByteVector* src);

// Write into register (setting `pcDirty = true` if the target register is PC)
void StateSetReg(State* this, uint32_t i, uint32_t val);

// Getter and setter of CPSR flags
bool StateGetFlag(const State* this, uint32_t i);
void StateSetFlag(State* this, uint32_t i, bool val);

// Check if condition code `cond` is satisfied by state `this`
bool StateCheckCond(const State* this, uint32_t cond);

// Print results
void StatePrint(const State* this);

#endif // STATE_H_
