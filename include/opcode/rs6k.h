/* IBM RS/6000 instruction set definitions, for GNU software.  */

typedef struct rs6000_insn {
  char	*operator;			/* opcode name		*/
  char	*opr_ext;			/* opcode name extension */
  char	*format;			/* opcode format	*/
  char	p_opcode;			/* primary opcode	*/
  int	e_opcode;			/* extended opcode	*/
  char	oprnd_format[6];		/* operand format	*/
} OPCODE;

/* operand format specifiers */

#define	TO	1
#define	RA	2
#define	SI	3
#define	RT	4
#define	UI	5
#define	BF	6
#define	BFA	7
#define	BT	8
#define	BA	9
#define	BB	10
#define	BO	11
#define	BI	12
#define	RB	13
#define	RS	14
#define	SH	15
#define	MB	16
#define	ME	17
#define	SPR	18
#define	DIS	19
#define	FXM	21
#define	FRT	22
#define	NB	23
#define	FRS	24
#define	FRA	25
#define	FRB	26
#define	FRC	27
#define	FLM	28
#define	I	29
#define	LI	30
#define	A2	31
#define	TA14	32		/* 14 bit representation of target address */
#define	TA24	33		/* 24 bit representation of target address */
#define	FL1	34
#define	FL2	35
#define	LEV	36

/*	RS/6000 INSTRUCTION SET
    (sorted on primary and extended opcode)

	     oprtr	      primary  ext.
operator      ext     format  opcode   opcode   operand format
-------	    -------   ------  -------  ------   ---------------   */

