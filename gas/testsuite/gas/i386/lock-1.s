# Lockable Instructions

	.text
foo:
	lock add (%ebx), %eax
	lock add $0x64, (%ebx)
	lock adc (%ebx), %eax
	lock adc $0x64, (%ebx)
	lock and (%ebx), %eax
	lock and $0x64, (%ebx)
	lock btc %eax, (%ebx)
	lock btc $0x64, (%ebx)
	lock btr %eax, (%ebx)
	lock btr $0x64, (%ebx)
	lock bts %eax, (%ebx)
	lock bts $0x64, (%ebx)
	lock cmpxchg %eax,(%ebx)
	lock cmpxchg8b (%ebx)
	lock decl (%ebx)
	lock incl (%ebx)
	lock negl (%ebx)
	lock notl (%ebx)
	lock or (%ebx), %eax
	lock or $0x64, (%ebx)
	lock sbb (%ebx), %eax
	lock sbb $0x64, (%ebx)
	lock sub (%ebx), %eax
	lock sub $0x64, (%ebx)
	lock xadd %eax, (%ebx)
	lock xchg (%ebx), %eax
	lock xchg %eax, (%ebx)
	lock xor (%ebx), %eax
	lock xor $0x64, (%ebx)

	.intel_syntax noprefix
	lock add eax,DWORD PTR [ebx]
	lock add DWORD PTR [ebx],0x64
	lock adc eax,DWORD PTR [ebx]
	lock adc DWORD PTR [ebx],0x64
	lock and eax,DWORD PTR [ebx]
	lock and DWORD PTR [ebx],0x64
	lock btc DWORD PTR [ebx],eax
	lock btc DWORD PTR [ebx],0x64
	lock btr DWORD PTR [ebx],eax
	lock btr DWORD PTR [ebx],0x64
	lock bts DWORD PTR [ebx],eax
	lock bts DWORD PTR [ebx],0x64
	lock cmpxchg DWORD PTR [ebx],eax
	lock cmpxchg8b QWORD PTR [ebx]
	lock dec DWORD PTR [ebx]
	lock inc DWORD PTR [ebx]
	lock neg DWORD PTR [ebx]
	lock not DWORD PTR [ebx]
	lock or eax,DWORD PTR [ebx]
	lock or DWORD PTR [ebx],0x64
	lock sbb eax,DWORD PTR [ebx]
	lock sbb DWORD PTR [ebx],0x64
	lock sub eax,DWORD PTR [ebx]
	lock sub DWORD PTR [ebx],0x64
	lock xadd DWORD PTR [ebx],eax
	lock xchg DWORD PTR [ebx],eax
	lock xchg DWORD PTR [ebx],eax
	lock xor eax,DWORD PTR [ebx]
	lock xor DWORD PTR [ebx],0x64
