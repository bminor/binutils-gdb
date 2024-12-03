#objdump: -dw -Mintel
#name: x86_64 AVX10.2/256 BF16 insns (Intel disassembly)
#source: x86-64-avx10_2-256-bf16.s

.*: +file format .*

Disassembly of section \.text:

#...
[a-f0-9]+ <_intel>:
\s*[a-f0-9]+:\s*62 05 15 20 58 f4\s+vaddnepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 58 f4\s+vaddnepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 58 b4 f5 00 00 00 10\s+vaddnepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 58 31\s+vaddnepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 58 71 7f\s+vaddnepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 58 72 80\s+vaddnepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 58 b4 f5 00 00 00 10\s+vaddnepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 58 31\s+vaddnepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 58 71 7f\s+vaddnepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 58 72 80\s+vaddnepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 15 20 5e f4\s+vdivnepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 5e f4\s+vdivnepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 5e b4 f5 00 00 00 10\s+vdivnepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 5e 31\s+vdivnepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 5e 71 7f\s+vdivnepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 5e 72 80\s+vdivnepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 5e b4 f5 00 00 00 10\s+vdivnepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 5e 31\s+vdivnepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 5e 71 7f\s+vdivnepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 5e 72 80\s+vdivnepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 15 20 5f f4\s+vmaxpbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 5f f4\s+vmaxpbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 5f b4 f5 00 00 00 10\s+vmaxpbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 5f 31\s+vmaxpbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 5f 71 7f\s+vmaxpbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 5f 72 80\s+vmaxpbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 5f b4 f5 00 00 00 10\s+vmaxpbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 5f 31\s+vmaxpbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 5f 71 7f\s+vmaxpbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 5f 72 80\s+vmaxpbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 15 20 5d f4\s+vminpbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 5d f4\s+vminpbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 5d b4 f5 00 00 00 10\s+vminpbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 5d 31\s+vminpbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 5d 71 7f\s+vminpbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 5d 72 80\s+vminpbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 5d b4 f5 00 00 00 10\s+vminpbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 5d 31\s+vminpbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 5d 71 7f\s+vminpbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 5d 72 80\s+vminpbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 15 20 59 f4\s+vmulnepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 59 f4\s+vmulnepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 59 b4 f5 00 00 00 10\s+vmulnepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 59 31\s+vmulnepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 59 71 7f\s+vmulnepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 59 72 80\s+vmulnepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 59 b4 f5 00 00 00 10\s+vmulnepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 59 31\s+vmulnepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 59 71 7f\s+vmulnepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 59 72 80\s+vmulnepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 2c f4\s+vscalefnepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 2c f4\s+vscalefnepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 2c b4 f5 00 00 00 10\s+vscalefnepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 2c 31\s+vscalefnepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 2c 71 7f\s+vscalefnepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 2c 72 80\s+vscalefnepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 2c b4 f5 00 00 00 10\s+vscalefnepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 2c 31\s+vscalefnepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 2c 71 7f\s+vscalefnepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 2c 72 80\s+vscalefnepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 15 20 5c f4\s+vsubnepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 05 15 00 5c f4\s+vsubnepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 25 15 27 5c b4 f5 00 00 00 10\s+vsubnepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 30 5c 31\s+vsubnepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 20 5c 71 7f\s+vsubnepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 15 b7 5c 72 80\s+vsubnepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 15 07 5c b4 f5 00 00 00 10\s+vsubnepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 15 10 5c 31\s+vsubnepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 15 00 5c 71 7f\s+vsubnepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 15 97 5c 72 80\s+vsubnepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 98 f4\s+vfmadd132nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 98 f4\s+vfmadd132nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 98 b4 f5 00 00 00 10\s+vfmadd132nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 98 31\s+vfmadd132nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 98 71 7f\s+vfmadd132nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 98 72 80\s+vfmadd132nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 98 b4 f5 00 00 00 10\s+vfmadd132nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 98 31\s+vfmadd132nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 98 71 7f\s+vfmadd132nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 98 72 80\s+vfmadd132nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 a8 f4\s+vfmadd213nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 a8 f4\s+vfmadd213nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 a8 b4 f5 00 00 00 10\s+vfmadd213nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 a8 31\s+vfmadd213nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 a8 71 7f\s+vfmadd213nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 a8 72 80\s+vfmadd213nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 a8 b4 f5 00 00 00 10\s+vfmadd213nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 a8 31\s+vfmadd213nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 a8 71 7f\s+vfmadd213nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 a8 72 80\s+vfmadd213nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 b8 f4\s+vfmadd231nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 b8 f4\s+vfmadd231nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 b8 b4 f5 00 00 00 10\s+vfmadd231nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 b8 31\s+vfmadd231nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 b8 71 7f\s+vfmadd231nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 b8 72 80\s+vfmadd231nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 b8 b4 f5 00 00 00 10\s+vfmadd231nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 b8 31\s+vfmadd231nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 b8 71 7f\s+vfmadd231nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 b8 72 80\s+vfmadd231nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 9a f4\s+vfmsub132nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 9a f4\s+vfmsub132nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 9a b4 f5 00 00 00 10\s+vfmsub132nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 9a 31\s+vfmsub132nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 9a 71 7f\s+vfmsub132nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 9a 72 80\s+vfmsub132nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 9a b4 f5 00 00 00 10\s+vfmsub132nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 9a 31\s+vfmsub132nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 9a 71 7f\s+vfmsub132nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 9a 72 80\s+vfmsub132nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 aa f4\s+vfmsub213nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 aa f4\s+vfmsub213nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 aa b4 f5 00 00 00 10\s+vfmsub213nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 aa 31\s+vfmsub213nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 aa 71 7f\s+vfmsub213nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 aa 72 80\s+vfmsub213nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 aa b4 f5 00 00 00 10\s+vfmsub213nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 aa 31\s+vfmsub213nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 aa 71 7f\s+vfmsub213nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 aa 72 80\s+vfmsub213nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 ba f4\s+vfmsub231nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 ba f4\s+vfmsub231nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 ba b4 f5 00 00 00 10\s+vfmsub231nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 ba 31\s+vfmsub231nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 ba 71 7f\s+vfmsub231nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 ba 72 80\s+vfmsub231nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 ba b4 f5 00 00 00 10\s+vfmsub231nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 ba 31\s+vfmsub231nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 ba 71 7f\s+vfmsub231nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 ba 72 80\s+vfmsub231nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 9c f4\s+vfnmadd132nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 9c f4\s+vfnmadd132nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 9c b4 f5 00 00 00 10\s+vfnmadd132nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 9c 31\s+vfnmadd132nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 9c 71 7f\s+vfnmadd132nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 9c 72 80\s+vfnmadd132nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 9c b4 f5 00 00 00 10\s+vfnmadd132nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 9c 31\s+vfnmadd132nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 9c 71 7f\s+vfnmadd132nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 9c 72 80\s+vfnmadd132nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 ac f4\s+vfnmadd213nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 ac f4\s+vfnmadd213nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 ac b4 f5 00 00 00 10\s+vfnmadd213nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 ac 31\s+vfnmadd213nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 ac 71 7f\s+vfnmadd213nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 ac 72 80\s+vfnmadd213nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 ac b4 f5 00 00 00 10\s+vfnmadd213nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 ac 31\s+vfnmadd213nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 ac 71 7f\s+vfnmadd213nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 ac 72 80\s+vfnmadd213nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 bc f4\s+vfnmadd231nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 bc f4\s+vfnmadd231nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 bc b4 f5 00 00 00 10\s+vfnmadd231nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 bc 31\s+vfnmadd231nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 bc 71 7f\s+vfnmadd231nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 bc 72 80\s+vfnmadd231nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 bc b4 f5 00 00 00 10\s+vfnmadd231nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 bc 31\s+vfnmadd231nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 bc 71 7f\s+vfnmadd231nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 bc 72 80\s+vfnmadd231nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 9e f4\s+vfnmsub132nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 9e f4\s+vfnmsub132nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 9e b4 f5 00 00 00 10\s+vfnmsub132nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 9e 31\s+vfnmsub132nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 9e 71 7f\s+vfnmsub132nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 9e 72 80\s+vfnmsub132nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 9e b4 f5 00 00 00 10\s+vfnmsub132nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 9e 31\s+vfnmsub132nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 9e 71 7f\s+vfnmsub132nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 9e 72 80\s+vfnmsub132nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 ae f4\s+vfnmsub213nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 ae f4\s+vfnmsub213nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 ae b4 f5 00 00 00 10\s+vfnmsub213nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 ae 31\s+vfnmsub213nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 ae 71 7f\s+vfnmsub213nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 ae 72 80\s+vfnmsub213nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 ae b4 f5 00 00 00 10\s+vfnmsub213nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 ae 31\s+vfnmsub213nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 ae 71 7f\s+vfnmsub213nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 ae 72 80\s+vfnmsub213nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 14 20 be f4\s+vfnmsub231nepbf16 ymm30,ymm29,ymm28
\s*[a-f0-9]+:\s*62 06 14 00 be f4\s+vfnmsub231nepbf16 xmm30,xmm29,xmm28
\s*[a-f0-9]+:\s*62 26 14 27 be b4 f5 00 00 00 10\s+vfnmsub231nepbf16 ymm30\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 30 be 31\s+vfnmsub231nepbf16 ymm30,ymm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 20 be 71 7f\s+vfnmsub231nepbf16 ymm30,ymm29,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 14 b7 be 72 80\s+vfnmsub231nepbf16 ymm30\{k7\}\{z\},ymm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 14 07 be b4 f5 00 00 00 10\s+vfnmsub231nepbf16 xmm30\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 14 10 be 31\s+vfnmsub231nepbf16 xmm30,xmm29,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 14 00 be 71 7f\s+vfnmsub231nepbf16 xmm30,xmm29,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 14 97 be 72 80\s+vfnmsub231nepbf16 xmm30\{k7\}\{z\},xmm29,WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 7c 08 42 f5\s+vgetexppbf16 xmm30,xmm29
\s*[a-f0-9]+:\s*62 06 7c 28 42 f5\s+vgetexppbf16 ymm30,ymm29
\s*[a-f0-9]+:\s*62 26 7c 0f 42 b4 f5 00 00 00 10\s+vgetexppbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 18 42 31\s+vgetexppbf16 xmm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 08 42 71 7f\s+vgetexppbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 7c 9f 42 72 80\s+vgetexppbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 7c 2f 42 b4 f5 00 00 00 10\s+vgetexppbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 38 42 31\s+vgetexppbf16 ymm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 28 42 71 7f\s+vgetexppbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 7c bf 42 72 80\s+vgetexppbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 7c 08 4c f5\s+vrcppbf16 xmm30,xmm29
\s*[a-f0-9]+:\s*62 06 7c 28 4c f5\s+vrcppbf16 ymm30,ymm29
\s*[a-f0-9]+:\s*62 26 7c 0f 4c b4 f5 00 00 00 10\s+vrcppbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 18 4c 31\s+vrcppbf16 xmm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 08 4c 71 7f\s+vrcppbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 7c 9f 4c 72 80\s+vrcppbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 7c 2f 4c b4 f5 00 00 00 10\s+vrcppbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 38 4c 31\s+vrcppbf16 ymm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 28 4c 71 7f\s+vrcppbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 7c bf 4c 72 80\s+vrcppbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 06 7c 08 4e f5\s+vrsqrtpbf16 xmm30,xmm29
\s*[a-f0-9]+:\s*62 06 7c 28 4e f5\s+vrsqrtpbf16 ymm30,ymm29
\s*[a-f0-9]+:\s*62 26 7c 0f 4e b4 f5 00 00 00 10\s+vrsqrtpbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 18 4e 31\s+vrsqrtpbf16 xmm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 08 4e 71 7f\s+vrsqrtpbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 66 7c 9f 4e 72 80\s+vrsqrtpbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 26 7c 2f 4e b4 f5 00 00 00 10\s+vrsqrtpbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 46 7c 38 4e 31\s+vrsqrtpbf16 ymm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 66 7c 28 4e 71 7f\s+vrsqrtpbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 66 7c bf 4e 72 80\s+vrsqrtpbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 05 7d 08 51 f5\s+vsqrtnepbf16 xmm30,xmm29
\s*[a-f0-9]+:\s*62 05 7d 28 51 f5\s+vsqrtnepbf16 ymm30,ymm29
\s*[a-f0-9]+:\s*62 25 7d 0f 51 b4 f5 00 00 00 10\s+vsqrtnepbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 7d 18 51 31\s+vsqrtnepbf16 xmm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 7d 08 51 71 7f\s+vsqrtnepbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\]
\s*[a-f0-9]+:\s*62 65 7d 9f 51 72 80\s+vsqrtnepbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 25 7d 2f 51 b4 f5 00 00 00 10\s+vsqrtnepbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 7d 38 51 31\s+vsqrtnepbf16 ymm30,WORD BCST \[r9\]
\s*[a-f0-9]+:\s*62 65 7d 28 51 71 7f\s+vsqrtnepbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\]
\s*[a-f0-9]+:\s*62 65 7d bf 51 72 80\s+vsqrtnepbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 03 7f 28 26 f5 7b\s+vgetmantpbf16 ymm30,ymm29,0x7b
\s*[a-f0-9]+:\s*62 03 7f 08 26 f5 7b\s+vgetmantpbf16 xmm30,xmm29,0x7b
\s*[a-f0-9]+:\s*62 23 7f 0f 26 b4 f5 00 00 00 10 7b\s+vgetmantpbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 18 26 31 7b\s+vgetmantpbf16 xmm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 08 26 71 7f 7b\s+vgetmantpbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 9f 26 72 80 7b\s+vgetmantpbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 23 7f 2f 26 b4 f5 00 00 00 10 7b\s+vgetmantpbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 38 26 31 7b\s+vgetmantpbf16 ymm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 28 26 71 7f 7b\s+vgetmantpbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f bf 26 72 80 7b\s+vgetmantpbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 03 7f 28 56 f5 7b\s+vreducenepbf16 ymm30,ymm29,0x7b
\s*[a-f0-9]+:\s*62 03 7f 08 56 f5 7b\s+vreducenepbf16 xmm30,xmm29,0x7b
\s*[a-f0-9]+:\s*62 23 7f 0f 56 b4 f5 00 00 00 10 7b\s+vreducenepbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 18 56 31 7b\s+vreducenepbf16 xmm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 08 56 71 7f 7b\s+vreducenepbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 9f 56 72 80 7b\s+vreducenepbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 23 7f 2f 56 b4 f5 00 00 00 10 7b\s+vreducenepbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 38 56 31 7b\s+vreducenepbf16 ymm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 28 56 71 7f 7b\s+vreducenepbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f bf 56 72 80 7b\s+vreducenepbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 03 7f 28 08 f5 7b\s+vrndscalenepbf16 ymm30,ymm29,0x7b
\s*[a-f0-9]+:\s*62 03 7f 08 08 f5 7b\s+vrndscalenepbf16 xmm30,xmm29,0x7b
\s*[a-f0-9]+:\s*62 23 7f 0f 08 b4 f5 00 00 00 10 7b\s+vrndscalenepbf16 xmm30\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 18 08 31 7b\s+vrndscalenepbf16 xmm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 08 08 71 7f 7b\s+vrndscalenepbf16 xmm30,XMMWORD PTR \[rcx\+0x7f0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 9f 08 72 80 7b\s+vrndscalenepbf16 xmm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 23 7f 2f 08 b4 f5 00 00 00 10 7b\s+vrndscalenepbf16 ymm30\{k7\},YMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 43 7f 38 08 31 7b\s+vrndscalenepbf16 ymm30,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 63 7f 28 08 71 7f 7b\s+vrndscalenepbf16 ymm30,YMMWORD PTR \[rcx\+0xfe0\],0x7b
\s*[a-f0-9]+:\s*62 63 7f bf 08 72 80 7b\s+vrndscalenepbf16 ymm30\{k7\}\{z\},WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 93 17 20 c2 ec 7b\s+vcmppbf16 k5,ymm29,ymm28,0x7b
\s*[a-f0-9]+:\s*62 93 17 00 c2 ec 7b\s+vcmppbf16 k5,xmm29,xmm28,0x7b
\s*[a-f0-9]+:\s*62 b3 17 07 c2 ac f5 00 00 00 10 7b\s+vcmppbf16 k5\{k7\},xmm29,XMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 d3 17 10 c2 29 7b\s+vcmppbf16 k5,xmm29,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 f3 17 00 c2 69 7f 7b\s+vcmppbf16 k5,xmm29,XMMWORD PTR \[rcx\+0x7f0\],0x7b
\s*[a-f0-9]+:\s*62 f3 17 17 c2 6a 80 7b\s+vcmppbf16 k5\{k7\},xmm29,WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 b3 17 27 c2 ac f5 00 00 00 10 7b\s+vcmppbf16 k5\{k7\},ymm29,YMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 d3 17 30 c2 29 7b\s+vcmppbf16 k5,ymm29,WORD BCST \[r9\],0x7b
\s*[a-f0-9]+:\s*62 f3 17 20 c2 69 7f 7b\s+vcmppbf16 k5,ymm29,YMMWORD PTR \[rcx\+0xfe0\],0x7b
\s*[a-f0-9]+:\s*62 f3 17 37 c2 6a 80 7b\s+vcmppbf16 k5\{k7\},ymm29,WORD BCST \[rdx-0x100\],0x7b
\s*[a-f0-9]+:\s*62 05 7d 08 2f f5\s+vcomsbf16 xmm30,xmm29
\s*[a-f0-9]+:\s*62 25 7d 08 2f b4 f5 00 00 00 10\s+vcomsbf16 xmm30,WORD PTR \[rbp\+r14\*8\+0x10000000\]
\s*[a-f0-9]+:\s*62 45 7d 08 2f 31\s+vcomsbf16 xmm30,WORD PTR \[r9\]
\s*[a-f0-9]+:\s*62 65 7d 08 2f 71 7f\s+vcomsbf16 xmm30,WORD PTR \[rcx\+0xfe\]
\s*[a-f0-9]+:\s*62 65 7d 08 2f 72 80\s+vcomsbf16 xmm30,WORD PTR \[rdx-0x100\]
\s*[a-f0-9]+:\s*62 93 7f 28 66 ed 7b\s+vfpclasspbf16 k5,ymm29,0x7b
\s*[a-f0-9]+:\s*62 93 7f 08 66 ed 7b\s+vfpclasspbf16 k5,xmm29,0x7b
\s*[a-f0-9]+:\s*62 b3 7f 0f 66 ac f5 00 00 00 10 7b\s+vfpclasspbf16 k5\{k7\},XMMWORD PTR \[rbp\+r14\*8\+0x10000000\],0x7b
\s*[a-f0-9]+:\s*62 d3 7f 18 66 29 7b\s+vfpclasspbf16 k5,WORD BCST \[r9\]\{1to8\},0x7b
\s*[a-f0-9]+:\s*62 f3 7f 08 66 69 7f 7b\s+vfpclasspbf16 k5,XMMWORD PTR \[rcx\+0x7f0\],0x7b
\s*[a-f0-9]+:\s*62 f3 7f 1f 66 6a 80 7b\s+vfpclasspbf16 k5\{k7\},WORD BCST \[rdx-0x100\]\{1to8\},0x7b
\s*[a-f0-9]+:\s*62 d3 7f 38 66 29 7b\s+vfpclasspbf16 k5,WORD BCST \[r9\]\{1to16\},0x7b
\s*[a-f0-9]+:\s*62 f3 7f 28 66 69 7f 7b\s+vfpclasspbf16 k5,YMMWORD PTR \[rcx\+0xfe0\],0x7b
\s*[a-f0-9]+:\s*62 f3 7f 3f 66 6a 80 7b\s+vfpclasspbf16 k5\{k7\},WORD BCST \[rdx-0x100\]\{1to16\},0x7b
#pass
