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
