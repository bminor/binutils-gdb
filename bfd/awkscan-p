# Awk filter, 1st filter for BFD prototype file extraction
#
# keep /*proto blocks
/^\/\*proto\*/,/^\*\/|^\*-\*\//
#
# Apparent bug in sed can discard last line in some situations; therefore
# make last line harmless.
END { print "\n" }
