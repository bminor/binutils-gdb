SECTIONS {
  .rom  (NOLOAD)   : { LONG(1234); }
  .ro   (READONLY) : { LONG(5678); }
  .over (OVERLAY)  : { LONG(0123); }
  /DISCARD/        : { *(*) }

}
