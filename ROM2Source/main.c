#include <stdio.h>

unsigned rom[16384];

unsigned read_ROM(const char *filename) {
    size_t l;
    unsigned length = 0;
    unsigned char *memory = (unsigned char*)rom;

    FILE *fp = fopen(filename, "rb");
    while((l = fread(memory, (size_t)1, (size_t)1024, fp)) == 1024) {
        memory += l;
        length += l;
        if (length >= 16384)
            break;
    }
    fclose(fp);
    
    printf("file %s %u bytes loaded\n", filename, length);
    return length;
}

void write_source(const char *filename, const char*variable_name, unsigned length) {
    FILE *fp = fopen(filename, "wt");
	fprintf(fp, "unsigned %s[] = {", variable_name);
    for(unsigned i = 0; i < length-4; i += 4) {
        if((i & 0b11111) == 0)
            fprintf(fp, "\n    /*0x%04X*/", i);
        fprintf(fp, " 0x%08X,", rom[i >> 2]);
    }
    fprintf(fp, " 0x%08X\n", rom[length]);
	fprintf(fp, "};\n");
    printf("file %s written\n", filename);
}

int main(int argc, char **argv)
{
    unsigned length;

    length = read_ROM("../characters.901460-03.bin");
    write_source("../../ConfigVIC-20/rom_character.c", "character_rom", length);
    length = read_ROM("../kernal.901486-07.bin");
    write_source("../../ConfigVIC-20/rom_kernal.c", "kernal_rom", length);
    length = read_ROM("../basic.901486-01.bin");
    write_source("../../ConfigVIC-20/rom_basic.c", "basic_rom", length);

	return 0;
}
