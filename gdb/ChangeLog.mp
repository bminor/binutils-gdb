2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-07  Pedro Alves  <pedro@codesourcery.com>

	(handle_inferior_event): On a TARGET_WAITKIND_EXITED or
	TARGET_WAITKIND_SIGNALLED, switch inferior_ptid to the event ptid.
	* linux_thread_db.c (thread_db_wait): On a TARGET_WAITKIND_EXITED
	or TARGET_WAITKIND_SIGNALLED, return the ptid the beneath target
	returned.
	* inf-ptrace.c (inf_ptrace_wait): Return inferior_ptid instead of
	minus_one_ptid if the inferior disappeared.
	* rs6000-nat.c (rs6000_wait): Likewise.
	* spu-linux-nat.c (spu_child_wait): Likewise.

2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-05  Pedro Alves  <pedro@codesourcery.com>

	* infcmd.c (step_1, step_once): Look up the stepping range based
	on the current frame's PC, not on stop_pc.
