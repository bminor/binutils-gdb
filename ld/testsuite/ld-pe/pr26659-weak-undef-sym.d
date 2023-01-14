#source: pr26659-weak-undef-sym.s
#target: x86_64-*-cygwin* x86_64-*-pe x86_64-*-mingw*
#ld: -e0
#objdump: -d

#...
0000000140001000 <foo>:
   140001000:	55                   	push   %rbp
   140001001:	48 89 e5             	mov    %rsp,%rbp
   140001004:	48 83 ec 20          	sub    \$0x20,%rsp
   140001008:	89 4d 10             	mov    %ecx,0x10\(%rbp\)
   14000100b:	48 8b 05 ee 0f 00 00 	mov    0xfee\(%rip\),%rax        # 140002000 <__data_end__>
   140001012:	48 85 c0             	test   %rax,%rax
   140001015:	74 05                	je     14000101c <foo\+0x1c>
   140001017:	e8 e4 ef ff bf       	call   100000000 <__size_of_stack_reserve__\+0xffe00000>
   14000101c:	48 8b 05 ed 0f 00 00 	mov    0xfed\(%rip\),%rax        # 140002010 <.refptr.bar2>
   140001023:	48 85 c0             	test   %rax,%rax
   140001026:	74 05                	je     14000102d <foo\+0x2d>
   140001028:	e8 d3 ef ff bf       	call   100000000 <__size_of_stack_reserve__\+0xffe00000>
   14000102d:	8b 45 10             	mov    0x10\(%rbp\),%eax
   140001030:	0f af c0             	imul   %eax,%eax
   140001033:	48 83 c4 20          	add    \$0x20,%rsp
   140001037:	5d                   	pop    %rbp
   140001038:	c3                   	ret    
   140001039:	90                   	nop
   14000103a:	90                   	nop
   14000103b:	90                   	nop
   14000103c:	90                   	nop
   14000103d:	90                   	nop
   14000103e:	90                   	nop
   14000103f:	90                   	nop
#pass
