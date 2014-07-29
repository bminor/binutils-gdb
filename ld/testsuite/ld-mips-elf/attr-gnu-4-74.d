#source: attr-gnu-4-7.s -32 -EB
#source: attr-gnu-4-4.s -W -32 -EB
#ld: -r -melf32btsmip
#error: \A[^\n]*: Warning: .* uses -mgp32 -mfp64 -mno-odd-spreg \(set by .*\), .* uses -mips32r2 -mfp64 \(12 callee-saved\)\n
#error:   [^\n]*: [^\n]* linking -mfp64 module with previous -mfp32 modules\n
#error:   [^\n]*: failed to merge target specific data of file [^\n]*\.o\Z
