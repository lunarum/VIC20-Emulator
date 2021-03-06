#ifndef MEMORY_H
#define MEMORY_H
/************************************************************************
 * copied from https://www.dwheeler.com/6502/oneelkruns/asm1step.html
 *
    Addressing Modes
    ----------------

   The 6502 has 13 addressing modes, or ways of accessing memory.  The 65C02
   introduces two additional modes.

   They are:
      +---------------------+--------------------------+
      |      mode           |     assembler format     |
      +=====================+==========================+
      | Immediate           |          #aa             |
      | Absolute            |          aaaa            |
      | Zero Page           |          aa              |
      | Implied             |                          |
      | Indirect Absolute   |          (aaaa)          |
      | Absolute Indexed,X  |          aaaa,X          |
      | Absolute Indexed,Y  |          aaaa,Y          |
      | Zero Page Indexed,X |          aa,X            |
      | Zero Page Indexed,Y |          aa,Y            |
      | Indexed Indirect    |          (aa,X)          |
      | Indirect Indexed    |          (aa),Y          |
      | Relative            |          aaaa            |
      | Accumulator         |          A               |
      +---------------------+--------------------------+
      (Table 2-3. _6502 Software Design_, Scanlon, 1980)

 ************************************************************************/

typedef enum { false = 0, true = !false } bool;

#define MEM_PAGE_SIZE  0xFF
#define MEM_MAX_PAGES  0xFF

enum memory_type {
        MEM_READ  = 0x1,
        MEM_WRITE = 0x2,
        MEM_IO    = 0x4,
};

enum io_type {
        IO_VIC    = 0x10,
        IO_VIA    = 0x20
};

typedef unsigned char byte;
typedef unsigned int word;

static inline byte getPage(word address) { return address >> 8; }
static inline byte getIndex(word address) { return address & 0xFF; }
static inline word getAddress(byte lowByte, byte highByte) { return ((word)highByte) << 8 | lowByte; }

extern byte *memory_get_ptr(word startAddress);
extern word memory_get_vector(word vectorAddress);

extern void memory_setPageType(byte startPage, byte endPage, byte memoryType);
extern byte memory_get(word address);
extern void memory_set(word address, byte value);
extern void memory_setLast(byte value);
extern byte memory_getZero(byte addressZ);
extern void memory_setZero(byte addressZ, byte value);

extern byte memory_stackPull();
extern void memory_stackPush(byte value);
extern word memory_stackPullAddress();
extern void memory_stackPushAddress(word address);

extern byte memory_getImmediate();
extern word memory_getAbsoluteAddress();
extern byte memory_getAbsolute();
extern void memory_setAbsolute(byte value);
extern byte memory_getZeroPage();
extern void memory_setZeroPage(byte value);
extern word memory_getIndirectAbsoluteAddress();
extern byte memory_getAbsoluteIndexedX();
extern byte memory_getAbsoluteIndexedY();
extern void memory_setAbsoluteIndexedX(byte value);
extern void memory_setAbsoluteIndexedY(byte value);
extern byte memory_getZeroPageIndexedX();
extern byte memory_getZeroPageIndexedY();
extern void memory_setZeroPageIndexedX(byte value);
extern void memory_setZeroPageIndexedY(byte value);
extern byte memory_getIndexedIndirectX();
extern void memory_setIndexedIndirectX(byte value);
extern void memory_setIndirectIndexedY(byte value);
extern byte memory_getIndirectIndexedY();
extern word memory_getRelativeAddress();

extern void memory_dump(word address);

extern byte vic_read_register(word address);
extern void vic_write_register(word address, byte value);
extern byte via_read_register(word address);
extern void via_write_register(word address, byte value);

#endif