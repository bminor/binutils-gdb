PHDRS
{
  header PT_PHDR FILEHDR PHDRS ;
	 
  image PT_LOAD FLAGS (4);
  tls PT_TLS FLAGS (4);
  
}
SECTIONS
{
  .text : { *(.text) } :image
  .tdata : { *(.tdata) } :image :tls
  .tbss : { *(.tbss) } :image : tls
  .map : {
    LONG (SIZEOF (.text))
    LONG (SIZEOF (.data))
    LONG (SIZEOF (.bss))
    LONG (SIZEOF (.tdata))
    LONG (SIZEOF (.tbss))
  } :image
}
