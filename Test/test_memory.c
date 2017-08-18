#include "test.h"


SCU_error test_stack_teardown() {
    byte* memory = memory_get_ptr(0x0100);
    for(long l = 0; l < 256; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    return SCU_SUCCESS;
}

SCU_error test_memory_teardown() {
    byte* memory = memory_get_ptr(0x0000);
    for(long l = 0; l < 256; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    memory = memory_get_ptr(DEFAULT_MEMORY_ADDRESS);
    for(long l = 0; l < 512; ++l) {
        *memory++ = DEFAULT_MEMORY_VALUE;
    }
    return SCU_SUCCESS;
}


SCU_error test_memory_get_set() {
    SCU_error error;
    byte* memory = memory_get_ptr(0);
    word address = DEFAULT_MEMORY_ADDRESS;
    memory_set(address, TEST1_MEMORY_VALUE);
    byte b = memory[address];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    b = memory_get(address);
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    memory_setLast(TEST2_MEMORY_VALUE);
    b = memory[address];
    error = SCU_ASSERT(b == TEST2_MEMORY_VALUE, b);
    // Set ROM value
    address = 0xF000;
    memory_set(address, TEST1_MEMORY_VALUE);
    b = memory_get(address);
    error = SCU_ASSERT(b == DEFAULT_MEMORY_VALUE, b);   // set must have failed so get returns default value
    
    return error;
}

SCU_error test_memory_get_set_zero() {
    SCU_error error;

    byte* memory = memory_get_ptr(0);
    byte addressZ = 0x10;
    memory_setZero(addressZ, TEST1_MEMORY_VALUE);
    byte b = memory[addressZ];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    b = memory_getZero(addressZ);
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_stack() {
    SCU_error error;

    cpu_RESET();
    byte *memory = memory_get_ptr(0);
    word address = 0x01FF;
    byte b, i;
    for(i = 0; i < 10; ++i) {
        memory_stackPush(i);
        b = memory[address--];
        error = SCU_ASSERT(b == i, b);
        if(i % 2 == 0) {
            b = memory_stackPull();
            ++address;
            error = SCU_ASSERT(b == i, b);
        }
    }

    // Check wrap-around of the stackpointer
    byte saved_SP = cpu.SP;
    for(word n = 0; n < 256; ++n) {
        memory_stackPush((byte)n);
    }
    error = SCU_ASSERT(saved_SP = cpu.SP, cpu.SP);
    b = memory_stackPull();
    error = SCU_ASSERT(b == 255, b);
    saved_SP = cpu.SP;
    for(word n = 0; n < 255; ++n) {
        b = memory_stackPull();
    }
    error = SCU_ASSERT(saved_SP = cpu.SP, cpu.SP);
    error = SCU_ASSERT(b == 0, b);

    cpu_RESET();
    address = 0x01FF;
    word w;
    for(i = 0; i < 10; ++i) {
        memory_stackPushAddress(0xF000 + i);
        b = memory[address--];
        error = SCU_ASSERT(b == 0xF0, b);
        b = memory[address--];
        error = SCU_ASSERT(b == i, b);
        if(i % 2 == 0) {
            w = memory_stackPullAddress();
            address += 2;
            error = SCU_ASSERT(w == 0xF000 + i, w);
        }
    }

    // Check wrap-around of the stackpointer
    saved_SP = cpu.SP;
    for(word n = 0; n < 128; ++n) {
        memory_stackPushAddress(0xCC00 + n);
    }
    error = SCU_ASSERT(saved_SP = cpu.SP, cpu.SP);
    w = memory_stackPullAddress();
    error = SCU_ASSERT(w == 0xCC7F, w);
    saved_SP = cpu.SP;
    for(word n = 0; n < 127; ++n) {
        w = memory_stackPullAddress();
    }
    error = SCU_ASSERT(saved_SP = cpu.SP, cpu.SP);
    error = SCU_ASSERT(w == 0xCC00, w);
    
    b = memory[0x00FF];
    error = SCU_ASSERT(b == DEFAULT_MEMORY_VALUE, b);   // no side effects below of stack page
    b = memory[0x0200];
    error = SCU_ASSERT(b == DEFAULT_MEMORY_VALUE, b);   // no side effects above of stack page

    return error;
}

SCU_error test_memory_immediate() {
    SCU_error error;

    byte* memory = memory_get_ptr(0);
    memory[DEFAULT_MEMORY_ADDRESS] = TEST1_MEMORY_VALUE;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    byte b = memory_getImmediate();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_absolute() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    word address = DEFAULT_MEMORY_ADDRESS + 256;
    
    memory[DEFAULT_MEMORY_ADDRESS  ] = address & 0xFF;
    memory[DEFAULT_MEMORY_ADDRESS+1] = address >> 8;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    memory_setAbsolute(TEST1_MEMORY_VALUE);
    b = memory[address];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getAbsolute();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_zero() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    byte addressZ = 0x10;
    
    memory[DEFAULT_MEMORY_ADDRESS] = addressZ;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    memory_setZeroPage(TEST1_MEMORY_VALUE);
    b = memory[addressZ];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getZeroPage();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_indirect_absolute() {
    SCU_error error;
    byte *memory = memory_get_ptr(0);
    word w, address = (DEFAULT_MEMORY_ADDRESS & 0xFF00) + 2;
    
    memory[DEFAULT_MEMORY_ADDRESS  ] = address & 0xFF;
    memory[DEFAULT_MEMORY_ADDRESS+1] = address >> 8;
    memory[address  ] = 0x34;
    memory[address+1] = 0x12;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    w = memory_getIndirectAbsoluteAddress();
    error = SCU_ASSERT(w == 0x1234, w);
    
    address = ((DEFAULT_MEMORY_ADDRESS & 0xFF00) | 0xFF) + 256;
    memory[DEFAULT_MEMORY_ADDRESS  ] = address & 0xFF;
    memory[DEFAULT_MEMORY_ADDRESS+1] = address >> 8;
    memory[address] = 0x78;
    memory[(address & 0xFF00)] = 0x56;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    w = memory_getIndirectAbsoluteAddress();
    error = SCU_ASSERT(w == 0x5678, w);
    
    return error;
}

SCU_error test_memory_absolute_indexed() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    word address = DEFAULT_MEMORY_ADDRESS + 256;
    
    memory[DEFAULT_MEMORY_ADDRESS  ] = address & 0xFF;
    memory[DEFAULT_MEMORY_ADDRESS+1] = address >> 8;

    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.X = 1;
    memory_setAbsoluteIndexedX(TEST1_MEMORY_VALUE);
    b = memory[address + 1];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getAbsoluteIndexedX();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);

    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.Y = 2;
    memory_setAbsoluteIndexedY(TEST2_MEMORY_VALUE);
    b = memory[address + 2];
    error = SCU_ASSERT(b == TEST2_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getAbsoluteIndexedY();
    error = SCU_ASSERT(b == TEST2_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_zero_indexed() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    byte addressZ = 250;
    
    memory[DEFAULT_MEMORY_ADDRESS] = addressZ;

    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.X = 10;
    memory_setZeroPageIndexedX(TEST1_MEMORY_VALUE);
    b = memory[(byte)(addressZ + 10)];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getZeroPageIndexedX();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);

    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.Y = 20;
    memory_setZeroPageIndexedY(TEST2_MEMORY_VALUE);
    b = memory[(byte)(addressZ + 20)];
    error = SCU_ASSERT(b == TEST2_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getZeroPageIndexedY();
    error = SCU_ASSERT(b == TEST2_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_indexed_indirect() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    byte addressZ = 0xB4;
    word address = DEFAULT_MEMORY_ADDRESS+1;
    
    memory[DEFAULT_MEMORY_ADDRESS] = addressZ;
    memory[addressZ+6] = address & 0xFF;
    memory[addressZ+7] = address >> 8;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.X = 6;
    memory_setIndexedIndirectX(TEST1_MEMORY_VALUE);
    b = memory[address];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getIndexedIndirectX();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_indirect_indexed() {
    SCU_error error;
    byte b, *memory = memory_get_ptr(0);
    byte addressZ = 0xB4;
    word address = DEFAULT_MEMORY_ADDRESS+6;
    
    memory[DEFAULT_MEMORY_ADDRESS] = addressZ;
    memory[addressZ] = (address-6) & 0xFF;
    memory[addressZ+1] = (address-6) >> 8;
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    cpu.Y = 6;
    memory_setIndirectIndexedY(TEST1_MEMORY_VALUE);
    b = memory[address];
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    cpu.PC = DEFAULT_MEMORY_ADDRESS;
    b = memory_getIndirectIndexedY();
    error = SCU_ASSERT(b == TEST1_MEMORY_VALUE, b);
    
    return error;
}

SCU_error test_memory_relative() {
    SCU_error error;
    byte *memory = memory_get_ptr(0);
    word w, address = DEFAULT_MEMORY_ADDRESS;
    
    memory[address] = 0x05;
    cpu.PC = address++;
    w = memory_getRelativeAddress();
    error = SCU_ASSERT(w == address+5, w);
    memory[address] = 0xFB;
    cpu.PC = address++;
    w = memory_getRelativeAddress();
    error = SCU_ASSERT(w == address-5, w);
    
    return error;
}

void testsuite_memory_create() {
    SCU_TestSuite *pSuite = SCU_TestSuite_create("Memory", testsuite_setup, NULL);
    SCU_TestCase_create(pSuite, "Get-Set",          test_memory_get_set,            test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Get-Set-Zero",     test_memory_get_set_zero,       test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Stack",            test_memory_stack,              test_setup, test_stack_teardown);
    SCU_TestCase_create(pSuite, "Immediate",        test_memory_immediate,          test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Absolute",         test_memory_absolute,           test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Zero-page",        test_memory_zero,               test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Indirect-absolute",test_memory_indirect_absolute,  test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Absolute-indexed", test_memory_absolute_indexed,   test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Zero-page-indexed",test_memory_zero_indexed,       test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Indexed-indirect", test_memory_indexed_indirect,   test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Indirect-indexed", test_memory_indirect_indexed,   test_setup, test_memory_teardown);
    SCU_TestCase_create(pSuite, "Relative",         test_memory_relative,           test_setup, test_memory_teardown);
}