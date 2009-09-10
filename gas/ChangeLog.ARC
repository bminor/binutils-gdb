2009-09-07  J"orn Rennecke  <joern.rennecke@arc.com>

	* config/tc-arc.c (md_number_to_chars): Add case for 8 chars.
	(arc_parse_name) Avoid truncating pointer on 64 bit machines.
	(tc_gen_reloc): Add ATTRIBUTE_UNUSED for section parameter.
	* symbols.h (local_symbol): Declare.

2008-11-14  Irfan Rafiq <irfan.rafiq@arc.com>  
            ARC'S GENERIC CHANGES
	as.c: Make "myname" non-static. - To search extlib files relative to caller path.

        * symbol.c:  Make the local_symbol_make non-static so that it could be used
        in target specific portion. It call is necessary in tc-arc.c because ARC's extension
        instruciton support through file needs to make symbols.
        
2008-11-10  Irfan Rafiq <irfan.rafiq@arc.com>  
	* configure.tgt,	arc-linux-uclibc target.
	  configure.in,
	  configure

	* Makefile.am,		extlib additions.
	  Makefile.in.

2008-04-09  J"orn Rennecke  <joern.rennecke@arc.com>

	* config/tc-arc.c (arc_generate_extinst32_operand_strings): Fix
	signedness of op1, op2, op3 and operand_string.

2008-03-19  James Mundstock <jimm@arc.com>
        * config/tc-arc.c: add support for simd and dsp instructions.
	* opcodes/arc-opc.c: Fix errors in write back and cache bypass fields.
	*                    Fix encodings for instructions S12 and LIMM fields
	*                    Add fields for simd instructions.
2008-03-17  J"orn Rennecke  <joern.rennecke@arc.com>

	* config/tc-arc.c (arc_handle_align): Fix signedness problem.

2007-11-23  J"orn Rennecke  <joern.rennecke@arc.com>

	* tc-arc.c (line_separator_chars): Add '`'.

2007-08-31  J"orn Rennecke  <joern.rennecke@arc.com>

	* config/tc-arc.c (arc_ld_ext_mask, arc_user_mode_only): Define.
	(enum options): Add OPTION_USER_MODE and OPTION_LD_EXT_MASK.
	(md_longopts): Add muser-mode-only and mld-extension-reg-mask.
	(md_parse_option): Handle OPTION_USER_MODE and OPTION_LD_EXT_MASK.
	* doc/c-arc.texi: Document new options.

2007-05-16  J"orn Rennecke  <joern.rennecke@arc.com>

	From codito configure.in patch:
	* configure.tgt (arc-*-elf*): Add bfd_gas=yes.
	(arc-*-linux-uclibc*): Add.

	* configure: Regenerate.
	* Makefile.in: Regenerate.

2007-05-11  J"orn Rennecke  <joern.rennecke@arc.com>

	* config/tc-arc.c (opcode/arc.h): Don't #include.
	(loop_target_list): Remove.
	(add_loop_target, arc_check_label): Use symbol_get_tc to find the
	matching struct loop_target.  Remove list insertion.
	(struct enriched_insn, struct loop_target): Move from here...
	* config/tc-arc.h: ... to here.
	(opcode/arc.h): #include.
	(struct loop_target): Remove 'next' member.
	(struct arc_tc_sy): Define.
	(TC_SYMFIELD_TYPE, tc_symbol_new_hook): #define.
	
	* config/tc-arc.c (arc_set_ext_seg): Change to static void.
	(insert_last_insn): Fix type of sym parameter.
	(arc_process_extinstr_options): Cast results of local_symbol_make.
