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
break;
 logflags:
 shiftflags:
 v = 0;
 incflags:
 z = !(dst & 0xff);
 n =  dst & 0x80; 
goto next;
 next: ;
pc = npc;
#ifdef __GO32__
if (kbhit())
      exception = SIGINT;
#endif
} while (!exception);

saved_state.cycles = cycles;
saved_state.reg[PC] = pc - mem;
saved_state.reg[CCR] = GET_CCR();
}
