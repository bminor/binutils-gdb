	.macro csr val
	csrr a0,\val
	.endm

	# Vector
	csr vstart
	csr vxsat
	csr vxrm
	csr vcsr
	csr vl
	csr vtype
	csr vlenb
