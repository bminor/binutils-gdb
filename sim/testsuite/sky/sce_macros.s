
; float x 4
;
.macro	fxyzw	f1, f2, f3, f4
.float	\f1, \f2, \f3, \f4
.endm

.macro fwzyx	f1, f2, f3, f4
.float	\f4, \f3, \f2, \f1
.endm

; int x 4
;
.macro	ixyzw	i1, i2, i3, i4
.int	\i1, \i2, \i3, \i4
.endm

.macro iwzyx	i1, i2, i3, i4
.int	\i4, \i3, \i2, \i1
.endm

; short x 4
;
.macro	sxyzw	s1, s2, s3, s4
.short	\s1, \s2, \s3, \s4
.endm

.macro swzyx	s1, s2, s3, s4
.short	\s4, \s3, \s2, \s1
.endm

; byte x 4
;
.macro	bxyzw	b1, b2, b3, b4
.byte	\b1, \b2, \b3, \b4
.endm

.macro bwzyx	b1, b2, b3, b4
.byte	\b4, \b3, \b2, \b1
.endm


; float x 3
;
.macro	fxyz	f1, f2, f3
.float	\f1, \f2, \f3
.endm

.macro fzyx	f1, f2, f3
.float	\f3, \f2, \f1
.endm

; int x 3
;
.macro	ixyz	i1, i2, i3
.int	\i1, \i2, \i3
.endm

.macro izyx	i1, i2, i3
.int	\i3, \i2, \i1
.endm

; short x 3
;
.macro	sxyz	s1, s2, s3
.short	\s1, \s2, \s3
.endm

.macro szyx	s1, s2, s3
.short	\s3, \s2, \s1
.endm

; byte x 3
;
.macro	bxyz	b1, b2, b3
.byte	\b1, \b2, \b3
.endm

.macro bzyx	b1, b2, b3
.byte	\b3, \b2, \b1
.endm


; float x 2
;
.macro	fxy	f1, f2
.float	\f1, \f2
.endm

.macro fyx	f1, f2
.float	\f2, \f1
.endm

; int x 2
;
.macro	ixy	i1, i2
.int	\i1, \i2
.endm

.macro iyx	i1, i2
.int	\i2, \i1
.endm

; short x 2
;
.macro	sxy	s1, s2
.short	\s1, \s2
.endm

.macro syx	s1, s2
.short	\s2, \s1
.endm

; byte x 2
;
.macro	bxy	b1, b2
.byte	\b1, \b2
.endm

.macro byx	b1, b2
.byte	\b2, \b1
.endm


; float x 1
;
.macro	fx	f1
.float	\f1
.endm

; int x 1
;
.macro	ix	i1
.int	\i1
.endm

; short x 1
;
.macro	sx	s1
.short	\s1
.endm

; byte x 1
;
.macro	bx	b1
.byte	\b1
.endm

.macro VIFNOP
PKENOP
.endm

;.macro MSCNT
;PKEMSCNT
;.endm

.macro MSCALF f1
PKEMSCALF
.endm
