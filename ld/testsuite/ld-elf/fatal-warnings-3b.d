#PR ld/31289
#source: start.s
#ld: --fatal-warnings -z bad-option1 -z bad-option2
#error: unsupported option: -z bad-option2
#xfail: ![is_standard_elf]
# These targets don't support -z options.
