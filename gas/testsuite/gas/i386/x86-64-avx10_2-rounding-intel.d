#objdump: -dw -Mintel
#name: x86_64 AVX10.2 rounding insns (Intel disassembly)
#source: x86-64-avx10_2-rounding.s

.*: +file format .*

Disassembly of section \.text:

0+ <_start>:
#...
\s*a83:\s*62 91 91 10 c2 ec 7b\s+vcmppd k5,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 91 91 17 c2 ec 7b\s+vcmppd k5\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 02 f9 18 42 f5\s+vgetexppd ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 02 f9 1f 42 f5\s+vgetexppd ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 02 f9 9f 42 f5\s+vgetexppd ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 51 f5\s+vsqrtpd ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f9 3f 51 f5\s+vsqrtpd ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f9 ff 51 f5\s+vsqrtpd ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 93 10 10 c2 ec 7b\s+vcmpph k5,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 93 10 17 c2 ec 7b\s+vcmpph k5\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 06 79 18 42 f5\s+vgetexpph ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 06 79 1f 42 f5\s+vgetexpph ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 06 79 9f 42 f5\s+vgetexpph ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 18 51 f5\s+vsqrtph ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 78 3f 51 f5\s+vsqrtph ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 78 ff 51 f5\s+vsqrtph ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 91 10 10 c2 ec 7b\s+vcmpps k5,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 91 10 17 c2 ec 7b\s+vcmpps k5\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 02 79 18 42 f5\s+vgetexpps ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 02 79 1f 42 f5\s+vgetexpps ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 02 79 9f 42 f5\s+vgetexpps ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 18 51 f5\s+vsqrtps ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 78 3f 51 f5\s+vsqrtps ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 78 ff 51 f5\s+vsqrtps ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 91 10 58 f4\s+vaddpd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 91 37 58 f4\s+vaddpd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 91 f7 58 f4\s+vaddpd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 10 10 58 f4\s+vaddph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 10 37 58 f4\s+vaddph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 10 f7 58 f4\s+vaddph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 10 10 58 f4\s+vaddps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 10 37 58 f4\s+vaddps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 10 f7 58 f4\s+vaddps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 91 10 5e f4\s+vdivpd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 91 37 5e f4\s+vdivpd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 91 f7 5e f4\s+vdivpd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 10 10 5e f4\s+vdivph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 10 37 5e f4\s+vdivph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 10 f7 5e f4\s+vdivph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 10 10 5e f4\s+vdivps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 10 37 5e f4\s+vdivps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 10 f7 5e f4\s+vdivps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 91 10 59 f4\s+vmulpd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 91 37 59 f4\s+vmulpd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 91 f7 59 f4\s+vmulpd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 10 10 59 f4\s+vmulph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 10 37 59 f4\s+vmulph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 10 f7 59 f4\s+vmulph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 10 10 59 f4\s+vmulps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 10 37 59 f4\s+vmulps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 10 f7 59 f4\s+vmulps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 2c f4\s+vscalefpd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 2c f4\s+vscalefpd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 2c f4\s+vscalefpd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 2c f4\s+vscalefph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 2c f4\s+vscalefph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 2c f4\s+vscalefph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 2c f4\s+vscalefps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 2c f4\s+vscalefps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 2c f4\s+vscalefps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 91 10 5c f4\s+vsubpd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 91 37 5c f4\s+vsubpd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 91 f7 5c f4\s+vsubpd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 10 10 5c f4\s+vsubph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 10 37 5c f4\s+vsubph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 10 f7 5c f4\s+vsubph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 10 10 5c f4\s+vsubps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 10 37 5c f4\s+vsubps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 10 f7 5c f4\s+vsubps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 91 10 5f f4\s+vmaxpd ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 91 17 5f f4\s+vmaxpd ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 91 97 5f f4\s+vmaxpd ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 10 5f f4\s+vmaxph ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 17 5f f4\s+vmaxph ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 97 5f f4\s+vmaxph ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 10 5f f4\s+vmaxps ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 17 5f f4\s+vmaxps ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 97 5f f4\s+vmaxps ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 91 10 5d f4\s+vminpd ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 91 17 5d f4\s+vminpd ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 91 97 5d f4\s+vminpd ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 10 5d f4\s+vminph ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 17 5d f4\s+vminph ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 05 10 97 5d f4\s+vminph ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 10 5d f4\s+vminps ymm30,ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 17 5d f4\s+vminps ymm30\{k7\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 01 10 97 5d f4\s+vminps ymm30\{k7\}\{z\},ymm29,ymm28\{sae\}
\s*[a-f0-9]+:\s*62 03 f9 18 26 f5 7b\s+vgetmantpd ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 1f 26 f5 7b\s+vgetmantpd ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 9f 26 f5 7b\s+vgetmantpd ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 18 26 f5 7b\s+vgetmantph ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 1f 26 f5 7b\s+vgetmantph ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 9f 26 f5 7b\s+vgetmantph ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 18 26 f5 7b\s+vgetmantps ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 1f 26 f5 7b\s+vgetmantps ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 9f 26 f5 7b\s+vgetmantps ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 18 56 f5 7b\s+vreducepd ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 1f 56 f5 7b\s+vreducepd ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 9f 56 f5 7b\s+vreducepd ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 18 56 f5 7b\s+vreduceph ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 1f 56 f5 7b\s+vreduceph ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 9f 56 f5 7b\s+vreduceph ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 18 56 f5 7b\s+vreduceps ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 1f 56 f5 7b\s+vreduceps ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 9f 56 f5 7b\s+vreduceps ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 18 09 f5 7b\s+vrndscalepd ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 1f 09 f5 7b\s+vrndscalepd ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 f9 9f 09 f5 7b\s+vrndscalepd ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 18 08 f5 7b\s+vrndscaleph ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 1f 08 f5 7b\s+vrndscaleph ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 78 9f 08 f5 7b\s+vrndscaleph ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 18 08 f5 7b\s+vrndscaleps ymm30,ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 1f 08 f5 7b\s+vrndscaleps ymm30\{k7\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 79 9f 08 f5 7b\s+vrndscaleps ymm30\{k7\}\{z\},ymm29\{sae\},0x7b
\s*[a-f0-9]+:\s*62 02 91 10 98 f4\s+vfmadd132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 98 f4\s+vfmadd132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 98 f4\s+vfmadd132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 98 f4\s+vfmadd132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 98 f4\s+vfmadd132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 98 f4\s+vfmadd132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 98 f4\s+vfmadd132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 98 f4\s+vfmadd132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 98 f4\s+vfmadd132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 a8 f4\s+vfmadd213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 a8 f4\s+vfmadd213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 a8 f4\s+vfmadd213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 a8 f4\s+vfmadd213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 a8 f4\s+vfmadd213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 a8 f4\s+vfmadd213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 a8 f4\s+vfmadd213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 a8 f4\s+vfmadd213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 a8 f4\s+vfmadd213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 b8 f4\s+vfmadd231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 b8 f4\s+vfmadd231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 b8 f4\s+vfmadd231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 b8 f4\s+vfmadd231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 b8 f4\s+vfmadd231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 b8 f4\s+vfmadd231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 b8 f4\s+vfmadd231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 b8 f4\s+vfmadd231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 b8 f4\s+vfmadd231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 96 f4\s+vfmaddsub132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 96 f4\s+vfmaddsub132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 96 f4\s+vfmaddsub132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 96 f4\s+vfmaddsub132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 96 f4\s+vfmaddsub132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 96 f4\s+vfmaddsub132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 96 f4\s+vfmaddsub132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 96 f4\s+vfmaddsub132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 96 f4\s+vfmaddsub132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 a6 f4\s+vfmaddsub213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 a6 f4\s+vfmaddsub213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 a6 f4\s+vfmaddsub213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 a6 f4\s+vfmaddsub213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 a6 f4\s+vfmaddsub213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 a6 f4\s+vfmaddsub213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 a6 f4\s+vfmaddsub213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 a6 f4\s+vfmaddsub213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 a6 f4\s+vfmaddsub213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 b6 f4\s+vfmaddsub231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 b6 f4\s+vfmaddsub231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 b6 f4\s+vfmaddsub231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 b6 f4\s+vfmaddsub231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 b6 f4\s+vfmaddsub231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 b6 f4\s+vfmaddsub231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 b6 f4\s+vfmaddsub231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 b6 f4\s+vfmaddsub231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 b6 f4\s+vfmaddsub231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 9a f4\s+vfmsub132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 9a f4\s+vfmsub132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 9a f4\s+vfmsub132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 9a f4\s+vfmsub132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 9a f4\s+vfmsub132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 9a f4\s+vfmsub132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 9a f4\s+vfmsub132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 9a f4\s+vfmsub132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 9a f4\s+vfmsub132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 aa f4\s+vfmsub213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 aa f4\s+vfmsub213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 aa f4\s+vfmsub213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 aa f4\s+vfmsub213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 aa f4\s+vfmsub213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 aa f4\s+vfmsub213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 aa f4\s+vfmsub213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 aa f4\s+vfmsub213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 aa f4\s+vfmsub213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 ba f4\s+vfmsub231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 ba f4\s+vfmsub231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 ba f4\s+vfmsub231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 ba f4\s+vfmsub231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 ba f4\s+vfmsub231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 ba f4\s+vfmsub231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 ba f4\s+vfmsub231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 ba f4\s+vfmsub231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 ba f4\s+vfmsub231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 97 f4\s+vfmsubadd132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 97 f4\s+vfmsubadd132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 97 f4\s+vfmsubadd132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 97 f4\s+vfmsubadd132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 97 f4\s+vfmsubadd132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 97 f4\s+vfmsubadd132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 97 f4\s+vfmsubadd132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 97 f4\s+vfmsubadd132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 97 f4\s+vfmsubadd132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 a7 f4\s+vfmsubadd213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 a7 f4\s+vfmsubadd213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 a7 f4\s+vfmsubadd213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 a7 f4\s+vfmsubadd213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 a7 f4\s+vfmsubadd213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 a7 f4\s+vfmsubadd213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 a7 f4\s+vfmsubadd213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 a7 f4\s+vfmsubadd213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 a7 f4\s+vfmsubadd213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 b7 f4\s+vfmsubadd231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 b7 f4\s+vfmsubadd231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 b7 f4\s+vfmsubadd231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 b7 f4\s+vfmsubadd231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 b7 f4\s+vfmsubadd231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 b7 f4\s+vfmsubadd231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 b7 f4\s+vfmsubadd231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 b7 f4\s+vfmsubadd231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 b7 f4\s+vfmsubadd231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 9c f4\s+vfnmadd132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 9c f4\s+vfnmadd132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 9c f4\s+vfnmadd132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 9c f4\s+vfnmadd132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 9c f4\s+vfnmadd132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 9c f4\s+vfnmadd132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 9c f4\s+vfnmadd132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 9c f4\s+vfnmadd132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 9c f4\s+vfnmadd132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 ac f4\s+vfnmadd213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 ac f4\s+vfnmadd213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 ac f4\s+vfnmadd213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 ac f4\s+vfnmadd213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 ac f4\s+vfnmadd213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 ac f4\s+vfnmadd213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 ac f4\s+vfnmadd213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 ac f4\s+vfnmadd213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 ac f4\s+vfnmadd213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 bc f4\s+vfnmadd231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 bc f4\s+vfnmadd231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 bc f4\s+vfnmadd231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 bc f4\s+vfnmadd231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 bc f4\s+vfnmadd231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 bc f4\s+vfnmadd231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 bc f4\s+vfnmadd231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 bc f4\s+vfnmadd231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 bc f4\s+vfnmadd231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 9e f4\s+vfnmsub132pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 9e f4\s+vfnmsub132pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 9e f4\s+vfnmsub132pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 9e f4\s+vfnmsub132ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 9e f4\s+vfnmsub132ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 9e f4\s+vfnmsub132ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 9e f4\s+vfnmsub132ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 9e f4\s+vfnmsub132ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 9e f4\s+vfnmsub132ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 ae f4\s+vfnmsub213pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 ae f4\s+vfnmsub213pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 ae f4\s+vfnmsub213pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 ae f4\s+vfnmsub213ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 ae f4\s+vfnmsub213ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 ae f4\s+vfnmsub213ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 ae f4\s+vfnmsub213ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 ae f4\s+vfnmsub213ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 ae f4\s+vfnmsub213ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 91 10 be f4\s+vfnmsub231pd ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 91 37 be f4\s+vfnmsub231pd ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 91 f7 be f4\s+vfnmsub231pd ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 11 10 be f4\s+vfnmsub231ph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 11 37 be f4\s+vfnmsub231ph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 11 f7 be f4\s+vfnmsub231ph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 02 11 10 be f4\s+vfnmsub231ps ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 02 11 37 be f4\s+vfnmsub231ps ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 02 11 f7 be f4\s+vfnmsub231ps ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 03 91 10 54 f4 7b\s+vfixupimmpd ymm30,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 91 17 54 f4 7b\s+vfixupimmpd ymm30\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 91 97 54 f4 7b\s+vfixupimmpd ymm30\{k7\}\{z\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 10 54 f4 7b\s+vfixupimmps ymm30,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 17 54 f4 7b\s+vfixupimmps ymm30\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 97 54 f4 7b\s+vfixupimmps ymm30\{k7\}\{z\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 91 10 50 f4 7b\s+vrangepd ymm30,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 91 17 50 f4 7b\s+vrangepd ymm30\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 91 97 50 f4 7b\s+vrangepd ymm30\{k7\}\{z\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 10 50 f4 7b\s+vrangeps ymm30,ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 17 50 f4 7b\s+vrangeps ymm30\{k7\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 03 11 97 50 f4 7b\s+vrangeps ymm30\{k7\}\{z\},ymm29,ymm28\{sae\},0x7b
\s*[a-f0-9]+:\s*62 06 13 10 56 f4\s+vfcmaddcph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 13 37 56 f4\s+vfcmaddcph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 13 f7 56 f4\s+vfcmaddcph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 13 10 d6 f4\s+vfcmulcph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 13 37 d6 f4\s+vfcmulcph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 13 f7 d6 f4\s+vfcmulcph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 12 10 56 f4\s+vfmaddcph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 12 37 56 f4\s+vfmaddcph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 12 f7 56 f4\s+vfmaddcph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 06 12 10 d6 f4\s+vfmulcph ymm30,ymm29,ymm28\{rn-sae\}
\s*[a-f0-9]+:\s*62 06 12 37 d6 f4\s+vfmulcph ymm30\{k7\},ymm29,ymm28\{rd-sae\}
\s*[a-f0-9]+:\s*62 06 12 f7 d6 f4\s+vfmulcph ymm30\{k7\}\{z\},ymm29,ymm28\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 78 18 5b f5\s+vcvtdq2ph xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 78 3f 5b f5\s+vcvtdq2ph xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 78 ff 5b f5\s+vcvtdq2ph xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 78 18 5b f5\s+vcvtdq2ps ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 78 3f 5b f5\s+vcvtdq2ps ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 78 ff 5b f5\s+vcvtdq2ps ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 7b 18 7a f5\s+vcvtudq2ph xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 7b 3f 7a f5\s+vcvtudq2ph xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 7b ff 7a f5\s+vcvtudq2ph xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 7b 18 7a f5\s+vcvtudq2ps ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 7b 3f 7a f5\s+vcvtudq2ps ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 7b ff 7a f5\s+vcvtudq2ps ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 fb 18 e6 f5\s+vcvtpd2dq xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 fb 3f e6 f5\s+vcvtpd2dq xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 fb ff e6 f5\s+vcvtpd2dq xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 f9 18 5a f5\s+vcvtpd2ph xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 f9 3f 5a f5\s+vcvtpd2ph xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 f9 ff 5a f5\s+vcvtpd2ph xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 5a f5\s+vcvtpd2ps xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f9 3f 5a f5\s+vcvtpd2ps xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f9 ff 5a f5\s+vcvtpd2ps xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f8 18 79 f5\s+vcvtpd2udq xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f8 3f 79 f5\s+vcvtpd2udq xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f8 ff 79 f5\s+vcvtpd2udq xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 7b f5\s+vcvtpd2qq ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f9 3f 7b f5\s+vcvtpd2qq ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f9 ff 7b f5\s+vcvtpd2qq ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 79 f5\s+vcvtpd2uqq ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f9 3f 79 f5\s+vcvtpd2uqq ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f9 ff 79 f5\s+vcvtpd2uqq ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 79 18 5b f5\s+vcvtph2dq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 79 3f 5b f5\s+vcvtph2dq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 79 ff 5b f5\s+vcvtph2dq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 79 18 7b f5\s+vcvtph2qq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 79 3f 7b f5\s+vcvtph2qq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 79 ff 7b f5\s+vcvtph2qq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 78 18 79 f5\s+vcvtph2udq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 78 3f 79 f5\s+vcvtph2udq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 78 ff 79 f5\s+vcvtph2udq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 79 18 79 f5\s+vcvtph2uqq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 79 3f 79 f5\s+vcvtph2uqq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 79 ff 79 f5\s+vcvtph2uqq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 78 18 5a f5\s+vcvtph2pd ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 1f 5a f5\s+vcvtph2pd ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 9f 5a f5\s+vcvtph2pd ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 02 79 18 13 f5\s+vcvtph2ps ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 02 79 1f 13 f5\s+vcvtph2ps ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 02 79 9f 13 f5\s+vcvtph2ps ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 06 79 18 13 f5\s+vcvtph2psx ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 06 79 1f 13 f5\s+vcvtph2psx ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 06 79 9f 13 f5\s+vcvtph2psx ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 18 7d f5\s+vcvtph2uw ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 78 3f 7d f5\s+vcvtph2uw ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 78 ff 7d f5\s+vcvtph2uw ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 79 18 7d f5\s+vcvtph2w ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 79 3f 7d f5\s+vcvtph2w ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 79 ff 7d f5\s+vcvtph2w ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 79 18 5b f5\s+vcvtps2dq ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 79 3f 5b f5\s+vcvtps2dq ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 79 ff 5b f5\s+vcvtps2dq ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 78 18 79 f5\s+vcvtps2udq ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 78 3f 79 f5\s+vcvtps2udq ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 78 ff 79 f5\s+vcvtps2udq ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 78 18 5a f5\s+vcvtps2pd ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 1f 5a f5\s+vcvtps2pd ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 9f 5a f5\s+vcvtps2pd ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 18 1d f5\s+vcvtps2phx xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 79 3f 1d f5\s+vcvtps2phx xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 79 ff 1d f5\s+vcvtps2phx xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 79 18 7b f5\s+vcvtps2qq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 79 3f 7b f5\s+vcvtps2qq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 79 ff 7b f5\s+vcvtps2qq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 79 18 79 f5\s+vcvtps2uqq ymm30,xmm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 79 3f 79 f5\s+vcvtps2uqq ymm30\{k7\},xmm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 79 ff 79 f5\s+vcvtps2uqq ymm30\{k7\}\{z\},xmm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 fa 18 e6 f5\s+vcvtqq2pd ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 fa 3f e6 f5\s+vcvtqq2pd ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 fa ff e6 f5\s+vcvtqq2pd ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 f8 18 5b f5\s+vcvtqq2ph xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 f8 3f 5b f5\s+vcvtqq2ph xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 f8 ff 5b f5\s+vcvtqq2ph xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f8 18 5b f5\s+vcvtqq2ps xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 f8 3f 5b f5\s+vcvtqq2ps xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 f8 ff 5b f5\s+vcvtqq2ps xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 fa 18 7a f5\s+vcvtuqq2pd ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 fa 3f 7a f5\s+vcvtuqq2pd ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 fa ff 7a f5\s+vcvtuqq2pd ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 fb 18 7a f5\s+vcvtuqq2ph xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 fb 3f 7a f5\s+vcvtuqq2ph xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 fb ff 7a f5\s+vcvtuqq2ph xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 fb 18 7a f5\s+vcvtuqq2ps xmm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 01 fb 3f 7a f5\s+vcvtuqq2ps xmm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 01 fb ff 7a f5\s+vcvtuqq2ps xmm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 e6 f5\s+vcvttpd2dq xmm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 1f e6 f5\s+vcvttpd2dq xmm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 9f e6 f5\s+vcvttpd2dq xmm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f8 18 78 f5\s+vcvttpd2udq xmm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f8 1f 78 f5\s+vcvttpd2udq xmm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f8 9f 78 f5\s+vcvttpd2udq xmm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 7a f5\s+vcvttpd2qq ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 1f 7a f5\s+vcvttpd2qq ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 9f 7a f5\s+vcvttpd2qq ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 18 78 f5\s+vcvttpd2uqq ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 1f 78 f5\s+vcvttpd2uqq ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 f9 9f 78 f5\s+vcvttpd2uqq ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 7a 18 5b f5\s+vcvttph2dq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 7a 1f 5b f5\s+vcvttph2dq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 7a 9f 5b f5\s+vcvttph2dq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 18 7a f5\s+vcvttph2qq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 1f 7a f5\s+vcvttph2qq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 9f 7a f5\s+vcvttph2qq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 18 78 f5\s+vcvttph2udq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 1f 78 f5\s+vcvttph2udq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 9f 78 f5\s+vcvttph2udq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 18 78 f5\s+vcvttph2uqq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 1f 78 f5\s+vcvttph2uqq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 9f 78 f5\s+vcvttph2uqq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 18 7c f5\s+vcvttph2uw ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 1f 7c f5\s+vcvttph2uw ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 78 9f 7c f5\s+vcvttph2uw ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 18 7c f5\s+vcvttph2w ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 1f 7c f5\s+vcvttph2w ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 05 79 9f 7c f5\s+vcvttph2w ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 7a 18 5b f5\s+vcvttps2dq ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 7a 1f 5b f5\s+vcvttps2dq ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 7a 9f 5b f5\s+vcvttps2dq ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 18 78 f5\s+vcvttps2udq ymm30,ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 1f 78 f5\s+vcvttps2udq ymm30\{k7\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 78 9f 78 f5\s+vcvttps2udq ymm30\{k7\}\{z\},ymm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 18 7a f5\s+vcvttps2qq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 1f 7a f5\s+vcvttps2qq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 9f 7a f5\s+vcvttps2qq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 18 78 f5\s+vcvttps2uqq ymm30,xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 1f 78 f5\s+vcvttps2uqq ymm30\{k7\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 01 79 9f 78 f5\s+vcvttps2uqq ymm30\{k7\}\{z\},xmm29\{sae\}
\s*[a-f0-9]+:\s*62 05 7b 18 7d f5\s+vcvtuw2ph ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 7b 3f 7d f5\s+vcvtuw2ph ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 7b ff 7d f5\s+vcvtuw2ph ymm30\{k7\}\{z\},ymm29\{rz-sae\}
\s*[a-f0-9]+:\s*62 05 7a 18 7d f5\s+vcvtw2ph ymm30,ymm29\{rn-sae\}
\s*[a-f0-9]+:\s*62 05 7a 3f 7d f5\s+vcvtw2ph ymm30\{k7\},ymm29\{rd-sae\}
\s*[a-f0-9]+:\s*62 05 7a ff 7d f5\s+vcvtw2ph ymm30\{k7\}\{z\},ymm29\{rz-sae\}
#pass
