# We get:
#
# m68k-coff-as x.s
# x.s:5: FATAL:failed sanity check.
#
# from the following .s file:

	.sect foo
tag:
	bra tag

# Changing the section to .text makes it work, but we need to
# use multiple sections other than .text to link the way we want.
