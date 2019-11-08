	.text
	.code16
	rep; movsd
	rep; cmpsd
	rep movsd %ds:(%si),%es:(%di)
	rep cmpsd %es:(%di),%ds:(%si)
	.intel_syntax noprefix
	rep movsd dword ptr es:[di], dword ptr ds:[si]
	rep cmpsd dword ptr ds:[si], dword ptr es:[di]
