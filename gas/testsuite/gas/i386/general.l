.*: Assembler messages:
.*:10: Warning:.*
.*:12: Warning:.*
.*:19: Warning:.*
.*:22: Warning:.*
.*:81: Warning:.*
.*:82: Warning:.*
.*:83: Warning:.*
.*:84: Warning:.*
.*:85: Warning:.*
.*:86: Warning:.*
.*:87: Warning:.*
.*:88: Warning:.*
.*:89: Warning:.*
.*:90: Warning:.*
.*:91: Warning:.*
.*:92: Warning:.*
.*:93: Warning:.*
.*:94: Warning:.*
.*:95: Warning:.*
.*:96: Warning:.*
.*:97: Warning:.*
.*:98: Warning:.*
.*:99: Warning:.*
.*:100: Warning:.*
.*:101: Warning:.*
.*:135: Warning:.*
   1                       	.psize 0
   2                       	.text
   3                       	# test various segment reg insns
   4 0000 1E                		push	%ds
   5 0001 1E                		pushl	%ds
   6 0002 1F                		pop	%ds
   7 0003 1F                		popl	%ds
   8 0004 8CD8              		mov	%ds,%eax
   9 0006 8CD8              		movl	%ds,%eax
  10 0008 8CD8              		movl	%ds,%ax
.*Warning:.*
  11 000a 8ED8              		mov	%eax,%ds
  12 000c 8ED8              		movl	%ax,%ds
.*Warning:.*
  13 000e 8ED8              		movl	%eax,%ds
  14                       	
  15 0010 661E              		pushw	%ds
  16 0012 661F              		popw	%ds
  17 0014 668CD8            		mov	%ds,%ax
  18 0017 668CD8            		movw	%ds,%ax
  19 001a 668CD8            		movw	%ds,%eax
.*Warning:.*
  20 001d 8ED8              		mov	%ax,%ds
  21 001f 8ED8              		movw	%ax,%ds
  22 0021 8ED8              		movw	%eax,%ds
.*Warning:.*
  23                       	
  24                       	# test various pushes
  25 0023 6A0A              		pushl	\$10
  26 0025 666A0A            		pushw	\$10
  27 0028 6A0A              		push	\$10
  28 002a 68E80300 00       		pushl	\$1000
  29 002f 6668E803          		pushw	\$1000
  30 0033 68E80300 00       		push	\$1000
  31 0038 FF355700 0000     		pushl	1f
  32 003e 66FF3557 000000   		pushw	1f
  33 0045 FF355700 0000     		push	1f
  34 004b FFB30C00 0000     		push	\(1f-.\)\(%ebx\)
  35 0051 FF350600 0000     		push	1f-.
  36                       	# these, and others like them should have no operand size prefix
  37 0057 0F00D1            	1:	lldt	%cx
  38 005a 0F01F0            		lmsw	%ax
  39                       	
  40                       	# Just to make sure these don't become illegal due to over-enthusiastic
  41                       	# register checking
  42 005d 660FBEF8          		movsbw	%al,%di
  43 0061 0FBEC8            		movsbl	%al,%ecx
  44 0064 0FBFC8            		movswl	%ax,%ecx
  45 0067 660FB6F8          		movzbw	%al,%di
  46 006b 0FB6C8            		movzbl	%al,%ecx
  47 006e 0FB7C8            		movzwl	%ax,%ecx
  48                       	
  49 0071 EC                		in	%dx,%al
  50 0072 66ED              		in	%dx,%ax
  51 0074 ED                		in	%dx,%eax
  52 0075 EC                		in	\(%dx\),%al
  53 0076 66ED              		in	\(%dx\),%ax
  54 0078 ED                		in	\(%dx\),%eax
  55 0079 EC                		inb	%dx,%al
  56 007a 66ED              		inw	%dx,%ax
  57 007c ED                		inl	%dx,%eax
  58 007d EC                		inb	%dx
  59 007e 66ED              		inw	%dx
  60 0080 ED                		inl	%dx
  61 0081 E4FF              		inb	\$255
  62 0083 66E502            		inw	\$2
  63 0086 E504              		inl	\$4
  64 0088 EF                		outl	%eax,%dx
  65 0089 E62A              		out	%al, \$42
  66 008b 66E50D            		in	\$13, %ax
  67                       	# These are used in AIX.
  68 008e 66ED              		inw	\(%dx\)
  69 0090 66EF              		outw	\(%dx\)
  70                       	
  71 0092 A4                		movsb
  72 0093 66A7              		cmpsw
  73 0095 AF                		scasl
  74 0096 D7                		xlatb
  75 0097 2EA5              		movsl	%cs:\(%esi\),%es:\(%edi\)
  76 0099 0F9303            		setae	\(%ebx\)
  77 009c 0F9303            		setaeb	\(%ebx\)
  78 009f 0F93C0            		setae	%al
  79                       	
  80                       	#these should give warnings
  81 00a2 0C01              		orb	\$1,%ax
