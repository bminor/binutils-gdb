/* A YACC grammer to parse a superset of the AT&T linker scripting languaue.
   Copyright (C) 1991 Free Software Foundation, Inc.
   Written by Steve Chamberlain of Cygnus Support (steve@cygnus.com).

This file is part of GNU ld.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

%{
/*

 */

#define DONTDECLARE_MALLOC

#include "bfd.h"
#include "sysdep.h"
#include "ld.h"    
#include "ldexp.h"
#include "ldver.h"
#include "ldlang.h"
#include "ldemul.h"
#include "ldfile.h"
#include "ldmisc.h"
#include "mri.h"

#define YYDEBUG 1

boolean option_v;
extern unsigned int lineno;
extern boolean trace_files;
extern boolean write_map;
extern boolean option_longmap;
boolean hex_mode;
static int typebits;
strip_symbols_type strip_symbols=STRIP_NONE;
discard_locals_type discard_locals=DISCARD_NONE;


lang_memory_region_type *region;


lang_memory_region_type *lang_memory_region_lookup();
lang_output_section_statement_type *lang_output_section_statement_lookup();
etree_type *lang_atin();
#ifdef __STDC__

void lang_add_data(int type, union etree_union *exp);
void lang_enter_output_section_statement(char *output_section_statement_name, etree_type *address_exp, int flags, bfd_vma block_value,etree_type*,etree_type*, etree_type*);

#else

void lang_add_data();
void lang_enter_output_section_statement();

#endif /* __STDC__ */

extern args_type command_line;
char *current_file;
boolean ldgram_want_filename = true;
boolean had_script = false;
boolean force_make_executable = false;

boolean ldgram_in_script = false;
boolean ldgram_had_equals = false;
/* LOCALS */


#define ERROR_NAME_MAX 20
static char *error_names[ERROR_NAME_MAX];
static int error_index;
#define PUSH_ERROR(x) if (error_index < ERROR_NAME_MAX) error_names[error_index] = x; error_index++;
#define POP_ERROR()   error_index--;
%}
%union {
  bfd_vma integer;
  int voidval;
  char *name;
  int token;
  union etree_union *etree;
struct sec *section;
  struct lang_output_section_statement_struct *output_section_statement;
  union  lang_statement_union **statement_ptr;
  int lineno;
  struct {
    FILE *file;
    char *name;
    unsigned int lineno;
  } state;

  
}

%type <etree> exp  opt_exp_with_type  mustbe_exp opt_at
%type <integer> fill_opt
%type <name> memspec_opt
%token <integer> INT  
%token <name> NAME
%type  <integer> length

%right <token> PLUSEQ MINUSEQ MULTEQ DIVEQ  '=' LSHIFTEQ RSHIFTEQ   ANDEQ OREQ 
%right <token> '?' ':'
%left <token> OROR
%left <token>  ANDAND
%left <token> '|'
%left <token>  '^'
%left  <token> '&'
%left <token>  EQ NE
%left  <token> '<' '>' LE GE
%left  <token> LSHIFT RSHIFT

%left  <token> '+' '-'
%left  <token> '*' '/' '%'

