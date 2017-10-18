#include <stdio.h>
#include "memory.h"
#include "cpu.h"

static byte memory[65536];
static byte pageType[256];
static word last_address;


byte *memory_get_ptr(word startAddress) {
    return memory + startAddress;
}

word memory_get_vector(word vectorAddress) {
    return getAddress(memory[vectorAddress], memory[vectorAddress+1]);
}

void memory_setPageType(byte startPage, byte endPage, byte memoryType) {
    for(int page = startPage; page <= endPage; ++page)
        pageType[page] = memoryType;
}

byte memory_get(word address) {
    byte page = getPage(address);
    last_address = address;
    if(pageType[page] & MEM_READ) {
        return memory[last_address];
    }
    else if(pageType[page] & MEM_IO) {
        if(pageType[page] & IO_VIC)
            return vic_read_register(address);
        else if(pageType[page] & IO_VIA)
            return via_read_register(address);
    }
    return 0xAA; /* dummy value */
}

void memory_set(word address, byte value) {
    byte page = getPage(address);
    if(pageType[page] & MEM_WRITE) {
        memory[address] = value;
    }
    else if(pageType[page] & MEM_IO) {
        if(pageType[page] & IO_VIC)
            vic_write_register(address, value);
        else if(pageType[page] & IO_VIA)
            via_write_register(address, value);
    }
}

void memory_setLast(byte value) {
    byte page = getPage(last_address);
    if(pageType[page] & MEM_WRITE) {
        memory[last_address] = value;
    }
    else if(pageType[page] & MEM_IO) {
        if(pageType[page] & IO_VIC)
            vic_write_register(last_address, value);
        else if(pageType[page] & IO_VIA)
            via_write_register(last_address, value);
    }
}

byte memory_getZero(byte addressZ) {
    last_address = addressZ;
     /* assume zero page is readable, therefore don't check */
   return memory[last_address];
}

void memory_setZero(byte addressZ, byte value) {
     /* assume zero page is writable, therefore don't check */
    memory[addressZ] = value;
}

byte memory_stackPull() {
     /* assume stack page is readable, therefore don't check */
    last_address = 0x0100 | ++cpu.SP;
    return memory[last_address];
}

void memory_stackPush(byte value) {
    /* assume stack page is writable, therefore don't check */
    word stackAddress = 0x0100 | cpu.SP--;
    memory[stackAddress] = value;
}

word memory_stackPullAddress() {
     /* assume stack page is readable, therefore don't check */
    last_address = 0x0100 | ++cpu.SP;    /* stay on stack page! */
    byte lowByte = memory[last_address]; /* low-byte first */
    last_address = 0x0100 | ++cpu.SP;    /* stay on stack page! */
    byte highByte = memory[last_address];/* high-byte last */
    return getAddress(lowByte, highByte);
}

void memory_stackPushAddress(word address) {
    /* assume stack page is writable, therefore don't check */
    last_address = 0x0100 | cpu.SP--;       /* stay on stack page! */
    memory[last_address] = address >> 8;    /* high-byte first */
    last_address = 0x0100 | cpu.SP--;       /* stay on stack page! */
    memory[last_address] = (address & 0xFF);/* low-byte last */
}
/*
    Immediate Addressing
    --------------------
    The value given is a number to be used immediately by the instruction.
    For example, LDA #$99 loads the value $99 into the accumulator.
 */
byte memory_getImmediate() {
    byte value = memory_get(cpu.PC++);
    return value;
}

/*!
    Absolute Addressing
    -------------------
    The value given is the address (16-bits) of a memory location that contains the 8-bit value to be used.
    For example, STA $3E32 stores the present value of the accumulator in memory location $3E32.
 */

word memory_getAbsoluteAddress() {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    return getAddress(lowByte, highByte);
}

byte memory_getAbsolute() {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    return memory_get(getAddress(lowByte, highByte));
}

