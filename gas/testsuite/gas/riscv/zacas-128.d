#as: -march=rv64i_zacas
#source: zacas-128.s
#objdump: -d

.*:[ 	]+file format .*

Disassembly of section .text:

0+000 <target>:
[ 	]+[0-9a-f]+:[ 	]+28b6652f[ 	]+amocas.q[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+28b6652f[ 	]+amocas.q[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6652f[ 	]+amocas.q.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6652f[ 	]+amocas.q.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6652f[ 	]+amocas.q.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6652f[ 	]+amocas.q.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6652f[ 	]+amocas.q.aqrl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6652f[ 	]+amocas.q.aqrl[ 	]+a0,a1,\(a2\)
