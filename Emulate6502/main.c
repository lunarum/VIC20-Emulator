#include <stdio.h>
#include "cpu.h"


int main(int argc, char **argv)
{
	printf("hello world\n");
    cpu my_cpu;
    my_cpu.A = 0;
    my_cpu.X = 0;
    my_cpu.Y = 0;
    my_cpu.PC = 0;
    my_cpu.SP = 0xFF;
	return 0;
}
