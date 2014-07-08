#source: startv1.s
#source: funref2.s
#as: -a64
#ld: -melf64ppc --emit-stub-syms
#ld_after_inputfiles: tmpdir/funv1.so
#readelf: -rs --wide
# Check that we do the right thing with funref2.s that doesn't have
# anything to mark it as ELFv1 or ELFv2.  Since my_func address is
# taken in a read-only section we should get a copy reloc for the OPD
# entry.

Relocation section .* contains 1 entries:
.*
.* R_PPC64_COPY .* my_func \+ 0

Symbol table '\.dynsym' contains 5 entries:
.*
     0: .*
     1: 0+10010390     4 FUNC    GLOBAL DEFAULT   12 my_func
     2: 0+10010390     0 NOTYPE  GLOBAL DEFAULT   12 __bss_start
     3: 0+10010390     0 NOTYPE  GLOBAL DEFAULT   11 _edata
     4: 0+10010398     0 NOTYPE  GLOBAL DEFAULT   12 _end

Symbol table '\.symtab' contains 20 entries:
.*
     0: .*
     1: 0+10000158     0 SECTION LOCAL  DEFAULT    1 
     2: 0+10000170     0 SECTION LOCAL  DEFAULT    2 
     3: 0+10000198     0 SECTION LOCAL  DEFAULT    3 
     4: 0+10000210     0 SECTION LOCAL  DEFAULT    4 
     5: 0+10000248     0 SECTION LOCAL  DEFAULT    5 
     6: 0+10000260     0 SECTION LOCAL  DEFAULT    6 
     7: 0+10000264     0 SECTION LOCAL  DEFAULT    7 
     8: 0+1000026c     0 SECTION LOCAL  DEFAULT    8 
     9: 0+10010270     0 SECTION LOCAL  DEFAULT    9 
    10: 0+10010370     0 SECTION LOCAL  DEFAULT   10 
    11: 0+10010388     0 SECTION LOCAL  DEFAULT   11 
    12: 0+10010390     0 SECTION LOCAL  DEFAULT   12 
    13: 0+10010270     0 OBJECT  LOCAL  DEFAULT    9 _DYNAMIC
    14: 0+10000264     0 NOTYPE  GLOBAL DEFAULT    7 func_tab
    15: 0+10010390     4 FUNC    GLOBAL DEFAULT   12 my_func
    16: 0+10010370     0 FUNC    GLOBAL DEFAULT   10 _start
    17: 0+10010390     0 NOTYPE  GLOBAL DEFAULT   12 __bss_start
    18: 0+10010390     0 NOTYPE  GLOBAL DEFAULT   11 _edata
    19: 0+10010398     0 NOTYPE  GLOBAL DEFAULT   12 _end
