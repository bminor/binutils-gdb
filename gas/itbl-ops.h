
/* External functions, constants and defines for itbl support */

#include "itbl-cpu.h"

/* Defaults for definitions required by generic code */ 
#ifndef ITBL_NUMBER_OF_PROCESSORS
#define ITBL_NUMBER_OF_PROCESSORS 1
#endif

#ifndef ITBL_MAX_BITPOS 
#define ITBL_MAX_BITPOS 31
#endif

#ifndef ITBL_TYPE 
#define ITBL_TYPE unsigned long
#endif

#ifndef ITBL_IS_INSN
#define ITBL_IS_INSN(insn) 1
#endif

#ifndef ITBL_DECODE_PNUM
#define ITBL_DECODE_PNUM(insn) 0
#endif

#ifndef ITBL_ENCODE_PNUM
#define ITBL_ENCODE_PNUM(pnum) 0
#endif

typedef ITBL_TYPE t_insn;

/* types of entries */
typedef enum 
{
    e_insn,
    e_dreg,
	e_regtype0 = e_dreg,
    e_creg,
    e_greg,
    e_addr,
	e_nregtypes = e_greg+1,
    e_immed,
    e_ntypes,
    e_invtype	/* invalid type */
} e_type;

typedef enum 
{
    e_p0,
    e_nprocs=NUMBER_OF_PROCESSORS,
    e_invproc	/* invalid processor */
} e_processor;

/* These routines are visible to the main part of the assembler */

int itbl_parse(char* insntbl);
void itbl_init(void);
char *itbl_get_field(char **s);
unsigned long itbl_assemble(char *name, char *operands);
int itbl_disassemble(char *str, unsigned long insn);
int itbl_parse(char *tbl); /* parses insn tbl */
unsigned long itbl_get_reg_val(char *name);
unsigned long itbl_get_val(e_processor processor, e_type type, char *name);
char *itbl_get_name(e_processor processor, e_type type, unsigned long val);

/* These routines are called by the table parser used to build the 
 * dynamic list of new processor instructions and registers. */

struct itbl_entry *itbl_add_reg(int yyproc, int yytype, char *regname, int regnum);
struct itbl_entry *itbl_add_insn(int yyproc, char *name, unsigned long value, 
	int sbit, int ebit, unsigned long flags);
struct itbl_field *itbl_add_operand(struct itbl_entry *e, int yytype, 
	int sbit, int ebit, unsigned long flags);

