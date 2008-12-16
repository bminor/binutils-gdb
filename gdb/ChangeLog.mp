2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-12  Pedro Alves  <pedro@codesourcery.com>

	* infrun.c (handle_inferior_event): Correctly tag non-executing
	threads in multi-process.
	(normal_stop): Correctly tag stopped threads in multi-process.

2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-12  Pedro Alves  <pedro@codesourcery.com>

	* remote.c (extended_remote_mourn_1): Always call
	generic_mourn_inferior.

2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-12  Pedro Alves  <pedro@codesourcery.com>

	* remote.c (remote_detach_1): Don't delete the inferior here.
	(process_stop_reply): Ditto.
	(extended_remote_kill): Ditto.

2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-11  Pedro Alves  <pedro@codesourcery.com>

	* infcmd.c (step_1): Use step_once in the synchronous case too.
	(step_1_continuation): Wrap line.
	(step_once): Adjust comment.  Only install the continuation in
	async mode.

2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-07  Pedro Alves  <pedro@codesourcery.com>

	* infrun.c (handle_inferior_event): On a TARGET_WAITKIND_EXITED or
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
