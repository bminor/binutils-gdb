/* Dwarf definitions.

   This file is derived from the DWARF specification, Draft 3 by the UNIX
   International Programming Languages Special Interest Group dated 6/5/91.

   Updated to Draft 4 dated 7/31/91.

   (Extracted from GCC 2.0, snapshot 267 and placed in Cygnus include
    tree on 9/28/91 by Fred Fish, as part of gdb SVR4 DWARF support.)
*/

/* Tag names: 4.1  Debugging Information Entry, figure 9.  */

#define TAG_padding			0x0000
#define TAG_array_type			0x0001
#define TAG_class_type			0x0002
#define TAG_entry_point			0x0003
#define TAG_enumeration_type		0x0004
#define TAG_formal_parameter		0x0005
#define TAG_global_subroutine		0x0006
#define TAG_global_variable		0x0007
#define TAG_imported_declaration	0x0008
#define TAG_inline_subroutine		0x0009
#define TAG_label			0x000a
#define TAG_lexical_block		0x000b
#define TAG_local_variable		0x000c
#define TAG_member			0x000d
#define TAG_member_function		0x000e
#define TAG_pointer_type		0x000f
#define TAG_reference_type		0x0010
#define TAG_compile_unit		0x0011
#define TAG_source_file			0x0011
#define TAG_string_type			0x0012
#define TAG_structure_type		0x0013
#define TAG_subroutine			0x0014
#define TAG_subroutine_type		0x0015
#define TAG_typedef			0x0016
#define TAG_union_type			0x0017
#define TAG_unspecified_parameters	0x0018
#define TAG_variant			0x0019

/* Mysterious gap introduced in draft 4!  */
#define TAG_format			0x0020
#define TAG_with_stmt			0x0021
#define TAG_set_type			0x0022

/* Coming soon?  */
#define TAG_file_type			0x8000

#define TAG_lo_user			0x8000
#define TAG_hi_user			0xffff

/* Form names: 4.2  Attributes Types, figure 10.  */

#define FORM_ADDR	0x1
#define FORM_REF	0x2
#define FORM_BLOCK2	0x3
#define FORM_BLOCK4	0x4
#define FORM_DATA2	0x5
#define FORM_DATA4	0x6
#define FORM_DATA8	0x7
#define FORM_STRING	0x8

/* Attribute form encodings: 4.2  Attributes Types, figure 11.

        Attribute name      Form           Value
*/

#define AT_sibling		/* reference */	(0x0010|FORM_REF)
#define AT_location		/* block2 */	(0x0020|FORM_BLOCK2)
#define AT_name			/* string */	(0x0030|FORM_STRING)
#define AT_fund_type		/* halfword */	(0x0050|FORM_DATA2)
#define AT_mod_fund_type	/* block2 */	(0x0060|FORM_BLOCK2)
#define AT_user_def_type	/* reference */	(0x0070|FORM_REF)
#define AT_mod_u_d_type		/* block2 */	(0x0080|FORM_BLOCK2)
#define AT_ordering		/* halfword */	(0x0090|FORM_DATA2)
#define AT_subscr_data		/* block2 */	(0x00a0|FORM_BLOCK2)
#define AT_byte_size		/* word */	(0x00b0|FORM_DATA4)
#define AT_bit_offset		/* halfword */	(0x00c0|FORM_DATA2)
#define AT_bit_size		/* word */	(0x00d0|FORM_DATA4)
#define AT_deriv_list		/* block2 */	(0x00e0|FORM_BLOCK2)
#define AT_element_list		/* block4 */	(0x00f0|FORM_BLOCK4)
#define AT_stmt_list		/* word */	(0x0100|FORM_DATA4)
#define AT_low_pc		/* address */	(0x0110|FORM_ADDR)
#define AT_high_pc		/* address */	(0x0120|FORM_ADDR)
#define AT_language		/* word */	(0x0130|FORM_DATA4)
#define AT_member		/* reference */	(0x0140|FORM_REF)
#define AT_discr		/* reference */	(0x0150|FORM_REF)
#define AT_discr_value		/* block2 */	(0x0160|FORM_BLOCK2)
#define AT_visibility		/* halfword */	(0x0170|FORM_DATA2)
#define AT_import		/* reference */	(0x0180|FORM_REF)
#define AT_string_length	/* block2 */	(0x0190|FORM_BLOCK2)
#define AT_comp_dir		/* string */	(0x01a0|FORM_STRING)
#define AT_producer		/* string */	(0x01b0|FORM_STRING)