/*%token <token> '+' '-' '*' '/' '%'*/
%right UNARY
%token END 
%left <token> '('
%token <token> ALIGN_K BLOCK LONG SHORT BYTE
%token SECTIONS  
%token '{' '}'
%token SIZEOF_HEADERS OUTPUT_FORMAT FORCE_COMMON_ALLOCATION OUTPUT_ARCH
%token SIZEOF_HEADERS
%token INCLUDE
%token MEMORY  DEFSYMEND
%token NOLOAD DSECT COPY INFO OVERLAY
%token NAME DEFINED TARGET_K SEARCH_DIR MAP ENTRY 
%token OPTION_e OPTION_c OPTION_noinhibit_exec OPTION_s OPTION_S OPTION_sort_common
%token OPTION_EB OPTION_EL
%token OPTION_format  OPTION_F OPTION_u OPTION_Bstatic OPTION_N
%token <integer> SIZEOF NEXT ADDR 
%token OPTION_d OPTION_dc OPTION_dp OPTION_x OPTION_X OPTION_defsym
%token OPTION_v OPTION_V OPTION_M OPTION_t STARTUP HLL SYSLIB FLOAT  NOFLOAT 
%token OPTION_Map
%token OPTION_n OPTION_r OPTION_o OPTION_b  OPTION_R OPTION_relax
%token <name> OPTION_l OPTION_L OPTION_T OPTION_Aarch OPTION_Tfile  OPTION_Texp
%token <name> OPTION_y
%token OPTION_Ur 
%token ORIGIN FILL OPTION_g
%token LENGTH    CREATE_OBJECT_SYMBOLS INPUT OUTPUT  CONSTRUCTORS
%token OPTION_RETAIN_SYMBOLS_FILE ALIGNMOD AT

%type <token> assign_op 

%type <name>  filename


%token CHIP LIST SECT ABSOLUTE  LOAD NEWLINE ENDWORD ORDER NAMEWORD
%token FORMAT PUBLIC DEFSYMEND BASE ALIAS TRUNCATE

%{
ld_config_type config;
%}

%%

file:	command_line  { lang_final(); };


filename:  NAME;


command_line:
		command_line command_line_option
	|
	;

command_line_option:
        	OPTION_Bstatic { }
	|	OPTION_v
			{	
			ldversion(0);
			option_v = true;
			}
	|	OPTION_V
			{	
			ldversion(1);
			option_v = true;
			}
	|	OPTION_t {
			trace_files = true;
			}
	|     OPTION_Map  NAME
		{
		write_map = true;
		config.map_filename = $2;
		}
	|	OPTION_M 
		{
		  config.map_filename = "-";
		}
	|	OPTION_n {
			config.magic_demand_paged = false;
			}
        |       OPTION_N {
			config.text_read_only = false;
			config.magic_demand_paged = false;
	                }
        |       OPTION_s {
	  		strip_symbols = STRIP_ALL;
			}
	|	OPTION_S {
			strip_symbols = STRIP_DEBUGGER;
			}
        |       OPTION_u NAME {
			ldlang_add_undef($2);
	      	}
	    
	|	OPTION_r {
			config.relocateable_output = true;
			config.build_constructors = false;
			config.magic_demand_paged = false;
 			config.text_read_only = false;
			}
        |       OPTION_Ur {
			config.relocateable_output = true;
			config.build_constructors = true;
			config.magic_demand_paged = false;
 			config.text_read_only = false;
		      }	            
	|	OPTION_o filename
			{
			lang_add_output($2); 
			}
	|	OPTION_e NAME
			{ lang_add_entry($2); 
			}
	|	OPTION_X {
			discard_locals = DISCARD_L;
		}
	|	OPTION_x {
			discard_locals = DISCARD_ALL;
		}

	| 	OPTION_noinhibit_exec
			{
			force_make_executable = true;
			}
        |      OPTION_sort_common {
	config.sort_common = true;
      }
    	|      OPTION_d {
			  command_line.force_common_definition = true;
			}

    	|      OPTION_relax {
			  command_line.relax = true;
			}
    	|      OPTION_dc
			 {
			  command_line.force_common_definition = true;
			}
	|	OPTION_g
			{
			/* Ignored */
			}
	|      	OPTION_dp
			 {
			  command_line.force_common_definition = true;
			}
	| 	OPTION_format NAME
	           {
			  lang_add_target($2);
       		   }
	| 	OPTION_Texp 
		{ ldlex_expression();
			hex_mode  = 16; 
		} 
		INT
		{  ldlex_popstate();
			lang_section_start($1,exp_intop($3));
			hex_mode = 0; 
		}
	|	OPTION_y
			{
			add_ysym($1);
			}
	| 	OPTION_Aarch 
		{ 
			ldfile_add_arch($1); 
		}
	|	 OPTION_b NAME
			{
			lang_add_target($2);
			}
	|	OPTION_L
			{
			ldfile_add_library_path($1);
			}
	|	OPTION_F
		{
		/* Ignore */
		}
        | 	NAME
		{ lang_add_input_file($1,lang_input_file_is_file_enum,
				 (char *)NULL); }
	|	OPTION_c filename 
			{ ldfile_open_command_file($2); } mri_script_file  END {  ldlex_command();}

	|	OPTION_Tfile 
			{ ldfile_open_command_file($1); } script_file
  			END {  ldlex_command();}

	|	OPTION_T filename 
			{ ldfile_open_command_file($2); } script_file
		END {  ldlex_command();}

	|	OPTION_l
			{
			  lang_add_input_file($1,
					 lang_input_file_is_l_enum,
					 (char *)NULL);
			}
	| 	OPTION_R filename
			{
			lang_add_input_file($2,
				lang_input_file_is_symbols_only_enum,
				(char *)NULL);
			}

	|	OPTION_defsym  { ldlex_defsym(); }
		NAME 	 '=' exp  DEFSYMEND { ldlex_popstate();
			lang_add_assignment(exp_assop($4,$3,$5));
			}
	|	OPTION_RETAIN_SYMBOLS_FILE filename
		{ lang_add_keepsyms_file ($2); }
	|	OPTION_EB
		{
		  /* FIXME: This is currently ignored.  It means
		     ``produce a big-endian object file''.  It could
		     be used to select an output format.  */
		}
	|	OPTION_EL
		{
		  /* FIXME: This is currently ignored.  It means
		     ``produce a little-endian object file''.  It could
		     be used to select an output format.  */
		}
	| '-' NAME
		 { info("%P%F Unrecognized option -%s\n", $2);  }

	| '{' script_file '}'  
	;


