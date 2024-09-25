#objdump: -dw -Mintel
#name: i386 AVX10.2 insns rounding (Intel disassembly)
#source: avx10_2-rounding.s

.*: +file format .*

Disassembly of section \.text:

#...
[a-f0-9]+ <_intel>:
\s*[a-f0-9]+:\s*62 f1 d1 18 c2 ec 7b\s+vcmppd k5,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f1 d1 1f c2 ec 7b\s+vcmppd k5\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f2 f9 18 42 f5\s+vgetexppd ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 f9 1f 42 f5\s+vgetexppd ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 f9 9f 42 f5\s+vgetexppd ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 51 f5\s+vsqrtpd ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 3f 51 f5\s+vsqrtpd ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 51 f5\s+vsqrtpd ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f3 50 18 c2 ec 7b\s+vcmpph k5,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 50 1f c2 ec 7b\s+vcmpph k5\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f6 79 18 42 f5\s+vgetexpph ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f6 79 1f 42 f5\s+vgetexpph ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f6 79 9f 42 f5\s+vgetexpph ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 51 f5\s+vsqrtph ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 78 3f 51 f5\s+vsqrtph ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 78 ff 51 f5\s+vsqrtph ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 c2 ec 7b\s+vcmpps k5,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f1 50 1f c2 ec 7b\s+vcmpps k5\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f2 79 18 42 f5\s+vgetexpps ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 79 1f 42 f5\s+vgetexpps ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 79 9f 42 f5\s+vgetexpps ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 18 51 f5\s+vsqrtps ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 78 3f 51 f5\s+vsqrtps ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 78 ff 51 f5\s+vsqrtps ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 58 f4\s+vaddpd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 3f 58 f4\s+vaddpd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 58 f4\s+vaddpd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 58 f4\s+vaddph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 50 3f 58 f4\s+vaddph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 50 ff 58 f4\s+vaddph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 58 f4\s+vaddps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 50 3f 58 f4\s+vaddps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 50 ff 58 f4\s+vaddps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5e f4\s+vdivpd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 3f 5e f4\s+vdivpd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 5e f4\s+vdivpd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 5e f4\s+vdivph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 50 3f 5e f4\s+vdivph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 50 ff 5e f4\s+vdivph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 5e f4\s+vdivps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 50 3f 5e f4\s+vdivps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 50 ff 5e f4\s+vdivps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 59 f4\s+vmulpd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 3f 59 f4\s+vmulpd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 59 f4\s+vmulpd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 59 f4\s+vmulph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 50 3f 59 f4\s+vmulph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 50 ff 59 f4\s+vmulph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 59 f4\s+vmulps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 50 3f 59 f4\s+vmulps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 50 ff 59 f4\s+vmulps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 2c f4\s+vscalefpd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 2c f4\s+vscalefpd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 2c f4\s+vscalefpd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 2c f4\s+vscalefph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 2c f4\s+vscalefph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 2c f4\s+vscalefph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 2c f4\s+vscalefps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 2c f4\s+vscalefps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 2c f4\s+vscalefps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5c f4\s+vsubpd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 3f 5c f4\s+vsubpd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 5c f4\s+vsubpd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 5c f4\s+vsubph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 50 3f 5c f4\s+vsubph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 50 ff 5c f4\s+vsubph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 5c f4\s+vsubps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 50 3f 5c f4\s+vsubps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 50 ff 5c f4\s+vsubps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5f f4\s+vmaxpd ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 d1 1f 5f f4\s+vmaxpd ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 d1 9f 5f f4\s+vmaxpd ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 5f f4\s+vmaxph ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 1f 5f f4\s+vmaxph ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 9f 5f f4\s+vmaxph ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 5f f4\s+vmaxps ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 1f 5f f4\s+vmaxps ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 9f 5f f4\s+vmaxps ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5d f4\s+vminpd ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 d1 1f 5d f4\s+vminpd ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 d1 9f 5d f4\s+vminpd ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 18 5d f4\s+vminph ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 1f 5d f4\s+vminph ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f5 50 9f 5d f4\s+vminph ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 18 5d f4\s+vminps ymm6,ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 1f 5d f4\s+vminps ymm6\{k7\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f1 50 9f 5d f4\s+vminps ymm6\{k7\}\{z\},ymm5,ymm4\{sae\}
\s*[a-f0-9]+:\s*62 f3 f9 18 26 f5 7b\s+vgetmantpd ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 1f 26 f5 7b\s+vgetmantpd ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 9f 26 f5 7b\s+vgetmantpd ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 18 26 f5 7b\s+vgetmantph ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 1f 26 f5 7b\s+vgetmantph ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 9f 26 f5 7b\s+vgetmantph ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 18 26 f5 7b\s+vgetmantps ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 1f 26 f5 7b\s+vgetmantps ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 9f 26 f5 7b\s+vgetmantps ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 18 56 f5 7b\s+vreducepd ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 1f 56 f5 7b\s+vreducepd ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 9f 56 f5 7b\s+vreducepd ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 18 56 f5 7b\s+vreduceph ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 1f 56 f5 7b\s+vreduceph ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 9f 56 f5 7b\s+vreduceph ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 18 56 f5 7b\s+vreduceps ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 1f 56 f5 7b\s+vreduceps ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 9f 56 f5 7b\s+vreduceps ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 18 09 f5 7b\s+vrndscalepd ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 1f 09 f5 7b\s+vrndscalepd ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 f9 9f 09 f5 7b\s+vrndscalepd ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 18 08 f5 7b\s+vrndscaleph ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 1f 08 f5 7b\s+vrndscaleph ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 78 9f 08 f5 7b\s+vrndscaleph ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 18 08 f5 7b\s+vrndscaleps ymm6,ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 1f 08 f5 7b\s+vrndscaleps ymm6\{k7\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 79 9f 08 f5 7b\s+vrndscaleps ymm6\{k7\}\{z\},ymm5\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f2 d1 18 98 f4\s+vfmadd132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 98 f4\s+vfmadd132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 98 f4\s+vfmadd132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 98 f4\s+vfmadd132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 98 f4\s+vfmadd132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 98 f4\s+vfmadd132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 98 f4\s+vfmadd132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 98 f4\s+vfmadd132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 98 f4\s+vfmadd132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a8 f4\s+vfmadd213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f a8 f4\s+vfmadd213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a8 f4\s+vfmadd213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 a8 f4\s+vfmadd213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f a8 f4\s+vfmadd213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff a8 f4\s+vfmadd213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 a8 f4\s+vfmadd213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f a8 f4\s+vfmadd213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff a8 f4\s+vfmadd213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b8 f4\s+vfmadd231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f b8 f4\s+vfmadd231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b8 f4\s+vfmadd231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 b8 f4\s+vfmadd231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f b8 f4\s+vfmadd231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff b8 f4\s+vfmadd231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 b8 f4\s+vfmadd231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f b8 f4\s+vfmadd231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff b8 f4\s+vfmadd231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 96 f4\s+vfmaddsub132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 96 f4\s+vfmaddsub132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 96 f4\s+vfmaddsub132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 96 f4\s+vfmaddsub132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 96 f4\s+vfmaddsub132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 96 f4\s+vfmaddsub132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 96 f4\s+vfmaddsub132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 96 f4\s+vfmaddsub132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 96 f4\s+vfmaddsub132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a6 f4\s+vfmaddsub213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f a6 f4\s+vfmaddsub213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a6 f4\s+vfmaddsub213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 a6 f4\s+vfmaddsub213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f a6 f4\s+vfmaddsub213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff a6 f4\s+vfmaddsub213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 a6 f4\s+vfmaddsub213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f a6 f4\s+vfmaddsub213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff a6 f4\s+vfmaddsub213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b6 f4\s+vfmaddsub231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f b6 f4\s+vfmaddsub231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b6 f4\s+vfmaddsub231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 b6 f4\s+vfmaddsub231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f b6 f4\s+vfmaddsub231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff b6 f4\s+vfmaddsub231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 b6 f4\s+vfmaddsub231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f b6 f4\s+vfmaddsub231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff b6 f4\s+vfmaddsub231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9a f4\s+vfmsub132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 9a f4\s+vfmsub132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9a f4\s+vfmsub132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 9a f4\s+vfmsub132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 9a f4\s+vfmsub132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9a f4\s+vfmsub132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 9a f4\s+vfmsub132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 9a f4\s+vfmsub132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9a f4\s+vfmsub132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 aa f4\s+vfmsub213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f aa f4\s+vfmsub213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff aa f4\s+vfmsub213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 aa f4\s+vfmsub213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f aa f4\s+vfmsub213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff aa f4\s+vfmsub213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 aa f4\s+vfmsub213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f aa f4\s+vfmsub213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff aa f4\s+vfmsub213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ba f4\s+vfmsub231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f ba f4\s+vfmsub231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ba f4\s+vfmsub231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 ba f4\s+vfmsub231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f ba f4\s+vfmsub231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff ba f4\s+vfmsub231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 ba f4\s+vfmsub231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f ba f4\s+vfmsub231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff ba f4\s+vfmsub231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 97 f4\s+vfmsubadd132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 97 f4\s+vfmsubadd132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 97 f4\s+vfmsubadd132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 97 f4\s+vfmsubadd132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 97 f4\s+vfmsubadd132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 97 f4\s+vfmsubadd132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 97 f4\s+vfmsubadd132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 97 f4\s+vfmsubadd132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 97 f4\s+vfmsubadd132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a7 f4\s+vfmsubadd213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f a7 f4\s+vfmsubadd213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a7 f4\s+vfmsubadd213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 a7 f4\s+vfmsubadd213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f a7 f4\s+vfmsubadd213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff a7 f4\s+vfmsubadd213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 a7 f4\s+vfmsubadd213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f a7 f4\s+vfmsubadd213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff a7 f4\s+vfmsubadd213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b7 f4\s+vfmsubadd231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f b7 f4\s+vfmsubadd231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b7 f4\s+vfmsubadd231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 b7 f4\s+vfmsubadd231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f b7 f4\s+vfmsubadd231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff b7 f4\s+vfmsubadd231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 b7 f4\s+vfmsubadd231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f b7 f4\s+vfmsubadd231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff b7 f4\s+vfmsubadd231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9c f4\s+vfnmadd132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 9c f4\s+vfnmadd132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9c f4\s+vfnmadd132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 9c f4\s+vfnmadd132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 9c f4\s+vfnmadd132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9c f4\s+vfnmadd132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 9c f4\s+vfnmadd132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 9c f4\s+vfnmadd132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9c f4\s+vfnmadd132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ac f4\s+vfnmadd213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f ac f4\s+vfnmadd213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ac f4\s+vfnmadd213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 ac f4\s+vfnmadd213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f ac f4\s+vfnmadd213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff ac f4\s+vfnmadd213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 ac f4\s+vfnmadd213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f ac f4\s+vfnmadd213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff ac f4\s+vfnmadd213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 bc f4\s+vfnmadd231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f bc f4\s+vfnmadd231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff bc f4\s+vfnmadd231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 bc f4\s+vfnmadd231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f bc f4\s+vfnmadd231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff bc f4\s+vfnmadd231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 bc f4\s+vfnmadd231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f bc f4\s+vfnmadd231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff bc f4\s+vfnmadd231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9e f4\s+vfnmsub132pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f 9e f4\s+vfnmsub132pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9e f4\s+vfnmsub132pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 9e f4\s+vfnmsub132ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f 9e f4\s+vfnmsub132ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9e f4\s+vfnmsub132ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 9e f4\s+vfnmsub132ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f 9e f4\s+vfnmsub132ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9e f4\s+vfnmsub132ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ae f4\s+vfnmsub213pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f ae f4\s+vfnmsub213pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ae f4\s+vfnmsub213pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 ae f4\s+vfnmsub213ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f ae f4\s+vfnmsub213ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff ae f4\s+vfnmsub213ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 ae f4\s+vfnmsub213ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f ae f4\s+vfnmsub213ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff ae f4\s+vfnmsub213ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 18 be f4\s+vfnmsub231pd ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 3f be f4\s+vfnmsub231pd ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 d1 ff be f4\s+vfnmsub231pd ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 51 18 be f4\s+vfnmsub231ph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 51 3f be f4\s+vfnmsub231ph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 51 ff be f4\s+vfnmsub231ph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f2 51 18 be f4\s+vfnmsub231ps ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f2 51 3f be f4\s+vfnmsub231ps ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f2 51 ff be f4\s+vfnmsub231ps ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f3 d1 18 54 f4 7b\s+vfixupimmpd ymm6,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 d1 1f 54 f4 7b\s+vfixupimmpd ymm6\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 d1 9f 54 f4 7b\s+vfixupimmpd ymm6\{k7\}\{z\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 18 54 f4 7b\s+vfixupimmps ymm6,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 1f 54 f4 7b\s+vfixupimmps ymm6\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 9f 54 f4 7b\s+vfixupimmps ymm6\{k7\}\{z\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 d1 18 50 f4 7b\s+vrangepd ymm6,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 d1 1f 50 f4 7b\s+vrangepd ymm6\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 d1 9f 50 f4 7b\s+vrangepd ymm6\{k7\}\{z\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 18 50 f4 7b\s+vrangeps ymm6,ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 1f 50 f4 7b\s+vrangeps ymm6\{k7\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f3 51 9f 50 f4 7b\s+vrangeps ymm6\{k7\}\{z\},ymm5,ymm4\{sae\},0x7b
\s*[a-f0-9]+:\s*62 f6 53 18 56 f4\s+vfcmaddcph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 53 3f 56 f4\s+vfcmaddcph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 53 ff 56 f4\s+vfcmaddcph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 53 18 d6 f4\s+vfcmulcph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 53 3f d6 f4\s+vfcmulcph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 53 ff d6 f4\s+vfcmulcph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 52 18 56 f4\s+vfmaddcph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 52 3f 56 f4\s+vfmaddcph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 52 ff 56 f4\s+vfmaddcph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f6 52 18 d6 f4\s+vfmulcph ymm6,ymm5,ymm4\{rn-sae\}
\s*[a-f0-9]+:\s*62 f6 52 3f d6 f4\s+vfmulcph ymm6\{k7\},ymm5,ymm4\{rd-sae\}
\s*[a-f0-9]+:\s*62 f6 52 ff d6 f4\s+vfmulcph ymm6\{k7\}\{z\},ymm5,ymm4\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 5b f5\s+vcvtdq2ph xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 78 3f 5b f5\s+vcvtdq2ph xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 78 ff 5b f5\s+vcvtdq2ph xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 78 18 5b f5\s+vcvtdq2ps ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 78 3f 5b f5\s+vcvtdq2ps ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 78 ff 5b f5\s+vcvtdq2ps ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 7b 18 7a f5\s+vcvtudq2ph xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 7b 3f 7a f5\s+vcvtudq2ph xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 7b ff 7a f5\s+vcvtudq2ph xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 7b 18 7a f5\s+vcvtudq2ps ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 7b 3f 7a f5\s+vcvtudq2ps ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 7b ff 7a f5\s+vcvtudq2ps ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 fb 18 e6 f5\s+vcvtpd2dq xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 fb 3f e6 f5\s+vcvtpd2dq xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 fb ff e6 f5\s+vcvtpd2dq xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 f9 18 5a f5\s+vcvtpd2ph xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 f9 3f 5a f5\s+vcvtpd2ph xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 f9 ff 5a f5\s+vcvtpd2ph xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 5a f5\s+vcvtpd2ps xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 3f 5a f5\s+vcvtpd2ps xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 5a f5\s+vcvtpd2ps xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 18 79 f5\s+vcvtpd2udq xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 3f 79 f5\s+vcvtpd2udq xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 ff 79 f5\s+vcvtpd2udq xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 7b f5\s+vcvtpd2qq ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 3f 7b f5\s+vcvtpd2qq ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 7b f5\s+vcvtpd2qq ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 79 f5\s+vcvtpd2uqq ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 3f 79 f5\s+vcvtpd2uqq ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 79 f5\s+vcvtpd2uqq ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 5b f5\s+vcvtph2dq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 79 3f 5b f5\s+vcvtph2dq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 79 ff 5b f5\s+vcvtph2dq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 7b f5\s+vcvtph2qq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 79 3f 7b f5\s+vcvtph2qq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 79 ff 7b f5\s+vcvtph2qq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 79 f5\s+vcvtph2udq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 78 3f 79 f5\s+vcvtph2udq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 78 ff 79 f5\s+vcvtph2udq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 79 f5\s+vcvtph2uqq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 79 3f 79 f5\s+vcvtph2uqq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 79 ff 79 f5\s+vcvtph2uqq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 5a f5\s+vcvtph2pd ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 1f 5a f5\s+vcvtph2pd ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 9f 5a f5\s+vcvtph2pd ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 79 18 13 f5\s+vcvtph2ps ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 79 1f 13 f5\s+vcvtph2ps ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f2 79 9f 13 f5\s+vcvtph2ps ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f6 79 18 13 f5\s+vcvtph2psx ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f6 79 1f 13 f5\s+vcvtph2psx ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f6 79 9f 13 f5\s+vcvtph2psx ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 7d f5\s+vcvtph2uw ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 78 3f 7d f5\s+vcvtph2uw ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 78 ff 7d f5\s+vcvtph2uw ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 7d f5\s+vcvtph2w ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 79 3f 7d f5\s+vcvtph2w ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 79 ff 7d f5\s+vcvtph2w ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 79 18 5b f5\s+vcvtps2dq ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 79 3f 5b f5\s+vcvtps2dq ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 79 ff 5b f5\s+vcvtps2dq ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 78 18 79 f5\s+vcvtps2udq ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 78 3f 79 f5\s+vcvtps2udq ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 78 ff 79 f5\s+vcvtps2udq ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 78 18 5a f5\s+vcvtps2pd ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 1f 5a f5\s+vcvtps2pd ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 9f 5a f5\s+vcvtps2pd ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 1d f5\s+vcvtps2phx xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 79 3f 1d f5\s+vcvtps2phx xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 79 ff 1d f5\s+vcvtps2phx xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 79 18 7b f5\s+vcvtps2qq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 79 3f 7b f5\s+vcvtps2qq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 79 ff 7b f5\s+vcvtps2qq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 79 18 79 f5\s+vcvtps2uqq ymm6,xmm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 79 3f 79 f5\s+vcvtps2uqq ymm6\{k7\},xmm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 79 ff 79 f5\s+vcvtps2uqq ymm6\{k7\}\{z\},xmm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 fa 18 e6 f5\s+vcvtqq2pd ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 fa 3f e6 f5\s+vcvtqq2pd ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 fa ff e6 f5\s+vcvtqq2pd ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 f8 18 5b f5\s+vcvtqq2ph xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 f8 3f 5b f5\s+vcvtqq2ph xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 f8 ff 5b f5\s+vcvtqq2ph xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 18 5b f5\s+vcvtqq2ps xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 3f 5b f5\s+vcvtqq2ps xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 f8 ff 5b f5\s+vcvtqq2ps xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 fa 18 7a f5\s+vcvtuqq2pd ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 fa 3f 7a f5\s+vcvtuqq2pd ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 fa ff 7a f5\s+vcvtuqq2pd ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 fb 18 7a f5\s+vcvtuqq2ph xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 fb 3f 7a f5\s+vcvtuqq2ph xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 fb ff 7a f5\s+vcvtuqq2ph xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 fb 18 7a f5\s+vcvtuqq2ps xmm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f1 fb 3f 7a f5\s+vcvtuqq2ps xmm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f1 fb ff 7a f5\s+vcvtuqq2ps xmm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 e6 f5\s+vcvttpd2dq xmm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 1f e6 f5\s+vcvttpd2dq xmm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 9f e6 f5\s+vcvttpd2dq xmm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f8 18 78 f5\s+vcvttpd2udq xmm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f8 1f 78 f5\s+vcvttpd2udq xmm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f8 9f 78 f5\s+vcvttpd2udq xmm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 7a f5\s+vcvttpd2qq ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 1f 7a f5\s+vcvttpd2qq ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 9f 7a f5\s+vcvttpd2qq ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 18 78 f5\s+vcvttpd2uqq ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 1f 78 f5\s+vcvttpd2uqq ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 f9 9f 78 f5\s+vcvttpd2uqq ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 7a 18 5b f5\s+vcvttph2dq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 7a 1f 5b f5\s+vcvttph2dq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 7a 9f 5b f5\s+vcvttph2dq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 7a f5\s+vcvttph2qq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 1f 7a f5\s+vcvttph2qq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 9f 7a f5\s+vcvttph2qq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 78 f5\s+vcvttph2udq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 1f 78 f5\s+vcvttph2udq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 9f 78 f5\s+vcvttph2udq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 78 f5\s+vcvttph2uqq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 1f 78 f5\s+vcvttph2uqq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 9f 78 f5\s+vcvttph2uqq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 18 7c f5\s+vcvttph2uw ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 1f 7c f5\s+vcvttph2uw ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 78 9f 7c f5\s+vcvttph2uw ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 18 7c f5\s+vcvttph2w ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 1f 7c f5\s+vcvttph2w ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 79 9f 7c f5\s+vcvttph2w ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 7a 18 5b f5\s+vcvttps2dq ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 7a 1f 5b f5\s+vcvttps2dq ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 7a 9f 5b f5\s+vcvttps2dq ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 18 78 f5\s+vcvttps2udq ymm6,ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 1f 78 f5\s+vcvttps2udq ymm6\{k7\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 78 9f 78 f5\s+vcvttps2udq ymm6\{k7\}\{z\},ymm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 18 7a f5\s+vcvttps2qq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 1f 7a f5\s+vcvttps2qq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 9f 7a f5\s+vcvttps2qq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 18 78 f5\s+vcvttps2uqq ymm6,xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 1f 78 f5\s+vcvttps2uqq ymm6\{k7\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f1 79 9f 78 f5\s+vcvttps2uqq ymm6\{k7\}\{z\},xmm5\{sae\}
\s*[a-f0-9]+:\s*62 f5 7b 18 7d f5\s+vcvtuw2ph ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 7b 3f 7d f5\s+vcvtuw2ph ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 7b ff 7d f5\s+vcvtuw2ph ymm6\{k7\}\{z\},ymm5\{rz-sae\}
\s*[a-f0-9]+:\s*62 f5 7a 18 7d f5\s+vcvtw2ph ymm6,ymm5\{rn-sae\}
\s*[a-f0-9]+:\s*62 f5 7a 3f 7d f5\s+vcvtw2ph ymm6\{k7\},ymm5\{rd-sae\}
\s*[a-f0-9]+:\s*62 f5 7a ff 7d f5\s+vcvtw2ph ymm6\{k7\}\{z\},ymm5\{rz-sae\}
#pass
