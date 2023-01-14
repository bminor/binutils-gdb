# Check Illegal 64bit AMX-FP16 instructions

.text
	#tdpfp16ps %tmm5,%tmm4,%tmm3 set VEX.W = 1 (illegal value).
	.byte 0xc4
	.byte 0xe2
	.byte 0xd3
	.byte 0x5c
	.byte 0xdc
	.fill 0x05, 0x01, 0x90
	#tdpfp16ps %tmm5,%tmm4,%tmm3 set VEX.L = 1 (illegal value).
	.byte 0xc4
	.byte 0xe2
	.byte 0x57
	.byte 0x5c
	.byte 0xdc
	.fill 0x05, 0x01, 0x90
	#tdpfp16ps %tmm5,%tmm4,%tmm3 set VEX.R = 0 (illegal value).
	.byte 0xc4
	.byte 0x62
	.byte 0x53
	.byte 0x5c
	.byte 0xdc
	#tdpbf16ps %tmm5,%tmm4,%tmm3 set VEX.B = 0 (illegal value).
	.byte 0xc4
	.byte 0xc2
	.byte 0x53
	.byte 0x5c
	.byte 0xdc
	#tdpbf16ps %tmm5,%tmm4,%tmm3 set VEX.VVVV = 0110 (illegal value).
	.byte 0xc4
	.byte 0xe2
	.byte 0x33
	.byte 0x5c
	.byte 0xdc
