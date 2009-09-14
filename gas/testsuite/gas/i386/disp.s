	.text
	mov 0x7fffffff(%eax),%ebx
	mov 0x80000000(%eax),%ebx
	mov -0x80000000(%eax),%ebx
	mov 0x7fffffff,%ebx
	mov 0x80000000,%ebx
	mov -0x80000000,%ebx
	mov 0x7fffffff,%eax
	mov 0x80000000,%eax
	mov -0x80000000,%eax