/* Added in draft 4.  */
#define AT_loclist		/* word */	(0x01c0|FORM_DATA4)
#define AT_frame_base		/* word */	(0x01d0|FORM_DATA4)
#define AT_incomplete		/* halfword */	(0x01e0|FORM_DATA2)
#define AT_start_scope		/* word */	(0x01f0|FORM_DATA4)
#define AT_stride_size		/* word */	(0x0200|FORM_DATA4)

/* Coming soon.  */
#define AT_src_info		/* word */	(0x8010|FORM_DATA4)
#define AT_prototyped		/* halfword */	(0x8020|FORM_DATA2)
#define AT_const_data		/* block4 */	(0x8030|FORM_BLOCK4)
#define AT_is_external		/* halfword */	(0x8040|FORM_DATA2)

#define AT_lo_user		/* - */		0x8000
#define AT_hi_user		/* - */		0xffff

/* Atom names: 4.5  Location Atoms, figure 12.  */

#define OP_REG		0x01
#define OP_BASEREG	0x02
#define OP_ADDR		0x03
#define OP_CONST	0x04
#define OP_DEREF2	0x05
#define OP_DEREF4	0x06
#define OP_ADD		0x07
   
#define OP_LO_USER	0x80
#define OP_HI_USER	0xff

/* Type names: 4.6  Fundamental Types, figure 13.  */

#define FT_char			0x0001
#define FT_signed_char		0x0002
#define FT_unsigned_char	0x0003
#define FT_short		0x0004
#define FT_signed_short		0x0005
#define FT_unsigned_short	0x0006
#define FT_integer		0x0007
#define FT_signed_integer	0x0008
#define FT_unsigned_integer	0x0009
#define FT_long			0x000a
#define FT_signed_long		0x000b
#define FT_unsigned_long	0x000c
#define FT_pointer		0x000d
#define FT_float		0x000e
#define FT_dbl_prec_float	0x000f
#define FT_ext_prec_float	0x0010
#define FT_complex		0x0011
#define FT_dbl_prec_complex	0x0012
#define FT_boolean		0x0013 /* Disputed value; added in draft 4.  */
#define FT_void			0x0014
  
/* GNU-specific fundamental type codes.  */
#define FT_long_long		0x8000
#define FT_signed_long_long	0x8001
#define FT_unsigned_long_long	0x8002
  
#define FT_lo_user		0x8000
#define FT_hi_user		0xffff

/* Type modifier names: 4.7  Type Modifiers, figure 14.  */

#define MOD_pointer_to		0x01
#define MOD_reference_to	0x02
     
#define MOD_const		0x03
#define MOD_volatile		0x04
     
#define MOD_lo_user		0x80
#define MOD_hi_user		0xff
     
/* Visibility names: 4.8  Visibility Entries, figure 15.  */
     
#define VIS_local		0x00
#define VIS_exported		0x01
     
#define VIS_lo_user		0x80
#define VIS_hi_user		0xff

/* Ordering names: 4.9.1  Array Ordering, figure 16.  */

#define ORD_row_major	0
#define ORD_col_major	1

/* Format names: 4.9.2  Array Subscripts, figure 17.  */

#define FMT_FT_C_C	0x0
#define FMT_FT_C_X	0x1
#define FMT_FT_X_C	0x2
#define FMT_FT_X_X	0x3
#define FMT_UT_C_C	0x4
#define FMT_UT_C_X	0x5
#define FMT_UT_X_C	0x6
#define FMT_UT_X_X	0x7
#define FMT_ET		0x8

/* Derived from above for ease of use.  */

#define FMT_CODE(_FUNDAMENTAL_TYPE_P, _UB_CONST_P, _LB_CONST_P) \
 (((_FUNDAMENTAL_TYPE_P) ? 0 : 4)	\
  | ((_UB_CONST_P) ? 0 : 2)		\
  | ((_LB_CONST_P) ? 0 : 1))

/* Language names: 4.13  Source Languages, figure 18.  */

#define LANG_NONE		0x0000
#define LANG_C89		0x0001
#define LANG_C			0x0002
#define LANG_C_PLUS_PLUS	0x0003
#define LANG_FORTRAN77		0x0004
#define LANG_FORTRAN90		0x0005
#define LANG_PASCAL83		0x0006
#define LANG_MODULA2		0x0007
#define LANG_ADA83		0x0008
#define LANG_COBOL74		0x0009
#define LANG_COBOL85		0x0010
#define LANG_LO_USER		0x8000
#define LANG_HI_USER		0xffff
