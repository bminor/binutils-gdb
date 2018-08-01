#skip: hppa*-*-*
#failif
#...
 +GNU_STACK .* RWE .*
#pass

# The skip for HPPA is because this target always needs an executable stack.