/* SYNTAX WITHIN AN MRI SCRIPT FILE */  
mri_script_file:
		{    	ldlex_mri_script();
			PUSH_ERROR("MRI style script");
		}
	     mri_script_lines
		{	ldlex_popstate(); 
			POP_ERROR();
		}
	;

mri_script_lines:
		mri_script_lines mri_script_command NEWLINE
          |
	;

mri_script_command:
		CHIP  exp 
	|	CHIP  exp ',' exp
	|	NAME 	{
			einfo("%P%F: unrecognised keyword in MRI style script '%s'\n",$1);
			}
	|	LIST  	{
			write_map = true;
			config.map_filename = "-";
			}
        |       ORDER ordernamelist
	|       ENDWORD 
        |       PUBLIC NAME '=' exp 
 			{ mri_public($2, $4); }
        |       PUBLIC NAME ',' exp 
 			{ mri_public($2, $4); }
        |       PUBLIC NAME  exp 
 			{ mri_public($2, $3); }
	| 	FORMAT NAME
			{ mri_format($2); }
	|	SECT NAME ',' exp 
			{ mri_output_section($2, $4);}
	|	SECT NAME  exp
			{ mri_output_section($2, $3);}
	|	SECT NAME '=' exp
			{ mri_output_section($2, $4);}
	|	ALIGN_K NAME '=' exp
			{ mri_align($2,$4); }
	|	ALIGNMOD NAME '=' exp
			{ mri_alignmod($2,$4); }
	|	ABSOLUTE mri_abs_name_list
	|	LOAD	 mri_load_name_list
	|       NAMEWORD NAME 
			{ mri_name($2); }   
	|	ALIAS NAME ',' NAME
			{ mri_alias($2,$4,0);}
	|	ALIAS NAME ',' INT
			{ mri_alias($2,0,$4);}
	|	BASE     exp
			{ mri_base($2); }
        |       TRUNCATE INT
		{  mri_truncate($2); }
        |
	;

