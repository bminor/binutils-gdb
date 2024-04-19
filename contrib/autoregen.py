#!/usr/bin/env python3

# This script helps to regenerate files managed by autotools and
# autogen in binutils-gdb and gcc repositories.

# It can be used by buildbots to check that the current repository
# contents has been updated correctly, and by developers to update
# such files as expected.

import os
import shutil
import subprocess
from pathlib import Path


# On Gentoo, vanilla unpatched autotools are packaged separately.
# We place the vanilla names first as we want to prefer those if both exist.
AUTOCONF_NAMES = ["autoconf-vanilla-2.69", "autoconf-2.69", "autoconf"]
AUTOMAKE_NAMES = ["automake-vanilla-1.15", "automake-1.15.1", "automake"]
ACLOCAL_NAMES = ["aclocal-vanilla-1.15", "aclocal-1.15.1", "aclocal"]
AUTOHEADER_NAMES = ["autoheader-vanilla-2.69", "autoheader-2.69", "autoheader"]
AUTORECONF_NAMES = ["autoreconf-vanilla-2.69", "autoreconf-2.69", "autoreconf"]

# Pick the first for each list that exists on this system.
AUTOCONF_BIN = next(name for name in AUTOCONF_NAMES if shutil.which(name))
AUTOMAKE_BIN = next(name for name in AUTOMAKE_NAMES if shutil.which(name))
ACLOCAL_BIN = next(name for name in ACLOCAL_NAMES if shutil.which(name))
AUTOHEADER_BIN = next(name for name in AUTOHEADER_NAMES if shutil.which(name))
AUTORECONF_BIN = next(name for name in AUTORECONF_NAMES if shutil.which(name))

AUTOGEN_BIN = "autogen"

# autoconf-wrapper and automake-wrapper from Gentoo look at this environment variable.
# It's harmless to set it on other systems though.
EXTRA_ENV = {
    "WANT_AUTOCONF": AUTOCONF_BIN.split("-", 1)[1] if "-" in AUTOCONF_BIN else "",
    "WANT_AUTOMAKE": AUTOMAKE_BIN.split("-", 1)[1] if "-" in AUTOMAKE_BIN else "",
    "AUTOCONF": AUTOCONF_BIN,
    "ACLOCAL": ACLOCAL_BIN,
    "AUTOMAKE": AUTOMAKE_BIN,
    "AUTOGEN": AUTOGEN_BIN,
}
ENV = os.environ.copy()
ENV.update(EXTRA_ENV)


# Directories we should skip entirely because they're vendored or have different
# autotools versions.
SKIP_DIRS = [
    # readline and minizip are maintained with different autotools versions
    "readline",
    "minizip",
]

# these directories are known to be re-generatable with a simple autoreconf
# without special -I flags
# Entries commented out (and directories not listed) are handled by
# regenerate_manually().
AUTORECONF_DIRS = [
    # subdirs common to binutils-gdb and gcc
    "libbacktrace",
    "libdecnumber", # No Makefile.am
    "libiberty", # No Makefile.am
    "zlib",

    # binutils-gdb subdirs
    "bfd",
    "binutils",
    "etc",
    "gas",
    "gdb",
    "gdbserver",
    "gdbsupport",
    "gnulib",
    "gold",
    "gprof",
    "gprofng",
    "gprofng/libcollector",
    "ld",
    "libctf",
    "libsframe",
    "opcodes",
    "sim",

    # gcc subdirs
    "c++tools", # No aclocal.m4
    "gcc", # No Makefile.am
    #"fixincludes", # autoreconf complains about GCC_AC_FUNC_MMAP_BLACKLIST
    "gnattools", # No aclocal.m4
    "gotools",
    "libada", # No aclocal.m4
    "libatomic",
    "libcc1",
    "libcody", # No aclocal.m4
    "libcpp", # No Makefile.am
    "libffi",
    "libgcc", # No aclocal.m4
    "libgfortran",
    # Hack: ACLOCAL_AMFLAGS = -I .. -I ../config in Makefile.in but we
    # apply -I../config -I.. otherwise we do not match the current
    # contents
    #"libgm2",
    "libgo",
    "libgomp",
    "libgrust",
    "libitm",
    "libobjc", # No Makefile.am
    "libphobos",
    "libquadmath",
    "libsanitizer",
    "libssp",
    "libstdc++-v3",
    # This does not cover libvtv/testsuite/other-tests/Makefile.in
    "libvtv",
    "lto-plugin",
]


