	.text
.macro entry fname
\fname:
.endm

.macro func fname, t
 entry \fname
 .macro data
  .dc.\()\t 0
 .endm
 data
.endm

func foo, a
