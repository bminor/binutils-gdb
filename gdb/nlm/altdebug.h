typedef short int16_t;
typedef unsigned short uint16_t;

typedef int int32_t;
typedef unsigned int uint32_t;

typedef struct int64_s {
    uint32_t   lo;
    uint32_t   hi;
} int64_t;

/* Define indexes into register array in StackFrame */
#define SF_REG_PC	0

#define SF_IREG_OFFSET  1	/* int regs start at index 1 */
#define SF_REG_V0      (0+SF_IREG_OFFSET)
#define SF_REG_T0	(1+SF_IREG_OFFSET)
#define SF_REG_T1	(2+SF_IREG_OFFSET)
#define SF_REG_T2	(3+SF_IREG_OFFSET)
#define SF_REG_T3	(4+SF_IREG_OFFSET)
#define SF_REG_T4	(5+SF_IREG_OFFSET)
#define SF_REG_T5	(6+SF_IREG_OFFSET)
#define SF_REG_T6	(7+SF_IREG_OFFSET)
#define SF_REG_T7	(8+SF_IREG_OFFSET)
#define SF_REG_S0	(9+SF_IREG_OFFSET)
#define SF_REG_S1	(10+SF_IREG_OFFSET)
#define SF_REG_S2	(11+SF_IREG_OFFSET)
#define SF_REG_S3	(12+SF_IREG_OFFSET)
#define SF_REG_S4	(13+SF_IREG_OFFSET)
#define SF_REG_S5	(14+SF_IREG_OFFSET)
#define SF_REG_S6	(15+SF_IREG_OFFSET)
#define SF_REG_FP	SF_REG_S6
#define SF_REG_A0	(16+SF_IREG_OFFSET)
#define SF_REG_A1	(17+SF_IREG_OFFSET)
#define SF_REG_A2	(18+SF_IREG_OFFSET)
#define SF_REG_A3	(19+SF_IREG_OFFSET)
#define SF_REG_A4	(20+SF_IREG_OFFSET)
#define SF_REG_A5	(21+SF_IREG_OFFSET)
#define SF_REG_T8	(22+SF_IREG_OFFSET)
#define SF_REG_T9	(23+SF_IREG_OFFSET)
#define SF_REG_T10	(24+SF_IREG_OFFSET)
#define SF_REG_T11	(25+SF_IREG_OFFSET)
#define SF_REG_RA	(26+SF_IREG_OFFSET)
#define SF_REG_PV	(27+SF_IREG_OFFSET)
#define SF_REG_T12	SF_REG_PV
#define SF_REG_AT	(28+SF_IREG_OFFSET)
#define SF_REG_GP	(29+SF_IREG_OFFSET)
#define SF_REG_SP	(30+SF_IREG_OFFSET)
#define SF_REG_ZERO	(31+SF_IREG_OFFSET)
#define NUMBER_OF_INT_REGS 	32

#define SF_FREG_OFFSET  (SF_IREG_OFFSET+NUMBER_OF_INT_REGS)
#define SF_REG_F0	(0+SF_FREG_OFFSET)
#define SF_REG_F1	(1+SF_FREG_OFFSET)
#define SF_REG_F2	(2+SF_FREG_OFFSET)
#define SF_REG_F3	(3+SF_FREG_OFFSET)
#define SF_REG_F4	(4+SF_FREG_OFFSET)
#define SF_REG_F5	(5+SF_FREG_OFFSET)
#define SF_REG_F6	(6+SF_FREG_OFFSET)
#define SF_REG_F7	(7+SF_FREG_OFFSET)
#define SF_REG_F8	(8+SF_FREG_OFFSET)
#define SF_REG_F9	(9+SF_FREG_OFFSET)
#define SF_REG_F10	(10+SF_FREG_OFFSET)
#define SF_REG_F11	(11+SF_FREG_OFFSET)
#define SF_REG_F12	(12+SF_FREG_OFFSET)
#define SF_REG_F13	(13+SF_FREG_OFFSET)
#define SF_REG_F14	(14+SF_FREG_OFFSET)
#define SF_REG_F15	(15+SF_FREG_OFFSET)
#define SF_REG_F16	(16+SF_FREG_OFFSET)
#define SF_REG_F17	(17+SF_FREG_OFFSET)
#define SF_REG_F18	(18+SF_FREG_OFFSET)
#define SF_REG_F19	(19+SF_FREG_OFFSET)
#define SF_REG_F20	(20+SF_FREG_OFFSET)
#define SF_REG_F21	(21+SF_FREG_OFFSET)
#define SF_REG_F22	(22+SF_FREG_OFFSET)
#define SF_REG_F23	(23+SF_FREG_OFFSET)
#define SF_REG_F24	(24+SF_FREG_OFFSET)
#define SF_REG_F25	(25+SF_FREG_OFFSET)
#define SF_REG_F26	(26+SF_FREG_OFFSET)
#define SF_REG_F27	(27+SF_FREG_OFFSET)
#define SF_REG_F28	(28+SF_FREG_OFFSET)
#define SF_REG_F29	(29+SF_FREG_OFFSET)
#define SF_REG_F30	(30+SF_FREG_OFFSET)
#define SF_REG_FZERO	(31+SF_FREG_OFFSET)

#define NUMBER_OF_FLOAT_REGS	32

struct StackFrame
{
    struct DomainStructure *ExceptionDomainID;
    struct ProcessorStructure *ExceptionProcessorID;
    BYTE *ExceptionDescription;
    LONG ExceptionFlags;
    LONG ExceptionErrorCode;
    LONG ExceptionNumber;
    
    int64_t ExceptionAddress; /* Address at which exception occured */
    int64_t ExceptionRegs[1+NUMBER_OF_INT_REGS+NUMBER_OF_FLOAT_REGS];
				/* First entry is for PC */
    int  ExceptionBrkptNum;	/* Set by the NSI debug exception handler
				   if the exception was caused by a
				   NID breakpoint */
    int  ExceptionBrkptFlags;	/* Flags for the breakpoint. See nsibrkpt.c
				   for details */
};
