	.code
	.export		f
f
	.proc
	.callinfo	frame=0,no_calls
	.entry

	fstws		%fr6R,0(%r26)
	fstws		%fr6L,4(%r26)
	fstws		%fr6,8(%r26)

	fstds		%fr6R,0(%r26)
	fstds		%fr6L,4(%r26)
	fstds		%fr6,8(%r26)

	fldws		0(%r26),%fr6R
	fldws		4(%r26),%fr6L
	fldws		8(%r26),%fr6

	fldds		0(%r26),%fr6R
	fldds		4(%r26),%fr6L
	fldds		8(%r26),%fr6

	fstws		%fr6R,0(%sr0,%r26)
	fstws		%fr6L,4(%sr0,%r26)
	fstws		%fr6,8(%sr0,%r26)

	fstds		%fr6R,0(%sr0,%r26)
	fstds		%fr6L,4(%sr0,%r26)
	fstds		%fr6,8(%sr0,%r26)

	fldws		0(%sr0,%r26),%fr6R
	fldws		4(%sr0,%r26),%fr6L
	fldws		8(%sr0,%r26),%fr6

	fldds		0(%sr0,%r26),%fr6R
	fldds		4(%sr0,%r26),%fr6L
	fldds		8(%sr0,%r26),%fr6

	bv		0(%r2)
	nop

	.exit
	.procend
