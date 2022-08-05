# Check that TLS relocations with an addend are disallowed.
#source: morello-illegal-tls.s
#as: -march=morello+c64
#ld: -shared
#error: .*\(\.text.*\): relocation R_MORELLO_TLSIE_ADR_GOTTPREL_PAGE20 against `a' is disallowed with addend
#error: .*\(\.text.*\): relocation R_MORELLO_TLSIE_ADD_LO12 against `a' is disallowed with addend
#error: .*\(\.text.*\): relocation R_MORELLO_TLSDESC_ADR_PAGE20 against `a' is disallowed with addend
#error: .*\(\.text.*\): relocation R_MORELLO_TLSDESC_LD128_LO12 against `a' is disallowed with addend
#error: .*\(\.text.*\): relocation R_AARCH64_TLSDESC_ADD_LO12 against `a' is disallowed with addend
#error: .*\(\.text.*\): relocation R_MORELLO_TLSDESC_CALL against `a' is disallowed with addend
