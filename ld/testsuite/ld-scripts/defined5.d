#ld: -Tdefined5.t
#warning: .*multiple definition of `defined'.*
#nm: -B
#xfail: [is_xcoff_format]
# xcoff outputs value of "defined" from the object file

# Check that a script can override an object file symbol, if multiple
# definitions are allowed.  See pr12356.
#...
0+1000 D defined
#pass
