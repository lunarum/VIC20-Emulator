#ifndef VIC_H
#define VIC_H

#include "cpu.h"
#include "graphics.h"

/* Timings from http://www.antimon.org/dl/c64/code/stable.txt:
 * 
 *   NTSC-M systems:
 *            Chip      Crystal  Dot      Processor Cycles/ Lines/
 *   Host    ID        freq/Hz  clock/Hz clock/Hz  line    frame
 *    ------  --------  -------- -------- --------- ------- ------
 *    VIC-20  6560-101  14318181  4090909   1022727      65    261
 *    C64     6567R56A  14318181  8181818   1022727      64    262
 *    C64     6567R8    14318181  8181818   1022727      65    263
 *
 *  PAL-B systems:
 *            Chip      Crystal  Dot      Processor Cycles/ Lines/
 *    Host    ID        freq/Hz  clock/Hz clock/Hz  line    frame
 *    ------  --------  -------- -------- --------- ------- ------
 *    VIC-20  6561-101   4433618  4433618   1108405      71    312
 *    C64     6569      17734472  7881988    985248      63    312
*/

#define VIC_COUNTER_CYCLES 71

extern void vic_plot_scan_line();
extern void vic_reset();

#endif