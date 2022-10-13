#!/usr/bin/env bash
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
set -xe
SAIL_DIR=${SAIL_MORELLO_DIR:-${HOME}/.opam/default/bin}
if [ ! -e "${SAIL_DIR}/sail" ]; then
  echo "Must set SAIL_DIR"
  exit 1
#else
#  SAIL_DIR=$(dirname $(command -v sail))
fi
SAIL_MORELLO_DIR=${SAIL_MORELLO_DIR:-${HOME}/cheri/arm-morello-dropzone/sail}
if [ ! -e "$SAIL_MORELLO_DIR" ]; then
  echo "Must set SAIL_MORELLO_DIR"
  exit 1
fi

sail128_srcs="prelude.sail builtins.sail decode_start.sail v8_base.sail"
extract_fns="CapGetBounds CapGetValue CapGetPermissions CapGetObjectType CapGetTag"
output_dir=${SCRIPT_DIR}

cd "$SAIL_MORELLO_DIR"
printf ":rewrites c\n:slice_roots $extract_fns\n:slice\n:compile c\n:quit\n" | "${SAIL_DIR}/sail" -i -c_no_main -static -c_prefix sailgen_ -c_specialize -no_effects -memo_z3  -non_lexical_flow -no_warn -o "$output_dir/sail_compression_128m" $sail128_srcs
cd "$output_dir"
