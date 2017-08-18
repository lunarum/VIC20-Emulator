#include "test.h"


SCU_error test_code_teardown() {
    byte* memory = memory_get_ptr(0x0000);
    for(long l = 0; l < 512; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    memory = memory_get_ptr(DEFAULT_MEMORY_ADDRESS);
    for(long l = 0; l < 512; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    memory = memory_get_ptr(DEFAULT_RUN_ADDRESS);
    for(long l = 0; l < 512; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    return SCU_SUCCESS;
}

unsigned long run_code(word address, char *code) {
    byte *memory = memory_get_ptr(0);
    byte *m = memory+address;
    byte bytes = 0;
    unsigned long cycles = 0;
    
    for(char *p = code; *p; ++bytes) {
        byte data = (byte)strtol(p, &p, 16);
        *m++ = data;
    }
    
    single_step = true;
    for(cpu.PC = address; cpu.PC >= address && cpu.PC < address+bytes; cycles += cpu.cycles) {
        if(cpu_run() != RESULT_IRQ) {
            cycles = 0;
            break;
        }
    }

    return cycles;
}

SCU_error test_BNE_BEQ() {
    SCU_error error;
//*=$E000
//LDX #$10
//Label1:
//TXA
//STA *$10,X
//DEX
//BNE Label1
//.END
    unsigned long cycles = run_code(0xE000, "A2 10 8A 95 10 CA D0 FA");
    error = SCU_ASSERT(cycles == 2+16*(2+4+2+3)-1, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE008, cpu.PC);
    byte *memory = memory_get_ptr(0);
    error = SCU_ASSERT(memory[0x20] == 16, memory[0x20]);
    error = SCU_ASSERT(memory[0x11] ==  1, memory[0x11]);
    error = SCU_ASSERT(cpu.PS_Z == true, cpu.PS_Z);
 
//*=$E000
//LDX #$10
//BEQ Label1
//NOP
//
//Label1:
//LDY #0
//BEQ Label2
//NOP
//
//Label2:
//NOP
//.END
    cycles = run_code(0xE000, "A2 10 F0 01 EA A0 00 F0 01 EA EA");
    error = SCU_ASSERT(cycles == 2+2+2+2+3+2, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE00B, cpu.PC);
    error = SCU_ASSERT(cpu.PS_Z == true, cpu.PS_Z);
   
    return error;
}

SCU_error test_BMI_BPL() {
    SCU_error error;
//*=$E000
//LDA #$01
//BMI Label2
//BPL Label1
//BRK
//
//Label1:
//LDA #$81
//BPL Label2
//BMI Label3
//
//Label2:
//BRK
//
//Label3:
//.END
    unsigned long cycles = run_code(0xE000, "A9 01 30 09 10 01 00 A9 81 10 02 30 01 00");
    error = SCU_ASSERT(cycles == 2+2+3+2+2+3, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE00E, cpu.PC);
    error = SCU_ASSERT(cpu.PS_N == true, cpu.PS_N);
    return error;
}

SCU_error test_BCC_BCS_SEC_CLC() {
    SCU_error error;
//*=$E000
//BCS Label2
//BCC Label1
//BRK
//
//Label1:
//SEC
//BCC Label2
//BCS Label3
//
//Label2:
//BRK
//
//Label3:
//.END
    unsigned long cycles = run_code(0xE000, "B0 08 90 01 00 38 90 02 B0 01 00");
    error = SCU_ASSERT(cycles == 2+3+2+2+3, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE00B, cpu.PC);
    error = SCU_ASSERT(cpu.PS_C == true, cpu.PS_C);

//*=$E000
//CLC
//.END
    cycles = run_code(0xE000, "18");
    error = SCU_ASSERT(cycles == 2, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE001, cpu.PC);
    error = SCU_ASSERT(cpu.PS_C == false, cpu.PS_C);

    return error;
}

SCU_error test_BVC_BVS_CLV() {
    SCU_error error;
//*=$E000
//BVS Label2
//BVC Label1
//BRK
//
//Label1:
//LDA #$7F
//ADC #$01
//BVC Label2
//BVS Label3
//
//Label2:
//BRK
//
//Label3:
//.END
    unsigned long cycles = run_code(0xE000, "70 0B 50 01 00 A9 7F 69 01 50 02 70 01 00");
    error = SCU_ASSERT(cycles == 2+3+2+2+2+3, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE00E, cpu.PC);
    error = SCU_ASSERT(cpu.PS_V == true, cpu.PS_V);

//*=$E000
//CLV
//.END
    cycles = run_code(0xE000, "B8");
    error = SCU_ASSERT(cycles == 2, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE001, cpu.PC);
    error = SCU_ASSERT(cpu.PS_V == false, cpu.PS_V);

    return error;
}

SCU_error test_SED_SEI_BRK_RTI() {
    SCU_error error;
//*=$E000
//SED
//SEI
//BRK
//.END
    unsigned long cycles = run_code(0xE000, "F8 78 00");
    error = SCU_ASSERT(cycles == 2+2+7, cycles);
    error = SCU_ASSERT(cpu.PC == MEM_IRQ_BREAK, cpu.PC);
    error = SCU_ASSERT(cpu.PS_B == true, cpu.PS_B);
    error = SCU_ASSERT(cpu.PS_D == true, cpu.PS_D);
    error = SCU_ASSERT(cpu.PS_I == true, cpu.PS_I);
    byte *sp = memory_get_ptr(0) + cpu.SP + 1;
    byte status = *sp++;
    error = SCU_ASSERT(status = cpu_get_state(), status);
    word address = (*sp++) | (word)(*sp++ << 8); 
    error = SCU_ASSERT(address = 0xE003, address);
    
    cpu.PS_D = false;
    cpu.PS_I = false;
//*=$E003
//RTI
//.END
    cycles = run_code(0xE003, "40");
    error = SCU_ASSERT(cycles == 6, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE004, cpu.PC);
    error = SCU_ASSERT(cpu.SP == 0xFF, cpu.SP);
    error = SCU_ASSERT(cpu.PS_B == false, cpu.PS_B);
    error = SCU_ASSERT(cpu.PS_D == true, cpu.PS_D);
    error = SCU_ASSERT(cpu.PS_I == true, cpu.PS_I);

    return error;
}

SCU_error test_JSR_RTS_JMP() {
    SCU_error error;
//*=$E000
//Label1:
//JSR Label1
//JMP Label2
//BRK
//
//Label1:
//RTS
//
//Label2:
//JMP (Label3)
//BRK
//
//Label3:
//.Word Label4
//
//Label4:
//.END
    unsigned long cycles = run_code(0xE000, "20 07 E0 4C 08 E0 00 60 6C 0C E0 00 0E E0");
    error = SCU_ASSERT(cycles == 6+6+3+5, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE00E, cpu.PC);
    error = SCU_ASSERT(cpu.SP == 0xFF, cpu.SP);
    byte *sp = memory_get_ptr(0) + cpu.SP - 1;
    word address = (*sp++) | (word)(*sp++ << 8); 
    error = SCU_ASSERT(address = 0xE002, address);

    return error;
}

SCU_error test_PHA_PLA_PHP_PLP() {
    SCU_error error;
//*=$E000
//LDA #$FF
//PHA
//PLP
//PHP
//PLA
//.END
    unsigned long cycles = run_code(0xE000, "A9 FF 48 28 08");
    error = SCU_ASSERT(cycles == 2+3+4+3, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE005, cpu.PC);
    error = SCU_ASSERT(cpu.PS_B == true, cpu.PS_B);
    error = SCU_ASSERT(cpu.PS_C == true, cpu.PS_C);
    error = SCU_ASSERT(cpu.PS_D == true, cpu.PS_D);
    error = SCU_ASSERT(cpu.PS_I == true, cpu.PS_I);
    error = SCU_ASSERT(cpu.PS_N == true, cpu.PS_N);
    error = SCU_ASSERT(cpu.PS_V == true, cpu.PS_V);
    error = SCU_ASSERT(cpu.PS_Z == true, cpu.PS_Z);

    cycles = run_code(0xE005, "68");
    error = SCU_ASSERT(cycles == 4, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE006, cpu.PC);
    error = SCU_ASSERT(cpu.A == 0xFF, cpu.A);

    return error;
}

SCU_error test_Transfers() {
    SCU_error error;
//*=$E000
//TSX
//TXA
//TAY
//.END
    unsigned long cycles = run_code(0xE000, "BA 8A A8");
    error = SCU_ASSERT(cycles == 2+2+2, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE003, cpu.PC);
    error = SCU_ASSERT(cpu.A == 0x20, cpu.A);
    error = SCU_ASSERT(cpu.X == 0x20, cpu.A);
    error = SCU_ASSERT(cpu.Y == 0x20, cpu.A);

//*=$E000
//LDY #$FF
//TYA
//TAX
//TXS
//.END
    cycles = run_code(0xE000, "A0 FF 98 AA 9A");
    error = SCU_ASSERT(cycles == 2+2+2+2, cycles);
    error = SCU_ASSERT(cpu.PC == 0xE005, cpu.PC);
    error = SCU_ASSERT(cpu.PS_B == true, cpu.PS_B);
    error = SCU_ASSERT(cpu.PS_C == true, cpu.PS_C);
    error = SCU_ASSERT(cpu.PS_D == true, cpu.PS_D);
    error = SCU_ASSERT(cpu.PS_I == true, cpu.PS_I);
    error = SCU_ASSERT(cpu.PS_N == true, cpu.PS_N);
    error = SCU_ASSERT(cpu.PS_V == true, cpu.PS_V);
    error = SCU_ASSERT(cpu.PS_Z == true, cpu.PS_Z);
    error = SCU_ASSERT(cpu.A == 0xFF, cpu.A);
    error = SCU_ASSERT(cpu.X == 0xFF, cpu.A);
    error = SCU_ASSERT(cpu.Y == 0xFF, cpu.A);

    return error;
}

void testsuite_opcodes_create() {
    SCU_TestSuite *pSuite = SCU_TestSuite_create("Opcodes", testsuite_setup, NULL);
    SCU_TestCase_create(pSuite, "BNE_BEQ",          test_BNE_BEQ,           test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "BMI_BPL",          test_BMI_BPL,           test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "BCC_BCS_SEC_CLC",  test_BCC_BCS_SEC_CLC,   test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "BVC_BVS_CLV",      test_BVC_BVS_CLV,       test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "SED_SEI_BRK_RTI",  test_SED_SEI_BRK_RTI,   test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "JSR_RTS_JMP",      test_JSR_RTS_JMP,       test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "PHA_PLA_PHP_PLP",  test_PHA_PLA_PHP_PLP,   test_setup, test_code_teardown);
    SCU_TestCase_create(pSuite, "Transfers",        test_Transfers,         test_setup, test_code_teardown);
}