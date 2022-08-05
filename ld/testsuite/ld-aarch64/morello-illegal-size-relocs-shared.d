# Checking that:
#    1) Size relocation is disallowed on a symbol the static linker does not
#       know about.
#    2) Relocations against symbols with too large a symbol are caught.
#source: morello-illegal-size-relocs.s
#as: -march=morello+c64
#ld: -shared
#error: .*: relocation R_MORELLO_MOVW_SIZE_G0 against `sym1' must be used against a non-interposable defined symbol
#error: .*: in function `_start':
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G0 against symbol `sym2' defined in \.data section in .*
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G0 against symbol `sym3' defined in \.data section in .*
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G1 against symbol `sym3' defined in \.data section in .*
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G0 against symbol `sym4' defined in \.data section in .*
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G1 against symbol `sym4' defined in \.data section in .*
#error: .*\(\.text.*\): relocation truncated to fit: R_MORELLO_MOVW_SIZE_G2 against symbol `sym4' defined in \.data section in .*
#error: .*: relocation R_MORELLO_MOVW_SIZE_G0 against `sym1' is disallowed with addend
#error: .*: relocation R_MORELLO_MOVW_SIZE_G0 against `\.data' is disallowed with addend
#error: .* relocation R_MORELLO_MOVW_SIZE_G0 against `othersym' must be used against a non-interposable defined symbol
#error: .* final link failed: bad value
