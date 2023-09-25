	ldl	r1,[r0,-4064]
	ld	r1,[r0,-4064]
	ldb	r1,[r0,-4064]
	lddl	r2,[r0,-4064]
	ldh	r1,[r0,-4064]

	ldl	r1,[-4064,r0]
	ld	r1,[-4064,r0]
	ldb	r1,[-4064,r0]
	lddl	r2,[-4064,r0]
	ldh	r1,[-4064,r0]

	ldl	r1,[0xfffff020,r0]
	ld	r1,[0xfffff020,r0]
	ldb	r1,[0xfffff020,r0]
	lddl	r2,[0xfffff020,r0]
	ldh	r1,[0xfffff020,r0]

	ldl	r1,[r0,0xfffff020]
	ld	r1,[r0,0xfffff020]
	ldb	r1,[r0,0xfffff020]
	lddl	r2,[r0,0xfffff020]
	ldh	r1,[r0,0xfffff020]


	ldl.aw	r1,[r0,-4064]
	ld.aw	r1,[r0,-4064]
	ldb.aw	r1,[r0,-4064]
	lddl.aw	r2,[r0,-4064]
	ldh.aw	r1,[r0,-4064]

	ldl.aw	r1,[-4064,r0]
	ld.aw	r1,[-4064,r0]
	ldb.aw	r1,[-4064,r0]
	;; 	lddl.aw	r2,[-4064,r0]
	ldh.aw	r1,[-4064,r0]

	ldl.aw	r1,[0xfffff020,r0]
	ld.aw	r1,[0xfffff020,r0]
	ldb.aw	r1,[0xfffff020,r0]
	;; 	lddl.aw	r2,[0xfffff020,r0]
	ldh.aw	r1,[0xfffff020,r0]

	ldl.aw	r1,[r0,0xfffff020]
	ld.aw	r1,[r0,0xfffff020]
	ldb.aw	r1,[r0,0xfffff020]
	lddl.aw	r2,[r0,0xfffff020]
	ldh.aw	r1,[r0,0xfffff020]


	ld.di	r1,[r0,-4064]
	ldb.di	r1,[r0,-4064]
	ldh.di	r1,[r0,-4064]

	ld.di	r1,[-4064,r0]
	ldb.di	r1,[-4064,r0]
	ldh.di	r1,[-4064,r0]

	ld.di	r1,[0xfffff020,r0]
	ldb.di	r1,[0xfffff020,r0]
	ldh.di	r1,[0xfffff020,r0]

	ld.di	r1,[r0,0xfffff020]
	ldb.di	r1,[r0,0xfffff020]
	ldh.di	r1,[r0,0xfffff020]


	ld.x	r1,[r0,-4064]
	ldb.x	r1,[r0,-4064]
	ldh.x	r1,[r0,-4064]

	ld.x	r1,[-4064,r0]
	ldb.x	r1,[-4064,r0]
	ldh.x	r1,[-4064,r0]

	ld.x	r1,[0xfffff020,r0]
	ldb.x	r1,[0xfffff020,r0]
	ldh.x	r1,[0xfffff020,r0]

	ld.x	r1,[r0,0xfffff020]
	ldb.x	r1,[r0,0xfffff020]
	ldh.x	r1,[r0,0xfffff020]
