# Check CET instructions
	.text
_start:
	incsspd %ecx
	rdsspd %ecx
	saveprevssp
	rstorssp (%ecx)
	wrssd %eax, (%edx, %eax)
	wrussd %edx, (%edi, %ebp)
	setssbsy
	clrssbsy (%esp, %eax)
	endbr64
	endbr32

	.intel_syntax noprefix
	incsspd ecx
	rdsspd ecx
	saveprevssp
	rstorssp QWORD PTR [ecx + eax]
	wrssd [edx],eax
	wrssd dword ptr [eax],edx
	wrussd [edi + ebp],edx
	wrussd dword ptr [edx + ebp],edi
	setssbsy
	clrssbsy QWORD PTR [esp + eax]
	endbr64
	endbr32
