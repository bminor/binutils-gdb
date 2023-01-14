# Check SNP instructions

	.text
.ifdef __amd64__
att64:
        psmash  %rax
        psmash
        psmash  %eax
        pvalidate  %rax
        pvalidate  %eax
        rmpupdate  %rax
        rmpupdate
        rmpupdate  %eax
        rmpadjust  %rax
        rmpadjust
        rmpadjust  %eax
.endif
.ifndef __amd64__
att32:
        pvalidate
        pvalidate  %eax
        pvalidate  %ax
.endif
