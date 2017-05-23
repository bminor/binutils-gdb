# Check CET instructions
	.text
_start:
	incsspd
	rdsspd %ecx
	savessp
	rstorssp (%ecx)
	wrssd %eax, (%edx, %eax)
	wrussd %edx, (%edi, %ebp)
	setssbsy (%eax)
	clrssbsy (%esp, %eax)
	endbr64
	endbr32

	.intel_syntax noprefix
	incsspd
	rdsspd ecx
	savessp
	rstorssp QWORD PTR [ecx + eax]
	wrssd [edx],eax
	wrussd [edi + ebp],edx
	setssbsy QWORD PTR [eax]
	clrssbsy QWORD PTR [esp + eax]
	endbr64
	endbr32
