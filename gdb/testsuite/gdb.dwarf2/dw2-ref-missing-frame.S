	.file	"ref-missing-frame.c"
	.section	.debug_abbrev,"",@progbits
.Ldebug_abbrev0:
	.section	.debug_info,"",@progbits
.Ldebug_info0:
	.section	.debug_line,"",@progbits
.Ldebug_line0:
	.text
.Ltext0:
.globl main
	.type	main, @function
main:
.LFB2:
	.file 1 "ref-missing-frame.c"
	.loc 1 3 0
	leal	4(%esp), %ecx
.LCFI0:
	andl	$-16, %esp
	pushl	-4(%ecx)
.LCFI1:
	pushl	%ebp
.LCFI2:
	movl	%esp, %ebp
.LCFI3:
	pushl	%ecx
.LCFI4:
	subl	$16, %esp
.LCFI5:
	.loc 1 6 0
	movl	$0, %eax
	.loc 1 7 0
	addl	$16, %esp
	popl	%ecx
	popl	%ebp
	leal	-4(%ecx), %esp
	ret
.LFE2:
	.size	main, .-main
	.section	.debug_frame,"",@progbits
.Lframe0:
	.long	.LECIE0-.LSCIE0
.LSCIE0:
	.long	0xffffffff
	.byte	0x1
	.string	""
	.uleb128 0x1
	.sleb128 -4
	.byte	0x8
	.byte	0xc
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x88
	.uleb128 0x1
	.align 4
.LECIE0:
.LSFDE0:
	.long	.LEFDE0-.LASFDE0
.LASFDE0:
	.long	.Lframe0
	.long	.LFB2
	.long	.LFE2-.LFB2
	.byte	0x4
	.long	.LCFI0-.LFB2
	.byte	0xc
	.uleb128 0x1
	.uleb128 0x0
	.byte	0x9
	.uleb128 0x4
	.uleb128 0x1
	.byte	0x4
	.long	.LCFI1-.LCFI0
	.byte	0xc
	.uleb128 0x4
	.uleb128 0x4
	.byte	0x4
	.long	.LCFI2-.LCFI1
	.byte	0xe
	.uleb128 0x8
	.byte	0x85
	.uleb128 0x2
	.byte	0x4
	.long	.LCFI3-.LCFI2
	.byte	0xd
	.uleb128 0x5
	.byte	0x4
	.long	.LCFI4-.LCFI3
	.byte	0x84
	.uleb128 0x3
	.align 4
.LEFDE0:
	.text
.Letext0:
	.section	.debug_loc,"",@progbits
.Ldebug_loc0:
.LLST0:
	.long	.LFB2-.Ltext0
	.long	.LCFI0-.Ltext0
	.value	0x2
#if 0
	.byte	0x74
#else
	/* DW_OP_fbreg.  */
	.byte	0x91
#endif
	.sleb128 4
	.long	.LCFI0-.Ltext0
	.long	.LCFI1-.Ltext0
	.value	0x2
#if 0
	.byte	0x74
#else
	/* DW_OP_fbreg.  */
	.byte	0x91
#endif
	.sleb128 4
	.long	.LCFI1-.Ltext0
	.long	.LCFI2-.Ltext0
	.value	0x2
#if 0
	.byte	0x74
#else
	/* DW_OP_fbreg.  */
	.byte	0x91
#endif
	.sleb128 4
	.long	.LCFI2-.Ltext0
	.long	.LCFI3-.Ltext0
	.value	0x2
#if 0
	.byte	0x74
#else
	/* DW_OP_fbreg.  */
	.byte	0x91
#endif
	.sleb128 8
	.long	.LCFI3-.Ltext0
	.long	.LFE2-.Ltext0
	.value	0x2
#if 0
	.byte	0x74
#else
	/* DW_OP_fbreg.  */
	.byte	0x91
#endif
	.sleb128 8
	.long	0x0
	.long	0x0
	.section	.debug_info
	.long	0x5a
	.value	0x2
	.long	.Ldebug_abbrev0
	.byte	0x4
	.uleb128 0x1
	.long	.LASF0
	.byte	0x1
	.long	.LASF1
	.long	.LASF2
	.long	.Ltext0
	.long	.Letext0
	.long	.Ldebug_line0
	.uleb128 0x2
	.byte	0x1
	.long	.LASF3
	.byte	0x1
	.byte	0x3
	.byte	0x1
	.long	0x51
	.long	.LFB2
	.long	.LFE2
	.long	.LLST0	/* DW_AT_frame_base */
	.long	0x51
	.uleb128 0x3
	.string	"var"
	.byte	0x1
	.byte	0x4
	.long	0x58
	.byte	0x2
	.byte	0x91
	.sleb128 -16
	.byte	0x0
	.uleb128 0x4
	.byte	0x4
	.byte	0x5
	.string	"int"
	.uleb128 0x5
	.long	0x51
	.byte	0x0
	.section	.debug_abbrev
	.uleb128 0x1
	.uleb128 0x11
	.byte	0x1
	.uleb128 0x25
	.uleb128 0xe
	.uleb128 0x13
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x1b
	.uleb128 0xe
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
	.uleb128 0x10
	.uleb128 0x6
	.byte	0x0
	.byte	0x0
	.uleb128 0x2
	.uleb128 0x2e
	.byte	0x1
	.uleb128 0x3f
	.uleb128 0xc
	.uleb128 0x3
	.uleb128 0xe
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x27
	.uleb128 0xc
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x11
	.uleb128 0x1
	.uleb128 0x12
	.uleb128 0x1
#ifndef NO_FRAME_BASE
	.uleb128 0x40 /* DW_AT_frame_base */
#else
	.uleb128 0x38 /* DW_AT_data_member_location */
#endif
	.uleb128 0x6
	.uleb128 0x1
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x34
	.byte	0x0
	.uleb128 0x3
	.uleb128 0x8
	.uleb128 0x3a
	.uleb128 0xb
	.uleb128 0x3b
	.uleb128 0xb
	.uleb128 0x49
	.uleb128 0x13
	.uleb128 0x2
	.uleb128 0xa
	.byte	0x0
	.byte	0x0
	.uleb128 0x4
	.uleb128 0x24
	.byte	0x0
	.uleb128 0xb
	.uleb128 0xb
	.uleb128 0x3e
	.uleb128 0xb
	.uleb128 0x3
	.uleb128 0x8
	.byte	0x0
	.byte	0x0
	.uleb128 0x5
	.uleb128 0x35
	.byte	0x0
	.uleb128 0x49
	.uleb128 0x13
	.byte	0x0
	.byte	0x0
	.byte	0x0
	.section	.debug_pubnames,"",@progbits
	.long	0x17
	.value	0x2
	.long	.Ldebug_info0
	.long	0x5e
	.long	0x25
	.string	"main"
	.long	0x0
	.section	.debug_aranges,"",@progbits
	.long	0x1c
	.value	0x2
	.long	.Ldebug_info0
	.byte	0x4
	.byte	0x0
	.value	0x0
	.value	0x0
	.long	.Ltext0
	.long	.Letext0-.Ltext0
	.long	0x0
	.long	0x0
	.section	.debug_str,"MS",@progbits,1
.LASF0:
	.string	"GNU C 4.3.0 20080416 (Red Hat 4.3.0-7)"
.LASF2:
	.string	"/tmp"
.LASF1:
	.string	"ref-missing-frame.c"
.LASF3:
	.string	"main"
	.ident	"GCC: (GNU) 4.3.0 20080416 (Red Hat 4.3.0-7)"
	.section	.note.GNU-stack,"",@progbits
