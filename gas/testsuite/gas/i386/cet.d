#objdump: -dw
#name: i386 CET

.*:     file format .*

Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	f3 0f ae e9          	incsspd %ecx
 +[a-f0-9]+:	f3 0f 1e c9          	rdsspd %ecx
 +[a-f0-9]+:	f3 0f 01 ea          	saveprevssp 
 +[a-f0-9]+:	f3 0f 01 29          	rstorssp \(%ecx\)
 +[a-f0-9]+:	0f 38 f6 04 02       	wrssd  %eax,\(%edx,%eax,1\)
 +[a-f0-9]+:	66 0f 38 f5 14 2f    	wrussd %edx,\(%edi,%ebp,1\)
 +[a-f0-9]+:	f3 0f 01 e8          	setssbsy 
 +[a-f0-9]+:	f3 0f ae 34 04       	clrssbsy \(%esp,%eax,1\)
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	f3 0f 1e fb          	endbr32 
 +[a-f0-9]+:	f3 0f ae e9          	incsspd %ecx
 +[a-f0-9]+:	f3 0f 1e c9          	rdsspd %ecx
 +[a-f0-9]+:	f3 0f 01 ea          	saveprevssp 
 +[a-f0-9]+:	f3 0f 01 2c 01       	rstorssp \(%ecx,%eax,1\)
 +[a-f0-9]+:	0f 38 f6 02          	wrssd  %eax,\(%edx\)
 +[a-f0-9]+:	0f 38 f6 10          	wrssd  %edx,\(%eax\)
 +[a-f0-9]+:	66 0f 38 f5 14 2f    	wrussd %edx,\(%edi,%ebp,1\)
 +[a-f0-9]+:	66 0f 38 f5 3c 2a    	wrussd %edi,\(%edx,%ebp,1\)
 +[a-f0-9]+:	f3 0f 01 e8          	setssbsy 
 +[a-f0-9]+:	f3 0f ae 34 04       	clrssbsy \(%esp,%eax,1\)
 +[a-f0-9]+:	f3 0f 1e fa          	endbr64 
 +[a-f0-9]+:	f3 0f 1e fb          	endbr32 
#pass
