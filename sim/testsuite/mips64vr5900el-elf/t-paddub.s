.include "t-macros.i"

	start

test_paddub:
        load $8 0x0007210642056304 0x8403a502c601e701
	load $9 0x01f902fa03fb04fc 0x05fd06fe07ff08ff
        paddub  $10,$9,$8
	check10 0x01ff23ff45ff67ff 0x89ffabffcdffefff
	
	exit0
