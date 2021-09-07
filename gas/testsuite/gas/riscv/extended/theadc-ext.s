.text

wsc

# cache ext
dcache.call
dcache.ciall
dcache.cipa a0
dcache.cisw a0
dcache.civa a0
dcache.cpa a0
dcache.cpal1 a0
dcache.cva a0
dcache.cval1 a0
dcache.ipa a0
dcache.isw a0
dcache.iva a0
dcache.iall
dcache.csw a0
icache.iall
icache.ialls
icache.iva a0
icache.ipa a0
l2cache.iall
l2cache.call
l2cache.ciall

# sync ext
sfence.vmas       a0, a1
sync
sync.i
sync.s
sync.is

# calc ext
addsl             a0, a1, a2, 1
mula              a0, a1, a2
muls              a0, a1, a2
mulaw             a0, a1, a2
mulsw             a0, a1, a2
mulah             a0, a1, a2
mulsh             a0, a1, a2
srri              a0, a1, 16
srriw             a0, a1, 16
mveqz             a0, a1, a2
mvnez             a0, a1, a2

# bit ext
tst               a0, a1, 16
tstnbz            a0, a1
ext               a0, a1, 16, 16
extu              a0, a1, 16, 16
ff1               a0, a1
ff0               a0, a1
rev               a0, a1
revw       a0, a1

# load/store ext
flrd       f0, a0, a1, 1
flrw       f0, a0, a1, 1
flurd      f0, a0, a1, 1
flurw      f0, a0, a1, 1
lrb               a0, a1, a2, 1
lrh               a0, a1, a2, 1
lrw               a0, a1, a2, 1
lrd               a0, a1, a2, 1
lrbu              a0, a1, a2, 1
lrhu              a0, a1, a2, 1
lrwu              a0, a1, a2, 1
lurb              a0, a1, a2, 1
lurh              a0, a1, a2, 1
lurw              a0, a1, a2, 1
lurd              a0, a1, a2, 1
lurbu             a0, a1, a2, 1
lurhu             a0, a1, a2, 1
lurwu             a0, a1, a2, 1
lbia       a0, (a1), 15, 1
lbib       a0, (a1), 15, 1
lhia       a0, (a1), 15, 1
lhib       a0, (a1), 15, 1
lwia       a0, (a1), 15, 1
lwib       a0, (a1), 15, 1
ldia       a0, (a1), 15, 1
ldib       a0, (a1), 15, 1
lbuia      a0, (a1), 15, 1
lbuib      a0, (a1), 15, 1
lhuia      a0, (a1), 15, 1
lhuib      a0, (a1), 15, 1
lwuia      a0, (a1), 15, 1
lwuib      a0, (a1), 15, 1
ldd               a0, a1, (a2), 1
lwd               a0, a1, (a2), 1
lwud              a0, a1, (a2), 1
fsrd       f0, a0, a1, 1
fsrw       f0, a0, a1, 1
fsurd      f0, a0, a1, 1
fsurw      f0, a0, a1, 1
srb               a0, a1, a2, 1
srh               a0, a1, a2, 1
srw               a0, a1, a2, 1
srd               a0, a1, a2, 1
surb              a0, a1, a2, 1
surh              a0, a1, a2, 1
surw              a0, a1, a2, 1
surd              a0, a1, a2, 1
sbia              a0, (a1), 15, 1
sbib              a0, (a1), 15, 1
shia              a0, (a1), 15, 1
shib              a0, (a1), 15, 1
swia              a0, (a1), 12, 1
swib              a0, (a1), 15, 1
sdia              a0, (a1), 15, 1
sdib              a0, (a1), 15, 1
sdd               a0, a1, (a2), 1
swd               a0, a1, (a2), 1
