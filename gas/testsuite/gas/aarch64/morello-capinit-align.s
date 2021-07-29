.data
f:
// A few capability initialisation directives with a single byte to show the
// padding gets introduced when needed.
.byte 0
.capinit f
.xword 0
.xword 0
.byte 0
.chericap f
// A few directives without any bytes in between to show no extra padding gets
// added when not needed.
.chericap f
.capinit f
.xword 0
.xword 0
