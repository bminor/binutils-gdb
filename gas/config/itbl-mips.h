
/* Defines for Mips itbl cop support */

#include "opcode/mips.h"

/* Values for processors will be from 0 to NUMBER_OF_PROCESSORS-1 */
#define NUMBER_OF_PROCESSORS 4
#define MAX_BITPOS 31

/* Mips specifics */
#define MIPS_OPCODE_COP0 (0x21)	/* COPz+CO, bits 31-25: 0100zz1 */
#define MIPS_ENCODE_COP_NUM(z) ((MIPS_OPCODE_COP0|z<<1)<<25)
#define MIPS_IS_COP_INSN(insn) ((MIPS_OPCODE_COP0&(insn>>25)) \
	== MIPS_OPCODE_COP0)
#define MIPS_DECODE_COP_NUM(insn) ((~MIPS_OPCODE_COP0&(insn>>25))>>1)
#define MIPS_DECODE_COP_COFUN(insn) ((~MIPS_ENCODE_COP_NUM(3))&(insn))

/* definitions required by generic code */ 
#define ITBL_IS_INSN(insn) MIPS_IS_COP_INSN(insn)
#define ITBL_DECODE_PNUM(insn) MIPS_DECODE_COP_NUM(insn)
#define ITBL_ENCODE_PNUM(pnum) MIPS_ENCODE_COP_NUM(pnum)

#define ITBL_OPCODE_STRUCT mips_opcode
#define ITBL_OPCODES mips_opcodes
#define ITBL_NUM_OPCODES NUMOPCODES
#define ITBL_NUM_MACROS M_NUM_MACROS

