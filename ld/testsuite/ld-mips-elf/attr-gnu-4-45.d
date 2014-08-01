#source: attr-gnu-4-4.s -W -32 -EB
#source: attr-gnu-4-5.s -32 -EB
#ld: -r -melf32btsmip
#error: \A[^\n]*: Warning: .* uses -mips32r2 -mfp64 \(12 callee-saved\) \(set by .*\), .* uses -mfpxx\n
#error:   [^\n]*: [^\n]* linking -mfp32 module with previous -mfp64 modules\n
#error:   [^\n]*: failed to merge target specific data of file [^\n]*\.o\Z
