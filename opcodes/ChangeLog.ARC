2009-09-10  J"orn Rennecke  <joern.rennecke@arc.com>

	* arc-opc-cgen.c (arc_cgen_init_opcode_table): Comment out
	warning-generating no-op.

2008-04-09  J"orn Rennecke  <joern.rennecke@arc.com>

	Copy from configure:
	* configure.in (bfd_arc_arch): Add arc-asm.lo, arc-ibld.lo,
	arc-opc-cgen.lo and arc-opinst.lo .  Set using_cgen=yes .
	* configure: Regenerate.

	* arc-dis.h (ARCTangent_decodeInstr): Comment out prototype.
	* arc-opc.c (arc_opcode_init_tables): Avoid signed/unsigned comparison.
	* Makefile.am (CFILES): Add arc-dis-cgen.c, arc-desc.c and
	 arcompact-dis.c.
	(ALL_MACHINES): Add arc-dis-cgen.lo, arc-desc.lo and arcompact-dis.lo.
	(arc-dis.lo): Depend on arcompact-dis.h.
	(arcompact-dis.lo): New rule.
	* Makefile.in: Regenerate.
	* arcompact-dis.c (my_sprintf) <*>: Remove warning, abort rather
	than ignore.
	* cgen-dis.c (hash_insn_array): Pass big_p parameter to cd->dis_hash.
	(hash_insn_list, cgen_dis_lookup_insn): Likewise.

2008-11-14  Irfan Rafiq  <irfan.rafiq@arc.com>
        * arc-dis.c,	Target specific improvements.
          arc-ext.c,
          arc-opc.c 
                

2008-11-10  Irfan Rafiq  <irfan.rafiq@arc.com>
        * arcompact-dis.c:	 Added arcompact-dis.c        
        * configure.in: 	 Added arcompact-dis.c
        * configure:		 Regenerated
        
2008-04-09  J"orn Rennecke  <joern.rennecke@arc.com>

	* arc-ext.c (arcExtMap_add): Fix pointer signedness mismatch.

2007-11-01  J"orn Rennecke  <joern.rennecke@arc.com>

	* arc-opc.c (insert_ld_syntax): Fix check for scaled index.
	* arcompact-dis.c (dsmOneArcInst): Also disassemble .aa field
	in the presence of a long immediate.

2007-10-31  J"orn Rennecke  <joern.rennecke@arc.com>

	* arc-opc.c (ADDRESS22S_AC): Define.
	(CACHEBYPASS5_AC): Update index value.
	(insert_addr_wb): Check for '&' format.
	(insert_ld_syntax): Allow long immediate with scaled index.
	(arc_opcodes): Likewise.
	(arc_suffixes_ac): Add entry for "as" / ADDRESS22S_AC.

2007-08-31  J"orn Rennecke  <joern.rennecke@arc.com>

	* arc-opc.c (insert_ld_syntax): Use arc_ld_ext_mask to decide
	if a load to an extension register is valid.
	Only reject a load to ilink[12] if arc_user_mode_only is set.
	(insert_ex_syntax): Use same operand checks as for insert_ld_syntax.
