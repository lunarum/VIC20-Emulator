#ifndef CPU_H
#define CPU_H
/************************************************************************
 * copied from https://www.dwheeler.com/6502/oneelkruns/asm1step.html
 * 
   The 6502 Architecture
   ---------------------
   The 6502 is an 8-bit microprocessor that follows the memory oriented 
   design philosophy of the Motorola 6800.  Several engineers left 
   Motorola and formed MOS Technology which introduced the 6502 in 1975.  
   The 6502 gained in popularity because of it's low price and became the 
   heart of several early personal computers including the Apple II, 
   Commodore 64, and Atari 400 and 800.
   
   Simplicity is key
   -----------------
   The 6502 handles data in its registers, each of which holds one byte 
   (8-bits) of data.  There are a total of three general use and two special
   purpose registers:
      accumulator (A)  -  Handles all arithmetic and logic.  The real heart
                          of the system.
      X and Y          -  General purpose registers with limited abilities.
      SP               -  Stack pointer.
      PS               -  Processor status.  Holds the result of tests 
                          and flags.
                             
   Stack Pointer
   -------------
   When the microprocessor executes a JSR (Jump to SubRoutine) 
   instruction it needs to know where to return when finished.  The 6502 
   keeps this information in low memory from $0100 to $01FF and uses the 
   stack pointer as an offset.  The stack grows down from $01FF and makes 
   it possible to nest subroutines up to 128 levels deep.  Not a problem 
   in most cases.
   
   Processor Status
   ----------------
   The processor status register is not directly accessible by any 6502 
   instruction.  Instead, there exist numerous instructions that test the 
   bits of the processor status register.  The flags within the register 
   are:
       bit ->   7                           0
              +---+---+---+---+---+---+---+---+
              | N | V |   | B | D | I | Z | C |  <-- flag, 0/1 = reset/set
              +---+---+---+---+---+---+---+---+

       N  =  NEGATIVE. Set if bit 7 of the accumulator is set.
       V  =  OVERFLOW. Set if the addition of two like-signed numbers or the
             subtraction of two unlike-signed numbers produces a result
             greater than +127 or less than -128.
       B  =  BRK COMMAND. Set if an interrupt caused by a BRK, reset if
             caused by an external interrupt.
       D  =  DECIMAL MODE. Set if decimal mode active.
       I  =  IRQ DISABLE.  Set if maskable interrupts are disabled.
       Z  =  ZERO.  Set if the result of the last operation (load/inc/dec/
             add/sub) was zero.
       C  =  CARRY. Set if the add produced a carry, or if the subtraction
             produced a borrow.  Also holds bits after a logical shift.
             
   Accumulator
   -----------
   The majority of the 6502's business makes use of the accumulator.  All 
   addition and subtraction is done in the accumulator.  It also handles 
   the majority of the logical comparisons (is A > B ?) and logical bit 
   shifts.
   
   
   X and Y
   -------
   These are index registers often used to hold offsets to memory 
   locations.  They can also be used for holding needed values.  Much of 
   their use lies in supporting some of the addressing modes.
 ************************************************************************/

#define MEM_STACK_PAGE 0x01
#define MEM_ZERO_PAGE  0x00
#define MEM_NMI        0xFFFA
#define MEM_RESET      0xFFFC
#define MEM_IRQ_BREAK  0xFFFE

#define COUNTER_VIC  0
#define COUNTER_VIA1 1
#define COUNTER_VIA2 2
#define MAX_COUNTERS 3

#include "memory.h"

typedef enum {RESULT_STEP, RESULT_NMI, RESULT_RESET, RESULT_IRQ, RESULT_ILLEGAL_INSTUCTION} cpu_result;

extern struct registers {
    byte A,      /*< Accumulator */
         X,      /*< X index */
         Y,      /*< Y index */
         SP;     /*< Stack Pointer offset (from page 1) */
    word PC;     /*< Program Counter */
    bool PS_N, PS_V, PS_B, PS_D, PS_I, PS_Z, PS_C;
    byte cycles; /* cycles count of current instruction */
} cpu;

extern bool single_step;
extern struct counters {
    unsigned counter;
    void (*signal)();
} counter[MAX_COUNTERS];

extern void cpu_reset();
extern void cpu_statusPush(void);
extern void cpu_statusPull(void);
extern void cpu_disassemble(word address);
extern void cpu_logStatus(cpu_result result);

extern cpu_result cpu_run(void);

static inline bool bitGet(byte by, byte bit) { return ((by & 1 << bit) >> bit); }
static inline void bitSet(byte by, byte bit, bool value) { if(value) by |= 1 << bit; else by &= ~(1 << bit); }

static inline byte cpu_get_state() {
    return((byte) 0x20
        | ((cpu.PS_N) ? 0x80 : 0)
        | ((cpu.PS_V) ? 0x40 : 0)
        | ((cpu.PS_B) ? 0x10 : 0)
        | ((cpu.PS_D) ? 0x08 : 0)
        | ((cpu.PS_I) ? 0x04 : 0)
        | ((cpu.PS_Z) ? 0x02 : 0)
        | ((cpu.PS_C) ? 0x01 : 0));
}

static inline void cpu_IRQ() {
    memory_stackPushAddress(cpu.PC+1);
    memory_stackPush(cpu_get_state());
    cpu.PS_B = false;
    cpu.PC = memory_get_vector(MEM_IRQ_BREAK);
}

static inline void cpu_NMI() {
    memory_stackPushAddress(cpu.PC+1);
    memory_stackPush(cpu_get_state());
    cpu.PC = memory_get_vector(MEM_NMI);
}

#endif
