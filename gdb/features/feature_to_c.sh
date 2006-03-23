#!/bin/sh

output=$1
shift

if test -z "$output" || test -z "$1"; then
  echo "Usage: $0 OUTPUTFILE INPUTFILE..."
  exit 1
fi

for input in dummy "$@"; do
  if test $input != dummy; then
    arrayname=xml_feature_`echo $input | sed 's,.*/,,; s/[-.]/_/g'`

    gawk 'BEGIN { n = 0
      print "static const char '$arrayname'[] = {"
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
    }' < $input >> $output
  fi
done

echo >> $output
echo "const char *const xml_builtin[][2] = {" >> $output

for input in dummy "$@"; do
  if test $input != dummy; then
    basename=`echo $input | sed 's,.*/,,'`
    arrayname=xml_feature_`echo $input | sed 's,.*/,,; s/[-.]/_/g'`
    echo "  { \"$basename\", $arrayname }," >> $output
  fi
done

echo "  { 0, 0 }" >> $output
echo "};" >> $output