ordernamelist:
	      ordernamelist ',' NAME         { mri_order($3); }
	|     ordernamelist  NAME         { mri_order($2); }
      	|
	;

mri_load_name_list:
		NAME
			{ mri_load($1); }
	|	mri_load_name_list ',' NAME { mri_load($3); }
	;

mri_abs_name_list:
 		NAME
 			{ mri_only_load($1); }
	|	mri_abs_name_list ','  NAME
 			{ mri_only_load($3); }
	;

script_file:
	{
	 ldlex_both();
	}
       ifile_list
	{
	ldlex_popstate();
	}
        ;


ifile_list:
       ifile_list ifile_p1
        |
	;



ifile_p1:
		memory
	|	sections
	|	startup
	|	high_level_library
	|	low_level_library
	|	floating_point_support
	|	statement_anywhere
        |	 ';'
	|	TARGET_K '(' NAME ')'
		{ lang_add_target($3); }
	|	SEARCH_DIR '(' filename ')'
		{ ldfile_add_library_path($3); }
	|	OUTPUT '(' filename ')'
		{ lang_add_output($3); }
        | OUTPUT_FORMAT '(' NAME ')'
		  { lang_add_output_format($3); }
        | OUTPUT_ARCH '(' NAME ')'
		  { ldfile_set_output_arch($3); }
	|	FORCE_COMMON_ALLOCATION
		{ command_line.force_common_definition = true ; }
	|	INPUT '(' input_list ')'
     	|	MAP '(' filename ')'
		{ lang_add_map($3); }
	|	INCLUDE filename 
		{ ldfile_open_command_file($2); } ifile_list END
	;

input_list:
		NAME
		{ lang_add_input_file($1,lang_input_file_is_file_enum,
				 (char *)NULL); }
	|	input_list ',' NAME
		{ lang_add_input_file($3,lang_input_file_is_file_enum,
				 (char *)NULL); }
	|	input_list NAME
		{ lang_add_input_file($2,
lang_input_file_is_file_enum,
				 (char *)NULL); }
	;

sections:
		SECTIONS '{' sec_or_group_p1 '}'
	;

sec_or_group_p1:
		sec_or_group_p1 section
	|	sec_or_group_p1 statement_anywhere
	|
	;

statement_anywhere:
		ENTRY '(' NAME ')'
		{ lang_add_entry($3); }
	|	assignment end
	;

file_NAME_list:
		NAME
			{ lang_add_wild($1, current_file); }
	|	file_NAME_list opt_comma NAME
			{ lang_add_wild($3, current_file); }
	;

input_section_spec:
		NAME
		{
		lang_add_wild((char *)NULL, $1);
		}
        |	'['
			{
			current_file = (char *)NULL;
			}
			file_NAME_list
		']'
	|	NAME
			{
			current_file =$1;
			}
		'(' file_NAME_list ')'
	|	'*'
			{
			current_file = (char *)NULL;
			}
		'(' file_NAME_list ')'
	;

statement:
	  	assignment end
	|	CREATE_OBJECT_SYMBOLS
		{
 		lang_add_attribute(lang_object_symbols_statement_enum); 
	      	}
        |	';'
        |	CONSTRUCTORS
		{
 		
		  lang_add_attribute(lang_constructors_statement_enum); 
		}
	| input_section_spec
        | length '(' exp ')'
        	        {
			lang_add_data($1,$3);
			}
  
	| FILL '(' exp ')'
			{
			  lang_add_fill
			    (exp_get_value_int($3,
					       0,
					       "fill value",
					       lang_first_phase_enum));
			}
	;

statement_list:
		statement_list statement
  	|  	statement
	;
  
length:
		LONG
			{ $$ = $1; }
	| 	SHORT
			{ $$ = $1; }
	|	BYTE
			{ $$ = $1; }
	;