# Run the shell command CMD.
#
# Print the command on stdout prior to running it.
def run_shell(cmd: str):
    print(f"+ {cmd}", flush=True)
    res = subprocess.run(
        f"{cmd}",
        shell=True,
        encoding="utf8",
        env=ENV,
    )
    res.check_returncode()


def regenerate_with_autoreconf():
    run_shell(f"{AUTORECONF_BIN} -f")

def regenerate_with_autogen():
    run_shell(f"{AUTOGEN_BIN} Makefile.def")

def regenerate_manually():
    configure_lines = open("configure.ac").read().splitlines()
    if folder.stem == "fixincludes" or folder.stem == "libgm2" or any(
            True for line in configure_lines if line.startswith("AC_CONFIG_MACRO_DIR")
    ):
        include_arg = ""
        include_arg2 = ""
        if (folder / ".." / "config").is_dir():
            include_arg = "-I../config"

        # this is really a hack just for binutils-gdb/gprofng/libcollector
        # make sure that the order of includes is done as --enable-maintainer-mode
        if (folder / ".." / ".." / "config").is_dir():
            include_arg = "-I../.."
            include_arg2 = "-I../../config"

        if folder.stem == "fixincludes":
            include_arg = "-I.."
            include_arg2 = "-I../config"

        if folder.stem == "libgm2":
            include_arg = "-I../config"
            include_arg2 = "-I.."

        # aclocal does not support the -f short option for force
        run_shell(f"{ACLOCAL_BIN} --force {include_arg} {include_arg2}")

    if (folder / "config.in").is_file() or any(
        True for line in configure_lines if line.startswith("AC_CONFIG_HEADERS")
    ):
        run_shell(f"{AUTOHEADER_BIN} -f")

    # The few lines below do not regenerate the exact same content as
    # currently in the repo. Disable them for now.
    # if (folder / "gm2-libs").is_dir():
    #     run_shell(f"{AUTOCONF_BIN} -f gm2-libs/config-host.in > gm2-libs/config-host")
    #     run_shell(f"{AUTOHEADER_BIN} gm2-libs/config-host.in")


    # apparently automake is somehow unstable -> skip it for gotools
    if any(
        True for line in configure_lines if line.startswith("AM_INIT_AUTOMAKE")
    ) and not str(folder).endswith("gotools"):
        run_shell(f"{AUTOMAKE_BIN} -f")

    run_shell(f"{AUTOCONF_BIN} -f")


run_shell(f"{AUTOCONF_BIN} --version")
run_shell(f"{AUTOMAKE_BIN} --version")
run_shell(f"{ACLOCAL_BIN} --version")
run_shell(f"{AUTOHEADER_BIN} --version")

print(f"Extra environment: {EXTRA_ENV}", flush=True)

config_folders: list[Path] = []
autogen_folders: list[Path] = []
repo_root = Path.cwd()

for root, _, files in os.walk("."):
    for file in files:
        if file == "configure.ac":
            config_folders.append(Path(root).resolve())
        if file == "Makefile.tpl":
            autogen_folders.append(Path(root).resolve())

for folder in sorted(autogen_folders):
    print(f"Entering directory {folder}", flush=True)
    os.chdir(folder)
    regenerate_with_autogen()

for folder in sorted(config_folders):
    if folder.stem in SKIP_DIRS:
        print(f"Skipping directory {folder}", flush=True)
        continue

    print(f"Entering directory {folder}", flush=True)
    os.chdir(folder)

    if str(folder.relative_to(repo_root)) in AUTORECONF_DIRS:
        regenerate_with_autoreconf()
    else:
        regenerate_manually()
