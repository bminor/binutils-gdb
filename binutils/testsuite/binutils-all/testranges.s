# Test .debug_info can reference .debug_ranges entries without ordering the
# offsets strictly as increasing.

	.text
start:
	.byte	1
sub:
	.byte	2
end:

	.section	.debug_ranges,"",%progbits
range:

range_sub:
	.long	sub, end
	.long	0, 0	;# range terminator

range_cu:
	.long	start, end
	.long	0, 0	;# range terminator

	.section	.debug_info,"",%progbits
	.long	debugE - debugS	;# Length of Compilation Unit Info
debugS:
	.short	0x2	;# DWARF version number
	.long	abbrev0	;# Offset Into Abbrev. Section
	.byte	0x4	;# Pointer Size (in bytes)

	.uleb128 0x1	;# (DIE (0xb) DW_TAG_compile_unit)
	.long	range_cu - range	;# DW_AT_ranges

	.uleb128 0x2	;# (DIE (0x6d) DW_TAG_subprogram)
	.ascii "A\0"	;# DW_AT_name
	.long	range_sub - range	;# DW_AT_ranges
debugE:

	.section	.debug_abbrev,"",%progbits
abbrev0:
	.uleb128 0x1	;# (abbrev code)
	.uleb128 0x11	;# (TAG: DW_TAG_compile_unit)
	.byte	0x0	;# DW_children_no
	.uleb128 0x55	;# (DW_AT_ranges)
	.uleb128 0x6	;# (DW_FORM_data4)
	.byte	0x0
	.byte	0x0

	.uleb128 0x2	;# (abbrev code)
	.uleb128 0x2e	;# (TAG: DW_TAG_subprogram)
	.byte	0x0	;# DW_children_no
	.uleb128 0x3	;# (DW_AT_name)
	.uleb128 0x8	;# (DW_FORM_string)
	.uleb128 0x55	;# (DW_AT_ranges)
	.uleb128 0x6	;# (DW_FORM_data4)
	.byte	0x0
	.byte	0x0

	.byte	0x0	;# abbrevs terminator
