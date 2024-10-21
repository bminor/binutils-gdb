#!/bin/bash

# Copyright (C) 2024 Free Software Foundation, Inc.
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Script to auto-correct common spelling mistakes.
#
# Example usage:
# $ ./gdb/contrib/spellcheck.sh gdb*

scriptdir=$(cd "$(dirname "$0")" || exit; pwd -P)
this_script=$scriptdir/$(basename "$0")

url=https://en.wikipedia.org/wiki/Wikipedia:Lists_of_common_misspellings/For_machines
cache_dir=$scriptdir/../../.git
cache_file=wikipedia-common-misspellings.txt
dictionary=$cache_dir/$cache_file
local_dictionary=$scriptdir/common-misspellings.txt
cache_file2=spell-check.pat1

# Separators: space, slash, tab, colon, comma.
declare -a grep_separators
grep_separators=(
    " "
    "/"
    "	"
    ":"
    ","
)
declare -a sed_separators
sed_separators=(
    " "
    "/"
    "\t"
    ":"
    ","
)

join ()
{
    local or
    or="$1"
    shift

    local res
    res=""

    local first
    first=true

    for item in "$@"; do
	if $first; then
	    first=false
	    res="$item"
	else
	    res="$res$or$item"
	fi
    done

    echo "$res"
}

grep_or="|"
sed_or="\|"

grep_join ()
{
    local res
    res=$(join $grep_or "$@")
    echo "($res)"
}

sed_join ()
{
    local res
    res=$(join $sed_or "$@")
    echo "\($res\)"
}

usage ()
{
    echo "usage: $(basename "$0") [--check] <file|dir>+"
}

