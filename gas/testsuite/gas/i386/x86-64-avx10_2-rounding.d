#objdump: -dw
#name: x86_64 AVX10.2 rounding insns

.*: +file format .*

Disassembly of section \.text:

0+ <_start>:
\s*[a-f0-9]+:\s*62 91 91 10 c2 ec 7b\s+vcmppd \$0x7b,\{sae\},%ymm28,%ymm29,%k5
\s*[a-f0-9]+:\s*62 91 91 17 c2 ec 7b\s+vcmppd \$0x7b,\{sae\},%ymm28,%ymm29,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 02 f9 18 42 f5\s+vgetexppd \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 f9 1f 42 f5\s+vgetexppd \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 f9 9f 42 f5\s+vgetexppd \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 51 f5\s+vsqrtpd \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 f9 3f 51 f5\s+vsqrtpd \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 ff 51 f5\s+vsqrtpd \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 93 10 10 c2 ec 7b\s+vcmpph \$0x7b,\{sae\},%ymm28,%ymm29,%k5
\s*[a-f0-9]+:\s*62 93 10 17 c2 ec 7b\s+vcmpph \$0x7b,\{sae\},%ymm28,%ymm29,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 06 79 18 42 f5\s+vgetexpph \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 79 1f 42 f5\s+vgetexpph \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 79 9f 42 f5\s+vgetexpph \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 51 f5\s+vsqrtph \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 3f 51 f5\s+vsqrtph \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 ff 51 f5\s+vsqrtph \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 91 10 10 c2 ec 7b\s+vcmpps \$0x7b,\{sae\},%ymm28,%ymm29,%k5
\s*[a-f0-9]+:\s*62 91 10 17 c2 ec 7b\s+vcmpps \$0x7b,\{sae\},%ymm28,%ymm29,%k5\{%k7\}
\s*[a-f0-9]+:\s*62 02 79 18 42 f5\s+vgetexpps \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 79 1f 42 f5\s+vgetexpps \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 79 9f 42 f5\s+vgetexpps \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 78 18 51 f5\s+vsqrtps \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 78 3f 51 f5\s+vsqrtps \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 78 ff 51 f5\s+vsqrtps \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 58 f4\s+vaddpd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 37 58 f4\s+vaddpd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 f7 58 f4\s+vaddpd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 58 f4\s+vaddph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 37 58 f4\s+vaddph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 f7 58 f4\s+vaddph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 58 f4\s+vaddps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 37 58 f4\s+vaddps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 f7 58 f4\s+vaddps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 5e f4\s+vdivpd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 37 5e f4\s+vdivpd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 f7 5e f4\s+vdivpd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 5e f4\s+vdivph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 37 5e f4\s+vdivph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 f7 5e f4\s+vdivph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 5e f4\s+vdivps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 37 5e f4\s+vdivps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 f7 5e f4\s+vdivps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 59 f4\s+vmulpd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 37 59 f4\s+vmulpd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 f7 59 f4\s+vmulpd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 59 f4\s+vmulph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 37 59 f4\s+vmulph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 f7 59 f4\s+vmulph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 59 f4\s+vmulps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 37 59 f4\s+vmulps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 f7 59 f4\s+vmulps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 2c f4\s+vscalefpd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 2c f4\s+vscalefpd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 2c f4\s+vscalefpd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 2c f4\s+vscalefph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 2c f4\s+vscalefph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 2c f4\s+vscalefph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 2c f4\s+vscalefps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 2c f4\s+vscalefps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 2c f4\s+vscalefps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 5c f4\s+vsubpd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 37 5c f4\s+vsubpd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 f7 5c f4\s+vsubpd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 5c f4\s+vsubph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 37 5c f4\s+vsubph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 f7 5c f4\s+vsubph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 5c f4\s+vsubps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 37 5c f4\s+vsubps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 f7 5c f4\s+vsubps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 5f f4\s+vmaxpd \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 17 5f f4\s+vmaxpd \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 97 5f f4\s+vmaxpd \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 5f f4\s+vmaxph \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 17 5f f4\s+vmaxph \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 97 5f f4\s+vmaxph \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 5f f4\s+vmaxps \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 17 5f f4\s+vmaxps \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 97 5f f4\s+vmaxps \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 91 10 5d f4\s+vminpd \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 91 17 5d f4\s+vminpd \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 91 97 5d f4\s+vminpd \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 10 10 5d f4\s+vminph \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 10 17 5d f4\s+vminph \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 10 97 5d f4\s+vminph \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 10 10 5d f4\s+vminps \{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 10 17 5d f4\s+vminps \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 10 97 5d f4\s+vminps \{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 f9 18 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 f9 1f 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 f9 9f 26 f5 7b\s+vgetmantpd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 78 18 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 78 1f 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 78 9f 26 f5 7b\s+vgetmantph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 79 18 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 79 1f 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 79 9f 26 f5 7b\s+vgetmantps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 f9 18 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 f9 1f 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 f9 9f 56 f5 7b\s+vreducepd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 78 18 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 78 1f 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 78 9f 56 f5 7b\s+vreduceph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 79 18 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 79 1f 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 79 9f 56 f5 7b\s+vreduceps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 f9 18 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 f9 1f 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 f9 9f 09 f5 7b\s+vrndscalepd \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 78 18 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 78 1f 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 78 9f 08 f5 7b\s+vrndscaleph \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 79 18 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 79 1f 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 79 9f 08 f5 7b\s+vrndscaleps \$0x7b,\{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 98 f4\s+vfmadd132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 98 f4\s+vfmadd132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 98 f4\s+vfmadd132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 98 f4\s+vfmadd132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 98 f4\s+vfmadd132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 98 f4\s+vfmadd132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 98 f4\s+vfmadd132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 98 f4\s+vfmadd132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 98 f4\s+vfmadd132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 a8 f4\s+vfmadd213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 a8 f4\s+vfmadd213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 a8 f4\s+vfmadd213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 a8 f4\s+vfmadd213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 a8 f4\s+vfmadd213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 a8 f4\s+vfmadd213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 a8 f4\s+vfmadd213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 a8 f4\s+vfmadd213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 a8 f4\s+vfmadd213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 b8 f4\s+vfmadd231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 b8 f4\s+vfmadd231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 b8 f4\s+vfmadd231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 b8 f4\s+vfmadd231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 b8 f4\s+vfmadd231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 b8 f4\s+vfmadd231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 b8 f4\s+vfmadd231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 b8 f4\s+vfmadd231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 b8 f4\s+vfmadd231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 96 f4\s+vfmaddsub132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 96 f4\s+vfmaddsub132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 96 f4\s+vfmaddsub132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 96 f4\s+vfmaddsub132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 96 f4\s+vfmaddsub132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 96 f4\s+vfmaddsub132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 96 f4\s+vfmaddsub132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 96 f4\s+vfmaddsub132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 96 f4\s+vfmaddsub132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 a6 f4\s+vfmaddsub213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 a6 f4\s+vfmaddsub213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 a6 f4\s+vfmaddsub213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 a6 f4\s+vfmaddsub213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 a6 f4\s+vfmaddsub213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 a6 f4\s+vfmaddsub213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 a6 f4\s+vfmaddsub213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 a6 f4\s+vfmaddsub213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 a6 f4\s+vfmaddsub213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 b6 f4\s+vfmaddsub231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 b6 f4\s+vfmaddsub231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 b6 f4\s+vfmaddsub231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 b6 f4\s+vfmaddsub231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 b6 f4\s+vfmaddsub231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 b6 f4\s+vfmaddsub231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 b6 f4\s+vfmaddsub231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 b6 f4\s+vfmaddsub231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 b6 f4\s+vfmaddsub231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 9a f4\s+vfmsub132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 9a f4\s+vfmsub132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 9a f4\s+vfmsub132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 9a f4\s+vfmsub132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 9a f4\s+vfmsub132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 9a f4\s+vfmsub132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 9a f4\s+vfmsub132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 9a f4\s+vfmsub132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 9a f4\s+vfmsub132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 aa f4\s+vfmsub213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 aa f4\s+vfmsub213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 aa f4\s+vfmsub213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 aa f4\s+vfmsub213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 aa f4\s+vfmsub213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 aa f4\s+vfmsub213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 aa f4\s+vfmsub213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 aa f4\s+vfmsub213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 aa f4\s+vfmsub213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 ba f4\s+vfmsub231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 ba f4\s+vfmsub231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 ba f4\s+vfmsub231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 ba f4\s+vfmsub231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 ba f4\s+vfmsub231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 ba f4\s+vfmsub231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 ba f4\s+vfmsub231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 ba f4\s+vfmsub231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 ba f4\s+vfmsub231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 97 f4\s+vfmsubadd132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 97 f4\s+vfmsubadd132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 97 f4\s+vfmsubadd132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 97 f4\s+vfmsubadd132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 97 f4\s+vfmsubadd132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 97 f4\s+vfmsubadd132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 97 f4\s+vfmsubadd132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 97 f4\s+vfmsubadd132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 97 f4\s+vfmsubadd132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 a7 f4\s+vfmsubadd213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 a7 f4\s+vfmsubadd213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 a7 f4\s+vfmsubadd213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 a7 f4\s+vfmsubadd213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 a7 f4\s+vfmsubadd213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 a7 f4\s+vfmsubadd213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 a7 f4\s+vfmsubadd213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 a7 f4\s+vfmsubadd213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 a7 f4\s+vfmsubadd213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 b7 f4\s+vfmsubadd231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 b7 f4\s+vfmsubadd231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 b7 f4\s+vfmsubadd231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 b7 f4\s+vfmsubadd231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 b7 f4\s+vfmsubadd231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 b7 f4\s+vfmsubadd231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 b7 f4\s+vfmsubadd231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 b7 f4\s+vfmsubadd231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 b7 f4\s+vfmsubadd231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 9c f4\s+vfnmadd132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 9c f4\s+vfnmadd132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 9c f4\s+vfnmadd132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 9c f4\s+vfnmadd132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 9c f4\s+vfnmadd132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 9c f4\s+vfnmadd132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 9c f4\s+vfnmadd132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 9c f4\s+vfnmadd132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 9c f4\s+vfnmadd132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 ac f4\s+vfnmadd213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 ac f4\s+vfnmadd213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 ac f4\s+vfnmadd213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 ac f4\s+vfnmadd213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 ac f4\s+vfnmadd213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 ac f4\s+vfnmadd213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 ac f4\s+vfnmadd213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 ac f4\s+vfnmadd213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 ac f4\s+vfnmadd213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 bc f4\s+vfnmadd231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 bc f4\s+vfnmadd231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 bc f4\s+vfnmadd231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 bc f4\s+vfnmadd231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 bc f4\s+vfnmadd231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 bc f4\s+vfnmadd231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 bc f4\s+vfnmadd231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 bc f4\s+vfnmadd231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 bc f4\s+vfnmadd231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 9e f4\s+vfnmsub132pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 9e f4\s+vfnmsub132pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 9e f4\s+vfnmsub132pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 9e f4\s+vfnmsub132ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 9e f4\s+vfnmsub132ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 9e f4\s+vfnmsub132ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 9e f4\s+vfnmsub132ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 9e f4\s+vfnmsub132ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 9e f4\s+vfnmsub132ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 ae f4\s+vfnmsub213pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 ae f4\s+vfnmsub213pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 ae f4\s+vfnmsub213pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 ae f4\s+vfnmsub213ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 ae f4\s+vfnmsub213ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 ae f4\s+vfnmsub213ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 ae f4\s+vfnmsub213ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 ae f4\s+vfnmsub213ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 ae f4\s+vfnmsub213ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 91 10 be f4\s+vfnmsub231pd \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 91 37 be f4\s+vfnmsub231pd \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 91 f7 be f4\s+vfnmsub231pd \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 11 10 be f4\s+vfnmsub231ph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 11 37 be f4\s+vfnmsub231ph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 11 f7 be f4\s+vfnmsub231ph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 11 10 be f4\s+vfnmsub231ps \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 02 11 37 be f4\s+vfnmsub231ps \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 11 f7 be f4\s+vfnmsub231ps \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 91 10 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 91 17 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 91 97 54 f4 7b\s+vfixupimmpd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 11 10 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 11 17 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 11 97 54 f4 7b\s+vfixupimmps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 91 10 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 91 17 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 91 97 50 f4 7b\s+vrangepd \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 03 11 10 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 03 11 17 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 03 11 97 50 f4 7b\s+vrangeps \$0x7b,\{sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 13 10 56 f4\s+vfcmaddcph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 13 37 56 f4\s+vfcmaddcph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 13 f7 56 f4\s+vfcmaddcph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 13 10 d6 f4\s+vfcmulcph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 13 37 d6 f4\s+vfcmulcph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 13 f7 d6 f4\s+vfcmulcph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 12 10 56 f4\s+vfmaddcph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 12 37 56 f4\s+vfmaddcph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 12 f7 56 f4\s+vfmaddcph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 12 10 d6 f4\s+vfmulcph \{rn-sae\},%ymm28,%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 06 12 37 d6 f4\s+vfmulcph \{rd-sae\},%ymm28,%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 12 f7 d6 f4\s+vfmulcph \{rz-sae\},%ymm28,%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 5b f5\s+vcvtdq2ph \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 78 3f 5b f5\s+vcvtdq2ph \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 ff 5b f5\s+vcvtdq2ph \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 78 18 5b f5\s+vcvtdq2ps \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 78 3f 5b f5\s+vcvtdq2ps \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 78 ff 5b f5\s+vcvtdq2ps \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 7b 18 7a f5\s+vcvtudq2ph \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 7b 3f 7a f5\s+vcvtudq2ph \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 7b ff 7a f5\s+vcvtudq2ph \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 7b 18 7a f5\s+vcvtudq2ps \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 7b 3f 7a f5\s+vcvtudq2ps \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 7b ff 7a f5\s+vcvtudq2ps \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 fb 18 e6 f5\s+vcvtpd2dq \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 fb 3f e6 f5\s+vcvtpd2dq \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 fb ff e6 f5\s+vcvtpd2dq \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 f9 18 5a f5\s+vcvtpd2ph \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 f9 3f 5a f5\s+vcvtpd2ph \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 f9 ff 5a f5\s+vcvtpd2ph \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 5a f5\s+vcvtpd2ps \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 f9 3f 5a f5\s+vcvtpd2ps \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 ff 5a f5\s+vcvtpd2ps \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f8 18 79 f5\s+vcvtpd2udq \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 f8 3f 79 f5\s+vcvtpd2udq \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f8 ff 79 f5\s+vcvtpd2udq \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 7b f5\s+vcvtpd2qq \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 f9 3f 7b f5\s+vcvtpd2qq \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 ff 7b f5\s+vcvtpd2qq \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 79 f5\s+vcvtpd2uqq \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 f9 3f 79 f5\s+vcvtpd2uqq \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 ff 79 f5\s+vcvtpd2uqq \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 5b f5\s+vcvtph2dq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 3f 5b f5\s+vcvtph2dq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 ff 5b f5\s+vcvtph2dq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 7b f5\s+vcvtph2qq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 3f 7b f5\s+vcvtph2qq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 ff 7b f5\s+vcvtph2qq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 79 f5\s+vcvtph2udq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 3f 79 f5\s+vcvtph2udq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 ff 79 f5\s+vcvtph2udq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 79 f5\s+vcvtph2uqq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 3f 79 f5\s+vcvtph2uqq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 ff 79 f5\s+vcvtph2uqq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 5a f5\s+vcvtph2pd \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 1f 5a f5\s+vcvtph2pd \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 9f 5a f5\s+vcvtph2pd \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 02 79 18 13 f5\s+vcvtph2ps \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 02 79 1f 13 f5\s+vcvtph2ps \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 02 79 9f 13 f5\s+vcvtph2ps \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 06 79 18 13 f5\s+vcvtph2psx \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 06 79 1f 13 f5\s+vcvtph2psx \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 06 79 9f 13 f5\s+vcvtph2psx \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 7d f5\s+vcvtph2uw \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 3f 7d f5\s+vcvtph2uw \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 ff 7d f5\s+vcvtph2uw \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 7d f5\s+vcvtph2w \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 3f 7d f5\s+vcvtph2w \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 ff 7d f5\s+vcvtph2w \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 79 18 5b f5\s+vcvtps2dq \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 79 3f 5b f5\s+vcvtps2dq \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 79 ff 5b f5\s+vcvtps2dq \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 78 18 79 f5\s+vcvtps2udq \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 78 3f 79 f5\s+vcvtps2udq \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 78 ff 79 f5\s+vcvtps2udq \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 78 18 5a f5\s+vcvtps2pd \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 01 78 1f 5a f5\s+vcvtps2pd \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 78 9f 5a f5\s+vcvtps2pd \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 1d f5\s+vcvtps2phx \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 79 3f 1d f5\s+vcvtps2phx \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 ff 1d f5\s+vcvtps2phx \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 79 18 7b f5\s+vcvtps2qq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 01 79 3f 7b f5\s+vcvtps2qq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 79 ff 7b f5\s+vcvtps2qq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 79 18 79 f5\s+vcvtps2uqq \{rn-sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 01 79 3f 79 f5\s+vcvtps2uqq \{rd-sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 79 ff 79 f5\s+vcvtps2uqq \{rz-sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 fa 18 e6 f5\s+vcvtqq2pd \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 fa 3f e6 f5\s+vcvtqq2pd \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 fa ff e6 f5\s+vcvtqq2pd \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 f8 18 5b f5\s+vcvtqq2ph \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 f8 3f 5b f5\s+vcvtqq2ph \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 f8 ff 5b f5\s+vcvtqq2ph \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f8 18 5b f5\s+vcvtqq2ps \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 f8 3f 5b f5\s+vcvtqq2ps \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f8 ff 5b f5\s+vcvtqq2ps \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 fa 18 7a f5\s+vcvtuqq2pd \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 fa 3f 7a f5\s+vcvtuqq2pd \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 fa ff 7a f5\s+vcvtuqq2pd \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 fb 18 7a f5\s+vcvtuqq2ph \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 05 fb 3f 7a f5\s+vcvtuqq2ph \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 fb ff 7a f5\s+vcvtuqq2ph \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 fb 18 7a f5\s+vcvtuqq2ps \{rn-sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 fb 3f 7a f5\s+vcvtuqq2ps \{rd-sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 fb ff 7a f5\s+vcvtuqq2ps \{rz-sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 e6 f5\s+vcvttpd2dq \{sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 f9 1f e6 f5\s+vcvttpd2dq \{sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 9f e6 f5\s+vcvttpd2dq \{sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f8 18 78 f5\s+vcvttpd2udq \{sae\},%ymm29,%xmm30
\s*[a-f0-9]+:\s*62 01 f8 1f 78 f5\s+vcvttpd2udq \{sae\},%ymm29,%xmm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f8 9f 78 f5\s+vcvttpd2udq \{sae\},%ymm29,%xmm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 7a f5\s+vcvttpd2qq \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 f9 1f 7a f5\s+vcvttpd2qq \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 9f 7a f5\s+vcvttpd2qq \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 f9 18 78 f5\s+vcvttpd2uqq \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 f9 1f 78 f5\s+vcvttpd2uqq \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 f9 9f 78 f5\s+vcvttpd2uqq \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 7a 18 5b f5\s+vcvttph2dq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 7a 1f 5b f5\s+vcvttph2dq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 7a 9f 5b f5\s+vcvttph2dq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 7a f5\s+vcvttph2qq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 1f 7a f5\s+vcvttph2qq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 9f 7a f5\s+vcvttph2qq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 78 f5\s+vcvttph2udq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 1f 78 f5\s+vcvttph2udq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 9f 78 f5\s+vcvttph2udq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 78 f5\s+vcvttph2uqq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 1f 78 f5\s+vcvttph2uqq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 9f 78 f5\s+vcvttph2uqq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 78 18 7c f5\s+vcvttph2uw \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 78 1f 7c f5\s+vcvttph2uw \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 78 9f 7c f5\s+vcvttph2uw \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 79 18 7c f5\s+vcvttph2w \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 79 1f 7c f5\s+vcvttph2w \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 79 9f 7c f5\s+vcvttph2w \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 7a 18 5b f5\s+vcvttps2dq \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 7a 1f 5b f5\s+vcvttps2dq \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 7a 9f 5b f5\s+vcvttps2dq \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 78 18 78 f5\s+vcvttps2udq \{sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 01 78 1f 78 f5\s+vcvttps2udq \{sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 78 9f 78 f5\s+vcvttps2udq \{sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 79 18 7a f5\s+vcvttps2qq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 01 79 1f 7a f5\s+vcvttps2qq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 79 9f 7a f5\s+vcvttps2qq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 01 79 18 78 f5\s+vcvttps2uqq \{sae\},%xmm29,%ymm30
\s*[a-f0-9]+:\s*62 01 79 1f 78 f5\s+vcvttps2uqq \{sae\},%xmm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 01 79 9f 78 f5\s+vcvttps2uqq \{sae\},%xmm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 7b 18 7d f5\s+vcvtuw2ph \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 7b 3f 7d f5\s+vcvtuw2ph \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 7b ff 7d f5\s+vcvtuw2ph \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
\s*[a-f0-9]+:\s*62 05 7a 18 7d f5\s+vcvtw2ph \{rn-sae\},%ymm29,%ymm30
\s*[a-f0-9]+:\s*62 05 7a 3f 7d f5\s+vcvtw2ph \{rd-sae\},%ymm29,%ymm30\{%k7\}
\s*[a-f0-9]+:\s*62 05 7a ff 7d f5\s+vcvtw2ph \{rz-sae\},%ymm29,%ymm30\{%k7\}\{z\}
#pass
