# sed script for BFD header files
# Merge adjacent blank lines.  Loop til no change.
:blin
/^$/,/^ *[^ ]*.*$/{
/^$/N
s/^ *\n *$//
}
t blin
