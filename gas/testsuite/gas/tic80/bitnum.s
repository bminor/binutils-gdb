;; Test that all the predefined symbol names for the BITNUM field
;; are properly accepted and translated to numeric values.  Also
;; verifies that they are disassembled correctly as symbolics.

	bbo	r10,r8,eq.b
	bbo	r10,r8,ne.b
	bbo	r10,r8,gt.b
	bbo	r10,r8,le.b
	bbo	r10,r8,lt.b
	bbo	r10,r8,ge.b
	bbo	r10,r8,hi.b
	bbo	r10,r8,ls.b
	bbo	r10,r8,lo.b
	bbo	r10,r8,hs.b

	bbo	r10,r8,eq.h
	bbo	r10,r8,ne.h
	bbo	r10,r8,gt.h
	bbo	r10,r8,le.h
	bbo	r10,r8,lt.h
	bbo	r10,r8,ge.h
	bbo	r10,r8,hi.h
	bbo	r10,r8,ls.h
	bbo	r10,r8,lo.h
	bbo	r10,r8,hs.h

	bbo	r10,r8,eq.w
	bbo	r10,r8,ne.w
	bbo	r10,r8,gt.w
	bbo	r10,r8,le.w
	bbo	r10,r8,lt.w
	bbo	r10,r8,ge.w
	bbo	r10,r8,hi.w
	bbo	r10,r8,ls.w
	bbo	r10,r8,lo.w
	bbo	r10,r8,hs.w

