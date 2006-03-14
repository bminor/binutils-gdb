#!/bin/sh

input=$1
output=$2

if test -z "$input" || test -z "$output"; then
  echo "Usage: $0 INPUTFILE OUTPUTFILE"
  exit 1
fi

arrayname=xml_feature_`basename $input | sed 's/\..*//g; s/-/_/g'`

gawk 'BEGIN { n = 0
  print "const char '$arrayname'[] = {"
  for (i = 0; i < 255; i++)
    _ord_[sprintf("%c", i)] = i
} {
  split($0, line, "");
  printf "  "
  for (i = 1; i <= length(line); i++) {
    c = line[i]
    if (c == "'\''") {
      printf "'\''\\'\'''\'', "
    } else if (c == "\\") {
      printf "'\''\\\\'\'', "
    } else if (match (c, "[[:print:]]") != 0) {
      printf "'\''" c "'\'', "
    } else {
      printf "'\''\\%03o'\'', ", _ord_[c]
    }
    if (i % 10 == 0)
      printf "\n   "
  }
  printf "'\''\\n'\'', \n"
} END {
  print "  0 };"
}' < $input > $output
