
Relocation section '\.rela\.dyn' at offset .* contains 2 entries:
 *Offset * Info * Type * Sym\. *Value * Sym\. *Name \+ Addend
# This must be an absolute relocation, there must not be a _gp reference.
[0-9a-f]+ * 0+03 * R_RISCV_RELATIVE * [0-9a-f]+
[0-9a-f]+ * [0-9a-f]+02 * R_RISCV_64 * [0-9a-f]+ * bar \+ 0
