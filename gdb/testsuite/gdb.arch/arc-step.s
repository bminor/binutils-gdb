_main:
	.global _main
main:	
	.global main

	mov   r0,1		; 32 bit instruction
Lmov:	
	mov_s r0,2		; 16 bit instruction
Lmov_s:	
	b Lb			; 32 bit, no delay slot
	mov r0,3
	nop
Lb:	
	b_s Lb_s		; 16 bit, no delay slot
	mov r0,4
	nop
Lb_s:	
	b.d Lbdotd		; 32 bit, delay slot
	mov r0,5
	nop
Lbdotd:

	bl Lbl
Lj_sdotd:
	j Lj
Lbl:	
	j_s.d [blink]		; 16 bit, delay slot
	mov r0,6
Lj:	
	mov lp_count,3		; zero-overhead loop
	lp ZOLend
ZOLstart:
	add r0,r0,1
ZOLmiddle:
	nop
ZOLend:	
	;; r0 should be 9
	
	
	;; exit(r0)
	;; mov r0,0
	mov r8,1
	trap_s 0
