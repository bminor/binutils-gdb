# Check ENQCMD[S] 64-bit instructions

	.allow_index_reg
	.text
_start:
	enqcmd (%rcx),%rax
	enqcmd (%ecx),%eax
	enqcmds (%rcx),%rax
	enqcmds (%ecx),%eax
	enqcmd foo(%rip),%rcx
	enqcmd foo(%rip),%ecx
	enqcmd foo(%eip),%ecx
	enqcmds foo(%rip),%rcx
	enqcmds foo(%rip),%ecx
	enqcmds foo(%eip),%ecx

	.intel_syntax noprefix
	enqcmd rax,[rcx]
	enqcmd eax,[ecx]
	enqcmds rax,[rcx]
	enqcmds eax,[ecx]
	enqcmd rcx,[rip+foo]
	enqcmd ecx,[rip+foo]
	enqcmd ecx,[eip+foo]
	enqcmds rcx,[rip+foo]
	enqcmds ecx,[rip+foo]
	enqcmds ecx,[eip+foo]
