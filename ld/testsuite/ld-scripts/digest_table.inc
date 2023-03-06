      . = ALIGN(2048) - 32;
      INCLUDE "begin_tag.inc"
      ASCII (16) "DIGEST TABLE"
      DIGEST TABLE "CRCTAB"
      ASCII (16) "END TABLE"
      INCLUDE "end_tag.inc"
