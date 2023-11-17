.*: Assembler messages:
.*:7: Warning: default
.*:46: Error: ambiguous operand size for `vcvtpd2ph'
.*:47: Error: ambiguous operand size for `vcvtpd2ps'
.*:48: Error: ambiguous operand size for `vfpclassps'
.*:7: Warning: \.avx10\.1/256
.*:12: Error: vector size .* `kunpckdq'
.*:16: Error: vector size .* `kaddq'
.*:17: Error: vector size .* `kandq'
.*:18: Error: vector size .* `kandnq'
.*:19: Error: vector size .* `kmovq'
.*:20: Error: vector size .* `kmovq'
.*:21: Error: vector size .* `kmovq'
.*:22: Error: vector size .* `knotq'
.*:23: Error: vector size .* `korq'
.*:24: Error: vector size .* `kortestq'
.*:25: Error: vector size .* `kshiftlq'
.*:26: Error: vector size .* `kshiftrq'
.*:27: Error: vector size .* `ktestq'
.*:28: Error: vector size .* `kxorq'
.*:29: Error: vector size .* `kxnorq'
.*:32: Error: vector size .* `vcvtpd2phz'
.*:34: Error: unsupported broadcast for `vcvtpd2ph'
.*:37: Error: vector size .* for `vcvtpd2ps'
.*:43: Error: vector size .* `vfpclasspsz'
.*:46: Error: ambiguous operand size for `vcvtpd2ph'
.*:47: Error: ambiguous operand size for `vcvtpd2ps'
.*:48: Error: ambiguous operand size for `vfpclassps'
.*:7: Warning: \.avx10\.1/128
.*:10: Error: vector size .* `kmovd'
.*:11: Error: vector size .* `kmovd'
.*:12: Error: vector size .* `kunpckdq'
.*:13: Error: vector size .* `kunpckwd'
.*:16: Error: vector size .* `kaddd'
.*:17: Error: vector size .* `kandd'
.*:18: Error: vector size .* `kandnd'
.*:19: Error: vector size .* `kmovd'
.*:20: Error: vector size .* `kmovd'
.*:21: Error: vector size .* `kmovd'
.*:22: Error: vector size .* `knotd'
.*:23: Error: vector size .* `kord'
.*:24: Error: vector size .* `kortestd'
.*:25: Error: vector size .* `kshiftld'
.*:26: Error: vector size .* `kshiftrd'
.*:27: Error: vector size .* `ktestd'
.*:28: Error: vector size .* `kxord'
.*:29: Error: vector size .* `kxnord'
.*:16: Error: vector size .* `kaddq'
.*:17: Error: vector size .* `kandq'
.*:18: Error: vector size .* `kandnq'
.*:19: Error: vector size .* `kmovq'
.*:20: Error: vector size .* `kmovq'
.*:21: Error: vector size .* `kmovq'
.*:22: Error: vector size .* `knotq'
.*:23: Error: vector size .* `korq'
.*:24: Error: vector size .* `kortestq'
.*:25: Error: vector size .* `kshiftlq'
.*:26: Error: vector size .* `kshiftrq'
.*:27: Error: vector size .* `ktestq'
.*:28: Error: vector size .* `kxorq'
.*:29: Error: vector size .* `kxnorq'
.*:32: Error: vector size .* `vcvtpd2phz'
.*:33: Error: vector size .* `vcvtpd2phy'
.*:34: Error: unsupported broadcast for `vcvtpd2ph'
.*:35: Error: unsupported broadcast for `vcvtpd2ph'
.*:37: Error: .*
.*:38: Error: vector size .* `vcvtpd2psy'
.*:39: Error: vector size .* `vcvtpd2psy'
.*:40: Error: unsupported broadcast for `vcvtpd2ps'
.*:42: Error: vector size .* `vfpclasspsy'
.*:43: Error: vector size .* `vfpclasspsz'
.*:7: Warning: \.avx10\.1
.*:46: Error: ambiguous operand size for `vcvtpd2ph'
.*:47: Error: ambiguous operand size for `vcvtpd2ps'
.*:48: Error: ambiguous operand size for `vfpclassps'
#...
[ 	]*[0-9]+[ 	]+>  \.arch generic32
[ 	]*[0-9]+[ 	]+>  \.arch default
[ 	]*[0-9]+[ 	]+\?+ C5FB93D1[ 	]+>  kmovd %k1,%edx
[ 	]*[0-9]+[ 	]+\?+ C5FB92D1[ 	]+>  kmovd %ecx,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC4B D9[ 	]+>  kunpckdq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C5EC4BD9[ 	]+>  kunpckwd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.irp sz,d,q
[ 	]*[0-9]+[ 	]+\?+ C4E1ED4A D9[ 	]+>>  kaddd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED41 D9[ 	]+>>  kandd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED42 D9[ 	]+>>  kandnd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 D1[ 	]+>>  kmovd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F991 0A[ 	]+>>  kmovd %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 11[ 	]+>>  kmovd \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F944 D1[ 	]+>>  knotd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED45 D9[ 	]+>>  kord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F998 D1[ 	]+>>  kortestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E37933 DA01[ 	]+>>  kshiftld \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E37931 DA01[ 	]+>>  kshiftrd \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F999 D1[ 	]+>>  ktestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED47 D9[ 	]+>>  kxord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED46 D9[ 	]+>>  kxnord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC4A D9[ 	]+>>  kaddq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC41 D9[ 	]+>>  kandq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC42 D9[ 	]+>>  kandnq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F890 D1[ 	]+>>  kmovq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F891 0A[ 	]+>>  kmovq %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+\?+ C4E1F890 11[ 	]+>>  kmovq \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F844 D1[ 	]+>>  knotq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC45 D9[ 	]+>>  korq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F898 D1[ 	]+>>  kortestq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E3F933 DA01[ 	]+>>  kshiftlq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E3F931 DA01[ 	]+>>  kshiftrq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F899 D1[ 	]+>>  ktestq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC47 D9[ 	]+>>  kxorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC46 D9[ 	]+>>  kxnorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F5FD48 5A00[ 	]+>  vcvtpd2phz \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD28 5A00[ 	]+>  vcvtpd2phy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD58 5A00[ 	]+>  vcvtpd2ph \(%eax\)\{1to8\},%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD38 5A00[ 	]+>  vcvtpd2ph \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F1FD48 5A00[ 	]+>  vcvtpd2ps \(%eax\),%ymm0
[ 	]*[0-9]+[ 	]+\?+ C5FD5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F1FD29 5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0\{%k1\}
[ 	]*[0-9]+[ 	]+\?+ 62F1FD38 5A00[ 	]+>  vcvtpd2ps \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F37D28 660000[ 	]+>  vfpclasspsy \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+\?+ 62F37D48 660000[ 	]+>  vfpclasspsz \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.intel_syntax noprefix
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vfpclassps k0,\[eax\],0
#...
[ 	]*[0-9]+[ 	]+>  \.arch generic32
[ 	]*[0-9]+[ 	]+>  \.arch \.avx10\.1/256
[ 	]*[0-9]+[ 	]+\?+ C5FB93D1[ 	]+>  kmovd %k1,%edx
[ 	]*[0-9]+[ 	]+\?+ C5FB92D1[ 	]+>  kmovd %ecx,%k2
[ 	]*[0-9]+[ 	]+>  kunpckdq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C5EC4BD9[ 	]+>  kunpckwd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.irp sz,d,q
[ 	]*[0-9]+[ 	]+\?+ C4E1ED4A D9[ 	]+>>  kaddd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED41 D9[ 	]+>>  kandd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED42 D9[ 	]+>>  kandnd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 D1[ 	]+>>  kmovd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F991 0A[ 	]+>>  kmovd %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 11[ 	]+>>  kmovd \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F944 D1[ 	]+>>  knotd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED45 D9[ 	]+>>  kord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F998 D1[ 	]+>>  kortestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E37933 DA01[ 	]+>>  kshiftld \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E37931 DA01[ 	]+>>  kshiftrd \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F999 D1[ 	]+>>  ktestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED47 D9[ 	]+>>  kxord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED46 D9[ 	]+>>  kxnord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kaddq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandnq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kmovq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kmovq %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+>>  kmovq \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+>>  knotq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  korq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kortestq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kshiftlq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kshiftrq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  ktestq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kxorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kxnorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  vcvtpd2phz \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD28 5A00[ 	]+>  vcvtpd2phy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph \(%eax\)\{1to8\},%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD38 5A00[ 	]+>  vcvtpd2ph \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps \(%eax\),%ymm0
[ 	]*[0-9]+[ 	]+\?+ C5FD5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F1FD29 5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0\{%k1\}
[ 	]*[0-9]+[ 	]+\?+ 62F1FD38 5A00[ 	]+>  vcvtpd2ps \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F37D28 660000[ 	]+>  vfpclasspsy \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+>  vfpclasspsz \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.intel_syntax noprefix
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vfpclassps k0,\[eax\],0
#...
[ 	]*[0-9]+[ 	]+>  \.arch generic32
[ 	]*[0-9]+[ 	]+>  \.arch \.avx10\.1/128
[ 	]*[0-9]+[ 	]+>  kmovd %k1,%edx
[ 	]*[0-9]+[ 	]+>  kmovd %ecx,%k2
[ 	]*[0-9]+[ 	]+>  kunpckdq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>  kunpckwd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.irp sz,d,q
[ 	]*[0-9]+[ 	]+>>  kaddd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandnd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kmovd %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kmovd %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+>>  kmovd \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+>>  knotd %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kortestd %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kshiftld \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kshiftrd \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  ktestd %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kxord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kxnord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kaddq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kandnq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kmovq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kmovq %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+>>  kmovq \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+>>  knotq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  korq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kortestq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kshiftlq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kshiftrq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  ktestq %k1,%k2
[ 	]*[0-9]+[ 	]+>>  kxorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+>>  kxnorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  vcvtpd2phz \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2phy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph \(%eax\)\{1to8\},%xmm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps \(%eax\),%ymm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2psy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+>  vcvtpd2psy \(%eax\),%xmm0\{%k1\}
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  vfpclasspsy \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+>  vfpclasspsz \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.intel_syntax noprefix
[ 	]*[0-9]+[ 	]+\?+ 62F5FD08 5A00[ 	]+>  vcvtpd2ph xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+\?+ C5F95A00[ 	]+>  vcvtpd2ps xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+\?+ 62F37D08 660000[ 	]+>  vfpclassps k0,\[eax\],0
#...
[ 	]*[0-9]+[ 	]+\?+ C5FB93D1[ 	]+>  kmovd %k1,%edx
[ 	]*[0-9]+[ 	]+\?+ C5FB92D1[ 	]+>  kmovd %ecx,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC4B D9[ 	]+>  kunpckdq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C5EC4BD9[ 	]+>  kunpckwd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.irp sz,d,q
[ 	]*[0-9]+[ 	]+\?+ C4E1ED4A D9[ 	]+>>  kaddd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED41 D9[ 	]+>>  kandd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED42 D9[ 	]+>>  kandnd %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 D1[ 	]+>>  kmovd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F991 0A[ 	]+>>  kmovd %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+\?+ C4E1F990 11[ 	]+>>  kmovd \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F944 D1[ 	]+>>  knotd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED45 D9[ 	]+>>  kord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F998 D1[ 	]+>>  kortestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E37933 DA01[ 	]+>>  kshiftld \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E37931 DA01[ 	]+>>  kshiftrd \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F999 D1[ 	]+>>  ktestd %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1ED47 D9[ 	]+>>  kxord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1ED46 D9[ 	]+>>  kxnord %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC4A D9[ 	]+>>  kaddq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC41 D9[ 	]+>>  kandq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC42 D9[ 	]+>>  kandnq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F890 D1[ 	]+>>  kmovq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F891 0A[ 	]+>>  kmovq %k1,\(%edx\)
[ 	]*[0-9]+[ 	]+\?+ C4E1F890 11[ 	]+>>  kmovq \(%ecx\),%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1F844 D1[ 	]+>>  knotq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC45 D9[ 	]+>>  korq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F898 D1[ 	]+>>  kortestq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E3F933 DA01[ 	]+>>  kshiftlq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E3F931 DA01[ 	]+>>  kshiftrq \$1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1F899 D1[ 	]+>>  ktestq %k1,%k2
[ 	]*[0-9]+[ 	]+\?+ C4E1EC47 D9[ 	]+>>  kxorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+\?+ C4E1EC46 D9[ 	]+>>  kxnorq %k1,%k2,%k3
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F5FD48 5A00[ 	]+>  vcvtpd2phz \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD28 5A00[ 	]+>  vcvtpd2phy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD58 5A00[ 	]+>  vcvtpd2ph \(%eax\)\{1to8\},%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F5FD38 5A00[ 	]+>  vcvtpd2ph \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F1FD48 5A00[ 	]+>  vcvtpd2ps \(%eax\),%ymm0
[ 	]*[0-9]+[ 	]+\?+ C5FD5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0
[ 	]*[0-9]+[ 	]+\?+ 62F1FD29 5A00[ 	]+>  vcvtpd2psy \(%eax\),%xmm0\{%k1\}
[ 	]*[0-9]+[ 	]+\?+ 62F1FD38 5A00[ 	]+>  vcvtpd2ps \(%eax\)\{1to4\},%xmm0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+\?+ 62F37D28 660000[ 	]+>  vfpclasspsy \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+\?+ 62F37D48 660000[ 	]+>  vfpclasspsz \$0,\(%eax\),%k0
[ 	]*[0-9]+[ 	]+> *
[ 	]*[0-9]+[ 	]+>  \.intel_syntax noprefix
[ 	]*[0-9]+[ 	]+>  vcvtpd2ph xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vcvtpd2ps xmm0,\[eax\]
[ 	]*[0-9]+[ 	]+>  vfpclassps k0,\[eax\],0
#pass
