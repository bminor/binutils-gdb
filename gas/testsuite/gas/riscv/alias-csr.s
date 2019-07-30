alias_csr:
	frcsr t0
	fscsr t0, t2
	fscsr t2
	frrm t0
	fsrm t0, t1
	fsrm t1
	fsrmi t0, 31
	fsrmi 31
	frflags t0
	fsflags t0, t1
	fsflags t1
	fsflagsi t0, 31
	fsflagsi 31
