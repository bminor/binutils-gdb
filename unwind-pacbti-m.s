
	.arch armv8.1-m.main
	.arch_extension pacbti
	.eabi_attribute 50, 2
	.eabi_attribute 52, 2
	.eabi_attribute 74, 1
	.eabi_attribute 76, 1
	.text
	.syntax unified
	.thumb
	.thumb_func
foo:
	.fnstart
	push	{r4, lr}
	.save {r4, lr}
	push	{r12}
	.save {ra_auth_code}
	pop	{r12}
	pop	{r4, pc}
	.fnend
