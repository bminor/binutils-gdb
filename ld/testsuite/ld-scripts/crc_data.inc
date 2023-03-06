      ecc_start = .;
      ASCIZ "123456789"
      ecc_end = . - 1;
      . = ALIGN(0x10);

      entry = .;
      *(.text)
      . = ALIGN(0x100);
      BYTE(1);
