; Simple testsuite, to get something going, plus placeholder
; for various random tests.

	.vu
foo:
	abs.xyz vf10xyz,vf20xyz               waitp

; Test / in dest spec.

	add/xyzw vf10xyzw,vf20xyzw,vf30xyzw   waitq
