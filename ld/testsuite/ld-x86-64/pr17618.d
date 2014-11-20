#name: PLT PC-relative offset overflow check
#as: --64
#ld: -shared -melf_x86_64
#error: .*PC-relative offset overflow in PLT entry for `bar'
