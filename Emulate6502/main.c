#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "memory.h"

#define INPUT_BUFFER_LENGTH 256


long readBinary(word startAddress, const char *filename);


void readLine(char *buffer, int *length) {
    char ch, *p;
    /* skip space */
    do {
        ch = getchar();
    } while(ch == ' ' || ch == '\t');
    /* read until newline or linefeed */
    for(p = buffer; ch != 10 && ch != 13; ch = getchar()) {
        /* only add char if there is room in the buffer */
        if(p - buffer < *length)
            *(p++) = ch;
    }
    *p = 0;
    /* return characters in buffer (excluded terminating zero */
    *length = p - buffer;
}

int char2digit(char ch) {
    static char digits[] = "0123456789ABCDEF\0";
    char *p;
    if(ch >= 'a' && ch <= 'f')
        ch = ch - 'a' + 'A';
    for(p = digits; *p; ++p) {
        if(ch == *p)
            return p - digits;
    }
    return -1;
}

int parseNumber(char **buffer) {
    bool hex = false;
    int number, digit;
    char *p;
    
    for(p = *buffer; *p; ++p) {
        if(*p != ' ' && *p != '\t')
            break;
    }
    if(*p == '$') {
        hex = true;
        ++p;
    }
    for(number = 0; *p; ++p) {
        digit = char2digit(*p);
        if(digit < 0)
            break;
        if(hex) {
            number <<= 4;
        } else {
            if(digit > 9)
                break;
            number *= 10;
        }
        number += digit;
    }
 
    *buffer = p;
    return number;
}

int main_debugger(int argc, char **argv)
{
    char command[INPUT_BUFFER_LENGTH+1], *p;
    int chars;
    word address;

	printf("hello 6502 emulator world\n\n");
    memory_setPageType(0x00, 0xDF, MEM_READ | MEM_WRITE); /* RAM */
//    memory_setPageType(0x00, 0x03, MEM_READ | MEM_WRITE); /* 1K Low RAM */
//    memory_setPageType(0x10, 0x1F, MEM_READ | MEM_WRITE); /* 4K Main RAM Screen memory 0x10-0x1D (>=8K Expansion RAM) or 0x1e-0x1F (standard)*/
//    memory_setPageType(0x20, 0x3F, MEM_READ); /* 8K Expansion RAM-1 */
//    memory_setPageType(0x40, 0x5F, MEM_READ); /* 8K Expansion RAM-2 */
//    memory_setPageType(0x60, 0x7F, MEM_READ); /* 8K Expansion RAM-3 */
//    memory_setPageType(0x80, 0x8F, MEM_READ); /* 4K Character ROM */
//    memory_setPageType(0x90, 0x90, MEM_READ | MEM_WRITE | MEM_IO); /* 6561 VIC-1 I/O page */
//    memory_setPageType(0x91, 0x91, MEM_READ | MEM_WRITE | MEM_IO); /* 6522 VIA (2x) I/O page */
//    memory_setPageType(0x94, 0x95, MEM_READ | MEM_WRITE); /* 0,5K Colour RAM (>=8K Expansion RAM)*/
//    memory_setPageType(0x96, 0x97, MEM_READ | MEM_WRITE); /* 0,5K Colour RAM (standard)*/
//    memory_setPageType(0xA0, 0xBF, MEM_READ); /* Expansion ROM */
//    memory_setPageType(0xC0, 0xDF, MEM_READ); /* Basic ROM */
    memory_setPageType(0xE0, 0xFF, MEM_READ); /* Kernel ROM */
    cpu_setCycleReset(71); /* VIC-PAL screenline cycle count */
    cpu_RESET();
    for(bool run = true; run;) {
        printf(">");
        chars = INPUT_BUFFER_LENGTH;
        readLine(command, &chars);
        if(chars == 0) /* empty command */
            continue;
        p = command;
        switch(*p) {
            case 'c':
                cpu_logStatus();
                break;
            case 'd':
                ++p;
                address = (word)parseNumber(&p);
                if(address == 0)
                    address = cpu.PC;
                cpu_disassemble(address);
                break;
            case 'h':
            case '?':
                printf("6502 debugger\n"
                       "=============\n"
                       "c - CPU status\n"
                       "d [address] - disassemble 8 instructions @address (or @PC if none given)\n"
                       "l filename address - load binary file at give address and adjust PC\n"
                       "m [address] - dump memory @address (or @PC if none given)\n"
                       "r [address] - run from @address (or @PC if none given)\n"
                       "s - execute a single instruction\n\n"
                       "IRQ - execute IRQ interrupt\n"
                       "NMI - execute NMI interrupt\n"
                       "RES - execute RESET interrupy\n\n"
                       "Numbers are decimal unless preceded with a $ to indicate hexadecimal\n"
                       "All input is case sensitive except hexadecimal numbers.\n");
                break;
            case 'I':
                if(p[1] == 'R' && p[2] == 'Q') {
                    cpu_IRQ();
                    cpu_logStatus();
                }
                break;
            case 'l':
                readBinary(0x0200, "..\\Test\\and_or_xor_test.bin");
                cpu_logStatus();
                break;
            case 'm':
                ++p;
                address = (word)parseNumber(&p);
                if(address == 0)
                    address = cpu.PC;
                memory_dump(address);
                break;
            case 'N':
                if(p[1] == 'M' && p[2] == 'I') {
                    cpu_NMI();
                    cpu_logStatus();
                }
                break;
            case 'q':
                run = false;
                break;
            case 'R':
                if(p[1] == 'E' && p[2] == 'S') {
                    cpu_RESET();
                    cpu_logStatus();
                }
                break;
            case 'r':
                ++p;
                address = (word)parseNumber(&p);
                if(address != 0)
                    cpu.PC = address;
                single_step = false;
                cpu_run();
                cpu_logStatus();
                break;
            case 's':
                single_step = true;
                cpu_run();
                cpu_logStatus();
                cpu_disassemble(cpu.PC);
                break;
        }
    }

	return 0;
}

long readBinary(word startAddress, const char *filename) {
    long length = 0, l = 0;
    byte *memory = memory_get_ptr(startAddress);
    FILE *fp = fopen(filename, "rb");
    while((l = fread(memory, (size_t)1, (size_t)1024, fp)) == 1024L) {
        length += l;
        memory += l;
    }
    length += l;
    fclose(fp);
    printf("file %s %ld bytes loaded @$%04X\n", filename, length, startAddress);
    if(length > 0)
        cpu.PC = startAddress;
    return length;
}