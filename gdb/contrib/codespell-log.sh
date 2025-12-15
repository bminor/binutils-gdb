#!/bin/sh

# Copyright (C) 2025 Free Software Foundation, Inc.
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

# Script to be used as post-commit hook to spell-check the commit log using
# codespell.
#
# Given that the post-commit hook gets no arguments from git, we need to
# provide the commit message ourselves.

# Exit on error.
set -e

tmp=

cleanup()
{
    if [ "$tmp" != "" ]; then
	rm -f "$tmp"
    fi
}

# Schedule cleanup.
trap cleanup EXIT

tmp=$(mktemp)
git show \
    -s \
    --pretty=%B HEAD \
    > "$tmp"

codespell \
    "$@" \
    "$tmp"
