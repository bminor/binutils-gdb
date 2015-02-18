#name: PIE with undefined symbol
#as: --64
#ld: -pie -melf_x86_64 --noinhibit-exec
#warning: \A[^\n]*\.o[^\n]*In function `_start':\n[^\n]*: undefined reference to `foo'\Z
