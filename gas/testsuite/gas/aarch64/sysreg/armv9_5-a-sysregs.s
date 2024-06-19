.include "sysreg-test-utils.inc"

.text

/* Delegated SError exceptions for EL3. */
rw_sys_reg sys_reg=vdisr_el3 xreg=x0 r=1 w=1
rw_sys_reg sys_reg=vsesr_el3 xreg=x0 r=1 w=1
