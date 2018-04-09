# Check 64bit WAITPKG instructions.

	.text
_start:
	umonitor %rax
	umonitor %r10
	umonitor %r10d
	umwait %rcx
	umwait %r10
	tpause %rcx
	tpause %r10
