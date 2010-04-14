# Linker script for Alpha VMS systems.
# Tristan Gingold <gingold@adacore.com>.

PAGESIZE=0x10000

cat <<EOF
OUTPUT_FORMAT("${OUTPUT_FORMAT}")
${LIB_SEARCH_DIRS}

SECTIONS
{
  ${RELOCATING+. = ${PAGESIZE};}

  \$DATA\$ ALIGN (${PAGESIZE}) : {
    *(\$DATA\$)
    *(\$LINK\$)
  }
  \$BSS\$ ALIGN (${PAGESIZE}) : {
    *(\$BSS\$)
  }
  \$CODE\$ ALIGN (${PAGESIZE}) : {
    *(\$CODE\$)
  }
  \$LITERAL\$ ALIGN (${PAGESIZE}) : {
    *(\$LITERAL\$)
    *(\$READONLY\$)
    *(\$READONLY_ADDR\$)
  }

  \$DST\$ 0 : {
    *(\$DST\$)
  }
}
EOF
