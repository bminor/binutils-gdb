#as: -march=rv64i -mpriv-spec=1.12
#source: h-ext-64.s
#objdump: -d

.*:[  	]+file format .*


Disassembly of section .text:

0+000 <.text>:
[ 	]+[0-9a-f]+:[ 	]+22000073[ 	]+hfence.vvma
[ 	]+[0-9a-f]+:[ 	]+22050073[ 	]+hfence.vvma[ 	]+a0
[ 	]+[0-9a-f]+:[ 	]+22b00073[ 	]+hfence.vvma[ 	]+zero,a1
[ 	]+[0-9a-f]+:[ 	]+22c58073[ 	]+hfence.vvma[ 	]+a1,a2
[ 	]+[0-9a-f]+:[ 	]+62000073[ 	]+hfence.gvma
[ 	]+[0-9a-f]+:[ 	]+62050073[ 	]+hfence.gvma[ 	]+a0
[ 	]+[0-9a-f]+:[ 	]+62b00073[ 	]+hfence.gvma[ 	]+zero,a1
[ 	]+[0-9a-f]+:[ 	]+62c58073[ 	]+hfence.gvma[ 	]+a1,a2
[     	]+[0-9a-f]+:[  	]+6005c573[    	]+hlv.b[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+6005c573[    	]+hlv.b[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+6015c573[    	]+hlv.bu[     	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+640645f3[    	]+hlv.h[      	]+a1,\(a2\)
[     	]+[0-9a-f]+:[  	]+6415c5f3[    	]+hlv.hu[     	]+a1,\(a1\)
[     	]+[0-9a-f]+:[  	]+643645f3[    	]+hlvx.hu[    	]+a1,\(a2\)
[     	]+[0-9a-f]+:[  	]+68064673[    	]+hlv.w[      	]+a2,\(a2\)
[     	]+[0-9a-f]+:[  	]+6816c673[    	]+hlv.wu[     	]+a2,\(a3\)
[     	]+[0-9a-f]+:[  	]+6836c673[    	]+hlvx.wu[    	]+a2,\(a3\)
[     	]+[0-9a-f]+:[  	]+6c0746f3[    	]+hlv.d[      	]+a3,\(a4\)
[     	]+[0-9a-f]+:[  	]+62a5c073[    	]+hsv.b[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+62a5c073[    	]+hsv.b[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+66a5c073[    	]+hsv.h[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+6aa5c073[    	]+hsv.w[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+6ea5c073[    	]+hsv.d[      	]+a0,\(a1\)
[     	]+[0-9a-f]+:[  	]+60002573[    	]+csrr[       	]+a0,hstatus
[     	]+[0-9a-f]+:[  	]+60059073[    	]+csrw[       	]+hstatus,a1
[     	]+[0-9a-f]+:[  	]+60202573[    	]+csrr[       	]+a0,hedeleg
[     	]+[0-9a-f]+:[  	]+60259073[    	]+csrw[       	]+hedeleg,a1
[     	]+[0-9a-f]+:[  	]+60302573[    	]+csrr[       	]+a0,hideleg
[     	]+[0-9a-f]+:[  	]+60359073[    	]+csrw[       	]+hideleg,a1
[     	]+[0-9a-f]+:[  	]+60402573[    	]+csrr[       	]+a0,hie
[     	]+[0-9a-f]+:[  	]+60459073[    	]+csrw[       	]+hie,a1
[     	]+[0-9a-f]+:[  	]+60602573[    	]+csrr[       	]+a0,hcounteren
[     	]+[0-9a-f]+:[  	]+60659073[    	]+csrw[       	]+hcounteren,a1
[     	]+[0-9a-f]+:[  	]+60702573[    	]+csrr[       	]+a0,hgeie
[     	]+[0-9a-f]+:[  	]+60759073[    	]+csrw[       	]+hgeie,a1
[     	]+[0-9a-f]+:[  	]+64302573[    	]+csrr[       	]+a0,htval
[     	]+[0-9a-f]+:[  	]+64359073[    	]+csrw[       	]+htval,a1
[     	]+[0-9a-f]+:[  	]+64402573[    	]+csrr[       	]+a0,hip
[     	]+[0-9a-f]+:[  	]+64459073[    	]+csrw[       	]+hip,a1
[     	]+[0-9a-f]+:[  	]+64502573[    	]+csrr[       	]+a0,hvip
[     	]+[0-9a-f]+:[  	]+64559073[    	]+csrw[       	]+hvip,a1
[     	]+[0-9a-f]+:[  	]+64a02573[    	]+csrr[       	]+a0,htinst
[     	]+[0-9a-f]+:[  	]+64a59073[    	]+csrw[       	]+htinst,a1
[     	]+[0-9a-f]+:[  	]+e1202573[    	]+csrr[       	]+a0,hgeip
[     	]+[0-9a-f]+:[  	]+e1259073[    	]+csrw[       	]+hgeip,a1
[     	]+[0-9a-f]+:[  	]+60a02573[    	]+csrr[       	]+a0,henvcfg
[     	]+[0-9a-f]+:[  	]+60a59073[    	]+csrw[       	]+henvcfg,a1
[     	]+[0-9a-f]+:[  	]+68002573[    	]+csrr[       	]+a0,hgatp
[     	]+[0-9a-f]+:[  	]+68059073[    	]+csrw[       	]+hgatp,a1
[     	]+[0-9a-f]+:[  	]+6a802573[    	]+csrr[       	]+a0,hcontext
[     	]+[0-9a-f]+:[  	]+6a859073[    	]+csrw[       	]+hcontext,a1
[     	]+[0-9a-f]+:[  	]+60502573[    	]+csrr[       	]+a0,htimedelta
[     	]+[0-9a-f]+:[  	]+60559073[    	]+csrw[       	]+htimedelta,a1
[     	]+[0-9a-f]+:[  	]+20002573[    	]+csrr[       	]+a0,vsstatus
[     	]+[0-9a-f]+:[  	]+20059073[    	]+csrw[       	]+vsstatus,a1
[     	]+[0-9a-f]+:[  	]+20402573[    	]+csrr[       	]+a0,vsie
[     	]+[0-9a-f]+:[  	]+20459073[    	]+csrw[       	]+vsie,a1
[     	]+[0-9a-f]+:[  	]+20502573[    	]+csrr[       	]+a0,vstvec
[     	]+[0-9a-f]+:[  	]+20559073[    	]+csrw[       	]+vstvec,a1
[     	]+[0-9a-f]+:[  	]+24002573[    	]+csrr[       	]+a0,vsscratch
[     	]+[0-9a-f]+:[  	]+24059073[    	]+csrw[       	]+vsscratch,a1
[     	]+[0-9a-f]+:[  	]+24102573[    	]+csrr[       	]+a0,vsepc
[     	]+[0-9a-f]+:[  	]+24159073[    	]+csrw[       	]+vsepc,a1
[     	]+[0-9a-f]+:[  	]+24202573[    	]+csrr[       	]+a0,vscause
[     	]+[0-9a-f]+:[  	]+24259073[    	]+csrw[       	]+vscause,a1
[     	]+[0-9a-f]+:[  	]+24302573[    	]+csrr[       	]+a0,vstval
[     	]+[0-9a-f]+:[  	]+24359073[    	]+csrw[       	]+vstval,a1
[     	]+[0-9a-f]+:[  	]+24402573[    	]+csrr[       	]+a0,vsip
[     	]+[0-9a-f]+:[  	]+24459073[    	]+csrw[       	]+vsip,a1
[     	]+[0-9a-f]+:[  	]+28002573[    	]+csrr[       	]+a0,vsatp
[     	]+[0-9a-f]+:[  	]+28059073[    	]+csrw[       	]+vsatp,a1
