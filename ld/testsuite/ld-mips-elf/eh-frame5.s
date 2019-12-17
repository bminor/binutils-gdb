	.cfi_startproc
	.cfi_personality 0x0,local_pers
	.cfi_lsda 0x0,LSDA
	.ent	f1
	.type	f1, @function
f1:
	nop
	.end	f1
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x0,hidden_pers
	.cfi_lsda 0x0,LSDA
	.ent	f2
	.type	f2, @function
f2:
	nop
	.end	f2
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x0,global_pers
	.cfi_lsda 0x0,LSDA
	.ent	f3
	.type	f3, @function
f3:
	nop
	.end	f3
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x0,extern_pers
	.cfi_lsda 0x0,LSDA
	.ent	f4
	.type	f4, @function
f4:
	nop
	.end	f4
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x80,local_indirect_ptr
	.cfi_lsda 0x0,LSDA
	.ent	f5
	.type	f5, @function
f5:
	nop
	.end	f5
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x80,hidden_indirect_ptr
	.cfi_lsda 0x0,LSDA
	.ent	f6
	.type	f6, @function
f6:
	nop
	.end	f6
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x80,global_indirect_ptr
	.cfi_lsda 0x0,LSDA
	.ent	f7
	.type	f7, @function
f7:
	nop
	.end	f7
	.cfi_endproc

	.cfi_startproc
	.cfi_personality 0x80,extern_indirect_ptr
	.cfi_lsda 0x0,LSDA
	.ent	f8
	.type	f8, @function
f8:
	nop
	.end	f8
	.cfi_endproc


	.ent	local_pers
	.type	local_pers, @function
local_pers:
	nop
	.end	local_pers

	.globl	hidden_pers
	.hidden	hidden_pers
	.ent	hidden_pers
	.type	hidden_pers, @function
hidden_pers:
	nop
	.end	hidden_pers

	.globl	global_pers
	.ent	global_pers
	.type	global_pers, @function
global_pers:
	nop
	.end	global_pers

	.section .data,"aw",@progbits

local_indirect_ptr:
	.4byte	pers1

	.globl hidden_indirect_ptr
	.hidden hidden_indirect_ptr
hidden_indirect_ptr:
	.4byte	pers2

	.globl global_indirect_ptr
global_indirect_ptr:
	.4byte	pers3

LSDA:
	.4byte	0
