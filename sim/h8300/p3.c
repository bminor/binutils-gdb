/* Interpreter fragment for the Hitachi H8/300 architecture simulator.

   Written by Steve Chamberlain of Cygnus Support.
   sac@cygnus.com

   This file is part of H8/300 sim


		THIS SOFTWARE IS NOT COPYRIGHTED

   Cygnus offers the following for use in the public domain.  Cygnus
   makes no warranty with regard to the software or it's performance
   and the user accepts the software "AS IS" with all faults.

   CYGNUS DISCLAIMS ANY WARRANTIES, EXPRESS OR IMPLIED, WITH REGARD TO
   THIS SOFTWARE INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

*/

 movflags8:
 n = dst & 0x80;
 z = !(dst & 0xff);
 v = 0;
goto next;
 movflags16:
 n = dst & 0x8000;
 z = !(dst & 0xffff);
 v = 0;
goto next;
 aluflags8:
 n = dst & 0x80;
 z = !(dst & 0xff);
 v = ((srca & 0x80) == (srcb & 0x80)) && ((srca & 0x80) != (dst & 0x80));
 c = dst & 0x100;
 goto next;
 aluflags16:
 n = dst & 0x8000;
 z = !(dst & 0xffff);
 v = ((srca & 0x8000) == (srcb & 0x8000)) && ((srca & 0x8000) != (dst & 0x8000));
 c = dst & 0x10000;
 goto next;
 setflags:;
SET_CCR(tmp);
 goto next;
 logflags:
 shiftflags:
 v = 0;
 incflags:
 z = !(dst & 0xff);
 n =  dst & 0x80; 
goto next;
 next: ;
pc = npc;
if (ni > checkfreq) 
{
  ni = 0;
  SAVE_INTERPRETER_STATE();
  perifs();
  LOAD_INTERPRETER_STATE();
#ifdef __GO32__
  if (kbhit())
   saved_state.exception = SIGINT;
#endif
}
ni++;
} while (!saved_state.exception);


SAVE_INTERPRETER_STATE();
}

  
sim_info()
{
  int cycles = saved_state.reg[CYCLES];
  printf("cycles (v approximate) %10d\n", cycles);
}
