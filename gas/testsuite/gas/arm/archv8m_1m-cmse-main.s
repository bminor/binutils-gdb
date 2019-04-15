.thumb
.syntax unified

T:

clrm {r0, r2} @ Accepts list without APSR
clrm {APSR} @ Accepts APSR alone
clrm {r3, APSR} @ Accepts core register and APSR together
clrmeq {r4} @ Accepts conditional execution
