/* HPPA linker stub instructions  */

/* Maximum size of a single stub routine in bytes.  */
#define STUB_MAX_SIZE	(sizeof(int) * 20)

/* When realloc'ing, use this size ?!? FIXME, what are these?  */
#define STUB_BUFFER_INCR	(STUB_MAX_SIZE * 20)
#define STUB_RELOC_INCR	20

/* These are the instructions which the linker may insert into the
   code stream when building final executables to handle out-of-range
   calls and argument relocations.  */

#define ADDI_8_SP          	0xb7de0010	/* addi        8,sp,sp */
#define STW_RP_M8SP        	0x6bc23ff1	/* stw         rp,-8(sp) */
#define STWS_ARG0_M8SP     	0x0fda1291	/* stws        arg0,-8(sp) */
#define STWS_ARG1_M8SP     	0x0fd91291	/* stws        arg1,-8(sp) */
#define STWS_ARG2_M8SP     	0x0fd81291	/* stws        arg2,-8(sp) */
#define STWS_ARG3_M8SP     	0x0fd71291	/* stws        arg3,-8(sp) */
#define STWS_ARG0_M4SP     	0x0fda1299	/* stws        arg0,-4(sp) */
#define STWS_ARG2_M4SP     	0x0fd81299	/* stws        arg2,-4(sp) */
#define FSTWS_FARG0_M8SP   	0x27d11204	/* fstws       farg0,-8(sp) */
#define FSTWS_FARG1_M8SP   	0x27d11205	/* fstws       farg1,-8(sp) */
#define FSTWS_FARG2_M8SP   	0x27d11206	/* fstws       farg2,-8(sp) */
#define FSTWS_FARG3_M8SP   	0x27d11207	/* fstws       farg3,-8(sp) */
#define FSTDS_FARG1_M8SP   	0x2fd11205	/* fstds       farg1,-8(sp) */
#define FSTDS_FARG3_M8SP   	0x2fd11207	/* fstds       farg3,-8(sp) */
#define STWS_RET0_M8SP     	0x0fda1291	/* stws        arg0,-8(sp) */
#define FSTWS_FRET0_M8SP   	0x27d11204	/* fstws       fret0,-8(sp) */
#define LDWS_M8SP_ARG1     	0x0fd11099	/* ldws        -8(sp),arg1 */
#define LDWS_M8SP_ARG3     	0x0fd11097	/* ldws        -8(sp),arg3 */
#define LDWS_M4SP_ARG0     	0x0fd9109a	/* ldws        -4(sp),arg0 */
#define LDWS_M4SP_ARG1     	0x0fd91099	/* ldws        -4(sp),arg1 */
#define LDWS_M4SP_ARG2     	0x0fd91098	/* ldws        -4(sp),arg2 */
#define LDWS_M4SP_ARG3     	0x0fd91097	/* ldws        -4(sp),arg3 */
#define FLDWS_M8SP_FARG0   	0x27d11004	/* fldws       -8(sp),farg0 */
#define FLDWS_M8SP_FARG1   	0x27d11005	/* fldws       -8(sp),farg1 */
#define FLDWS_M8SP_FARG2   	0x27d11006	/* fldws       -8(sp),farg2 */
#define FLDWS_M8SP_FARG3   	0x27d11007	/* fldws       -8(sp),farg3 */
#define FLDDS_M8SP_FARG1   	0x2fd11005	/* fldds       -8(sp),farg1 */
#define FLDDS_M8SP_FARG3   	0x2fd11007	/* fldds       -8(sp),farg3 */
#define LDWS_M4SP_RET0     	0x0fd9109c	/* ldws        -4(sp),ret0 */
#define FLDWS_M8SP_FRET0   	0x27d11004	/* fldws       -8(sp),fret0 */
#define BLE_XXX_0_0        	0xe4000000	/* ble         0(sr0,0) */
#define BLE_N_XXX_0_0      	0xe4000002	/* ble,n       0(sr0,0) */
#define BLE_XXX_0_31       	0xe7e00000	/* ble         0(sr0,r31) */
#define BE_N_XXX_0_31      	0xe3e00002	/* be,n        0(sr0,r31) */
#define BE_XXX_0_31        	0xe3e00000	/* be          0(sr0,r31) */
#define COPY_31_2          	0x081f0242	/* copy        r31,r2 */
#define COPY_31_1          	0x081f0241	/* copy        r31,r1 */
#define COPY_1_31          	0x0801025f	/* copy        r1,r31 */
#define LDIL_XXX_31        	0x23e00000	/* ldil        0,r31 */
#define LDSID_31_1         	0x03e010a1	/* ldsid       (r31),r1 */
#define MTSP_1_SR0         	0x00011820	/* mtsp        r1,sr0 */
#define ADDI_M4_31_RP      	0xb7e207f9	/* addi        -4,r31,rp */
#define ADDI_M4_31_1       	0xb7e107f9	/* addi        -4,r31,r1 */
#define ADDI_M8_SP_SP      	0xb7de07f1	/* addi        -8,sp,sp */
#define LDW_M8SP_RP        	0x4bc23ff1	/* ldw         -8(sp),rp */
#define BV_N_0_RP          	0xe840c002	/* bv,n        0(rp) */
