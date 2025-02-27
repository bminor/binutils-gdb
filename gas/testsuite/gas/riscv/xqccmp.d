#as: -march=rv32i_xqccmp
#source: xqccmp.s
#objdump: -dr -Mno-aliases

.*:[	 ]+file format .*


Disassembly of section .text:

0+000 <target>:
[	 ]*[0-9a-f]+:[	 ]+ba42[	 ]+qc.cm.pop[	 ]+\{ra\},16
[	 ]*[0-9a-f]+:[	 ]+be42[	 ]+qc.cm.popret[	 ]+\{ra\},16
[	 ]*[0-9a-f]+:[	 ]+bc42[	 ]+qc.cm.popretz[	 ]+\{ra\},16
[	 ]*[0-9a-f]+:[	 ]+b942[	 ]+qc.cm.pushfp[	 ]+\{ra\},16
[	 ]*[0-9a-f]+:[	 ]+b842[	 ]+qc.cm.push[	 ]+\{ra\},-16
[	 ]*[0-9a-f]+:[	 ]+ac7e[	 ]+qc.cm.mva01s[	 ]+s0,s7
[	 ]*[0-9a-f]+:[	 ]+afa2[	 ]+qc.cm.mvsa01[	 ]+s7,s0
