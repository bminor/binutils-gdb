#objdump: -dw
#name: i386 AVX10.2 rounding insns

.*: +file format .*

Disassembly of section \.text:

0+ <_start>:
\s*[a-f0-9]+:\s*62 f1 d1 18 c2 ec 7b\s+vcmppd \$0x7b,\{sae\},%ymm4,%ymm5,%k5
\s*[a-f0-9]+:\s*62 f1 d1 1f c2 ec 7b\s+vcmppd \$0x7b,\{sae\},%ymm4,%ymm5,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 f2 f9 18 42 f5\s+vgetexppd \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 f9 1f 42 f5\s+vgetexppd \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 f9 9f 42 f5\s+vgetexppd \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 51 f5\s+vsqrtpd \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 f9 3f 51 f5\s+vsqrtpd \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 51 f5\s+vsqrtpd \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 50 18 c2 ec 7b\s+vcmpph \$0x7b,\{sae\},%ymm4,%ymm5,%k5
\s*[a-f0-9]+:\s*62 f3 50 1f c2 ec 7b\s+vcmpph \$0x7b,\{sae\},%ymm4,%ymm5,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 f6 79 18 42 f5\s+vgetexpph \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 79 1f 42 f5\s+vgetexpph \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 79 9f 42 f5\s+vgetexpph \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 51 f5\s+vsqrtph \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 3f 51 f5\s+vsqrtph \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 ff 51 f5\s+vsqrtph \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 c2 ec 7b\s+vcmpps \$0x7b,\{sae\},%ymm4,%ymm5,%k5
\s*[a-f0-9]+:\s*62 f1 50 1f c2 ec 7b\s+vcmpps \$0x7b,\{sae\},%ymm4,%ymm5,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 f2 79 18 42 f5\s+vgetexpps \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 79 1f 42 f5\s+vgetexpps \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 79 9f 42 f5\s+vgetexpps \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 78 18 51 f5\s+vsqrtps \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 78 3f 51 f5\s+vsqrtps \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 78 ff 51 f5\s+vsqrtps \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 58 f4\s+vaddpd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 3f 58 f4\s+vaddpd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 58 f4\s+vaddpd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 58 f4\s+vaddph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 3f 58 f4\s+vaddph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 ff 58 f4\s+vaddph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 58 f4\s+vaddps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 3f 58 f4\s+vaddps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 ff 58 f4\s+vaddps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5e f4\s+vdivpd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 3f 5e f4\s+vdivpd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 5e f4\s+vdivpd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 5e f4\s+vdivph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 3f 5e f4\s+vdivph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 ff 5e f4\s+vdivph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 5e f4\s+vdivps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 3f 5e f4\s+vdivps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 ff 5e f4\s+vdivps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 59 f4\s+vmulpd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 3f 59 f4\s+vmulpd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 59 f4\s+vmulpd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 59 f4\s+vmulph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 3f 59 f4\s+vmulph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 ff 59 f4\s+vmulph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 59 f4\s+vmulps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 3f 59 f4\s+vmulps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 ff 59 f4\s+vmulps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 2c f4\s+vscalefpd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 2c f4\s+vscalefpd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 2c f4\s+vscalefpd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 2c f4\s+vscalefph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 2c f4\s+vscalefph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 2c f4\s+vscalefph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 2c f4\s+vscalefps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 2c f4\s+vscalefps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 2c f4\s+vscalefps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5c f4\s+vsubpd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 3f 5c f4\s+vsubpd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 ff 5c f4\s+vsubpd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 5c f4\s+vsubph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 3f 5c f4\s+vsubph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 ff 5c f4\s+vsubph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 5c f4\s+vsubps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 3f 5c f4\s+vsubps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 ff 5c f4\s+vsubps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5f f4\s+vmaxpd \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 1f 5f f4\s+vmaxpd \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 9f 5f f4\s+vmaxpd \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 5f f4\s+vmaxph \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 1f 5f f4\s+vmaxph \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 9f 5f f4\s+vmaxph \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 5f f4\s+vmaxps \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 1f 5f f4\s+vmaxps \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 9f 5f f4\s+vmaxps \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 d1 18 5d f4\s+vminpd \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 d1 1f 5d f4\s+vminpd \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 d1 9f 5d f4\s+vminpd \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 50 18 5d f4\s+vminph \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 50 1f 5d f4\s+vminph \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 50 9f 5d f4\s+vminph \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 50 18 5d f4\s+vminps \{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 50 1f 5d f4\s+vminps \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 50 9f 5d f4\s+vminps \{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 f9 18 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 f9 1f 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 f9 9f 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 78 18 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 78 1f 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 78 9f 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 79 18 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 79 1f 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 79 9f 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 f9 18 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 f9 1f 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 f9 9f 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 78 18 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 78 1f 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 78 9f 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 79 18 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 79 1f 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 79 9f 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 f9 18 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 f9 1f 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 f9 9f 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 78 18 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 78 1f 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 78 9f 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 79 18 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 79 1f 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 79 9f 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 98 f4\s+vfmadd132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 98 f4\s+vfmadd132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 98 f4\s+vfmadd132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 98 f4\s+vfmadd132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 98 f4\s+vfmadd132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 98 f4\s+vfmadd132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 98 f4\s+vfmadd132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 98 f4\s+vfmadd132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 98 f4\s+vfmadd132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a8 f4\s+vfmadd213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f a8 f4\s+vfmadd213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a8 f4\s+vfmadd213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 a8 f4\s+vfmadd213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f a8 f4\s+vfmadd213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff a8 f4\s+vfmadd213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 a8 f4\s+vfmadd213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f a8 f4\s+vfmadd213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff a8 f4\s+vfmadd213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b8 f4\s+vfmadd231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f b8 f4\s+vfmadd231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b8 f4\s+vfmadd231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 b8 f4\s+vfmadd231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f b8 f4\s+vfmadd231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff b8 f4\s+vfmadd231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 b8 f4\s+vfmadd231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f b8 f4\s+vfmadd231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff b8 f4\s+vfmadd231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 96 f4\s+vfmaddsub132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 96 f4\s+vfmaddsub132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 96 f4\s+vfmaddsub132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 96 f4\s+vfmaddsub132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 96 f4\s+vfmaddsub132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 96 f4\s+vfmaddsub132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 96 f4\s+vfmaddsub132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 96 f4\s+vfmaddsub132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 96 f4\s+vfmaddsub132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a6 f4\s+vfmaddsub213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f a6 f4\s+vfmaddsub213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a6 f4\s+vfmaddsub213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 a6 f4\s+vfmaddsub213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f a6 f4\s+vfmaddsub213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff a6 f4\s+vfmaddsub213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 a6 f4\s+vfmaddsub213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f a6 f4\s+vfmaddsub213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff a6 f4\s+vfmaddsub213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b6 f4\s+vfmaddsub231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f b6 f4\s+vfmaddsub231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b6 f4\s+vfmaddsub231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 b6 f4\s+vfmaddsub231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f b6 f4\s+vfmaddsub231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff b6 f4\s+vfmaddsub231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 b6 f4\s+vfmaddsub231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f b6 f4\s+vfmaddsub231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff b6 f4\s+vfmaddsub231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9a f4\s+vfmsub132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 9a f4\s+vfmsub132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9a f4\s+vfmsub132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 9a f4\s+vfmsub132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 9a f4\s+vfmsub132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9a f4\s+vfmsub132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 9a f4\s+vfmsub132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 9a f4\s+vfmsub132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9a f4\s+vfmsub132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 aa f4\s+vfmsub213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f aa f4\s+vfmsub213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff aa f4\s+vfmsub213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 aa f4\s+vfmsub213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f aa f4\s+vfmsub213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff aa f4\s+vfmsub213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 aa f4\s+vfmsub213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f aa f4\s+vfmsub213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff aa f4\s+vfmsub213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ba f4\s+vfmsub231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f ba f4\s+vfmsub231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ba f4\s+vfmsub231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 ba f4\s+vfmsub231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f ba f4\s+vfmsub231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff ba f4\s+vfmsub231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 ba f4\s+vfmsub231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f ba f4\s+vfmsub231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff ba f4\s+vfmsub231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 97 f4\s+vfmsubadd132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 97 f4\s+vfmsubadd132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 97 f4\s+vfmsubadd132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 97 f4\s+vfmsubadd132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 97 f4\s+vfmsubadd132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 97 f4\s+vfmsubadd132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 97 f4\s+vfmsubadd132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 97 f4\s+vfmsubadd132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 97 f4\s+vfmsubadd132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 a7 f4\s+vfmsubadd213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f a7 f4\s+vfmsubadd213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff a7 f4\s+vfmsubadd213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 a7 f4\s+vfmsubadd213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f a7 f4\s+vfmsubadd213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff a7 f4\s+vfmsubadd213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 a7 f4\s+vfmsubadd213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f a7 f4\s+vfmsubadd213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff a7 f4\s+vfmsubadd213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 b7 f4\s+vfmsubadd231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f b7 f4\s+vfmsubadd231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff b7 f4\s+vfmsubadd231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 b7 f4\s+vfmsubadd231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f b7 f4\s+vfmsubadd231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff b7 f4\s+vfmsubadd231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 b7 f4\s+vfmsubadd231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f b7 f4\s+vfmsubadd231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff b7 f4\s+vfmsubadd231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9c f4\s+vfnmadd132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 9c f4\s+vfnmadd132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9c f4\s+vfnmadd132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 9c f4\s+vfnmadd132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 9c f4\s+vfnmadd132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9c f4\s+vfnmadd132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 9c f4\s+vfnmadd132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 9c f4\s+vfnmadd132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9c f4\s+vfnmadd132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ac f4\s+vfnmadd213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f ac f4\s+vfnmadd213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ac f4\s+vfnmadd213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 ac f4\s+vfnmadd213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f ac f4\s+vfnmadd213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff ac f4\s+vfnmadd213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 ac f4\s+vfnmadd213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f ac f4\s+vfnmadd213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff ac f4\s+vfnmadd213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 bc f4\s+vfnmadd231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f bc f4\s+vfnmadd231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff bc f4\s+vfnmadd231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 bc f4\s+vfnmadd231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f bc f4\s+vfnmadd231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff bc f4\s+vfnmadd231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 bc f4\s+vfnmadd231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f bc f4\s+vfnmadd231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff bc f4\s+vfnmadd231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 9e f4\s+vfnmsub132pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f 9e f4\s+vfnmsub132pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff 9e f4\s+vfnmsub132pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 9e f4\s+vfnmsub132ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f 9e f4\s+vfnmsub132ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff 9e f4\s+vfnmsub132ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 9e f4\s+vfnmsub132ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f 9e f4\s+vfnmsub132ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff 9e f4\s+vfnmsub132ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 ae f4\s+vfnmsub213pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f ae f4\s+vfnmsub213pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff ae f4\s+vfnmsub213pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 ae f4\s+vfnmsub213ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f ae f4\s+vfnmsub213ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff ae f4\s+vfnmsub213ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 ae f4\s+vfnmsub213ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f ae f4\s+vfnmsub213ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff ae f4\s+vfnmsub213ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 d1 18 be f4\s+vfnmsub231pd \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 d1 3f be f4\s+vfnmsub231pd \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 d1 ff be f4\s+vfnmsub231pd \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 51 18 be f4\s+vfnmsub231ph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 51 3f be f4\s+vfnmsub231ph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 51 ff be f4\s+vfnmsub231ph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 51 18 be f4\s+vfnmsub231ps \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 51 3f be f4\s+vfnmsub231ps \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 51 ff be f4\s+vfnmsub231ps \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 d1 18 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 d1 1f 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 d1 9f 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 51 18 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 51 1f 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 51 9f 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 d1 18 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 d1 1f 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 d1 9f 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f3 51 18 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f3 51 1f 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f3 51 9f 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 53 18 56 f4\s+vfcmaddcph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 53 3f 56 f4\s+vfcmaddcph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 53 ff 56 f4\s+vfcmaddcph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 53 18 d6 f4\s+vfcmulcph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 53 3f d6 f4\s+vfcmulcph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 53 ff d6 f4\s+vfcmulcph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 52 18 56 f4\s+vfmaddcph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 52 3f 56 f4\s+vfmaddcph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 52 ff 56 f4\s+vfmaddcph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 52 18 d6 f4\s+vfmulcph \{rn-sae\},%ymm4,%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 52 3f d6 f4\s+vfmulcph \{rd-sae\},%ymm4,%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 52 ff d6 f4\s+vfmulcph \{rz-sae\},%ymm4,%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 5b f5\s+vcvtdq2ph \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 78 3f 5b f5\s+vcvtdq2ph \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 ff 5b f5\s+vcvtdq2ph \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 78 18 5b f5\s+vcvtdq2ps \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 78 3f 5b f5\s+vcvtdq2ps \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 78 ff 5b f5\s+vcvtdq2ps \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 7b 18 7a f5\s+vcvtudq2ph \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 7b 3f 7a f5\s+vcvtudq2ph \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 7b ff 7a f5\s+vcvtudq2ph \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 7b 18 7a f5\s+vcvtudq2ps \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 7b 3f 7a f5\s+vcvtudq2ps \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 7b ff 7a f5\s+vcvtudq2ps \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 fb 18 e6 f5\s+vcvtpd2dq \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 fb 3f e6 f5\s+vcvtpd2dq \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 fb ff e6 f5\s+vcvtpd2dq \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 f9 18 5a f5\s+vcvtpd2ph \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 f9 3f 5a f5\s+vcvtpd2ph \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 f9 ff 5a f5\s+vcvtpd2ph \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 5a f5\s+vcvtpd2ps \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 f9 3f 5a f5\s+vcvtpd2ps \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 5a f5\s+vcvtpd2ps \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f8 18 79 f5\s+vcvtpd2udq \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 f8 3f 79 f5\s+vcvtpd2udq \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f8 ff 79 f5\s+vcvtpd2udq \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 7b f5\s+vcvtpd2qq \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 f9 3f 7b f5\s+vcvtpd2qq \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 7b f5\s+vcvtpd2qq \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 79 f5\s+vcvtpd2uqq \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 f9 3f 79 f5\s+vcvtpd2uqq \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 ff 79 f5\s+vcvtpd2uqq \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 5b f5\s+vcvtph2dq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 3f 5b f5\s+vcvtph2dq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 ff 5b f5\s+vcvtph2dq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 7b f5\s+vcvtph2qq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 3f 7b f5\s+vcvtph2qq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 ff 7b f5\s+vcvtph2qq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 79 f5\s+vcvtph2udq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 3f 79 f5\s+vcvtph2udq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 ff 79 f5\s+vcvtph2udq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 79 f5\s+vcvtph2uqq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 3f 79 f5\s+vcvtph2uqq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 ff 79 f5\s+vcvtph2uqq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 5a f5\s+vcvtph2pd \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 1f 5a f5\s+vcvtph2pd \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 9f 5a f5\s+vcvtph2pd \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f2 79 18 13 f5\s+vcvtph2ps \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f2 79 1f 13 f5\s+vcvtph2ps \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f2 79 9f 13 f5\s+vcvtph2ps \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f6 79 18 13 f5\s+vcvtph2psx \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f6 79 1f 13 f5\s+vcvtph2psx \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f6 79 9f 13 f5\s+vcvtph2psx \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 7d f5\s+vcvtph2uw \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 3f 7d f5\s+vcvtph2uw \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 ff 7d f5\s+vcvtph2uw \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 7d f5\s+vcvtph2w \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 3f 7d f5\s+vcvtph2w \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 ff 7d f5\s+vcvtph2w \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 79 18 5b f5\s+vcvtps2dq \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 79 3f 5b f5\s+vcvtps2dq \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 79 ff 5b f5\s+vcvtps2dq \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 78 18 79 f5\s+vcvtps2udq \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 78 3f 79 f5\s+vcvtps2udq \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 78 ff 79 f5\s+vcvtps2udq \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 78 18 5a f5\s+vcvtps2pd \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 78 1f 5a f5\s+vcvtps2pd \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 78 9f 5a f5\s+vcvtps2pd \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 1d f5\s+vcvtps2phx \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 79 3f 1d f5\s+vcvtps2phx \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 ff 1d f5\s+vcvtps2phx \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 79 18 7b f5\s+vcvtps2qq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 79 3f 7b f5\s+vcvtps2qq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 79 ff 7b f5\s+vcvtps2qq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 79 18 79 f5\s+vcvtps2uqq \{rn-sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 79 3f 79 f5\s+vcvtps2uqq \{rd-sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 79 ff 79 f5\s+vcvtps2uqq \{rz-sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 fa 18 e6 f5\s+vcvtqq2pd \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 fa 3f e6 f5\s+vcvtqq2pd \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 fa ff e6 f5\s+vcvtqq2pd \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 f8 18 5b f5\s+vcvtqq2ph \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 f8 3f 5b f5\s+vcvtqq2ph \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 f8 ff 5b f5\s+vcvtqq2ph \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f8 18 5b f5\s+vcvtqq2ps \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 f8 3f 5b f5\s+vcvtqq2ps \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f8 ff 5b f5\s+vcvtqq2ps \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 fa 18 7a f5\s+vcvtuqq2pd \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 fa 3f 7a f5\s+vcvtuqq2pd \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 fa ff 7a f5\s+vcvtuqq2pd \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 fb 18 7a f5\s+vcvtuqq2ph \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f5 fb 3f 7a f5\s+vcvtuqq2ph \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 fb ff 7a f5\s+vcvtuqq2ph \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 fb 18 7a f5\s+vcvtuqq2ps \{rn-sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 fb 3f 7a f5\s+vcvtuqq2ps \{rd-sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 fb ff 7a f5\s+vcvtuqq2ps \{rz-sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 e6 f5\s+vcvttpd2dq \{sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 f9 1f e6 f5\s+vcvttpd2dq \{sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 9f e6 f5\s+vcvttpd2dq \{sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f8 18 78 f5\s+vcvttpd2udq \{sae\},%ymm5,%xmm6
\s*[a-f0-9]+:\s*62 f1 f8 1f 78 f5\s+vcvttpd2udq \{sae\},%ymm5,%xmm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f8 9f 78 f5\s+vcvttpd2udq \{sae\},%ymm5,%xmm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 7a f5\s+vcvttpd2qq \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 f9 1f 7a f5\s+vcvttpd2qq \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 9f 7a f5\s+vcvttpd2qq \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 f9 18 78 f5\s+vcvttpd2uqq \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 f9 1f 78 f5\s+vcvttpd2uqq \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 f9 9f 78 f5\s+vcvttpd2uqq \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 7a 18 5b f5\s+vcvttph2dq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 7a 1f 5b f5\s+vcvttph2dq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 7a 9f 5b f5\s+vcvttph2dq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 7a f5\s+vcvttph2qq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 1f 7a f5\s+vcvttph2qq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 9f 7a f5\s+vcvttph2qq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 78 f5\s+vcvttph2udq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 1f 78 f5\s+vcvttph2udq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 9f 78 f5\s+vcvttph2udq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 78 f5\s+vcvttph2uqq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 1f 78 f5\s+vcvttph2uqq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 9f 78 f5\s+vcvttph2uqq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 78 18 7c f5\s+vcvttph2uw \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 78 1f 7c f5\s+vcvttph2uw \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 78 9f 7c f5\s+vcvttph2uw \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 79 18 7c f5\s+vcvttph2w \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 79 1f 7c f5\s+vcvttph2w \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 79 9f 7c f5\s+vcvttph2w \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 7a 18 5b f5\s+vcvttps2dq \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 7a 1f 5b f5\s+vcvttps2dq \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 7a 9f 5b f5\s+vcvttps2dq \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 78 18 78 f5\s+vcvttps2udq \{sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 78 1f 78 f5\s+vcvttps2udq \{sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 78 9f 78 f5\s+vcvttps2udq \{sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 79 18 7a f5\s+vcvttps2qq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 79 1f 7a f5\s+vcvttps2qq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 79 9f 7a f5\s+vcvttps2qq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f1 79 18 78 f5\s+vcvttps2uqq \{sae\},%xmm5,%ymm6
\s*[a-f0-9]+:\s*62 f1 79 1f 78 f5\s+vcvttps2uqq \{sae\},%xmm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f1 79 9f 78 f5\s+vcvttps2uqq \{sae\},%xmm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 7b 18 7d f5\s+vcvtuw2ph \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 7b 3f 7d f5\s+vcvtuw2ph \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 7b ff 7d f5\s+vcvtuw2ph \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 f5 7a 18 7d f5\s+vcvtw2ph \{rn-sae\},%ymm5,%ymm6
\s*[a-f0-9]+:\s*62 f5 7a 3f 7d f5\s+vcvtw2ph \{rd-sae\},%ymm5,%ymm6\{%k7\}
\s*[a-f0-9]+:\s*62 f5 7a ff 7d f5\s+vcvtw2ph \{rz-sae\},%ymm5,%ymm6\{%k7\}\{z\}
#pass
