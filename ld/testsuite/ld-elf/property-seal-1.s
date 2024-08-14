	.section ".note.gnu.property", "a"
	.p2align ALIGN
	.long 1f - 0f		/* name length */
	.long 3f - 2f		/* data length */
	.long 5			/* note type */
0:	.asciz "GNU"		/* vendor name */
1:
	.p2align ALIGN
2:	.long 3			/* pr_type.  */
	.long 0			/* pr_datasz.  */
3:
