BEGIN { n = 0
    printf "static const char %s[] = {\n", arrayname
    for (i = 0; i < 255; i++)
	_ord_[sprintf("%c", i)] = i
}

{
    split($0, line, "");
    printf "  "
    for (i = 1; i <= length($0); i++) {
	c = line[i]
	if (c == "'") {
	    printf "'\\''"
	} else if (c == "\\") {
	    printf "'\\\\'"
	} else if (_ord_[c] >= 32 && _ord_[c] < 127) {
	    printf "'%s'", c
	} else {
	    printf "'\\%03o'", _ord_[c]
	}
	printf ", "
	if (i % 10 == 0)
	    printf "\n   "
    }
    printf "'\\n', \n"
}

END {
    print "  0 };"
}
