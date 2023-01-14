#source: x86-64-lfence-ret.s
#as: -mlfence-before-ret=not
#warning_output: x86-64-lfence-ret.e
#objdump: -dw -Mintel64
#name: x86-64 -mlfence-before-ret=not

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 c3                	data16 retq 
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 c2 14 00          	data16 retq \$0x14
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c3                   	retq   
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c2 1e 00             	retq   \$0x1e
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 48 c3             	data16 rex.W retq 
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 48 c2 28 00       	data16 rex.W retq \$0x28
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%rsp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 cb                	lretw  
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%rsp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 ca 28 00          	lretw  \$0x28
 +[a-f0-9]+:	f7 14 24             	notl   \(%rsp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	cb                   	lret   
 +[a-f0-9]+:	f7 14 24             	notl   \(%rsp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	ca 28 00             	lret   \$0x28
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	48 cb                	lretq  
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	48 f7 14 24          	notq   \(%rsp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	48 ca 28 00          	lretq  \$0x28
#pass