struct rs6000_insn rs6k_ops [] = {

{"ti",		0,	"d",	3,	-1,	{TO,RA,SI,0}	},
{"muli",	0,	"d",	7,	-1,	{RT,RA,SI,0}	},
{"sfi",		0,	"d",	8,	-1,	{RT,RA,SI,0}	},
{"dozi",	0,	"d",	9,	-1,	{RT,RA,SI,0}	},
{"cmpli",	0,	"d",	10,	-1,	{BF,RA,UI,0}	},
{"cmpi",	0,	"d",	11,	-1,	{BF,RA,SI,0}	},
{"ai",		0,	"d",	12,	-1,	{RT,RA,SI,0}	},
{"ai.",		0,	"d",	13,	-1,	{RT,RA,SI,0}	},
{"lil",		0,	"d",	14,	-1,	{RT,SI,0}	}, /* same as `cal' */
{"cal",		0,	"d",	14,	-1,	{RT,DIS,RA,0}	},
{"liu",		0,	"d",	15,	-1,	{RT, UI,0}	}, /* same as `cau' */
{"cau",		0,	"d",	15,	-1,	{RT,RA,UI,0}	},
{"bb",		"1tfl",	"b",	16,	-1,	{LI,A2,0}	},
/*		^^^^^
  exception #1. Either fix this, or document what you are doing here. FIXMEmgo.
*/
{"bc",		"la",	"b",	16,	-1,	{BO,BI,TA14,0}	},
{"svc",		"la",	"sc",	17,	-1,	{LEV,FL1,FL2,0}	},
{"b",		"la",	"i",	18,	-1,	{TA24,0}	},
{"mcrf",	0,	"xl",	19,	0,	{BF,BFA,0}	},
{"bcr",		"l",	"xl",	19,	16,	{BO,BI,0}	},
{"cror",	0,	"xl",	19,	33,	{BT,BA,BB,0}	},
{"crandc",	0,	"xl",	19,	129,	{BT,BA,BB,0}	},
{"bcc",		"l",	"xl",	19,	193,	{BO,BI,0}	},
{"crnor",	0,	"xl",	19,	225,	{BT,BA,BB,0}	},
{"crand",	0,	"xl",	19,	257,	{BT,BA,BB,0}	},
{"creqv",	0,	"xl",	19,	289,	{BT,BA,BB,0}	},
{"crnand",	0,	"xl",	19,	289,	{BT,BA,BB,0}	},
{"crxor",	0,	"xl",	19,	417,	{BT,BA,BB,0}	},
{"crorc",	0,	"xl",	19,	449,	{BT,BA,BB,0}	},
{"bcr",		"l",	"xl",	19,	528,	{BO,BI,0}	},
{"rlimi",	".",	"m",	20,	-1,	{RA,RS,SH,MB,ME,0} /*??*/},
{"rlinm",	".",	"m",	21,	-1,	{RA,RS,SH,MB,ME,0} /*??*/},
{"rlmi",	".",	"m",	22,	-1,	{RA,RS,RB,MB,ME,0} /*??*/},
{"rlnm",	".",	"m",	23,	-1,	{RA,RS,RB,MB,ME,0} /*??*/},
{"oril",	0,	"d",	24,	-1,	{RA,RS,UI,0}	},
{"oriu",	0,	"d",	25,	-1,	{RA,RS,UI,0}	},
{"xoril",	0,	"d",	26,	-1,	{RA,RS,UI,0}	},
{"xoriu",	0,	"d",	27,	-1,	{RA,RS,UI,0}	},
{"andil.",	0,	"d",	28,	-1,	{RA,RS,UI,0}	},
{"andiu.",	0,	"d",	29,	-1,	{RA,RS,UI,0}	},
{"cmp",		0,	"x",	31,	0,	{BF,RA,RB,0}	},
{"t",		0,	"x",	31,	4,	{TO,RA,RB,0}	},
{"sf",		"o.",	"xo",	31,	8,	{RT,RA,RB,0}	},
{"a",		"o.",	"xo",	31,	10,	{RT,RA,RB,0}	},
{"mfcr",	0,	"x",	31,	19,	{RT,0}		},
{"lx",		0,	"x",	31,	23,	{RT,RA,RB,0}	},
{"sl",		".",	"x",	31,	24,	{RA,RS,RB,0}	},
{"cntlz",	".",	"xo",	31,	26,	{RA,RS,0}	},
{"and",		".",	"x",	31,	28,	{RA,RS,RB,0}	},
{"maskg",	".",	"x",	31,	29,	{RA,RS,RB,0}	},
{"cmpl",	0,	"x",	31,	32,	{BF,RA,RB,0}	},
{"sfe",		"o.",	"xo",	31,	136,	{RT,RA,RB,0}	},
{"lux",		0,	"x",	31,	55,	{RT,RA,RB,0}	},
{"andc",	".",	"x",	31,	60,	{RA,RS,RB,0}	},
{"mfmsr",	0,	"x",	31,	83,	{RT,0}		},
{"lbzx",	0,	"x",	31,	87,	{RT,RA,RB,0}	},
{"neg",		"o.",	"xo",	31,	104,	{RT,RA,0}	},
{"mul",		"o.",	"xo",	31,	107,	{RT,RA,RB,0}	},
{"lbzux",	0,	"x",	31,	119,	{RT,RA,RB,0}	},
{"nor",		".",	"x",	31,	124,	{RA,RS,RB,0}	},
{"ae",		"o.",	"xo",	31,	138,	{RT,RA,RB,0}	},
{"mtcrf",	0,	"xfx",	31,	144,	{FXM,RS,0}	},
{"stx",		0,	"x",	31,	151,	{RS,RA,RB,0}	},
{"slq",		".",	"x",	31,	152,	{RA,RS,RB,0}	},
{"sle",		".",	"x",	31,	153,	{RA,RS,RB,0}	},
{"stux",	0,	"x",	31,	183,	{RS,RA,RB,0}	},
{"sliq",	".",	"x",	31,	184,	{RA,RS,SH,0}	},
{"sfze",	"o.",	"xo",	31,	200,	{RT,RA,0}	},
{"aze",		"o.",	"xo",	31,	202,	{RT,RA,0}	},
{"stbx",	0,	"x",	31,	215,	{RS,RA,RB,0}	},
{"sllq",	".",	"x",	31,	216,	{RA,RS,RB,0}	},
{"sleq",	".",	"x",	31,	217,	{RA,RS,RB,0}	},
{"sfme",	"o.",	"xo",	31,	232,	{RT,RA,0}	},
{"ame",		"o.",	"xo",	31,	234,	{RT,RA,0}	},
{"muls",	"o.",	"xo",	31,	235,	{RT,RA,RB,0}	},
{"stbux",	0,	"x",	31,	247,	{RS,RA,RB,0}	},
{"slliq", 	".",	"x",	31,	248,	{RA,RS,SH,0}	},
{"doz",		"o.",	"x",	31,	264,	{RT,RA,RB,0}	},
{"cax",		"o.",	"xo",	31,	266,	{RT,RA,RB,0}	},
{"lscbx",	".",	"x",	31,	277,	{RT,RA,RB,0}	},
{"lhzx",	0,	"x",	31,	279,	{RT,RA,RB,0}	},
{"eqv",		".",	"x",	31,	284,	{RA,RS,RB,0}	},
{"lhzux",	0,	"x",	31,	311,	{RT,RA,RB,0}	},
{"xor",		".",	"x",	31,	316,	{RA,RS,RB,0}	},
{"div",		"o.",	"xo",	31,	331,	{RT,RA,RB,0}	},
{"mfspr",	0,	"x",	31,	339,	{RT,SPR,0}	},
{"lhax",	0,	"x",	31,	343,	{RT,RA,RB,0}	},
{"abs",		"o.",	"xo",	31,	360,	{RT,RA,0}	},
{"divs",	"o.",	"xo",	31,	363,	{RT,RA,RB,0}	},
{"lhaux",	0,	"x",	31,	375,	{RT,RA,RB,0}	},
{"sthx",	0,	"x",	31,	407,	{RS,RA,RB,0}	},
{"orc",		".",	"x",	31,	412,	{RA,RS,RB,0}	},
{"sthux",	0,	"x",	31,	439,	{RS,RA,RB,0}	},
{"or",		".",	"x",	31,	444,	{RA,RS,RB,0}	},
{"mtspr",	0,	"x",	31,	467,	{SPR,RS,0}	},
{"nand",	".",	"x",	31,	476,	{RA,RS,RB,0}	},
{"nabs",	"o.",	"xo",	31,	488,	{RT,RA,0}	},
{"mcrxr",	0,	"x",	31,	512,	{BF,0}		},
{"lsx",		0,	"x",	31,	533,	{RT,RA,RB,0}	},
{"lbrx",	0,	"x",	31,	534,	{RT,RA,RB,0}	},
{"lfsx",	0,	"x",	31,	535,	{FRT,RA,RB,0}	},
{"sr",		".",	"x",	31,	536,	{RA,RS,RB,0}	},
{"rrib",	".",	"x",	31,	537,	{RA,RS,RB,0}	},
{"maskir",	".",	"x",	31,	541,	{RA,RS,RB,0}	},
{"lfsux",	0,	"x",	31,	567,	{FRT,RA,RB,0}	},
{"lsi",		0,	"x",	31,	597,	{RT,RA,NB,0}	},
{"lfdx",	0,	"x",	31,	599,	{FRT,RA,RB,0}	},
{"lfdux",	0,	"x",	31,	631,	{FRT,RA,RB,0}	},
{"stsx",	0,	"x",	31,	661,	{RS,RA,RB,0}	},
{"stbrx",	0,	"x",	31,	662,	{RS,RA,RB,0}	},
{"stfsx",	0,	"x",	31,	663,	{FRS,RA,RB,0}	},
{"srq",		".",	"x",	31,	664,	{RA,RS,RB,0}	},
{"sre",		".",	"x",	31,	665,	{RA,RS,RB,0}	},
{"stfsux",	0,	"x",	31,	695,	{FRS,RA,RB,0}	},
{"sriq",	".",	"x",	31,	696,	{RA,RS,SH,0}	},
{"stsi",	0,	"x",	31,	725,	{RS,RA,NB,0}	},
{"stfdx",	0,	"x",	31,	727,	{FRS,RA,RB,0}	},
{"srlq",	".",	"x",	31,	728,	{RA,RS,RB,0}	},
{"sreq",	".",	"x",	31,	729,	{RA,RS,RB,0}	},
{"stfdux",	0,	"x",	31,	759,	{FRS,RA,RB,0}	},
{"srliq",	".",	"x",	31,	760,	{RA,RS,SH,0}	},
{"lhbrx",	0,	"x",	31,	790,	{RT,RA,RB,0}	},
{"sra",		".",	"x",	31,	792,	{RA,RS,RB,0}	},
{"srai",	".",	"x",	31,	824,	{RA,RS,SH,0}	},
{"sthbrx",	0,	"x",	31,	918,	{RS,RA,RB,0}	},
{"sraq",	".",	"x",	31,	920,	{RA,RS,RB,0}	},
{"srea",	".",	"x",	31,	921,	{RA,RS,RB,0}	},
{"exts",	".",	"x",	31,	922,	{RA,RS,0}	},
{"sraiq",	".",	"x",	31,	952,	{RA,RS,SH,0}	},
{"l",		0,	"d",	32,	-1,	{RT,DIS,RA,0}	},
{"lu",		0,	"d",	33,	-1,	{RT,DIS,RA,0}	},
{"lbz",		0,	"d",	34,	-1,	{RT,DIS,RA,0}	},
{"lbzu",	0,	"d",	35,	-1,	{RT,DIS,RA,0}	},
{"st",		0,	"d",	36,	-1,	{RS,DIS,RA,0}	},
{"stu",		0,	"d",	37,	-1,	{RS,DIS,RA,0}	},
{"stb",		0,	"d",	38,	-1,	{RS,DIS,RA,0}	},
{"stbu",	0,	"d",	39,	-1,	{RS,DIS,RA,0}	},
{"lhz",		0,	"d",	40,	-1,	{RT,DIS,RA,0}	},
{"lhzu",	0,	"d",	41,	-1,	{RT,DIS,RA,0}	},
{"lha",		0,	"d",	42,	-1,	{RT,DIS,RA,0}	},
{"lhau",	0,	"d",	43,	-1,	{RT,DIS,RA,0}	},
{"sth",		0,	"d",	44,	-1,	{RS,DIS,RA,0}	},
{"sthu",	0,	"d",	45,	-1,	{RS,DIS,RA,0}	},
{"lm",		0,	"d",	46,	-1,	{RT,DIS,RA,0}	},
{"stm",		0,	"d",	47,	-1,	{RS,DIS,RA,0}	},
{"lfs",		0,	"d",	48,	-1,	{FRT,DIS,RA,0}	},
{"lfsu",	0,	"d",	49,	-1,	{FRT,DIS,RA,0}	},
{"lfd",		0,	"d",	50,	-1,	{FRT,DIS,RA,0}	},
{"lfdu",	0,	"d",	51,	-1,	{FRT,DIS,RA,0}	},
{"stfs",	0,	"d",	52,	-1,	{FRS,DIS,RA,0}	},
{"stfsu",	0,	"d",	53,	-1,	{FRS,DIS,RA,0}	},
{"stfd",	0,	"d",	54,	-1,	{FRS,DIS,RA,0}	},
{"stfdu",	0,	"d",	55,	-1,	{FRS,DIS,RA,0}	},
{"fcmpu",	0,	"x",	63,	0,	{BF,FRA,FRB,0}	},
{"frsp",	".",	"x",	63,	12,	{FRT,FRB,0}	},
{"fd",		".",	"a",	63,	18,	{FRT,FRA,FRB,0}	},
{"fs",		".",	"a",	63,	20,	{FRT,FRA,FRB,0}	},
{"fa",		".",	"a",	63,	21,	{FRT,FRA,FRB,0}	},
{"fm",		".",	"a",	63,	25,	{FRT,FRA,FRC,0}	},
{"fms",		".",	"a",	63,	28,	{FRT,FRA,FRC,FRB,0}	},
{"fma",		".",	"a",	63,	29,	{FRT,FRA,FRC,FRB,0}	},
{"fnms",	".",	"a",	63,	30,	{FRT,FRA,FRC,FRB,0}	},
{"fnma",	".",	"a",	63,	31,	{FRT,FRA,FRC,FRB,0}	},
{"fcmpo",	0,	"x",	63,	32,	{BF,FRA,FRB,0}	},
{"mtfsb1",	".",	"x",	63,	38,	{BT,0}		},
{"fneg",	".",	"x",	63,	40,	{FRT,FRB,0}	},
{"mcrfs",	0,	"x",	63,	64,	{BF,BFA,0}	},
{"mtfsb0",	".",	"x",	63,	70,	{BT,0}		},
{"fmr",		".",	"x",	63,	72,	{FRT,FRB,0}	},
{"mtfsfi",	".",	"x",	63,	134,	{BF,I,0}	},
{"fnabs",	".",	"x",	63,	136,	{FRT,FRB,0}	},
{"fabs",	".",	"x",	63,	264,	{FRT,FRB,0}	},
{"mffs",	".",	"x",	63,	583,	{FRT,0}		},
{"mtfsf",	".",	"xfl",	63,	711,	{FLM,FRB,0}	},
};

#define	NOPCODES	(sizeof (rs6k_ops) / sizeof (struct rs6000_insn))
