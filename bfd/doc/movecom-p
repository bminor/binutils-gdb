# sed script for BFD header files:
# Transpose <blank line> <end comment>
/^$/,/^ *[^ ]*.*$/{
/^$/N
/^ *\n\*\/$/c\
*\/\

}
