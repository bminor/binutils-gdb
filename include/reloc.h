/* Relocation types for a.out files using reloc_info_extended
   (SPARC and AMD 29000). */

#ifndef _RELOC_H_READ_
#define _RELOC_H_READ_ 1

enum reloc_type
{
    RELOC_8,        RELOC_16,        RELOC_32, /* simple relocations */
    RELOC_DISP8,    RELOC_DISP16,    RELOC_DISP32, /* pc-rel displacement */
    RELOC_WDISP30,  RELOC_WDISP22,
    RELOC_HI22,     RELOC_22,
    RELOC_13,       RELOC_LO10,
    RELOC_SFA_BASE, RELOC_SFA_OFF13,
    RELOC_BASE10,   RELOC_BASE13,    RELOC_BASE22, /* P.I.C. (base-relative) */
    RELOC_PC10,     RELOC_PC22,	/* for some sort of pc-rel P.I.C. (?) */
    RELOC_JMP_TBL,		/* P.I.C. jump table */
    RELOC_SEGOFF16,		/* reputedly for shared libraries somehow */
    RELOC_GLOB_DAT,  RELOC_JMP_SLOT, RELOC_RELATIVE,

/* 29K relocation types */
    RELOC_JUMPTARG, RELOC_CONST,     RELOC_CONSTH,

    NO_RELOC
};

#define	RELOC_TYPE_NAMES \
"8",		"16",		"32",		"DISP8",	\
"DISP16",	"DISP32",	"WDISP30",	"WDISP22",	\
"HI22",		"22",		"13",		"LO10",		\
"SFA_BASE",	"SFAOFF13",	"BASE10",	"BASE13",	\
"BASE22",	"PC10",		"PC22",		"JMP_TBL",	\
"SEGOFF16",	"GLOB_DAT",	"JMP_SLOT",	"RELATIVE",	\
"JUMPTARG",	"CONST",	"CONSTH",			\
"NO_RELOC",							\
"XXX_28", "XXX_29", "XXX_30", "XXX_31"

#endif /* _RELOC_H_READ_ */
