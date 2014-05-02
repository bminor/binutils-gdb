.text ; foo: addr16 fstcw %es:(%si)
 fstsw; fstsw %ax;
 addr16 fstsw %ax ;addr16 rep cmpsw %es:(%di),%ss:(%si)

 es fwait

# Get a good alignment.
 .p2align	4,0
