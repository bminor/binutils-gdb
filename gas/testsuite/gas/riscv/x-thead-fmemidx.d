#as: -march=rv64i_xtheadfmemidx
#source: x-thead-fmemidx.s
#objdump: -dr

.*:[ 	]+file format .*

Disassembly of section .text:

0+000 <target>:
[ 	]+[0-9a-f]+:[ 	]+60c5e50b[ 	]+th.flrd[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+66c5e50b[ 	]+th.flrd[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+40c5e50b[ 	]+th.flrw[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+46c5e50b[ 	]+th.flrw[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+70c5e50b[ 	]+th.flurd[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+76c5e50b[ 	]+th.flurd[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+50c5e50b[ 	]+th.flurw[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+56c5e50b[ 	]+th.flurw[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+60c5f50b[ 	]+th.fsrd[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+66c5f50b[ 	]+th.fsrd[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+40c5f50b[ 	]+th.fsrw[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+46c5f50b[ 	]+th.fsrw[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+70c5f50b[ 	]+th.fsurd[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+76c5f50b[ 	]+th.fsurd[ 	]+a0,a1,a2,3
[ 	]+[0-9a-f]+:[ 	]+50c5f50b[ 	]+th.fsurw[ 	]+a0,a1,a2,0
[ 	]+[0-9a-f]+:[ 	]+56c5f50b[ 	]+th.fsurw[ 	]+a0,a1,a2,3
