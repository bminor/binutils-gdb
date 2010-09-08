MEMORY
{
  region1        : ORIGIN = 0x1000, LENGTH = 0x1000 ,
  region2 (r)    :    org = 0x2000, len    =    300
  region3 (wx)   :      o = 0x4000, l      =      4
  region4 (!r)   : o = 0x6000 + 60, len    = 0x30 * 0x6
}

SECTIONS
{
  .sec0 : { *(*.sec0) }
  
  .sec1 ORIGIN (region1) : AT(LENGTH (region2)) { *(*.sec1) }

  fred = ORIGIN (region1) + LENGTH (region1) ;
  
  .sec2 : { *(*.sec2) } > region3 AT> region4

  .sec3 0x5000 : { *(*.sec3) }

  /* In theory we could put:

     /DISCARD/ : { *(*) }

     here as we do not need any other sections for this test.
     In practice however doing so breaks GOLD as it relies upon
     being able to create/find various other sections such as
     .dynamic, .dynsym and .gnu.hash.  */
}
