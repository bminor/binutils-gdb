/* 
Script by mohring@informatik.tu-muenchen.de
for ISC SYSV 3.2/386 
*/
TARGET("coff-i386")
OUTPUT_FORMAT("coff-i386")
OUTPUT_ARCH(i386)
SEARCH_DIR(/lib)
SEARCH_DIR(/usr/lib)
SEARCH_DIR(/usr/local/lib)
SEARCH_DIR(/usr2/lib)
ENTRY(_start)

SECTIONS {
	.text SIZEOF_HEADERS :
		{
		CREATE_OBJECT_SYMBOLS
		*(.init);
		*(.text);
		*(.fini);
		etext = . ;
		}

	.data NEXT(0x400000) + (SIZEOF(.text) + ADDR(.text)) % 0x2000:
		{
		*(.data);
		edata = . ;
		}

  	.bss SIZEOF(.data) + ADDR(.data) :
		{ 
		*(.bss);
		[COMMON]
		end = . ;
		}
}
