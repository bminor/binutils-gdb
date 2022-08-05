#as: -march=morello+c64
#ld: tmpdir/tls-shared.so
#error: .*: in function `_start':
#error: .*: relocation truncated to fit: R_MORELLO_TLSIE_ADR_GOTTPREL_PAGE20 against symbol `w1' defined in \.tbss section .*
