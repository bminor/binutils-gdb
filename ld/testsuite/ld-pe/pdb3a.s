.equ CV_SIGNATURE_C13, 4
.equ DEBUG_S_STRINGTABLE, 0xf3
.equ DEBUG_S_FILECHKSMS, 0xf4
.equ CHKSUM_TYPE_MD5, 1

.equ NUM_MD5_BYTES, 16

.section ".debug$S", "rn"
.long CV_SIGNATURE_C13
.long DEBUG_S_STRINGTABLE
.long .strings_end - .strings_start

.strings_start:

.asciz ""

.src1:
.asciz "foo"

.src2:
.asciz "bar"

.strings_end:

.balign 4

.long DEBUG_S_FILECHKSMS
.long .chksms_end - .chksms_start

.chksms_start:

.long .src1 - .strings_start
.byte NUM_MD5_BYTES
.byte CHKSUM_TYPE_MD5
.long 0x01234567
.long 0x89abcdef
.long 0xfedcba98
.long 0x67452310
.short 0 # padding

.long .src2 - .strings_start
.byte NUM_MD5_BYTES
.byte CHKSUM_TYPE_MD5
.long 0xfedcba98
.long 0x67452310
.long 0x01234567
.long 0x89abcdef
.short 0 # padding

.chksms_end:

.balign 4
