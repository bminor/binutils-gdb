2007-11-13  Irfan Rafiq     <irfan.rafiq@arc.com>

	* Ingegrating CGEN simulator with gdb.
		- model5.c, model6.c, model7.c : Synchronizing model strings like A7 with GDB internal DS.

2007-11-05  Irfan Rafiq     <irfan.rafiq@arc.com>
	* Ingegrating CGEN simulator with gdb.
		- tconfig.in		       : defined SIM_HAVE_BIENDIAN 1

2007-10-26  Irfan Rafiq     <irfan.rafiq@arc.com>
	* Ingegrating CGEN simulator with gdb.
		- sim-if.c		       : Fix of crash
		- sim-main.c		       : For arc-opc-cgen.h
	
2007-09-17  J"orn Rennecke  <joern.rennecke@arc.com>

	* gentvals.sh: Allow target-specific definitions to override
	later generic ones.
	* gennltvals.sh: Use ${srcroot}/newlib/libc/sys/arc/sys/fcntl.h
	for open on arc.
	* nltvals.def: Regenerate.

2007-08-22  J"orn Rennecke  <joern.rennecke@arc.com>

	* (cgen-trace.c): Check for CGEN_INSN_DISASM_BITSIZE.

2007-08-21  J"orn Rennecke  <joern.rennecke@arc.com>

	* cgen-trace.c (sim_cgen_disassemble_insn): Disable bogus sanity check.

2007-05-14  J"orn Rennecke  <joern.rennecke@arc.com>

	* callback.c (fdmap, fdbad): No longer static.

2007-05-02  J"orn Rennecke  <joern.rennecke@arc.com>

	* syscall.c (get_path): No longer static.

2007-04-30  J"orn Rennecke  <joern.rennecke@arc.com>

	* gennltvals.sh: Add arc code.
	* nltvals.def: Regenerate.

	Speed up simulator startup:
	* sim-utils.c (zalloc): Use xcalloc.

	Debugging utility functions:
	* sim-utils.c (dump_hex, dump_strn_1, dump_asc): New functions.
	* (dump_str, dump_strn): Likewise.

	Fix descriptor 0/1/2 handling in pesence of freopen:
	* syscall.c (cb_syscall): Add comments about descriptors 0/1/2.
	* callback.c (os_read_stdin): Use os_read.
	(os_write): Use fwrite to stdout / stderr for descriptor 1 / 2.
	(os_write_stdout, os_write_stderr): Use os_write.
