#objdump: -dr
#name: TIc80 coverage of symbolic condition code values

.*: +file format .*tic80.*

Disassembly of section .text:

00000000 <.text>:
   0:	07 a0 79 01 	bcnd\.a    r7,r5,nev\.b
   4:	07 a0 79 09 	bcnd\.a    r7,r5,gt0\.b
   8:	07 a0 79 11 	bcnd\.a    r7,r5,eq0\.b
   c:	07 a0 79 19 	bcnd\.a    r7,r5,ge0\.b
  10:	07 a0 79 21 	bcnd\.a    r7,r5,lt0\.b
  14:	07 a0 79 29 	bcnd\.a    r7,r5,ne0\.b
  18:	07 a0 79 31 	bcnd\.a    r7,r5,le0\.b
  1c:	07 a0 79 39 	bcnd\.a    r7,r5,alw\.b
  20:	07 a0 79 41 	bcnd\.a    r7,r5,nev\.h
  24:	07 a0 79 49 	bcnd\.a    r7,r5,gt0\.h
  28:	07 a0 79 51 	bcnd\.a    r7,r5,eq0\.h
  2c:	07 a0 79 59 	bcnd\.a    r7,r5,ge0\.h
  30:	07 a0 79 61 	bcnd\.a    r7,r5,lt0\.h
  34:	07 a0 79 69 	bcnd\.a    r7,r5,ne0\.h
  38:	07 a0 79 71 	bcnd\.a    r7,r5,le0\.h
  3c:	07 a0 79 79 	bcnd\.a    r7,r5,alw\.h
  40:	07 a0 79 81 	bcnd\.a    r7,r5,nev\.w
  44:	07 a0 79 89 	bcnd\.a    r7,r5,gt0\.w
  48:	07 a0 79 91 	bcnd\.a    r7,r5,eq0\.w
  4c:	07 a0 79 99 	bcnd\.a    r7,r5,ge0\.w
  50:	07 a0 79 a1 	bcnd\.a    r7,r5,lt0\.w
  54:	07 a0 79 a9 	bcnd\.a    r7,r5,ne0\.w
  58:	07 a0 79 b1 	bcnd\.a    r7,r5,le0\.w
  5c:	07 a0 79 b9 	bcnd\.a    r7,r5,alw\.w
