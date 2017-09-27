#include "test.h"
#include <string.h>


SCU_error testsuite_setup() {
    memory_setPageType(0x00, 0xDF, MEM_READ | MEM_WRITE); /* RAM */
    memory_setPageType(0xE0, 0xFF, MEM_READ); /* Kernel ROM */
    byte* memory = memory_get_ptr(0);
    for(long l = 0; l < 65536; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    cpu_RESET();
    return SCU_SUCCESS;
}

SCU_error test_setup() {
    cpu_RESET();
    cpu.A = 0;
    cpu.X = 0;
    cpu.Y = 0;
    cpu.PS_N = false;
    cpu.PS_V = false;
    cpu.PS_B = false;
    cpu.PS_D = false;
    cpu.PS_I = false;
    cpu.PS_Z = false;
    cpu.PS_C = false;
    return SCU_SUCCESS;
}

int main(int argc, char **argv)
{
	printf("Unit test 6502 Emulator\n\n");
 
    SCU_runMode = SCU_RUN_MODE_VERBOSE;
//    SCU_runMode = SCU_RUN_MODE_FAILED;
    testsuite_memory_create();
    testsuite_opcodes_create();
    SCU_error error = SCU_executeAllTests();

	return error;
}
