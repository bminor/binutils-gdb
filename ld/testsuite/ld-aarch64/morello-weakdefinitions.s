# N.b. It should not matter what is contained in this file for the tests we
# run.  What we want to show is that the behavior of weak undefined symbols is
# correct for dynamically linked PDE's.  That requires linking to a shared
# object which is the point of this separate assembly file.
      .global globalsym
      .type globalsym, %object
globalsym:
  .xword 100

      .global hiddensym
      .type hiddensym, %object
hiddensym:
  .xword 100

      .global localsym
      .type localsym, %object
localsym:
  .xword 100