fill_opt:
          '=' mustbe_exp
		{
		  $$ =	 exp_get_value_int($2,
					   0,
					   "fill value",
					   lang_first_phase_enum);
		}
	| 	{ $$ = 0; }
	;

		

assign_op:
		PLUSEQ
			{ $$ = '+'; }
	|	MINUSEQ
			{ $$ = '-'; }
	| 	MULTEQ
			{ $$ = '*'; }
	| 	DIVEQ
			{ $$ = '/'; }
	| 	LSHIFTEQ
			{ $$ = LSHIFT; }
	| 	RSHIFTEQ
			{ $$ = RSHIFT; }
	| 	ANDEQ
			{ $$ = '&'; }
	| 	OREQ
			{ $$ = '|'; }

	;

end:	';' | ','
	;


assignment:
		NAME '=' mustbe_exp
		{
		  lang_add_assignment(exp_assop($2,$1,$3));
		}
	|	NAME assign_op mustbe_exp
		{
		
lang_add_assignment(exp_assop('=',$1,exp_binop($2,exp_nameop(NAME,$1),$3)));
		}
		
	;


opt_comma:
		','	|	;


memory:
		MEMORY '{' memory_spec memory_spec_list '}'
	;

memory_spec_list:
		memory_spec_list memory_spec
	|	memory_spec_list ',' memory_spec
	|
	;


memory_spec: 		NAME
			{ region = lang_memory_region_lookup($1); }
		attributes_opt ':'
		origin_spec opt_comma length_spec

	; origin_spec:
	ORIGIN '=' mustbe_exp
		{ region->current =
		 region->origin =
		 exp_get_vma($3, 0L,"origin", lang_first_phase_enum);
}
	; length_spec:
             LENGTH '=' mustbe_exp
               { region->length = exp_get_vma($3,
					       ~((bfd_vma)0),
					       "length",
					       lang_first_phase_enum);
		}
	

attributes_opt:
		  '(' NAME ')'
			{
			lang_set_flags(&region->flags, $2);
			}
	|
  
	;

startup:
	STARTUP '(' filename ')'
		{ lang_startup($3); }
	;

high_level_library:
		HLL '(' high_level_library_NAME_list ')'
	|	HLL '(' ')'
			{ ldemul_hll((char *)NULL); }
	;

high_level_library_NAME_list:
		high_level_library_NAME_list opt_comma filename
			{ ldemul_hll($3); }
	|	filename
			{ ldemul_hll($1); }

	;

low_level_library:
	SYSLIB '(' low_level_library_NAME_list ')'
	; low_level_library_NAME_list:
		low_level_library_NAME_list opt_comma filename
			{ ldemul_syslib($3); }
	|
	;

floating_point_support:
		FLOAT
			{ lang_float(true); }
	|	NOFLOAT
			{ lang_float(false); }
	;
		

mustbe_exp:		 { ldlex_expression(); }
		exp
			 { ldlex_popstate(); $$=$2;}
	;

