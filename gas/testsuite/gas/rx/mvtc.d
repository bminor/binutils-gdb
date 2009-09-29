#objdump: -dr

dump\.o:     file format .*


Disassembly of section \.text:

00000000 <\.text>:
   0:	fd 77 00 80                   	mvtc	#-128, psw
   4:	fd 77 03 80                   	mvtc	#-128, fpsw
   8:	fd 77 02 80                   	mvtc	#-128, usp
   c:	fd 77 0a 80                   	mvtc	#-128, isp
  10:	fd 77 0c 80                   	mvtc	#-128, intb
  14:	fd 77 08 80                   	mvtc	#-128, bpsw
  18:	fd 77 09 80                   	mvtc	#-128, bpc
  1c:	fd 77 0b 80                   	mvtc	#-128, fintv
  20:	fd 77 04 80                   	mvtc	#-128, cpen
  24:	fd 77 00 7f                   	mvtc	#127, psw
  28:	fd 77 03 7f                   	mvtc	#127, fpsw
  2c:	fd 77 02 7f                   	mvtc	#127, usp
  30:	fd 77 0a 7f                   	mvtc	#127, isp
  34:	fd 77 0c 7f                   	mvtc	#127, intb
  38:	fd 77 08 7f                   	mvtc	#127, bpsw
  3c:	fd 77 09 7f                   	mvtc	#127, bpc
  40:	fd 77 0b 7f                   	mvtc	#127, fintv
  44:	fd 77 04 7f                   	mvtc	#127, cpen
  48:	fd 7b 00 00 80                	mvtc	#0xffff8000, psw
  4d:	fd 7b 03 00 80                	mvtc	#0xffff8000, fpsw
  52:	fd 7b 02 00 80                	mvtc	#0xffff8000, usp
  57:	fd 7b 0a 00 80                	mvtc	#0xffff8000, isp
  5c:	fd 7b 0c 00 80                	mvtc	#0xffff8000, intb
  61:	fd 7b 08 00 80                	mvtc	#0xffff8000, bpsw
  66:	fd 7b 09 00 80                	mvtc	#0xffff8000, bpc
  6b:	fd 7b 0b 00 80                	mvtc	#0xffff8000, fintv
  70:	fd 7b 04 00 80                	mvtc	#0xffff8000, cpen
  75:	fd 7f 00 00 80 00             	mvtc	#0x8000, psw
  7b:	fd 7f 03 00 80 00             	mvtc	#0x8000, fpsw
  81:	fd 7f 02 00 80 00             	mvtc	#0x8000, usp
  87:	fd 7f 0a 00 80 00             	mvtc	#0x8000, isp
  8d:	fd 7f 0c 00 80 00             	mvtc	#0x8000, intb
  93:	fd 7f 08 00 80 00             	mvtc	#0x8000, bpsw
  99:	fd 7f 09 00 80 00             	mvtc	#0x8000, bpc
  9f:	fd 7f 0b 00 80 00             	mvtc	#0x8000, fintv
  a5:	fd 7f 04 00 80 00             	mvtc	#0x8000, cpen
  ab:	fd 7f 00 00 00 80             	mvtc	#0xff800000, psw
  b1:	fd 7f 03 00 00 80             	mvtc	#0xff800000, fpsw
  b7:	fd 7f 02 00 00 80             	mvtc	#0xff800000, usp
  bd:	fd 7f 0a 00 00 80             	mvtc	#0xff800000, isp
  c3:	fd 7f 0c 00 00 80             	mvtc	#0xff800000, intb
  c9:	fd 7f 08 00 00 80             	mvtc	#0xff800000, bpsw
  cf:	fd 7f 09 00 00 80             	mvtc	#0xff800000, bpc
  d5:	fd 7f 0b 00 00 80             	mvtc	#0xff800000, fintv
  db:	fd 7f 04 00 00 80             	mvtc	#0xff800000, cpen
  e1:	fd 7f 00 ff ff 7f             	mvtc	#0x7fffff, psw
  e7:	fd 7f 03 ff ff 7f             	mvtc	#0x7fffff, fpsw
  ed:	fd 7f 02 ff ff 7f             	mvtc	#0x7fffff, usp
  f3:	fd 7f 0a ff ff 7f             	mvtc	#0x7fffff, isp
  f9:	fd 7f 0c ff ff 7f             	mvtc	#0x7fffff, intb
  ff:	fd 7f 08 ff ff 7f             	mvtc	#0x7fffff, bpsw
 105:	fd 7f 09 ff ff 7f             	mvtc	#0x7fffff, bpc
 10b:	fd 7f 0b ff ff 7f             	mvtc	#0x7fffff, fintv
 111:	fd 7f 04 ff ff 7f             	mvtc	#0x7fffff, cpen
 117:	fd 73 00 00 00 00 80          	mvtc	#0x80000000, psw
 11e:	fd 73 03 00 00 00 80          	mvtc	#0x80000000, fpsw
 125:	fd 73 02 00 00 00 80          	mvtc	#0x80000000, usp
 12c:	fd 73 0a 00 00 00 80          	mvtc	#0x80000000, isp
 133:	fd 73 0c 00 00 00 80          	mvtc	#0x80000000, intb
 13a:	fd 73 08 00 00 00 80          	mvtc	#0x80000000, bpsw
 141:	fd 73 09 00 00 00 80          	mvtc	#0x80000000, bpc
 148:	fd 73 0b 00 00 00 80          	mvtc	#0x80000000, fintv
 14f:	fd 73 04 00 00 00 80          	mvtc	#0x80000000, cpen
 156:	fd 73 00 ff ff ff 7f          	mvtc	#0x7fffffff, psw
 15d:	fd 73 03 ff ff ff 7f          	mvtc	#0x7fffffff, fpsw
 164:	fd 73 02 ff ff ff 7f          	mvtc	#0x7fffffff, usp
 16b:	fd 73 0a ff ff ff 7f          	mvtc	#0x7fffffff, isp
 172:	fd 73 0c ff ff ff 7f          	mvtc	#0x7fffffff, intb
 179:	fd 73 08 ff ff ff 7f          	mvtc	#0x7fffffff, bpsw
 180:	fd 73 09 ff ff ff 7f          	mvtc	#0x7fffffff, bpc
 187:	fd 73 0b ff ff ff 7f          	mvtc	#0x7fffffff, fintv
 18e:	fd 73 04 ff ff ff 7f          	mvtc	#0x7fffffff, cpen
 195:	fd 68 00                      	mvtc	r0, psw
 198:	fd 68 03                      	mvtc	r0, fpsw
 19b:	fd 68 02                      	mvtc	r0, usp
 19e:	fd 68 0a                      	mvtc	r0, isp
 1a1:	fd 68 0c                      	mvtc	r0, intb
 1a4:	fd 68 08                      	mvtc	r0, bpsw
 1a7:	fd 68 09                      	mvtc	r0, bpc
 1aa:	fd 68 0b                      	mvtc	r0, fintv
 1ad:	fd 68 04                      	mvtc	r0, cpen
 1b0:	fd 68 f0                      	mvtc	r15, psw
 1b3:	fd 68 f3                      	mvtc	r15, fpsw
 1b6:	fd 68 f2                      	mvtc	r15, usp
 1b9:	fd 68 fa                      	mvtc	r15, isp
 1bc:	fd 68 fc                      	mvtc	r15, intb
 1bf:	fd 68 f8                      	mvtc	r15, bpsw
 1c2:	fd 68 f9                      	mvtc	r15, bpc
 1c5:	fd 68 fb                      	mvtc	r15, fintv
 1c8:	fd 68 f4                      	mvtc	r15, cpen