.*Warning:.*
  82 00a4 0C01              		orb	\$1,%eax
.*Warning:.*
  83 00a6 80CB01            		orb	\$1,%bx
.*Warning:.*
  84 00a9 80CB01            		orb	\$1,%ebx
.*Warning:.*
  85 00ac D9C1              		fldl	%st\(1\)
.*Warning:.*
  86 00ae DDD2              		fstl	%st\(2\)
.*Warning:.*
  87 00b0 DDDB              		fstpl	%st\(3\)
.*Warning:.*
  88 00b2 D8D4              		fcoml	%st\(4\)
.*Warning:.*
  89 00b4 D8DD              		fcompl	%st\(5\)
.*Warning:.*
  90 00b6 DEC1              		faddp	%st\(1\),%st
.*Warning:.*
  91 00b8 DECA              		fmulp	%st\(2\),%st
.*Warning:.*
  92 00ba DEE3              		fsubp	%st\(3\),%st
.*Warning:.*
  93 00bc DEEC              		fsubrp	%st\(4\),%st
.*Warning:.*
  94 00be DEF5              		fdivp	%st\(5\),%st
.*Warning:.*
  95 00c0 DEFE              		fdivrp	%st\(6\),%st
.*Warning:.*
  96 00c2 DEC1              		fadd
.*Warning:.*
  97 00c4 DEE1              		fsub
.*Warning:.*
  98 00c6 DEC9              		fmul
.*Warning:.*
  99 00c8 DEF1              		fdiv
.*Warning:.*
 100 00ca DEE9              		fsubr
.*Warning:.*
 101 00cc DEF9              		fdivr
.*Warning:.*
 102                       	#these should all be legal
 103 00ce 0FA31556 341200   		btl	%edx, 0x123456
 104 00d5 0FA3D0            		btl	%edx, %eax
 105 00d8 0C01              		orb	\$1,%al
 106 00da 80CB01            		orb	\$1,%bl
 107 00dd A1110000 00       		movl	17,%eax
 108 00e2 A1110000 00       		mov	17,%eax
 109 00e7 66ED              		inw	%dx,%ax
 110 00e9 ED                		inl	%dx,%eax
 111 00ea 66ED              		inw	\(%dx\),%ax
 112 00ec ED                		inl	\(%dx\),%eax
 113 00ed EC                		in	\(%dx\),%al
 114 00ee 66ED              		in	\(%dx\),%ax
 115 00f0 ED                		in	\(%dx\),%eax
 116 00f1 0FB61437          		movzbl	\(%edi,%esi\),%edx
 117 00f5 0FB6451C          		movzbl	28\(%ebp\),%eax
 118 00f9 0FB6C0            		movzbl	%al,%eax
 119 00fc 0FB6F1            		movzbl	%cl,%esi
 120 00ff 26D7              		xlat	%es:\(%ebx\)
 121 0101 D7                		xlat
 122 0102 D7                		xlatb
 123 0103 DDD8              	1:	fstp	%st\(0\)
 124 0105 E2FC              		loop	1b
 125 0107 F6F1              		divb    %cl 
 126 0109 66F7F1            		divw    %cx 
 127 010c F7F1              		divl    %ecx
 128 010e F6F1              		div	%cl
 129 0110 66F7F1            		div	%cx
 130 0113 F7F1              		div	%ecx
 131 0115 F6F1              		div	%cl,%al
 132 0117 66F7F1            		div	%cx,%ax
 133 011a F7F1              		div	%ecx,%eax
 134 011c 8EDE              		mov	%si,%ds
 135 011e 8EDE              		movl	%si,%ds		# warning here
.*Warning:.*
 136 0120 1E                		pushl	%ds
 137 0121 1E                		push	%ds
 138 0122 A0000000 00       		mov	0,%al
 139 0127 66A10000 0100     		mov	0x10000,%ax
 140 012d 89C3              		mov	%eax,%ebx
 141 012f 9C                		pushf
 142 0130 9C                		pushfl
 143 0131 669C              		pushfw
 144 0133 9D                		popf
 145 0134 9D                		popfl
 146 0135 669D              		popfw
 147 0137 89341D00 000000   		mov	%esi,\(,%ebx,1\)
 148 013e 80250000 00007F   		andb	\$~0x80,foo
 149                       	
 150                       	#check 16-bit code auto address prefix
 151                       	.code16gcc
 152 0145 67668D95 00FFFFFF 		leal	-256\(%ebp\),%edx
 153 014d 6788857F FFFFFF   		mov	%al,-129\(%ebp\)
 154 0154 67886580          		mov	%ah,-128\(%ebp\)
 155 0158 67668D9D 20F9FFFF 		leal	-1760\(%ebp\),%ebx
 156 0160 67668984 248C0000 		movl	%eax,140\(%esp\)
 156      00
 157                       	
 158                       		# Force a good alignment.
 159 0169 00000000 000000[ 	]*.p2align	4,0
