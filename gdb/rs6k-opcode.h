
typedef int (*FUN)();

typedef	struct {
  char	*operator;			/* opcode name		*/
  char	*opr_ext;			/* opcode name ext.	*/
  char	*format;			/* opcode format	*/
  char	p_opcode;			/* primary opcode	*/
  int	e_opcode;			/* extended opcode	*/
  char	oprnd_format [6];		/* operand format	*/
} OPCODE;


OPCODE rs6k_ops [] = {

#include "rs6k-opcode.def"
};

#define	NOPCODES	(sizeof (rs6k_ops) / sizeof (OPCODE))
