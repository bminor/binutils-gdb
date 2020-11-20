#as:
#source: data-func-1.c
#source: data-func-2.c
#objdump: --ctf=.ctf
#ld: -shared -s
#name: Conflicted data syms, partially indexed, stripped

.*: +file format .*

Contents of CTF section \.ctf:

  Header:
    Magic number: dff2
    Version: 4 \(CTF_VERSION_3\)
#...
    Data object section:	.* \(0xc bytes\)
    Function info section:	.* \(0x40 bytes\)
    Object index section:	.* \(0xc bytes\)
    Type section:	.* \(0xe8 bytes\)
    String section:	.*
#...
  Data objects:
    bar -> struct var_3
    var_1 -> foo_t
    var_666 -> foo_t \*

  Function objects:
    func_[0-9]* -> void \*\(\*\) \(const char \*restrict, int \(\*\)\(\*\) \(const char \*\)\)
#...
  Types:
#...
   .*: struct var_3 .*
#...
CTF archive member: .*/data-func-1\.c:

  Header:
    Magic number: dff2
    Version: 4 \(CTF_VERSION_3\)
#...
    Parent name: \.ctf
    Compilation unit name: .*/data-func-1\.c
    Data object section:	.* \(0xfa4 bytes\)
    Type section:	.* \(0xc bytes\)
    String section:	.*

  Labels:

  Data objects:
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
    var_[0-9]* -> foo_t
#...
  Function objects:

  Variables:

  Types:
     80000001: foo_t .* -> .* int .*
#...
