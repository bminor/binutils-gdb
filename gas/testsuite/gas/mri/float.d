#objcopy: -O srec
#name: MRI floating point constants
#as: -M

# Test MRI floating point constants

S0.*
S118....123456789ABCDEF03F800000412000004120000042.*
S10.....C80000.*
#pass
