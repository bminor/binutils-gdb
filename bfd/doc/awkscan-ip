# Awk filter, 1st filter for BFD internal prototype file extraction
#
# keep /*proto-internal blocks
/^\/\*proto-internal\*/,/^\*\/|^\*-\*\//
#
# Apparent bug in sed can discard last line in some situations; therefore
# make last line harmless.
END { print "\n" }
