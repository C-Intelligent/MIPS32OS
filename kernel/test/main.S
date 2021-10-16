#include <asm/regdef.h>
#include <asm/cp0regdef.h>
#include <asm/asm.h>


.globl main
  
main:
   lui   $12, 0xB060    # $12 = address of LEDs
   lui   $13, 0xB0C0    # $13 = address of switches

readIO: 	
   lw    $10, 0($13)    # read switches: $10 = switch values
   sw    $10, 0($12)    # write switch values to LEDs 
   beq   $0, $0, readIO # repeat
   nop                  # branch delay slot


