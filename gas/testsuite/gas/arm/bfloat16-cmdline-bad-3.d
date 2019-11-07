#name: Bfloat 16 bad extension
#source: bfloat16-non-neon.s
#as: -mno-warn-deprecated -march=armv8.1-a+bf16
#error: .*Error: extension does not apply to the base architecture.*
