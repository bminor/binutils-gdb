/* Ptrace request codes implemented by the VxWorks remote debug protocol.
   These values match those used by SunOS in some old release.  */

enum ptracereq {
	PTRACE_GETREGS		= 12,
	PTRACE_GETFPREGS	= 14,
	PTRACE_SETREGS		= 13,
	PTRACE_SETFPREGS	= 15,
	PTRACE_WRITEDATA	= 17,
	PTRACE_READDATA		= 16,
	PTRACE_SINGLESTEP	= 9,
	PTRACE_CONT		= 7,
	PTRACE_ATTACH		= 10,
	PTRACE_DETACH		= 11,
	PTRACE_KILL		= 8	/* no trailing comma */
};