void memory_setAbsolute(byte value) {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    memory_set(getAddress(lowByte, highByte), value);
}

/*
    Zero Page Addressing
    --------------------
    The first 256 memory locations ($0000-00FF) are called "zero page".  The next 256 instructions ($0100-01FF) are page 1, etc.
    Instructions making use of the zero page save memory by not using an extra $00 to indicate the high part of the address.
    For example,
      LDA $0023   -- works but uses an extra byte
      LDA $23     -- the zero page address
 */

byte memory_getZeroPage() {
    byte addressZ = memory_getImmediate();
    return memory_getZero(addressZ);
}

void memory_setZeroPage(byte value) {
    byte addressZ = memory_getImmediate();
    memory_setZero(addressZ, value);
}

/*
    Indirect Absolute Addressing
    ----------------------------
    Only used by JMP (JuMP).  It takes the given address and uses it as a pointer to the low part of a 16-bit address in memory,
    then jumps to that address.  For example,
        JMP ($2345)   -- jump to the address in $2345 low and $2346 high
            So if $2345 contains $EA and $2346 contains $12 then the next instruction executed is the one stored at $12EA.
            Remember, the 6502 puts its addresses in low/high format.
 */

word memory_getIndirectAbsoluteAddress() {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    word address = getAddress(lowByte, highByte);
    byte low2Byte = memory_get(address);
    if(lowByte == 0xFF)
        address &= 0xFF00;  /* 6502 bug: dont cross page boundary */
    else
        ++address;
    byte high2Byte = memory_get(address);
    return getAddress(low2Byte, high2Byte);
}

/*
    Absolute Indexed Addressing
    ---------------------------
    The final address is found by taking the given address as a base and adding the current value of the X or Y register to it as an offset. So when 
        LDA $F453,X  where X contains 3
            Load the accumulator with the contents of address $F453 + 3 = $F456.
 */

byte memory_getAbsoluteIndexedX() {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    word address = getAddress(lowByte, highByte) + cpu.X;
    if(getPage(address) != highByte) // crossong page boundary adds extra cycle
        ++cpu.cycles;
    return memory_get(address);
}

byte memory_getAbsoluteIndexedY() {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    word address = getAddress(lowByte, highByte) + cpu.Y;
    if(getPage(address) != highByte) // crossong page boundary adds extra cycle
        ++cpu.cycles;
    return memory_get(address);
}

void memory_setAbsoluteIndexedX(byte value) {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    word address = getAddress(lowByte, highByte) + cpu.X;
    if(getPage(address) != highByte) // crossong page boundary adds extra cycle
        ++cpu.cycles;
    memory_set(address, value);
}

void memory_setAbsoluteIndexedY(byte value) {
    byte lowByte = memory_getImmediate();
    byte highByte = memory_getImmediate();
    word address = getAddress(lowByte, highByte) + cpu.Y;
    if(getPage(address) != highByte) // crossong page boundary adds extra cycle
        ++cpu.cycles;
    memory_set(address, value);
}

/*
    Zero Page Indexed Addressing
    ----------------------------
    Same as Absolute Indexed but the given address is in the zero page thereby saving a byte of memory.
 */
 
byte memory_getZeroPageIndexedX() {
    byte addressZ = memory_getImmediate() + cpu.X;    /* don't leave zero page */
    return memory_getZero(addressZ);
}

byte memory_getZeroPageIndexedY() {
    byte addressZ = memory_getImmediate() + cpu.Y;    /* don't leave zero page */
    return memory_getZero(addressZ);
}

void memory_setZeroPageIndexedX(byte value) {
    byte addressZ = memory_getImmediate() + cpu.X;    /* don't leave zero page */
    memory_setZero(addressZ, value);
}

void memory_setZeroPageIndexedY(byte value) {
    byte addressZ = memory_getImmediate() + cpu.Y;    /* don't leave zero page */
    memory_setZero(addressZ, value);
}

