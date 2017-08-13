SECTIONS {
        . = 0x10000;
        _start = . ;
        .data : {
                *(.data)
        }
}
