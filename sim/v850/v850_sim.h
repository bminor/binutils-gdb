struct simops 
{
  long   opcode;
  long   mask;
  int (* func) PARAMS ((void));
  int    numops;
  int    operands[12];
};
