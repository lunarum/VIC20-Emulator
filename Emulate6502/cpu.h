#ifndef CPU_H
#define CPU_H

#define MEM_MAX        0xFFFF
#define MEM_STACK_BASE 0x0100
#define MEM_NMI        0xFFFA
#define MEM_RESET      0xFFFC
#define MEM_IRQ_BREAK  0xFFFE

#define byte unsigned short

struct cpu {
    byte A, X, Y, SP;
    int PC
} cpu;


void cpu_run(*byte memory);
void cpu_log_status(void);

#endif