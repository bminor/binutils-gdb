_start = 0x000000;

SECTIONS
{
  . = 0x1000 + SIZEOF_HEADERS;

  .header ALIGN (0x100) (READONLY) :
    {
      ASCII (16) "program name"
      ASCII (64) "empty"
      ASCII (4 * 8) "comment 1\n"
      ASCII (32) "comment 2\n"
      ASCII (32) "comment 3\n"
      ASCII (24) "comment 4\n"
      ASCII (64) "I meant to say: This is way too long"
    }

  .text ALIGN (0x100) :
  {
      entry = .;
      *(.text)
  }

  .data : AT (0x400000)
  {
	*(.data)
  }
  
  . = ALIGN(0x20);
  
  .bss :
  {
	*(.bss)
  }

  /DISCARD/ : { *(*) }
}

