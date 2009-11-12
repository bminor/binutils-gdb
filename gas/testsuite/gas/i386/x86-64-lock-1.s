# 64bit lockable Instructions

	.text
foo:
	lock add (%rbx), %eax
	lock add $0x64, (%rbx)
	lock adc (%rbx), %eax
	lock adc $0x64, (%rbx)
	lock and (%rbx), %eax
	lock and $0x64, (%rbx)
	lock btc %eax, (%rbx)
	lock btc $0x64, (%rbx)
	lock btr %eax, (%rbx)
	lock btr $0x64, (%rbx)
	lock bts %eax, (%rbx)
	lock bts $0x64, (%rbx)
	lock cmpxchg %eax,(%rbx)
	lock cmpxchg8b (%rbx)
	lock decl (%rbx)
	lock incl (%rbx)
	lock negl (%rbx)
	lock notl (%rbx)
	lock or (%rbx), %eax
	lock or $0x64, (%rbx)
	lock sbb (%rbx), %eax
	lock sbb $0x64, (%rbx)
	lock sub (%rbx), %eax
	lock sub $0x64, (%rbx)
	lock xadd %eax, (%rbx)
	lock xchg (%rbx), %eax
	lock xchg %eax, (%rbx)
	lock xor (%rbx), %eax
	lock xor $0x64, (%rbx)

	.intel_syntax noprefix
	lock add eax,DWORD PTR [rbx]
	lock add DWORD PTR [rbx],0x64
	lock adc eax,DWORD PTR [rbx]
	lock adc DWORD PTR [rbx],0x64
	lock and eax,DWORD PTR [rbx]
	lock and DWORD PTR [rbx],0x64
	lock btc DWORD PTR [rbx],eax
	lock btc DWORD PTR [rbx],0x64
	lock btr DWORD PTR [rbx],eax
	lock btr DWORD PTR [rbx],0x64
	lock bts DWORD PTR [rbx],eax
	lock bts DWORD PTR [rbx],0x64
	lock cmpxchg DWORD PTR [rbx],eax
	lock cmpxchg8b QWORD PTR [rbx]
	lock dec DWORD PTR [rbx]
	lock inc DWORD PTR [rbx]
	lock neg DWORD PTR [rbx]
	lock not DWORD PTR [rbx]
	lock or eax,DWORD PTR [rbx]
	lock or DWORD PTR [rbx],0x64
	lock sbb eax,DWORD PTR [rbx]
	lock sbb DWORD PTR [rbx],0x64
	lock sub eax,DWORD PTR [rbx]
	lock sub DWORD PTR [rbx],0x64
	lock xadd DWORD PTR [rbx],eax
	lock xchg DWORD PTR [rbx],eax
	lock xchg DWORD PTR [rbx],eax
	lock xor eax,DWORD PTR [rbx]
	lock xor DWORD PTR [rbx],0x64
