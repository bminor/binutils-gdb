# sed script for preprocessing BFD header files
# <start comment> activity:
/^\/\*$/{
N
# Delete empty comment blocks
/^\/\*\n\*\/ *$/d
# Transpose <start comment><blank line>
s/^\/\*\n *$/\
\/*/
# merge <start comment> on line by itself with following line
s/^\/\*\n\(.*\)/\/* \1/
}
