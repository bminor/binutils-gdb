GAS LISTING .*


   1                                	.TeXt
   2 0000 8803                       		mov % al , \( % ebx \)
   3 0002 C705D711 00007B00 0000     	        mOvl \$ 123 , 4567
   4 000c 678A787B                   	 ADDr16	mov 123 \( % bx , % si , 1 \) , % bh
   5 0010 FFE0                       		jmp \* % eax
   6 0012 6626FF23                   	foo:	jmpw % es : \* \( % ebx \)
   7                                	
   8 0016 A0500000 00                		mov \( 0x8 \* 0Xa \) , % al
   9 001b B020                       		mov \$ \( 8 \* 4 \) , % al
  10 001d B712                       		mov \$ foo , % bH 
  11 001f B712                       		movb \$ foo , % BH
  12                                	
  13                                	.CODE16
  14 0021 66B81200 0000              		Mov \$ foo , %eAx
  15                                	.Code32
  16 0027 66B81200                   		mov \$ foo , %ax
