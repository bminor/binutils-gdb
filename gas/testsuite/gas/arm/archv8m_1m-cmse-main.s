.thumb
.syntax unified

T:

clrm {r0, r2} @ Accepts list without APSR
clrm {APSR} @ Accepts APSR alone
clrm {r3, APSR} @ Accepts core register and APSR together
clrmeq {r4} @ Accepts conditional execution

vscclrm {VPR} @ Accepts list with only VPR
vscclrm {s30, VPR} @ Accept single-precision VFP register and VPR together
vscclrm {d14, VPR} @ Likewise for double-precision VFP register
vscclrm {s1-s4, VPR} @ Accept range of single-precision VFP registers
		     @ and VPR together
vscclrm {d1-d4, VPR} @ Likewise for double-precision VFP registers
vscclrm {s0-s31, VPR} @ Accept all single-precision VFP registers and VPR
		      @ together
vscclrm {d0-d15, VPR} @ Likewise for double-precision VFP registers
vscclrmne {s3, VPR} @ Accepts conditional execution
