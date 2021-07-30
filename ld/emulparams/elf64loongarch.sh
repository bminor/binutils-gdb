. ${srcdir}/emulparams/elf64loongarch-defs.sh
OUTPUT_FORMAT="elf64-loongarch"

case "$target" in
  loong64*-linux*)
    case "$EMULATION_NAME" in
      *64*)
	LIBPATH_SUFFIX="64/lib64 64";;
    esac
    ;;
esac
