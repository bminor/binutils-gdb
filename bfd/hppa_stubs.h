/* HPPA linker stub instructions */

#define STUB_MAX_SIZE	(sizeof(int) * 20)	/* maximum size of a single stub routine */
#define STUB_BUFFER_INCR	(STUB_MAX_SIZE * 20)	/* when realloc'ing, use this size */
#define STUB_RELOC_INCR	20	/* when realloc'ing, use this size */

#define ADDI_8_SP          	0xb7de0010	/* addi        8,sp,sp */
#define STW_RP_M8SP        	0x6bc23ff1	/* stw         rp,-8(sp) */
#define STWS_ARG0_M8SP     	0x0fda1291	/* stws        arg0,-8(sp) */
#define STWS_ARG1_M8SP     	0x0fd91291	/* stws        arg1,-8(sp) */
#define STWS_ARG2_M8SP     	0x0fd81291	/* stws        arg2,-8(sp) */
#define STWS_ARG3_M8SP     	0x0fd71291	/* stws        arg3,-8(sp) */
#define FSTWS_FARG0_M8SP   	0x27d11204	/* fstws       farg0,-8(sp) */
#define FSTWS_FARG1_M8SP   	0x27d11205	/* fstws       farg1,-8(sp) */
#define FSTWS_FARG2_M8SP   	0x27d11206	/* fstws       farg2,-8(sp) */
#define FSTWS_FARG3_M8SP   	0x27d11207	/* fstws       farg3,-8(sp) */
#define STWS_RET0_M8SP     	0x0fda1291	/* stws        arg0,-8(sp) */
#define FSTWS_FRET0_M8SP   	0x27d11204	/* fstws       fret0,-8(sp) */
#define LDWS_M8SP_ARG0     	0x0fd1109a	/* ldws        -8(sp),arg0 */
#define LDWS_M8SP_ARG1     	0x0fd11099	/* ldws        -8(sp),arg1 */
#define LDWS_M8SP_ARG2     	0x0fd11098	/* ldws        -8(sp),arg2 */
#define LDWS_M8SP_ARG3     	0x0fd11097	/* ldws        -8(sp),arg3 */
#define FLDWS_M8SP_FARG0   	0x27d11004	/* fldws       -8(sp),farg0 */
#define FLDWS_M8SP_FARG1   	0x27d11005	/* fldws       -8(sp),farg1 */
#define FLDWS_M8SP_FARG2   	0x27d11006	/* fldws       -8(sp),farg2 */
#define FLDWS_M8SP_FARG3   	0x27d11007	/* fldws       -8(sp),farg3 */
#define LDWS_M8SP_RET0     	0x0fd1109c	/* ldws        -8(sp),ret0 */
#define FLDWS_M8SP_FRET0   	0x27d11004	/* fldws       -8(sp),fret0 */
#define STWS_MA_ARG0_8SP   	0x0fda12b0	/* stws,ma     arg0,8(sp) */
#define STWS_MA_ARG1_8SP   	0x0fd912b0	/* stws,ma     arg1,8(sp) */
#define STWS_MA_ARG2_8SP   	0x0fd812b0	/* stws,ma     arg2,8(sp) */
#define STWS_MA_ARG3_8SP   	0x0fd712b0	/* stws,ma     arg3,8(sp) */
#define FSTWS_MA_FARG0_8SP 	0x27d01224	/* fstws,ma    farg0,8(sp) */
#define FSTWS_MA_FARG1_8SP 	0x27d01225	/* fstws,ma    farg1,8(sp) */
#define FSTWS_MA_FARG2_8SP 	0x27d01226	/* fstws,ma    farg2,8(sp) */
#define FSTWS_MA_FARG3_8SP 	0x27d01227	/* fstws,ma    farg3,8(sp) */
#define LDWS_MB_M8SP_ARG0  	0x0fd130ba	/* ldws,mb     -8(sp),arg0 */
#define LDWS_MB_M8SP_ARG1  	0x0fd130b9	/* ldws,mb     -8(sp),arg1 */
#define LDWS_MB_M8SP_ARG2  	0x0fd130b8	/* ldws,mb     -8(sp),arg2 */
#define LDWS_MB_M8SP_ARG3  	0x0fd130b7	/* ldws,mb     -8(sp),arg3 */
#define FLDWS_MB_M8SP_FARG0	0x27d13024	/* fldws,mb    -8(sp),farg0 */
#define FLDWS_MB_M8SP_FARG1	0x27d13025	/* fldws,mb    -8(sp),farg1 */
#define FLDWS_MB_M8SP_FARG2	0x27d13026	/* fldws,mb    -8(sp),farg2 */
#define FLDWS_MB_M8SP_FARG3	0x27d13027	/* fldws,mb    -8(sp),farg3 */
#define BL_XXX_RP          	0xe8400000	/* bl          0,rp */
#define NOP                	0x08000240	/* nop */
#define SUBI_8_SP          	0x97de0010	/* subi        8,sp,sp */
#define LDW_M8SP_RP        	0x4bc23ff1	/* ldw         -8(sp),rp */
#define BV_N_0RP           	0xe840c002	/* bv,n        0(rp) */
