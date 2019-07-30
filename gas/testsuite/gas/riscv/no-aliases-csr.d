#source: alias-csr.s
#as: -march=rv64if
#objdump: -dr -Mno-aliases

.*:[ 	]+file format .*


Disassembly of section .text:

0+000 <alias_csr>:
[ 	]+0:[ 	]+003022f3[ 	]+csrrs[ 	]+t0,fcsr,zero
[ 	]+4:[ 	]+003392f3[ 	]+csrrw[ 	]+t0,fcsr,t2
[ 	]+8:[ 	]+00339073[ 	]+csrrw[ 	]+zero,fcsr,t2
[ 	]+c:[ 	]+002022f3[ 	]+csrrs[ 	]+t0,frm,zero
[ 	]+10:[ 	]+002312f3[ 	]+csrrw[ 	]+t0,frm,t1
[ 	]+14:[ 	]+00231073[ 	]+csrrw[ 	]+zero,frm,t1
[ 	]+18:[ 	]+002fd2f3[ 	]+csrrwi[ 	]+t0,frm,31
[ 	]+1c:[ 	]+002fd073[ 	]+csrrwi[ 	]+zero,frm,31
[ 	]+20:[ 	]+001022f3[ 	]+csrrs[ 	]+t0,fflags,zero
[ 	]+24:[ 	]+001312f3[ 	]+csrrw[ 	]+t0,fflags,t1
[ 	]+28:[ 	]+00131073[ 	]+csrrw[ 	]+zero,fflags,t1
[ 	]+2c:[ 	]+001fd2f3[ 	]+csrrwi[ 	]+t0,fflags,31
[ 	]+30:[ 	]+001fd073[ 	]+csrrwi[ 	]+zero,fflags,31
