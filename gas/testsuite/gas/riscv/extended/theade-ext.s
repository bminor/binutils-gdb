.text

wsc

# cache ext
dcache.ipa a0
dcache.cpa a0
dcache.cipa a0
dcache.isw a0
dcache.csw a0
dcache.cisw a0
dcache.iall
dcache.call
dcache.ciall
icache.iall
icache.ipa a0

# sync ext
sync
sync.i

# calc ext
addsl             a0, a1, a2, 1
srri              a0, a1, 16
mula              a0, a1, a2
mulah             a0, a1, a2
muls              a0, a1, a2
mulsh             a0, a1, a2
mveqz             a0, a1, a2
mvnez             a0, a1, a2

# bit ext
ext               a0, a1, 16, 16
extu              a0, a1, 16, 16
ff1               a0, a1
ff0               a0, a1
rev               a0, a1
tst               a0, a1, 16
tstnbz            a0, a1

# load/store ext
lrb               a0, a1, a2, 1
lrh               a0, a1, a2, 1
lrw               a0, a1, a2, 1
lrbu              a0, a1, a2, 1
lrhu              a0, a1, a2, 1
lbia       a0, (a1), 15, 1
lbib       a0, (a1), 15, 1
lhia       a0, (a1), 15, 1
lhib       a0, (a1), 15, 1
lwia       a0, (a1), 15, 1
lwib       a0, (a1), 15, 1
srb               a0, a1, a2, 1
srh               a0, a1, a2, 1
srw               a0, a1, a2, 1
sbia              a0, (a1), 15, 1
sbib              a0, (a1), 15, 1
shia              a0, (a1), 15, 1
shib              a0, (a1), 15, 1
swia              a0, (a1), 12, 1
swib              a0, (a1), 15, 1

fmv.x.hw   a0, f0
fmv.hw.x   f0, a0
ipush
ipop
