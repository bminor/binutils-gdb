/* Realm Management Extension.  */

/* Read from RME system registers.  */
mrs x0, mfar_el3
mrs x0, gpccr_el3
mrs x0, gptbr_el3

/* Write to RME system registers.  */
msr gpccr_el3, x0
msr gptbr_el3, x0
