# SED script for preprocessing embedded headers from C source comments 
# (S. Chamberlain markup)
# beginning of many passes of cleanup work 
#
# Delete empty comment blocks
/^\/\*$/N
/^\/\*\n\*\/ *$/d
#
# Locate and coalesce adjacent comments
/\*\/$/N
s/\*\/\n\/\*/\
/
