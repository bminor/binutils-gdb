:loop
/\\$/N
/\\$/b loop

s! @BFD_H@!!g
s!@INCDIR@!$(INCDIR)!g
s!@srcdir@/!!g
s!hosts/.*\.h ! !g
s/ sysdep.h//g
s/ libbfd.h//g
s! \$(INCDIR)/fopen-.*\.h!!g
s! \$(INCDIR)/ansidecl\.h!!g
s! \$(INCDIR)/obstack\.h!!g

s/\\\n *\\/\\/g
s/\\\n *$/ /g

/^ *\\$/d
s/ *$//
s/  */ /g
/:$/d
