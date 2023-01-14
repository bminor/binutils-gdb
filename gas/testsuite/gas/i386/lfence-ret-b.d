#source: lfence-ret.s
#as: -mlfence-before-ret=not
#objdump: -dw
#name: -mlfence-before-ret=not

.*: +file format .*


Disassembly of section .text:

0+ <_start>:
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 c3                	retw   
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 c2 14 00          	retw   \$0x14
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c3                   	ret    
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	c2 1e 00             	ret    \$0x1e
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 cb                	lretw  
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	66 f7 14 24          	notw   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	66 ca 28 00          	lretw  \$0x28
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	cb                   	lret   
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	f7 14 24             	notl   \(%esp\)
 +[a-f0-9]+:	0f ae e8             	lfence 
 +[a-f0-9]+:	ca 28 00             	lret   \$0x28
#pass
