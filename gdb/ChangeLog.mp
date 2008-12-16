2008-12-16  Pedro Alves  <pedro@codesourcery.com>

	Merge from head:
	2008-12-05  Pedro Alves  <pedro@codesourcery.com>

	* infcmd.c (step_1, step_once): Look up the stepping range based
	on the current frame's PC, not on stop_pc.