/*
    Indexed Indirect Addressing
    ---------------------------
    Find the 16-bit address starting at the given location plus the current X register. The value is the contents of that address.
    For example,
        LDA ($B4,X)  where X contains 6 gives an address of $B4 + 6 = $BA.
            If $BA and $BB contain $12 and $EE respectively, then the final address is $EE12.
            The value at location $EE12 is put in the accumulator.
 */
 
byte memory_getIndexedIndirectX() {
    byte addressZ = memory_getImmediate() + cpu.X; /* don't leave zero page */
    byte lowByte = memory_getZero(addressZ++);
    byte highByte = memory_getZero(addressZ);
    word address = getAddress(lowByte, highByte);
    return memory_get(address);
}

void memory_setIndexedIndirectX(byte value) {
    byte addressZ = memory_getImmediate() + cpu.X; /* don't leave zero page */
    byte lowByte = memory_getZero(addressZ++);
    byte highByte = memory_getZero(addressZ);
    word address = getAddress(lowByte, highByte);
    memory_set(address, value);
}

/*
    Indirect Indexed Addressing
    ---------------------------
    Find the 16-bit address contained in the given location (and the one following). Add to that address the contents of the Y register.
    Fetch the value stored at that address.  For example,
        LDA ($B4),Y  where Y contains 6
            If $B4 contains $EE and $B5 contains $12 then the value at memory location $12EE + Y (6) = $12F4 is fetched and put in the accumulator.
 */
 
byte memory_getIndirectIndexedY() {
    byte addressZ = memory_getImmediate();
    byte lowByte = memory_getZero(addressZ++);
    byte highByte = memory_getZero(addressZ);
    word address = getAddress(lowByte, highByte) + cpu.Y;
    return memory_get(address);
}

void memory_setIndirectIndexedY(byte value) {
    byte addressZ = memory_getImmediate();
    byte lowByte = memory_getZero(addressZ++);
    byte highByte = memory_getZero(addressZ);
    word address = getAddress(lowByte, highByte) + cpu.Y;
    memory_set(address, value);
}

/*
    Relative Addressing
    -------------------
    The 6502 branch instructions use relative addressing.  The next byte is a signed offset from the current address, and the net sum is the
    address of the next instruction executed.  For example,
        BNE $7F   (branch on zero flag reset)
            will add 127 to the current program counter (address to execute) and start executing the instruction at that address.  SImilarly,
        BEQ $F9   (branch on zero flag set)
            will add a -7 to the current program counter and start execution at the new program counter address.

    Remember, if one treats the highest bit (bit 7) of a byte as a sign (0 = positive, 1 = negative) then it is possible to have numbers in the
    range -128 ($80) to +127 (7F).  So, if the high bit is set, i.e. the number is > $7F, it is a negative branch.  How far is the branch?  If
    the value is < $80 (positive) it is simply that many bytes.  If the value is > $7F (negative) then it is the 2's compliment of the given
    value in the negative direction. The 2's compliment of a number is found by switching all the bits from 0->1 and 1->0, then adding 1.
 */
word memory_getRelativeAddress() {
    int offset = memory_getImmediate();
    if(offset & 0x80)  /* Two's complement negative number */
        offset -= 256;
    word address = cpu.PC + offset;
    if(getPage(address) != getPage(cpu.PC)) // crossong page boundary adds extra cycle
        ++cpu.cycles;
    return address;
}

void memory_dump(word address) {
    byte page = getPage(address);
    word base = address & 0xFFF0;
    fprintf(stderr, " %04X %c%c%c",
        (unsigned int)base,
        (pageType[page] & MEM_READ)  ? 'R' : '-',
        (pageType[page] & MEM_WRITE) ? 'W' : '-',
        (pageType[page] & MEM_IO)    ? 'I' : '-');
    for(int n = 0; n < 16; ++n) {
        fprintf(stderr, " %02X", (unsigned int)memory[base+n]);
    }
    fprintf(stderr, "\n");
}