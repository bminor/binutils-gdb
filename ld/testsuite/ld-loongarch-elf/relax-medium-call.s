.section "ta", "ax"
a:
  ret
  ret
  ret
b:
  ret

.text
  call36 a	# min offset, can relax
  call36 a	# overflow, not relax
  tail36 $t0, b	# min offset, can relax
  tail36 $t0, b	# overflow, not relax

  call36 c	# overflow, not relax
  call36 c	# max offset, can relax
  tail36 $t0, d	# overflow, no relax
  tail36 $t0, d # max offset, can relax

.section "tb", "ax"
  ret
  ret
  ret
  ret
  ret
  ret
  ret
  ret
c:
  ret
  ret
  ret
d:
  ret

