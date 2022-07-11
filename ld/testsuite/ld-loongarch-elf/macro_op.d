#as:
#objdump: -dr

.*:[    ]+file format .*


Disassembly of section .text:

00000000.* <.text>:
[ 	]+0:[ 	]+00150004[ 	]+move[ 	]+\$a0,[ 	]+\$zero
[ 	]+4:[ 	]+02bffc04[ 	]+addi.w[ 	]+\$a0,[ 	]+\$zero,[ 	]+-1\(0xfff\)
[ 	]+8:[ 	]+00150004[ 	]+move[ 	]+\$a0,[ 	]+\$zero
[ 	]+c:[ 	]+02bffc04[ 	]+addi.w[ 	]+\$a0,[ 	]+\$zero,[ 	]+-1\(0xfff\)
[ 	]+10:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+10:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+14:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+14:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+18:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+18:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+1c:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+1c:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+20:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+20:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+24:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+24:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+28:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+28:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+.text
[ 	]+2c:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+2c:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+.text
[ 	]+30:[ 	]+380c1484[ 	]+ldx.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+34:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+34:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+38:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+38:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+3c:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+3c:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+40:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+40:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+44:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+44:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+.text
[ 	]+48:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+48:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+.text
[ 	]+4c:[ 	]+380c1484[ 	]+ldx.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+50:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+50:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+54:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+54:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+58:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+58:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+5c:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+5c:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+60:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+60:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+.text
[ 	]+64:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+64:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+.text
[ 	]+68:[ 	]+380c1484[ 	]+ldx.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+6c:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+6c:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+70:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+70:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+74:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+74:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+78:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+78:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+7c:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+7c:[ 	]+R_LARCH_PCALA64_LO20[ 	]+.text
[ 	]+80:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+80:[ 	]+R_LARCH_PCALA64_HI12[ 	]+.text
[ 	]+84:[ 	]+00109484[ 	]+add.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+88:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+88:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+8c:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+8c:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+90:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+90:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+94:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+94:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+98:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+98:[ 	]+R_LARCH_PCALA64_LO20[ 	]+.text
[ 	]+9c:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+9c:[ 	]+R_LARCH_PCALA64_HI12[ 	]+.text
[ 	]+a0:[ 	]+00109484[ 	]+add.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+a4:[ 	]+14000004[ 	]+lu12i.w[ 	]+\$a0,[ 	]+0
[ 	]+a4:[ 	]+R_LARCH_MARK_LA[ 	]+\*ABS\*
[ 	]+a4:[ 	]+R_LARCH_ABS_HI20[ 	]+.text
[ 	]+a8:[ 	]+03800084[ 	]+ori[ 	]+\$a0,[ 	]+\$a0,[ 	]+0x0
[ 	]+a8:[ 	]+R_LARCH_ABS_LO12[ 	]+.text
[ 	]+ac:[ 	]+16000004[ 	]+lu32i.d[ 	]+\$a0,[ 	]+0
[ 	]+ac:[ 	]+R_LARCH_ABS64_LO20[ 	]+.text
[ 	]+b0:[ 	]+03000084[ 	]+lu52i.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+b0:[ 	]+R_LARCH_ABS64_HI12[ 	]+.text
[ 	]+b4:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+b4:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+b8:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+b8:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+bc:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+bc:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+c0:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+c0:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+c4:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+c4:[ 	]+R_LARCH_PCALA_HI20[ 	]+.text
[ 	]+c8:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+c8:[ 	]+R_LARCH_PCALA_LO12[ 	]+.text
[ 	]+cc:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+cc:[ 	]+R_LARCH_PCALA64_LO20[ 	]+.text
[ 	]+d0:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+d0:[ 	]+R_LARCH_PCALA64_HI12[ 	]+.text
[ 	]+d4:[ 	]+00109484[ 	]+add.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+d8:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+d8:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+dc:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+dc:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+e0:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+e0:[ 	]+R_LARCH_GOT_PC_HI20[ 	]+.text
[ 	]+e4:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+e4:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+.text
[ 	]+e8:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+e8:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+.text
[ 	]+ec:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+ec:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+.text
[ 	]+f0:[ 	]+380c1484[ 	]+ldx.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+f4:[ 	]+14000004[ 	]+lu12i.w[ 	]+\$a0,[ 	]+0
[ 	]+f4:[ 	]+R_LARCH_TLS_LE_HI20[ 	]+TLS1
[ 	]+f8:[ 	]+03800084[ 	]+ori[ 	]+\$a0,[ 	]+\$a0,[ 	]+0x0
[ 	]+f8:[ 	]+R_LARCH_TLS_LE_LO12[ 	]+TLS1
[ 	]+fc:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+fc:[ 	]+R_LARCH_TLS_IE_PC_HI20[ 	]+TLS1
[ 	]+100:[ 	]+28c00084[ 	]+ld.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+100:[ 	]+R_LARCH_TLS_IE_PC_LO12[ 	]+TLS1
[ 	]+104:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+104:[ 	]+R_LARCH_TLS_IE_PC_HI20[ 	]+TLS1
[ 	]+108:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+108:[ 	]+R_LARCH_TLS_IE_PC_LO12[ 	]+TLS1
[ 	]+10c:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+10c:[ 	]+R_LARCH_TLS_IE64_PC_LO20[ 	]+TLS1
[ 	]+110:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+110:[ 	]+R_LARCH_TLS_IE64_PC_HI12[ 	]+TLS1
[ 	]+114:[ 	]+380c1484[ 	]+ldx.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+118:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+118:[ 	]+R_LARCH_TLS_LD_PC_HI20[ 	]+TLS1
[ 	]+11c:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+11c:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+TLS1
[ 	]+120:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+120:[ 	]+R_LARCH_TLS_LD_PC_HI20[ 	]+TLS1
[ 	]+124:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+124:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+TLS1
[ 	]+128:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+128:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+TLS1
[ 	]+12c:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+12c:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+TLS1
[ 	]+130:[ 	]+00109484[ 	]+add.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
[ 	]+134:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+134:[ 	]+R_LARCH_TLS_GD_PC_HI20[ 	]+TLS1
[ 	]+138:[ 	]+02c00084[ 	]+addi.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+0
[ 	]+138:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+TLS1
[ 	]+13c:[ 	]+1a000004[ 	]+pcalau12i[ 	]+\$a0,[ 	]+0
[ 	]+13c:[ 	]+R_LARCH_TLS_GD_PC_HI20[ 	]+TLS1
[ 	]+140:[ 	]+02c00005[ 	]+addi.d[ 	]+\$a1,[ 	]+\$zero,[ 	]+0
[ 	]+140:[ 	]+R_LARCH_GOT_PC_LO12[ 	]+TLS1
[ 	]+144:[ 	]+16000005[ 	]+lu32i.d[ 	]+\$a1,[ 	]+0
[ 	]+144:[ 	]+R_LARCH_GOT64_PC_LO20[ 	]+TLS1
[ 	]+148:[ 	]+030000a5[ 	]+lu52i.d[ 	]+\$a1,[ 	]+\$a1,[ 	]+0
[ 	]+148:[ 	]+R_LARCH_GOT64_PC_HI12[ 	]+TLS1
[ 	]+14c:[ 	]+00109484[ 	]+add.d[ 	]+\$a0,[ 	]+\$a0,[ 	]+\$a1
