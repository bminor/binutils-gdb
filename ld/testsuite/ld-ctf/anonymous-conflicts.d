#as:
#source: anonymous-conflicts.c
#source: anonymous-conflicts-B.c
#objdump: --ctf
#ld: -shared --ctf-variables
#name: Conflicted anonymous struct/union names

.*: +file format .*

Contents of CTF section .ctf:

  Header:
    Magic number: 0xdff2
    Version: 5 \(CTF_VERSION_4\)
#...
    0x[0-9a-f]*: \(kind 6\) struct A \(.*
                \[0x0\] : ID 0x[0-9a-f]*: \(kind 7\) union  \(.*
#...
    0x[0-9a-f]*: \(kind 6\) struct A \(.*
                \[0x0\] : ID 0x[0-9a-f]*: \(kind 7\) union  \(.*
#...
