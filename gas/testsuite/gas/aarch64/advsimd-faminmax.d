#objdump: -dr
#as: -march=armv8-a+faminmax

.*:     file format .*

Disassembly of section \.text:

0+ <.*>:
   0:	0ec01c00 	famax	v0.4h, v0.4h, v0.4h
   4:	0ec01c1f 	famax	v31.4h, v0.4h, v0.4h
   8:	0ec01fe0 	famax	v0.4h, v31.4h, v0.4h
   c:	0edf1c00 	famax	v0.4h, v0.4h, v31.4h
  10:	0edb1eb1 	famax	v17.4h, v21.4h, v27.4h
  14:	4ec01c00 	famax	v0.8h, v0.8h, v0.8h
  18:	4ec01c1f 	famax	v31.8h, v0.8h, v0.8h
  1c:	4ec01fe0 	famax	v0.8h, v31.8h, v0.8h
  20:	4edf1c00 	famax	v0.8h, v0.8h, v31.8h
  24:	4ec41ce2 	famax	v2.8h, v7.8h, v4.8h
  28:	0ea0dc00 	famax	v0.2s, v0.2s, v0.2s
  2c:	0ea0dc1f 	famax	v31.2s, v0.2s, v0.2s
  30:	0ea0dfe0 	famax	v0.2s, v31.2s, v0.2s
  34:	0ebfdc00 	famax	v0.2s, v0.2s, v31.2s
  38:	0eb3dc23 	famax	v3.2s, v1.2s, v19.2s
  3c:	4ea0dc00 	famax	v0.4s, v0.4s, v0.4s
  40:	4ea0dc1f 	famax	v31.4s, v0.4s, v0.4s
  44:	4ea0dfe0 	famax	v0.4s, v31.4s, v0.4s
  48:	4ebfdc00 	famax	v0.4s, v0.4s, v31.4s
  4c:	4ea7dd09 	famax	v9.4s, v8.4s, v7.4s
  50:	4ee0dc00 	famax	v0.2d, v0.2d, v0.2d
  54:	4ee0dc1f 	famax	v31.2d, v0.2d, v0.2d
  58:	4ee0dfe0 	famax	v0.2d, v31.2d, v0.2d
  5c:	4effdc00 	famax	v0.2d, v0.2d, v31.2d
  60:	4ef0ddd2 	famax	v18.2d, v14.2d, v16.2d
  64:	0ef3dc23 	.inst	0x0ef3dc23 ; undefined
  68:	2ec01c00 	famin	v0.4h, v0.4h, v0.4h
  6c:	2ec01c1f 	famin	v31.4h, v0.4h, v0.4h
  70:	2ec01fe0 	famin	v0.4h, v31.4h, v0.4h
  74:	2edf1c00 	famin	v0.4h, v0.4h, v31.4h
  78:	2edb1eb1 	famin	v17.4h, v21.4h, v27.4h
  7c:	6ec01c00 	famin	v0.8h, v0.8h, v0.8h
  80:	6ec01c1f 	famin	v31.8h, v0.8h, v0.8h
  84:	6ec01fe0 	famin	v0.8h, v31.8h, v0.8h
  88:	6edf1c00 	famin	v0.8h, v0.8h, v31.8h
  8c:	6ec41ce2 	famin	v2.8h, v7.8h, v4.8h
  90:	2ea0dc00 	famin	v0.2s, v0.2s, v0.2s
  94:	2ea0dc1f 	famin	v31.2s, v0.2s, v0.2s
  98:	2ea0dfe0 	famin	v0.2s, v31.2s, v0.2s
  9c:	2ebfdc00 	famin	v0.2s, v0.2s, v31.2s
  a0:	2eb3dc23 	famin	v3.2s, v1.2s, v19.2s
  a4:	6ea0dc00 	famin	v0.4s, v0.4s, v0.4s
  a8:	6ea0dc1f 	famin	v31.4s, v0.4s, v0.4s
  ac:	6ea0dfe0 	famin	v0.4s, v31.4s, v0.4s
  b0:	6ebfdc00 	famin	v0.4s, v0.4s, v31.4s
  b4:	6ea7dd09 	famin	v9.4s, v8.4s, v7.4s
  b8:	6ee0dc00 	famin	v0.2d, v0.2d, v0.2d
  bc:	6ee0dc1f 	famin	v31.2d, v0.2d, v0.2d
  c0:	6ee0dfe0 	famin	v0.2d, v31.2d, v0.2d
  c4:	6effdc00 	famin	v0.2d, v0.2d, v31.2d
  c8:	6ef0ddd2 	famin	v18.2d, v14.2d, v16.2d
  cc:	2ef3dc23 	.inst	0x2ef3dc23 ; undefined