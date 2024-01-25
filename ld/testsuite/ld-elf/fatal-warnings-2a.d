#PR ld/31289
#source: start.s
#ld: -z bad-option1 -z bad-option2
#warning: -z bad-option1 ignored
#xfail: ![is_standard_elf]
# These targets don't support -z options.
