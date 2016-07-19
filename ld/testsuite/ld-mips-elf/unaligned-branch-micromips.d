#name: microMIPS link branch to unaligned symbol
#as: -EB -n32 -march=from-abi
#ld: -EB -Ttext 0x1c000000 -e 0x1c000000
#source: ../../../gas/testsuite/gas/mips/unaligned-branch-micromips-2.s
#error: \A[^\n]*: In function `foo':\n
#error:   \(\.text\+0x1002\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x100a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x100a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1012\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1012\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x101a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x101a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1022\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x102a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x102a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1032\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1032\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x103a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x103a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1042\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1062\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1072\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1082\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1088\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1088\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x108e\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x108e\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1094\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1094\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x109a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10a0\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10a0\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10a6\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10a6\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10ac\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10ac\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10b2\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10ca\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10d6\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10e2\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10e8\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10e8\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10ee\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10ee\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10f4\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10f4\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x10fa\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1100\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1100\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1106\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1106\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x110c\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x110c\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1112\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x112a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1136\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1142\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1146\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1146\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x114a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x114a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x114e\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x114e\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1152\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1156\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1156\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x115a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x115a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x115e\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x115e\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1162\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1172\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x117a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1182\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1186\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1186\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x118a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x118a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x118e\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x118e\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1192\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1196\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x1196\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x119a\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x119a\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x119e\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x119e\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x11a2\): Unsupported branch between ISA modes\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x11b2\): Branch to a non-instruction-aligned address\n
#error:   [^\n]*: In function `foo':\n
#error:   \(\.text\+0x11ba\): Branch to a non-instruction-aligned address\Z
