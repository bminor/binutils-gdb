#name: PR 22319 - required undefined symbols in output
#ld: -u undefined_symbol -e 0
#nm: -u
#xfail: dlx-*-* 
# The DLX target fails because it needs a special linker script to handle file symbols.

[ 	]+U+[ 	]+undefined_symbol
#pass

