#include <stdio.h>
#include "defs.h"

/* an easy to debug H8 stack frame looks like:
0x6df2	push	r2
0x6df3	push	r3
0x6df6	push	r6
0x	mov.w	r7,r6
	subs	stuff,sp  mov.w #x,r5
	                  subs  r5,sp

 */
#define IS_PUSH(x) ((x & 0xff00)==0x6d00)
CORE_ADDR h8300_skip_prologue(start_pc)
CORE_ADDR start_pc;

{
  /* Skip past all push insns */
  short int w;
  
  w = read_memory_integer(start_pc, 2);
  while (IS_PUSH(w)) 
  {
    start_pc+=2;  
    w = read_memory_integer(start_pc, 2);
  }
return start_pc;  
  
}

h8300_pop_frame()
{
  printf("pop frame\n");
  
}

int
print_insn(memaddr, stream)
CORE_ADDR memaddr;
FILE *stream;
{
  /* Nothing is bigger than 8 bytes */
  char   data[8];
  read_memory (memaddr, data, sizeof(data));
  return print_insn_h8300(memaddr,  data   , stream);
     
     
}
     
     
     
     FRAME_CHAIN()
     {
     
     printf("Frame chain\n");
     
     }
     
     
