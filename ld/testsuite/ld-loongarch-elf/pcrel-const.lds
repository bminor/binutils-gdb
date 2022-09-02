ENTRY(foo);
SECTIONS
{
	.text : {
		*(.text*)
	}

	.data : {
		__sec_start = .;
		*(.gzdata)
		__sec_end = .;
	}
}
PROVIDE(__sec_size = __sec_end);
