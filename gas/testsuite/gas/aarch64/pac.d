#objdump: -dr
#as: -march=armv8.3-a

.*:     file .*

Disassembly of section \.text:

0000000000000000 <.*>:
   0:	dac10083 	pacia	x3, x4
   4:	dac103e5 	pacia	x5, sp
   8:	dac10483 	pacib	x3, x4
   c:	dac107e5 	pacib	x5, sp
  10:	dac10883 	pacda	x3, x4
  14:	dac10be5 	pacda	x5, sp
  18:	dac10c83 	pacdb	x3, x4
  1c:	dac10fe5 	pacdb	x5, sp
  20:	dac11083 	autia	x3, x4
  24:	dac113e5 	autia	x5, sp
  28:	dac11483 	autib	x3, x4
  2c:	dac117e5 	autib	x5, sp
  30:	dac11883 	autda	x3, x4
  34:	dac11be5 	autda	x5, sp
  38:	dac11c83 	autdb	x3, x4
  3c:	dac11fe5 	autdb	x5, sp
  40:	dac123e5 	paciza	x5
  44:	dac127e5 	pacizb	x5
  48:	dac12be5 	pacdza	x5
  4c:	dac12fe5 	pacdzb	x5
  50:	dac133e5 	autiza	x5
  54:	dac137e5 	autizb	x5
  58:	dac13be5 	autdza	x5
  5c:	dac13fe5 	autdzb	x5
  60:	dac143e5 	xpaci	x5
  64:	dac147e5 	xpacd	x5
  68:	9ac33041 	pacga	x1, x2, x3
  6c:	9adf3041 	pacga	x1, x2, sp
  70:	d71f0822 	braa	x1, x2
  74:	d71f087f 	braa	x3, sp
  78:	d71f0c22 	brab	x1, x2
  7c:	d71f0c7f 	brab	x3, sp
  80:	d73f0822 	blraa	x1, x2
  84:	d73f087f 	blraa	x3, sp
  88:	d73f0c22 	blrab	x1, x2
  8c:	d73f0c7f 	blrab	x3, sp
  90:	d61f08bf 	braaz	x5
  94:	d61f0cbf 	brabz	x5
  98:	d63f08bf 	blraaz	x5
  9c:	d63f0cbf 	blrabz	x5
  a0:	d65f0bff 	retaa
  a4:	d65f0fff 	retab
  a8:	d69f0bff 	eretaa
  ac:	d69f0fff 	eretab
