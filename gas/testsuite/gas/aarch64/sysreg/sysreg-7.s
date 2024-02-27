.arch armv8-a+lor

msr lorc_el1, x0
mrs x0, lorc_el1
msr lorea_el1, x0
mrs x0, lorea_el1
msr lorn_el1, x0
mrs x0, lorn_el1
msr lorsa_el1, x0
mrs x0, lorsa_el1
msr icc_ctlr_el3, x0
mrs x0, icc_ctlr_el3
msr icc_sre_el1, x0
mrs x0, icc_sre_el1
msr icc_sre_el2, x0
mrs x0, icc_sre_el2
msr icc_sre_el3, x0
mrs x0, icc_sre_el3

mrs x0, ich_vtr_el2
