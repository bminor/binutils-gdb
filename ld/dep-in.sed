:loop
/\\$/N
/\\$/b loop

s!@INCDIR@!$(INCDIR)!g
s!@srcdir@/!!g
s!\.\./bfd/hosts/[^ ]*\.h ! !g

s/\\\n */ /g

s/ *$//
s/  */ /g
/:$/d

s/\(.\{50\}[^ ]*\) /\1 \\\
  /g