make_absolute ()
{
    local arg
    arg="$1"

    case "$arg" in
	/*)
	;;
	*)
	    arg=$(pwd -P)/"$arg"
	    ;;
    esac

    echo "$arg"
}

parse_args ()
{
    local files
    files=$(mktemp)
    trap 'rm -f "$files"' EXIT

    while true; do
	case " $1 " in
	    " --check ")
		check=true
		shift
		;;
	    *)
		break
		;;
	esac
    done

    if [ $# -eq -0 ]; then
	usage
	exit 1
    fi

    local arg
    for arg in "$@"; do
	if [ -f "$arg" ]; then
	    arg=$(make_absolute "$arg")
	    readlink -e "$arg" \
		     >> "$files"
	elif [ -d "$arg" ]; then
	    arg=$(make_absolute "$arg")
	    local f
	    find "$arg" -type f -exec readlink -e {} \; \
		 >> "$files"
	else
	    echo "Not a file or directory: $arg"
	    exit 1
	fi
    done

    mapfile -t unique_files \
	    < <(sort -u "$files" \
		    | grep -v ChangeLog)

    rm -f "$files"
    trap "" EXIT
}

get_dictionary ()
{
    if [ -f "$dictionary" ]; then
	return
    fi

    local webpage
    webpage=$(mktemp)
    trap 'rm -f "$webpage"' EXIT

    # Download web page containing table.
    wget $url -O "$webpage"

    # Extract table from web page.
    awk '/<pre>/,/<\/pre>/' "$webpage" \
	| sed 's/<pre>//;s/<\/pre>//' \
	| grep -E -v "^$" \
	       > "$dictionary"

    rm -f "$webpage"
    trap "" EXIT
}

output_local_dictionary ()
{
    # Filter out comments and empty lines.
    grep -E -v \
	 "^#|^$" \
	 "$local_dictionary"
}

output_dictionaries ()
{
    output_local_dictionary
    cat "$dictionary"
}

parse_dictionary ()
{
    # Parse dictionary.
    mapfile -t words \
	    < <(awk -F '->' '{print $1}' <(output_dictionaries))
    mapfile -t replacements \
	    < <(awk -F '->' '{print $2}' <(output_dictionaries))
}

find_files_matching_words ()
{
    local cache_id
    cache_id=$(cat "$local_dictionary" "$dictionary" "$this_script" \
		 | md5sum  \
		 | awk '{print $1}')

    local patfile
    patfile="$cache_dir/$cache_file2".$cache_id

    local pat
    if [ -f "$patfile" ]; then
	pat=$(cat "$patfile")
    else
	rm -f "$cache_dir/$cache_file2".*

	pat=$(grep_join "${words[@]}")

	local before after
	before=$(grep_join \
		     "^" \
		     "${grep_separators[@]}")
	after=$(grep_join \
		    "${grep_separators[@]}" \
		    "\." \
		    "$")

	pat="$before$pat$after"

	echo "$pat" \
	     > "$patfile"
    fi

    grep -E \
	-l \
	"$pat" \
	"$@"
}

find_files_matching_word ()
{
    local pat
    pat="$1"
    shift

    local before after
    before=$(grep_join \
		 "^" \
		 "${grep_separators[@]}")
    after=$(grep_join \
		"${grep_separators[@]}" \
		"\." \
		"$")

    pat="$before$pat$after"

    grep -E \
	-l \
	"$pat" \
	"$@"
}

replace_word_in_file ()
{
    local word
    word="$1"

    local replacement
    replacement="$2"

    local file
    file="$3"

    local before after
    before=$(sed_join \
		 "^" \
		 "${sed_separators[@]}")
    after=$(sed_join \
		"${sed_separators[@]}" \
		"\." \
		"$")

    local repl
    repl="s%$before$word$after%\1$replacement\2%g"

    sed -i \
	"$repl" \
	"$file"
}

replace_word_in_files ()
{
    local word
    word="$1"

    local replacement
    replacement="$2"

    shift 2

    local id
    id="$word -> $replacement"

    # Reduce set of files for sed to operate on.
    local files_matching_word
    declare -a files_matching_word
    mapfile -t files_matching_word \
	    < <(find_files_matching_word "$word" "$@")

    if [ ${#files_matching_word[@]} -eq 0 ]; then
	return
    fi

    if echo "$replacement"| grep -q ","; then
	echo "TODO: $id"
	return
    fi

    declare -A md5sums

    local changed f before after
    changed=false
    for f in "${files_matching_word[@]}"; do
	if [ "${md5sums[$f]}" = "" ]; then
	    md5sums[$f]=$(md5sum "$f")
	fi

	before="${md5sums[$f]}"

	replace_word_in_file \
	    "$word" \
	    "$replacement" \
	    "$f"

	after=$(md5sum "$f")

	if [ "$after" != "$before" ]; then
	    md5sums[$f]="$after"
	    changed=true
	fi
    done

    if $changed; then
	echo "$id"
    fi

    find_files_matching_word "$word" "${files_matching_word[@]}" \
	| awk "{ printf \"TODO: $id: replacement failed: %s\n\", \$0}"
}

main ()
{
    declare -a unique_files
    check=false
    parse_args "$@"

    get_dictionary

    declare -a words
    declare -a replacements
    parse_dictionary

    # Reduce set of files for sed to operate on.
    local files_matching_words
    declare -a files_matching_words
    mapfile -t files_matching_words \
	    < <(find_files_matching_words "${unique_files[@]}")

    if [ ${#files_matching_words[@]} -eq 0 ]; then
	return
    fi

    if $check; then
	exit 1
    fi

    declare -A words_done
    local i word replacement
    i=0
    for word in "${words[@]}"; do
	replacement=${replacements[$i]}
	i=$((i + 1))

	# Skip words that are already handled.  This ensures that the local
	# dictionary overrides the wiki dictionary.
	if [ "${words_done[$word]}" == 1 ]; then
	    continue
	fi
	words_done[$word]=1

	replace_word_in_files \
	    "$word" \
	    "$replacement" \
	    "${files_matching_words[@]}"
    done
}

main "$@"
