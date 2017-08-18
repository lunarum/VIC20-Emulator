#ifndef TEST_H
#define TEST_H

#include "scunit.h"

#include "cpu.h"
#include "memory.h"

#define TEST1_MEMORY_VALUE 0xFF
#define TEST2_MEMORY_VALUE 0xAA
#define DEFAULT_MEMORY_VALUE 0x00
#define DEFAULT_MEMORY_ADDRESS 0x1000
#define DEFAULT_RUN_ADDRESS 0xE000

SCU_error testsuite_setup();
SCU_error test_setup();

void testsuite_memory_create();
void testsuite_opcodes_create();

#endif