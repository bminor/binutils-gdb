# The assembler is failing with error 'FATAL:failed sanity check', giving
# the line of the 'dbf' instruction as the offending line.
#
# This will assemble ok if the section is '.text'.  However we must
# be able to use a different section name.
#
# Our only workaround is to recode all of our loops to not use dbf.


    .sect other

    loop1:
	    move.l	%d1,%a0@+
	    dbf		%d0,loop1
