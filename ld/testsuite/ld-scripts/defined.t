SECTIONS {
	.text : { *(.text) }
}
value1 = DEFINED (defined) ? 1 : 2;
value2 = DEFINED (undefined) ? 1 : 2;
