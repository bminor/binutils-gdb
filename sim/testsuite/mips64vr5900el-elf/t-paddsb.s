.include "t-macros.i"

	start

test_paddsb:
	load $8 0x0087218642856384 0x8403a502c601e701
	load $9 0x01f902fa03fb04fc 0x057d067e077f087f
        paddsb  $10,$9,$8
	check10 0x0180238045806780 0x897fab7fcd7fef7f

	exit0
