#include "v850_sim.h"

static void write_header PARAMS ((void));
static void write_opcodes PARAMS ((void));
static void write_template PARAMS ((void));

int
main (argc, argv)
     int argc;
     char *argv[];
{
  if ((argc > 1) && (strcmp (argv[1],"-h") == 0))
    write_header();
  else if ((argc > 1) && (strcmp (argv[1],"-t") == 0))
    write_template ();
  else
    write_opcodes();
  return 0;
}


static void
write_header ()
{
  struct v850_opcode *opcode;

  for (opcode = (struct v850_opcode *)v850_opcodes; opcode->name; opcode++)
    printf("void OP_%X PARAMS ((void));\t\t/* %s */\n",opcode->opcode, opcode->name);
}


/* write_template creates a file all required functions, ready */
/* to be filled out */

static void
write_template ()
{
  struct v850_opcode *opcode;
  int i,j;

  printf ("#include \"v850_sim.h\"\n");
  printf ("#include \"simops.h\"\n");

  for (opcode = (struct v850_opcode *)v850_opcodes; opcode->name; opcode++)
    {
      printf("/* %s */\nvoid\nOP_%X ()\n{\n",opcode->name,opcode->opcode);
	  
      /* count operands */
      j = 0;
      for (i = 0; i < 6; i++)
	{
	  int flags = v850_operands[opcode->operands[i]].flags;

	  if (flags & (V850_OPERAND_REG | V850_OPERAND_SRG | V850_OPERAND_CC))
	    j++;
	}
      switch (j)
	{
	case 0:
	  printf ("printf(\"   %s\\n\");\n",opcode->name);
	  break;
	case 1:
	  printf ("printf(\"   %s\\t%%x\\n\",OP[0]);\n",opcode->name);
	  break;
	case 2:
	  printf ("printf(\"   %s\\t%%x,%%x\\n\",OP[0],OP[1]);\n",opcode->name);
	  break;
	case 3:
	  printf ("printf(\"   %s\\t%%x,%%x,%%x\\n\",OP[0],OP[1],OP[2]);\n",opcode->name);
	  break;
	default:
	  fprintf (stderr,"Too many operands: %d\n",j);
	}
      printf ("}\n\n");
    }
}


long Opcodes[512];
static int curop=0;

check_opcodes( long op)
{
  int i;

  for (i=0;i<curop;i++)
    if (Opcodes[i] == op)
      fprintf(stderr,"DUPLICATE OPCODES: %x\n",op);
}


static void
write_opcodes ()
{
  struct v850_opcode *opcode;
  int i, j;
  
  /* write out opcode table */
  printf ("#include \"v850_sim.h\"\n");
  printf ("#include \"simops.h\"\n\n");
  printf ("struct simops Simops[] = {\n");
  
  for (opcode = (struct v850_opcode *)v850_opcodes; opcode->name; opcode++)
    {
      printf ("  { %ld,%ld,OP_%X,",
	      opcode->opcode, opcode->mask, opcode->opcode);
      
      /* REMOVE ME */
      check_opcodes (opcode->opcode);
      Opcodes[curop++] = opcode->opcode;

      /* count operands */
      j = 0;
      for (i = 0; i < 6; i++)
	{
	  int flags = v850_operands[opcode->operands[i]].flags;

	  if (flags & (V850_OPERAND_REG | V850_OPERAND_SRG | V850_OPERAND_CC))
	    j++;
	}
      printf ("%d,{",j);
	  
      j = 0;
      for (i = 0; i < 6; i++)
	{
	  int flags = v850_operands[opcode->operands[i]].flags;
	  int shift = v850_operands[opcode->operands[i]].shift;

	  if (flags & (V850_OPERAND_REG | V850_OPERAND_SRG | V850_OPERAND_CC))
	    {
	      if (j)
		printf (", ");
#if 0
	      if ((flags & OPERAND_REG) && (opcode->format == LONG_L))
		shift += 15;
#endif
	      printf ("%d,%d,%d",shift,v850_operands[opcode->operands[i]].bits,flags);
	      j = 1;
	    }
	}
      printf ("}},\n");
    }
  printf ("{ 0,0,NULL,0,{ }},\n};\n");
}
