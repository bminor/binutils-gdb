# NOTE: BEGIN pattern gives errors if other than 1st line; 
# END ditto if other than last.
BEGIN { print "@c ------------------------------START TEXT FROM " FILENAME }
#
# Keep /*doc* blocks (terminated by either */ or *-*/)
/^\/\*doc\*/,/^\*\/|^\*-\*\//
#
# Also keep two kinds of /*proto blocks
/^\/\*proto\*/,/^\*\/|^\*-\*\//
/^\/\*proto-internal\*/,/^\*\/|^\*-\*\//
#
END { print "@c ------------------------------END TEXT FROM " FILENAME }
