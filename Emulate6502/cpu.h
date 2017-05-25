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

typedef enum { false = 0, true = !false } bool;

#include "memory.h"

extern struct registers {
    byte A,      /*< Accumulator */
         X,      /*< X index */
         Y,      /*< Y index */
         SP;     /*< Stack Pointer offset (from page 1) */
    word PC;     /*< Program Counter */
    bool PS_N, PS_V, PS_B, PS_D, PS_I, PS_Z, PS_C;
} cpu;
extern bool single_step;
extern int cycle_counter;
extern int cycle_reset;

extern void cpu_setCycleReset(int value);
extern void cpu_RESET();
extern void cpu_statusPush(void);
extern void cpu_statusPull(void);
extern void cpu_disassemble(word address);
extern void cpu_logStatus(void);

extern void cpu_run(void);

static inline bool bitGet(byte by, byte bit) { return ((by & 1 << bit) >> bit); }
static inline void bitSet(byte by, byte bit, bool value) { if(value) by |= 1 << bit; else by &= ~(1 << bit); }

static inline void cpu_IRQ() {
    memory_stackPushAddress(cpu.PC+1);
    cpu_statusPush();
    cpu.PS_B = false;
    cpu.PC = MEM_IRQ_BREAK;
}

static inline void cpu_NMI() {
    memory_stackPushAddress(cpu.PC+1);
    cpu_statusPush();
    cpu.PC = MEM_NMI;
}

#endif
