#as: -mppc -me6500
#objdump: -dr -Me6500
#name: Power E6500 tests

.*: +file format elf(32)?(64)?-powerpc.*

Disassembly of section \.text:

0+00 <start>:
   0:	10 01 10 c0 	vabsdub v0,v1,v2
   4:	10 01 11 00 	vabsduh v0,v1,v2
   8:	10 01 11 40 	vabsduw v0,v1,v2
   c:	7c 01 10 dc 	mvidsplt v0,r1,r2
  10:	7c 01 11 1c 	mviwsplt v0,r1,r2
  14:	7c 00 12 0a 	lvexbx  v0,0,r2
  18:	7c 01 12 0a 	lvexbx  v0,r1,r2
  1c:	7c 00 12 4a 	lvexhx  v0,0,r2
  20:	7c 01 12 4a 	lvexhx  v0,r1,r2
  24:	7c 00 12 8a 	lvexwx  v0,0,r2
  28:	7c 01 12 8a 	lvexwx  v0,r1,r2
  2c:	7c 00 13 0a 	stvexbx v0,0,r2
  30:	7c 01 13 0a 	stvexbx v0,r1,r2
  34:	7c 00 13 4a 	stvexhx v0,0,r2
  38:	7c 01 13 4a 	stvexhx v0,r1,r2
  3c:	7c 00 13 8a 	stvexwx v0,0,r2
  40:	7c 01 13 8a 	stvexwx v0,r1,r2
  44:	7c 00 12 4e 	lvepx   v0,0,r2
  48:	7c 01 12 4e 	lvepx   v0,r1,r2
  4c:	7c 00 12 0e 	lvepxl  v0,0,r2
  50:	7c 01 12 0e 	lvepxl  v0,r1,r2
  54:	7c 00 16 4e 	stvepx  v0,0,r2
  58:	7c 01 16 4e 	stvepx  v0,r1,r2
  5c:	7c 00 16 0e 	stvepxl v0,0,r2
  60:	7c 01 16 0e 	stvepxl v0,r1,r2
  64:	7c 00 14 8a 	lvtlx   v0,0,r2
  68:	7c 01 14 8a 	lvtlx   v0,r1,r2
  6c:	7c 00 16 8a 	lvtlxl  v0,0,r2
  70:	7c 01 16 8a 	lvtlxl  v0,r1,r2
  74:	7c 00 14 4a 	lvtrx   v0,0,r2
  78:	7c 01 14 4a 	lvtrx   v0,r1,r2
  7c:	7c 00 16 4a 	lvtrxl  v0,0,r2
  80:	7c 01 16 4a 	lvtrxl  v0,r1,r2
  84:	7c 00 15 8a 	stvflx  v0,0,r2
  88:	7c 01 15 8a 	stvflx  v0,r1,r2
  8c:	7c 00 17 8a 	stvflxl v0,0,r2
  90:	7c 01 17 8a 	stvflxl v0,r1,r2
  94:	7c 00 15 4a 	stvfrx  v0,0,r2
  98:	7c 01 15 4a 	stvfrx  v0,r1,r2
  9c:	7c 00 17 4a 	stvfrxl v0,0,r2
  a0:	7c 01 17 4a 	stvfrxl v0,r1,r2
  a4:	7c 00 14 ca 	lvswx   v0,0,r2
  a8:	7c 01 14 ca 	lvswx   v0,r1,r2
  ac:	7c 00 16 ca 	lvswxl  v0,0,r2
  b0:	7c 01 16 ca 	lvswxl  v0,r1,r2
  b4:	7c 00 15 ca 	stvswx  v0,0,r2
  b8:	7c 01 15 ca 	stvswx  v0,r1,r2
  bc:	7c 00 17 ca 	stvswxl v0,0,r2
  c0:	7c 01 17 ca 	stvswxl v0,r1,r2
  c4:	7c 00 16 0a 	lvsm    v0,0,r2
  c8:	7c 01 16 0a 	lvsm    v0,r1,r2
  cc:	7f 5a d3 78 	miso
  d0:	7c 00 04 ac 	sync    
  d4:	7c 00 04 ac 	sync    
  d8:	7c 20 04 ac 	lwsync
  dc:	7c 00 04 ac 	sync    
  e0:	7c 07 04 ac 	sync    0,7
  e4:	7c 28 04 ac 	sync    1,8
  e8:	7c 00 00 c3 	dni     0,0
  ec:	7f ff 00 c3 	dni     31,31
  f0:	7c 40 0b 4d 	dcblq.  2,0,r1
  f4:	7c 43 0b 4d 	dcblq.  2,r3,r1
  f8:	7c 40 09 8d 	icblq.  2,0,r1
  fc:	7c 43 09 8d 	icblq.  2,r3,r1
 100:	7c 10 02 dc 	mftmr   r0,16
 104:	7c 10 03 dc 	mttmr   16,r0
