#as: -march=rv64i_zacas
#source: zacas-64.s
#objdump: -d

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <target>:
[ 	]+[0-9a-f]+:[ 	]+28b6252f[ 	]+amocas.w[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+28b6252f[ 	]+amocas.w[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6252f[ 	]+amocas.w.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6252f[ 	]+amocas.w.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6252f[ 	]+amocas.w.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6252f[ 	]+amocas.w.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6252f[ 	]+amocas.w.aqrl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6252f[ 	]+amocas.w.aqrl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+28b6352f[ 	]+amocas.d[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+28b6352f[ 	]+amocas.d[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6352f[ 	]+amocas.d.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2cb6352f[ 	]+amocas.d.aq[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6352f[ 	]+amocas.d.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2ab6352f[ 	]+amocas.d.rl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6352f[ 	]+amocas.d.aqrl[ 	]+a0,a1,\(a2\)
[ 	]+[0-9a-f]+:[ 	]+2eb6352f[ 	]+amocas.d.aqrl[ 	]+a0,a1,\(a2\)
