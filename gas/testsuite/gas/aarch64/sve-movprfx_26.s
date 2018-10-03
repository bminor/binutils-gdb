/* Checks the special cases for FCVT and LSL.
   Has invalid usages.  Diagnosis required.  */
  .text
  .arch armv8-a+sve

f:
  /* Not OK, 64-bit operation, upper 32-bits cleared.  */
  movprfx Z0.S, P1/M, Z1.S
  fcvt Z0.S, P1/M, Z2.D

  /* OK, 64-bit operation, upper 32-bits cleared.  */
  movprfx Z0.D, P1/M, Z1.D
  fcvt Z0.S, P1/M, Z2.D

  /* Not OK, 64-bit operation ignoring 32-bits.  */
  movprfx Z0.S, P1/M, Z1.S
  fcvt Z0.D, P1/M, Z2.S

  /* OK, 64-bit operation ignoring 32-bits.  */
  movprfx Z0.D, P1/M, Z1.D
  fcvt Z0.D, P1/M, Z2.S

  /* OK, 8-bit operation.  */
  movprfx Z0.B, P1/M, Z1.B
  lsl Z0.B, P1/M, Z0.B, Z2.D

  /* Not Ok, destination register sizes don't match.  */
  movprfx Z0.D, P1/M, Z1.D
  lsl Z0.B, P1/M, Z0.B, Z2.D
  ret