exp	:
		'-' exp %prec UNARY
			{ $$ = exp_unop('-', $2); }
	|	'(' exp ')'
			{ $$ = $2; }
	|	NEXT '(' exp ')' %prec UNARY
			{ $$ = exp_unop($1,$3); }
	|	'!' exp %prec UNARY
			{ $$ = exp_unop('!', $2); }
	|	'+' exp %prec UNARY
			{ $$ = $2; }
	|	'~' exp %prec UNARY
			{ $$ = exp_unop('~', $2);}

	|	exp '*' exp
			{ $$ = exp_binop('*', $1, $3); }
	|	exp '/' exp
			{ $$ = exp_binop('/', $1, $3); }
	|	exp '%' exp
			{ $$ = exp_binop('%', $1, $3); }
	|	exp '+' exp
			{ $$ = exp_binop('+', $1, $3); }
	|	exp '-' exp
			{ $$ = exp_binop('-' , $1, $3); }
	|	exp LSHIFT exp
			{ $$ = exp_binop(LSHIFT , $1, $3); }
	|	exp RSHIFT exp
			{ $$ = exp_binop(RSHIFT , $1, $3); }
	|	exp EQ exp
			{ $$ = exp_binop(EQ , $1, $3); }
	|	exp NE exp
			{ $$ = exp_binop(NE , $1, $3); }
	|	exp LE exp
			{ $$ = exp_binop(LE , $1, $3); }
  	|	exp GE exp
			{ $$ = exp_binop(GE , $1, $3); }
	|	exp '<' exp
			{ $$ = exp_binop('<' , $1, $3); }
	|	exp '>' exp
			{ $$ = exp_binop('>' , $1, $3); }
	|	exp '&' exp
			{ $$ = exp_binop('&' , $1, $3); }
	|	exp '^' exp
			{ $$ = exp_binop('^' , $1, $3); }
	|	exp '|' exp
			{ $$ = exp_binop('|' , $1, $3); }
	|	exp '?' exp ':' exp
			{ $$ = exp_trinop('?' , $1, $3, $5); }
	|	exp ANDAND exp
			{ $$ = exp_binop(ANDAND , $1, $3); }
	|	exp OROR exp
			{ $$ = exp_binop(OROR , $1, $3); }
	|	DEFINED '(' NAME ')'
			{ $$ = exp_nameop(DEFINED, $3); }
	|	INT
			{ $$ = exp_intop($1); }
        |	SIZEOF_HEADERS
			{ $$ = exp_nameop(SIZEOF_HEADERS,0); }

	|	SIZEOF '(' NAME ')'
			{ $$ = exp_nameop(SIZEOF,$3); }
	|	ADDR '(' NAME ')'
			{ $$ = exp_nameop(ADDR,$3); }
	|	ABSOLUTE '(' exp ')'
			{ $$ = exp_unop(ABSOLUTE, $3); }
	|	ALIGN_K '(' exp ')'
			{ $$ = exp_unop(ALIGN_K,$3); }
	|	NAME
			{ $$ = exp_nameop(NAME,$1); }
	;


opt_at:
		AT '(' exp ')' { $$ = $3; }
	|	{ $$ = 0; }
	;

section:	NAME 		{ ldlex_expression(); }
		opt_exp_with_type 
		opt_at   	{ ldlex_popstate(); }
		'{'
			{
			lang_enter_output_section_statement($1,$3,typebits,0,0,0,$4);
			}
		statement_list 	
 		'}' {ldlex_expression();} fill_opt memspec_opt
		{
		  ldlex_popstate();
		  lang_leave_output_section_statement($11, $12);
		}
opt_comma

	;

type:
	   NOLOAD  { typebits = SEC_NEVER_LOAD; }
	|  DSECT   { typebits = 0; }
	|  COPY    { typebits = 0; }
	|  INFO    { typebits = 0; }
	|  OVERLAY { typebits = 0; }
  	| { typebits = SEC_ALLOC | SEC_LOAD | SEC_HAS_CONTENTS; }
	;


opt_exp_with_type:
		exp ':'			{ $$ = $1; typebits =0;}
	|	exp '(' type ')' ':' 	{ $$ = $1; }
	|	':'			{ $$= (etree_type *)NULL; typebits = 0; }
	|	'(' type ')' ':'	{ $$= (etree_type *)NULL;  }
	;

memspec_opt:
		'>' NAME
		{ $$ = $2; }
	|	{ $$ = "*default*"; }
	;
%%
void
yyerror(arg) 
char *arg;
{ 
  if (error_index> 0  && error_index < ERROR_NAME_MAX)
     einfo("%P%F: %S syntax error in %s\n",error_names[error_index-1]);
  else
     einfo("%P%F: %S syntax error\n");
}
