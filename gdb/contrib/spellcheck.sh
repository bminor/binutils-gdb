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

url=https://en.wikipedia.org/wiki/Wikipedia:Lists_of_common_misspellings/For_machines
cache_dir=$scriptdir/../../.git
cache_file=wikipedia-common-misspellings.txt
dictionary=$cache_dir/$cache_file

# Separators: space, slash, tab.
grep_separator=" |/|	"
sed_separator=" \|/\|\t"

usage ()
{
    echo "usage: $(basename "$0") <file|dir>+"
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

parse_dictionary ()
{
    # Parse dictionary.
    mapfile -t words \
	    < <(awk -F '->' '{print $1}' "$dictionary")
    mapfile -t replacements \
	    < <(awk -F '->' '{print $2}' "$dictionary")
}

find_files_matching_words ()
{
    local pat
    pat=""
    for word in "${words[@]}"; do
	if [ "$pat" = "" ]; then
	    pat="$word"
	else
	    pat="$pat|$word"
	fi
    done
    pat="($pat)"

    local sep
    sep=$grep_separator

    pat="(^|$sep)$pat($sep|$)"

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

    local sep
    sep=$grep_separator

    pat="(^|$sep)$pat($sep|$)"

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

    local sep
    sep=$sed_separator

    # Save separator.
    sep="\($sep\)"

    local repl1 repl2 repl3

    repl1="s%$sep$word$sep%\1$replacement\2%g"

    repl2="s%^$word$sep%$replacement\1%"

    repl3="s%$sep$word$%\1$replacement%"

    sed -i \
	"$repl1;$repl2;$repl3" \
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

    local i word replacement
    i=0
    for word in "${words[@]}"; do
	replacement=${replacements[$i]}
	i=$((i + 1))

	replace_word_in_files \
	    "$word" \
	    "$replacement" \
	    "${files_matching_words[@]}"
    done
}

main "$@"
