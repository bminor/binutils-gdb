#objdump: -dr
#name: TIc80 coverage of symbolic BITNUM values

.*: +file format .*tic80.*

Disassembly of section .text:

00000000 <.text>:
   0:	0a 40 39 fa 	bbo       r10,r8,eq\.b
   4:	0a 40 39 f2 	bbo       r10,r8,ne\.b
   8:	0a 40 39 ea 	bbo       r10,r8,gt\.b
   c:	0a 40 39 e2 	bbo       r10,r8,le\.b
  10:	0a 40 39 da 	bbo       r10,r8,lt\.b
  14:	0a 40 39 d2 	bbo       r10,r8,ge\.b
  18:	0a 40 39 ca 	bbo       r10,r8,hi\.b
  1c:	0a 40 39 c2 	bbo       r10,r8,ls\.b
  20:	0a 40 39 ba 	bbo       r10,r8,lo\.b
  24:	0a 40 39 b2 	bbo       r10,r8,hs\.b
  28:	0a 40 39 aa 	bbo       r10,r8,eq\.h
  2c:	0a 40 39 a2 	bbo       r10,r8,ne\.h
  30:	0a 40 39 9a 	bbo       r10,r8,gt\.h
  34:	0a 40 39 92 	bbo       r10,r8,le\.h
  38:	0a 40 39 8a 	bbo       r10,r8,lt\.h
  3c:	0a 40 39 82 	bbo       r10,r8,ge\.h
  40:	0a 40 39 7a 	bbo       r10,r8,hi\.h
  44:	0a 40 39 72 	bbo       r10,r8,ls\.h
  48:	0a 40 39 6a 	bbo       r10,r8,lo\.h
  4c:	0a 40 39 62 	bbo       r10,r8,hs\.h
  50:	0a 40 39 5a 	bbo       r10,r8,eq\.w
  54:	0a 40 39 52 	bbo       r10,r8,ne\.w
  58:	0a 40 39 4a 	bbo       r10,r8,gt\.w
  5c:	0a 40 39 42 	bbo       r10,r8,le\.w
  60:	0a 40 39 3a 	bbo       r10,r8,lt\.w
  64:	0a 40 39 32 	bbo       r10,r8,ge\.w
  68:	0a 40 39 2a 	bbo       r10,r8,hi\.w
  6c:	0a 40 39 22 	bbo       r10,r8,ls\.w
  70:	0a 40 39 1a 	bbo       r10,r8,lo\.w
  74:	0a 40 39 12 	bbo       r10,r8,hs\.w
