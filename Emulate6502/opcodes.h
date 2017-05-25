#ifndef OPCODES_H
#define OPCODES_H

enum access_type {
    AM_IMM, /* Immediate           	#aa    */
    AM_ABS, /* Absolute            	aaaa   */
    AM_ZPG, /* Zeo Page           	aa     */
    AM_IMP, /* Implied             	       */
    AM_IND, /* Indirect Absolute   	(aaaa) */
    AM_AIX, /* Absolute Indexed,X  	aaaa,X */
    AM_AIY, /* Absolute Indexed,Y  	aaaa,Y */
    AM_ZIX, /* Zero Page Indexed,X 	aa,X   */
    AM_ZIY, /* Zero Page Indexed,Y 	aa,Y   */
    AM_IIX, /* Indexed Indirect    	(aa,X) */
    AM_IIY, /* Indirect Indexed    	(aa),Y */
    AM_REL /* Relative            	aaaa   */
};

struct {
    char *mnemonic;
    bool illegal;
    enum access_type access;
    byte cycles;
    byte page_switch_cycles;
} opcodes[] = {
    {"BRK", false, AM_IMP, 7, 0},
    {"ORA", false, AM_ZIX, 6, 0},
    {"KIL", true, AM_IMP, 0, 0},
    {"SLO", true, AM_ZIX, 8, 0},
    {"NOP", true, AM_ZPG, 3, 0},
    {"ORA", false, AM_ZPG, 3, 0},
    {"ASL", false, AM_ZPG, 5, 0},
    {"SLO", true, AM_ZPG, 5, 0},
    {"PHP", false, AM_IMP, 3, 0},
    {"ORA", false, AM_IMM, 2, 0},
    {"ASL", false, AM_IMP, 2, 0},
    {"ANC", true, AM_IMM, 2, 0},
    {"NOP", true, AM_ABS, 4, 0},
    {"ORA", false, AM_ABS, 4, 0},
    {"ASL", false, AM_ABS, 6, 0},
    {"SLO", true, AM_ABS, 6, 0},
    {"BPL", false, AM_REL, 2, 1},
    {"ORA", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"SLO", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"ORA", false, AM_IIX, 4, 0},
    {"ASL", false, AM_IIX, 6, 0},
    {"SLO", true, AM_IIX, 6, 0},
    {"CLC", false, AM_IMP, 2, 0},
    {"ORA", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"SLO", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"ORA", false, AM_AIX, 4, 1},
    {"ASL", false, AM_AIX, 7, 0},
    {"SLO", true, AM_AIX, 7, 0},
    {"JSR", false, AM_ABS, 6, 0},
    {"AND", false, AM_ZIX, 6, 0},
    {"KIL", true, AM_IMP, 0, 0},
    {"RLA", true, AM_ZIX, 8, 0},
    {"BIT", false, AM_ZPG, 3, 0},
    {"AND", false, AM_ZPG, 3, 0},
    {"ROL", false, AM_ZPG, 5, 0},
    {"RLA", true, AM_ZPG, 5, 0},
    {"PLP", false, AM_IMP, 4, 0},
    {"AND", false, AM_IMM, 2, 0},
    {"ROL", false, AM_IMP, 2, 0},
    {"ANC", true, AM_IMM, 2, 0},
    {"BIT", false, AM_ABS, 4, 0},
    {"AND", false, AM_ABS, 4, 0},
    {"ROL", false, AM_ABS, 6, 0},
    {"RLA", true, AM_ABS, 6, 0},
    {"BMI", false, AM_REL, 2, 1},
    {"AND", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"RLA", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"AND", false, AM_IIX, 4, 0},
    {"ROL", false, AM_IIX, 6, 0},
    {"RLA", true, AM_IIX, 6, 0},
    {"SEC", false, AM_IMP, 2, 0},
    {"AND", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"RLA", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"AND", false, AM_AIX, 4, 1},
    {"ROL", false, AM_AIX, 7, 0},
    {"RLA", true, AM_AIX, 7, 0},
    {"RTI", false, AM_IMP, 6, 0},
    {"EOR", false, AM_ZIX, 6, 0},
    {"KIL", true, AM_IMP, 0, 0},
    {"SRE", true, AM_ZIX, 8, 0},
    {"NOP", true, AM_ZPG, 3, 0},
    {"EOR", false, AM_ZPG, 3, 0},
    {"LSR", false, AM_ZPG, 5, 0},
    {"SRE", true, AM_ZPG, 5, 0},
    {"PHA", false, AM_IMP, 3, 0},
    {"EOR", false, AM_IMM, 2, 0},
    {"LSR", false, AM_IMP, 2, 0},
    {"ALR", true, AM_IMM, 2, 0},
    {"JMP", false, AM_ABS, 3, 0},
    {"EOR", false, AM_ABS, 4, 0},
    {"LSR", false, AM_ABS, 6, 0},
    {"SRE", true, AM_ABS, 6, 0},
    {"BVC", false, AM_REL, 2, 1},
    {"EOR", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"SRE", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"EOR", false, AM_IIX, 4, 0},
    {"LSR", false, AM_IIX, 6, 0},
    {"SRE", true, AM_IIX, 6, 0},
    {"CLI", false, AM_IMP, 2, 0},
    {"EOR", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"SRE", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"EOR", false, AM_AIX, 4, 1},
    {"LSR", false, AM_AIX, 7, 0},
    {"SRE", true, AM_AIX, 7, 0},
    {"RTS", false, AM_IMP, 6, 0},
    {"ADC", false, AM_ZIX, 6, 0},
    {"KIL", true, AM_IMP, 0, 0},
    {"RRA", true, AM_ZIX, 8, 0},
    {"NOP", true, AM_ZPG, 3, 0},
    {"ADC", false, AM_ZPG, 3, 0},
    {"ROR", false, AM_ZPG, 5, 0},
    {"RRA", true, AM_ZPG, 5, 0},
    {"PLA", false, AM_IMP, 4, 0},
    {"ADC", false, AM_IMM, 2, 0},
    {"ROR", false, AM_IMP, 2, 0},
    {"ARR", true, AM_IMM, 2, 0},
    {"JMP", false, AM_IND, 5, 0},
    {"ADC", false, AM_ABS, 4, 0},
    {"ROR", false, AM_ABS, 6, 0},
    {"RRA", true, AM_ABS, 6, 0},
    {"BVS", false, AM_REL, 2, 1},
    {"ADC", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"RRA", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"ADC", false, AM_IIX, 4, 0},
    {"ROR", false, AM_IIX, 6, 0},
    {"RRA", true, AM_IIX, 6, 0},
    {"SEI", false, AM_IMP, 2, 0},
    {"ADC", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"RRA", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"ADC", false, AM_AIX, 4, 1},
    {"ROR", false, AM_AIX, 7, 0},
    {"RRA", true, AM_AIX, 7, 0},
    {"NOP", true, AM_IMM, 2, 0},
    {"STA", false, AM_ZIX, 6, 0},
    {"NOP", true, AM_IMM, 2, 0},
    {"SAX", true, AM_ZIX, 6, 0},
    {"STY", false, AM_ZPG, 3, 0},
    {"STA", false, AM_ZPG, 3, 0},
    {"STX", false, AM_ZPG, 3, 0},
    {"SAX", true, AM_ZPG, 3, 0},
    {"DEY", false, AM_IMP, 2, 0},
    {"NOP", true, AM_IMM, 2, 0},
    {"TXA", false, AM_IMP, 2, 0},
    {"XAA", true, AM_IMM, 2, 0},
    {"STY", false, AM_ABS, 4, 0},
    {"STA", false, AM_ABS, 4, 0},
    {"STX", false, AM_ABS, 4, 0},
    {"SAX", true, AM_ABS, 4, 0},
    {"BCC", false, AM_REL, 2, 1},
    {"STA", false, AM_ZIY, 6, 0},
    {"KIL", true, AM_IMP, 0, 0},
    {"AHX", true, AM_ZIY, 6, 0},
    {"STY", false, AM_IIX, 4, 0},
    {"STA", false, AM_IIX, 4, 0},
    {"STX", false, AM_IIY, 4, 0},
    {"SAX", true, AM_IIY, 4, 0},
    {"TYA", false, AM_IMP, 2, 0},
    {"STA", false, AM_AIY, 5, 0},
    {"TXS", false, AM_IMP, 2, 0},
    {"TAS", true, AM_AIY, 5, 0},
    {"SHY", true, AM_AIX, 5, 0},
    {"STA", false, AM_AIX, 5, 0},
    {"SHX", true, AM_AIY, 5, 0},
    {"AHX", true, AM_AIY, 5, 0},
    {"LDY", false, AM_IMM, 2, 0},
    {"LDA", false, AM_ZIX, 6, 0},
    {"LDX", false, AM_IMM, 2, 0},
    {"LAX", true, AM_ZIX, 6, 0},
    {"LDY", false, AM_ZPG, 3, 0},
    {"LDA", false, AM_ZPG, 3, 0},
    {"LDX", false, AM_ZPG, 3, 0},
    {"LAX", true, AM_ZPG, 3, 0},
    {"TAY", false, AM_IMP, 2, 0},
    {"LDA", false, AM_IMM, 2, 0},
    {"TAX", false, AM_IMP, 2, 0},
    {"LAX", true, AM_IMM, 2, 0},
    {"LDY", false, AM_ABS, 4, 0},
    {"LDA", false, AM_ABS, 4, 0},
    {"LDX", false, AM_ABS, 4, 0},
    {"LAX", true, AM_ABS, 4, 0},
    {"BCS", false, AM_REL, 2, 1},
    {"LDA", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"LAX", true, AM_ZIY, 5, 1},
    {"LDY", false, AM_IIX, 4, 0},
    {"LDA", false, AM_IIX, 4, 0},
    {"LDX", false, AM_IIY, 4, 0},
    {"LAX", true, AM_IIY, 4, 0},
    {"CLV", false, AM_IMP, 2, 0},
    {"LDA", false, AM_AIY, 4, 1},
    {"TSX", false, AM_IMP, 2, 0},
    {"LAS", true, AM_AIY, 4, 1},
    {"LDY", false, AM_AIX, 4, 1},
    {"LDA", false, AM_AIX, 4, 1},
    {"LDX", false, AM_AIY, 4, 1},
    {"LAX", true, AM_AIY, 4, 1},
    {"CPY", false, AM_IMM, 2, 0},
    {"CMP", false, AM_ZIX, 6, 0},
    {"NOP", true, AM_IMM, 2, 0},
    {"DCP", true, AM_ZIX, 8, 0},
    {"CPY", false, AM_ZPG, 3, 0},
    {"CMP", false, AM_ZPG, 3, 0},
    {"DEC", false, AM_ZPG, 5, 0},
    {"DCP", true, AM_ZPG, 5, 0},
    {"INY", false, AM_IMP, 2, 0},
    {"CMP", false, AM_IMM, 2, 0},
    {"DEX", false, AM_IMP, 2, 0},
    {"AXS", true, AM_IMM, 2, 0},
    {"CPY", false, AM_ABS, 4, 0},
    {"CMP", false, AM_ABS, 4, 0},
    {"DEC", false, AM_ABS, 6, 0},
    {"DCP", true, AM_ABS, 6, 0},
    {"BNE", false, AM_REL, 2, 1},
    {"CMP", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"DCP", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"CMP", false, AM_IIX, 4, 0},
    {"DEC", false, AM_IIX, 6, 0},
    {"DCP", true, AM_IIX, 6, 0},
    {"CLD", false, AM_IMP, 2, 0},
    {"CMP", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"DCP", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"CMP", false, AM_AIX, 4, 1},
    {"DEC", false, AM_AIX, 7, 0},
    {"DCP", true, AM_AIX, 7, 0},
    {"CPX", false, AM_IMM, 2, 0},
    {"SBC", false, AM_ZIX, 6, 0},
    {"NOP", true, AM_IMM, 2, 0},
    {"ISC", true, AM_ZIX, 8, 0},
    {"CPX", false, AM_ZPG, 3, 0},
    {"SBC", false, AM_ZPG, 3, 0},
    {"INC", false, AM_ZPG, 5, 0},
    {"ISC", true, AM_ZPG, 5, 0},
    {"INX", false, AM_IMP, 2, 0},
    {"SBC", false, AM_IMM, 2, 0},
    {"NOP", false, AM_IMP, 2, 0},
    {"SBC", true, AM_IMM, 2, 0},
    {"CPX", false, AM_ABS, 4, 0},
    {"SBC", false, AM_ABS, 4, 0},
    {"INC", false, AM_ABS, 6, 0},
    {"ISC", true, AM_ABS, 6, 0},
    {"BEQ", false, AM_REL, 2, 1},
    {"SBC", false, AM_ZIY, 5, 1},
    {"KIL", true, AM_IMP, 0, 0},
    {"ISC", true, AM_ZIY, 8, 0},
    {"NOP", true, AM_IIX, 4, 0},
    {"SBC", false, AM_IIX, 4, 0},
    {"INC", false, AM_IIX, 6, 0},
    {"ISC", true, AM_IIX, 6, 0},
    {"SED", false, AM_IMP, 2, 0},
    {"SBC", false, AM_AIY, 4, 1},
    {"NOP", true, AM_IMP, 2, 0},
    {"ISC", true, AM_AIY, 7, 0},
    {"NOP", true, AM_AIX, 4, 1},
    {"SBC", false, AM_AIX, 4, 1},
    {"INC", false, AM_AIX, 7, 0},
    {"ISC", true, AM_AIX, 7, 0}
};

#endif