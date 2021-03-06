#include <stdio.h>
#include <stdlib.h>
#include "VIC-20.h"

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
    cpu_result result;

	printf("hello 6502 emulator world\n\n");
    // Default 5K VIC-20
    vic20_config(0);

    for(bool run = true; run;) {
        printf(">");
        chars = INPUT_BUFFER_LENGTH;
        readLine(command, &chars);
        if(chars == 0) /* empty command */
            continue;
        p = command;
        switch(*p) {
            case 'c':
                cpu_logStatus(RESULT_STEP);
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
                    cpu_logStatus(RESULT_IRQ);
                }
                break;
            case 'l':
                readBinary(0x0200, "..\\Test\\and_or_xor_test.bin");
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
                    cpu_logStatus(RESULT_NMI);
                }
                break;
            case 'q':
                run = false;
                break;
            case 'R':
                if(p[1] == 'E' && p[2] == 'S') {
                    vic20_config(0);
                    cpu_logStatus(RESULT_RESET);
                }
                break;
            case 'r':
                ++p;
                address = (word)parseNumber(&p);
                if(address != 0)
                    cpu.PC = address;
                single_step = false;
                result = cpu_run();
                cpu_logStatus(result);
                break;
            case 's':
                single_step = true;
                result = cpu_run();
                cpu_logStatus(result);
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