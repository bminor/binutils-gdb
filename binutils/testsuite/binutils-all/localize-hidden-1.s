	.globl		Gdefault
	.globl		Ghidden
	.globl		Ginternal
	.globl		Gprotected

	.weak		Wdefault
	.weak		Whidden
	.weak		Winternal
	.weak		Wprotected

	.hidden		Lhidden
	.hidden		Ghidden
	.hidden		Whidden

	.internal	Linternal
	.internal	Ginternal
	.internal	Winternal

	.protected	Lprotected
	.protected	Gprotected
	.protected	Wprotected

	.set		Ldefault, 0x1100
	.set		Lhidden, 0x1200
	.set		Linternal, 0x1300
	.set		Lprotected, 0x1400

	.set		Gdefault, 0x2100
	.set		Ghidden, 0x2200
	.set		Ginternal, 0x2300
	.set		Gprotected, 0x2400

	.set		Wdefault, 0x3100
	.set		Whidden, 0x3200
	.set		Winternal, 0x3300
	.set		Wprotected, 0x3400
