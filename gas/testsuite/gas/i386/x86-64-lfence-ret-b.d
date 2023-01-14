#source: x86-64-lfence-ret.s
#as: -mlfence-before-ret=not
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
#pass
