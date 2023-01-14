# Check INVLPGB instructions

	.text
_start:
        invlpgb
.ifdef __amd64__
att64:
        invlpgb %rax, %edx
.endif
att32:
        invlpgb %eax, %edx
.ifndef __amd64__
att16:
        invlpgb %ax, %edx
.endif
