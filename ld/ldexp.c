/* This module handles expression trees.
Copyright (C) 1991 Free Software Foundation, Inc.
Written by Steve Chamberlain of Cygnus Support (sac@cygnus.com).

This file is part of GLD, the Gnu Linker.

GLD is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GLD is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GLD; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

/*
This module is in charge of working out the contents of expressions.

It has to keep track of the relative/absness of a symbol etc. This is
done by keeping all values in a struct (an etree_value_type) which
contains a value, a section to which it is relative and a valid bit.

*/


#include "bfd.h"
#include "sysdep.h"

#include "ld.h"
#include "ldsym.h"
#include "ldmain.h"
#include "ldmisc.h"
#include "ldexp.h"
#include "ldgram.h"
#include "ldlang.h"

static void
exp_print_token (code)
     token_code_type code;
{
  static CONST struct  {
    token_code_type code;
    char *name;
  } table[] =
      {
	INT,	"int",
	NAME,"NAME",
	PLUSEQ,"+=",
	MINUSEQ,"-=",
	MULTEQ,"*=",
	DIVEQ,"/=",
	LSHIFTEQ,"<<=",
	RSHIFTEQ,">>=",
	ANDEQ,"&=",
	OREQ,"|=",
	OROR,"||",
	ANDAND,"&&",
	EQ,"==",
	NE,"!=",
	LE,"<=",
	GE,">=",
	LSHIFT,"<<",
	RSHIFT,">>=",
	ALIGN_K,"ALIGN",
	BLOCK,"BLOCK",
	SECTIONS,"SECTIONS",
	SIZEOF_HEADERS,"SIZEOF_HEADERS",
	NEXT,"NEXT",
	SIZEOF,"SIZEOF",
	ADDR,"ADDR",
	MEMORY,"MEMORY",





	DEFINED,"DEFINED",
	TARGET_K,"TARGET",
	SEARCH_DIR,"SEARCH_DIR",
	MAP,"MAP",
	LONG,"LONG",
	SHORT,"SHORT",
	BYTE,"BYTE",
	ENTRY,"ENTRY",
	0,(char *)NULL} ;



  unsigned int idx;
  for (idx = 0; table[idx].name != (char*)NULL; idx++) {
    if (table[idx].code == code) {
      fprintf(config.map_file, "%s", table[idx].name);
      return;
    }
  }
  /* Not in table, just print it alone */
  fprintf(config.map_file, "%c",code);
}

static void 
make_abs (ptr)
     etree_value_type *ptr;
{
    asection *s = ptr->section->bfd_section;
    ptr->value += s->vma;
    ptr->section = abs_output_section;
}

static etree_value_type
new_abs (value)
     bfd_vma value;
{
  etree_value_type new;
  new.valid = true;
  new.section = abs_output_section;
  new.value = value;
  return new;
}

static void 
check (os, name, op)
     lang_output_section_statement_type *os;
     CONST char *name;
     CONST char *op;
{
  if (os == (lang_output_section_statement_type *)NULL) {
    einfo("%F%P: %s uses undefined section %s\n", op, name);
  }
  if (os->processed == false) {
    einfo("%F%P: %s forward reference of section %s\n",op, name);
  }
}

etree_type *
exp_intop (value)
     bfd_vma value;
{
  etree_type *new = (etree_type *)stat_alloc((bfd_size_type)(sizeof(new->value)));
  new->type.node_code = INT;
  new->value.value = value;
  new->type.node_class = etree_value;
  return new;

}


static etree_value_type
new_rel (value, section)
     bfd_vma value;
     lang_output_section_statement_type *section;
{
  etree_value_type new;
  new.valid = true;
  new.value = value;
  new.section = section;
  return new;
}

static etree_value_type
new_rel_from_section (value, section)
     bfd_vma value;
     lang_output_section_statement_type *section;
{
  etree_value_type new;
  new.valid = true;
  new.value = value;
  new.section = section;

    new.value -= section->bfd_section->vma;

  return new;
}

static etree_value_type 
fold_binary (tree, current_section, allocation_done, dot, dotp)
     etree_type *tree;
     lang_output_section_statement_type *current_section;
     lang_phase_type  allocation_done;
     bfd_vma dot;
     bfd_vma *dotp;
{
  etree_value_type result;

  result =  exp_fold_tree(tree->binary.lhs,  current_section,
			  allocation_done, dot, dotp);
  if (result.valid) {
    etree_value_type other;
    other = exp_fold_tree(tree->binary.rhs,
			  current_section,
			  allocation_done, dot,dotp) ;
    if (other.valid) {
	/* If values are from different sections, or this is an */
	/* absolute expression, make both source args absolute */
      if (result.section !=  other.section ||
	  current_section == abs_output_section) 
      {
	make_abs(&result);
	make_abs(&other);
      }
	  
      switch (tree->type.node_code) 
	{
	case '%':
	  /* Mod,  both absolule*/

	  if (other.value == 0) {
	    einfo("%F%S %% by zero\n");
	  }
	  result.value = (int)result.value % (int)other.value;
	  break;
	case '/':
	  if (other.value == 0) {
	    einfo("%F%S / by zero\n");
	  }
	  result.value = (int)result.value / (int) other.value;
	  break;
#define BOP(x,y) case x : result.value = result.value y other.value;break;
	  BOP('+',+);
	  BOP('*',*);
	  BOP('-',-);
	  BOP(LSHIFT,<<);
	  BOP(RSHIFT,>>);
	  BOP(EQ,==);
	  BOP(NE,!=);
	  BOP('<',<);
	  BOP('>',>);
	  BOP(LE,<=);
	  BOP(GE,>=);
	  BOP('&',&);
	  BOP('^',^);
	  BOP('|',|);
	  BOP(ANDAND,&&);
	  BOP(OROR,||);
	default:
	  FAIL();
	}
    }
    else {
      result.valid = false;
    }
  }
  return result;
}
etree_value_type 
invalid ()
{
  etree_value_type new;
  new.valid = false;
  return new;
}

etree_value_type 
fold_name (tree, current_section, allocation_done, dot)
     etree_type *tree;
     lang_output_section_statement_type *current_section;
     lang_phase_type  allocation_done;
     bfd_vma dot;
{
  etree_value_type result;
  switch (tree->type.node_code) 
      {
      case SIZEOF_HEADERS:
	if (allocation_done != lang_first_phase_enum) 
	    {
	      result = new_abs(bfd_sizeof_headers(output_bfd,
						config.relocateable_output));

	    }
	else {
	  result.valid = false;
	}
	break;
      case DEFINED:
	result.value =
	  ldsym_get_soft(tree->name.name) != (ldsym_type *)NULL;
	result.section = 0;
	result.valid = true;
	break;
      case NAME:
	result.valid = false;
	if (tree->name.name[0] == '.' && tree->name.name[1] == 0) {

	  if (allocation_done != lang_first_phase_enum) {
	    result = new_rel_from_section(dot, current_section);
	  }
	  else {
	    result = invalid();
	  }
	}
	else {
	  if (allocation_done == lang_final_phase_enum) {
	    ldsym_type *sy = ldsym_get_soft(tree->name.name);
	  
	    if (sy) {
	      asymbol **sdefp = sy->sdefs_chain;

	      if (sdefp) {
		asymbol *sdef = *sdefp;
#if 0
		if (sdef->section == (asection *)NULL) {
		  /* This is an absolute symbol */
		  result = new_abs(sdef->value);
		}
		else
#endif
 {
		  lang_output_section_statement_type *os =
		    lang_output_section_statement_lookup(
							 sdef->section->output_section->name);
		  /* If the symbol is from a file which we are not
		     relocating (-R) then return an absolute for its
		     value */
		  if (bfd_asymbol_bfd(sdef)->usrdata && 
		      ((lang_input_statement_type*)(bfd_asymbol_bfd(sdef)->usrdata))->just_syms_flag == true) 
		      {
			result = new_abs(sdef->value +sdef->section->vma);

		      }
		  else {
		    result = new_rel(sdef->value + sdef->section->output_offset, os);
		  }
		}
	      }
	    }
	    if (result.valid == false) {
	      einfo("%F%S: undefined symbol `%s' referenced in expression\n",
		   tree->name.name);
	    }

	  }
	}

	break;

      case ADDR:

	if (allocation_done != lang_first_phase_enum) {
	  lang_output_section_statement_type *os =
	    lang_output_section_find(tree->name.name);
	  check(os,tree->name.name,"ADDR");
	  result =    new_rel((bfd_vma)0,  os);
	}
	else {
	  result = invalid();
	}
	break;
      case SIZEOF:
	if(allocation_done != lang_first_phase_enum) {
	  lang_output_section_statement_type *os = 
	    lang_output_section_find(tree->name.name);
	  check(os,tree->name.name,"SIZEOF");
	  result = new_abs((bfd_vma)(os->bfd_section->_raw_size));
	}
	else {
	  result = invalid();
	}
	break;

      default:
	FAIL();
	break;
      }

  return result;
}
etree_value_type 
exp_fold_tree (tree, current_section, allocation_done, dot, dotp)
     etree_type *tree;
     lang_output_section_statement_type *current_section;
     lang_phase_type  allocation_done;
     bfd_vma dot;
     bfd_vma *dotp;
{
  etree_value_type result;

  if (tree == (etree_type *)NULL) {
    result.valid = false;
  }
  else {
    switch (tree->type.node_class) 
    {
     case etree_value:
      result = new_rel(tree->value.value, current_section);
      break;
     case etree_unary:
      result = exp_fold_tree(tree->unary.child,
			     current_section,
			     allocation_done, dot, dotp);
      if (result.valid == true)
      {
	switch(tree->type.node_code) 
	{
	 case ALIGN_K:
	  if (allocation_done != lang_first_phase_enum) {
	    result = new_rel_from_section(ALIGN_N(dot,
						result.value) ,
					  current_section);

	  }
	  else {
	    result.valid = false;
	  }
	  break;
	 case ABSOLUTE:
	  if (allocation_done != lang_first_phase_enum) 
	  {
	    if (current_section 
		== (lang_output_section_statement_type*)NULL) 
	    {
	      /* Outside a section, so it's all ok */

	    }
	    else {
	      /* Inside a section, subtract the base of the section,
		 so when it's added again (in an assignment), everything comes out fine
		 */
	      result.section = abs_output_section;
	      result.value -= current_section->bfd_section->vma;
	      result.valid = true;
	    }
	  }
	  else 
	  {
	    result.valid = false;
	  }

	  break;
	 case '~':
	  make_abs(&result);
	  result.value = ~result.value;
	  break;
	 case '!':
	  make_abs(&result);
	  result.value = !result.value;
	  break;
	 case '-':
	  make_abs(&result);
	  result.value = -result.value;
	  break;
	 case NEXT:
	  if (allocation_done ==lang_allocating_phase_enum) {
	    make_abs(&result);
	    result.value = ALIGN_N(dot, result.value);
	  }
	  else {
	    /* Return next place aligned to value */
	    result.valid = false;
	  }
	  break;
	 default:
	  FAIL();
	}
      }

      break;
     case etree_trinary:

      result = exp_fold_tree(tree->trinary.cond,
			     current_section,
			     allocation_done, dot, dotp);
      if (result.valid) {
	result = exp_fold_tree(result.value ?
			       tree->trinary.lhs:tree->trinary.rhs,
			       current_section,
			       allocation_done, dot, dotp);
      }

      break;
     case etree_binary:
      result = fold_binary(tree, current_section, allocation_done,
			   dot, dotp);
      break;
     case etree_assign:
      if (tree->assign.dst[0] == '.' && tree->assign.dst[1] == 0) {
	/* Assignment to dot can only be done during allocation */
	if (allocation_done == lang_allocating_phase_enum) {
	  result = exp_fold_tree(tree->assign.src,
				 current_section,
				 lang_allocating_phase_enum, dot, dotp);
	  if (result.valid == false) {
	    einfo("%F%S invalid assignment to location counter\n");
	  }
	  else {
	    if (current_section ==
		(lang_output_section_statement_type  *)NULL) {
	      einfo("%F%S assignment to location counter invalid outside of SECTION\n");
	    }
	    else {
	      bfd_vma nextdot =result.value +
	       current_section->bfd_section->vma;
	      if (nextdot < dot) {
		einfo("%F%S cannot move location counter backwards (from %V to %V)\n", dot, nextdot);
	      }
	      else {
		*dotp = nextdot; 
	      }
	    }
	  }
	}
      }
      else {
	ldsym_type *sy = ldsym_get(tree->assign.dst);

	/* If this symbol has just been created then we'll place it into 
	 * a section of our choice
	 */
	result = exp_fold_tree(tree->assign.src,
			       current_section, allocation_done,
			       dot, dotp);
	if (result.valid)
	{
	  asymbol *def;
	  asymbol **def_ptr ;
	  /* Add this definition to script file */
	  if (sy->sdefs_chain) 
	  {
	    def_ptr = sy->sdefs_chain;
	    def = *def_ptr;
		    
	  }
	  else 
	  {
	    def_ptr = (asymbol **)stat_alloc((bfd_size_type)(sizeof(asymbol **)));
	    def = (asymbol   *)bfd_make_empty_symbol(script_file->the_bfd);

		  
	    def->flags = 0;
		
	    sy->sdefs_chain = def_ptr;
	    *def_ptr = def;
	  }

	  def->value = result.value;

	  def->section = result.section->bfd_section;
	  def->flags |= BSF_GLOBAL | BSF_EXPORT;


	  def->udata = (PTR)NULL;
	  def->name = sy->name;

	  if (sy->sdefs_chain == 0)
	   enter_global_ref(def_ptr, sy->name);
	}

      }

  
      break;
     case etree_name:
      result = fold_name(tree, current_section, allocation_done, dot);
      break;
     default:
      einfo("%F%S need more of these %d\n",tree->type.node_class );

    }
  }

  return result;
}


etree_value_type 
exp_fold_tree_no_dot (tree, current_section, allocation_done)
     etree_type *tree;
     lang_output_section_statement_type *current_section;
     lang_phase_type allocation_done;
{
return exp_fold_tree(tree, current_section, allocation_done, (bfd_vma)
		     0, (bfd_vma *)NULL);
}

etree_type *
exp_binop (code, lhs, rhs)
     int code;
     etree_type *lhs;
     etree_type *rhs;
{
  etree_type value, *new;
  etree_value_type r;

  value.type.node_code = code;
  value.binary.lhs = lhs;
  value.binary.rhs = rhs;
  value.type.node_class = etree_binary;
  r = exp_fold_tree_no_dot(&value,
			   abs_output_section,
			   lang_first_phase_enum );
  if (r.valid)
    {
      return exp_intop(r.value);
    }
  new = (etree_type *)stat_alloc((bfd_size_type)(sizeof(new->binary)));
  memcpy((char *)new, (char *)&value, sizeof(new->binary));
  return new;
}

etree_type *
exp_trinop (code, cond, lhs, rhs)
     int code;
     etree_type *cond;
     etree_type *lhs;
     etree_type *rhs;
{
  etree_type value, *new;
  etree_value_type r;
  value.type.node_code = code;
  value.trinary.lhs = lhs;
  value.trinary.cond = cond;
  value.trinary.rhs = rhs;
  value.type.node_class = etree_trinary;
  r= exp_fold_tree_no_dot(&value,  (lang_output_section_statement_type
				    *)NULL,lang_first_phase_enum);
  if (r.valid) {
    return exp_intop(r.value);
  }
  new = (etree_type *)stat_alloc((bfd_size_type)(sizeof(new->trinary)));
  memcpy((char *)new,(char *) &value, sizeof(new->trinary));
  return new;
}


etree_type *
exp_unop (code, child)
     int code;
     etree_type *child;
{
  etree_type value, *new;

  etree_value_type r;
  value.unary.type.node_code = code;
  value.unary.child = child;
  value.unary.type.node_class = etree_unary;
  r = exp_fold_tree_no_dot(&value,abs_output_section,
			   lang_first_phase_enum);
  if (r.valid) {
    return exp_intop(r.value);
  }
  new = (etree_type *)stat_alloc((bfd_size_type)(sizeof(new->unary)));
  memcpy((char *)new, (char *)&value, sizeof(new->unary));
  return new;
}


etree_type *
exp_nameop (code, name)
     int code;
     CONST char *name;
{
  etree_type value, *new;
  etree_value_type r;
  value.name.type.node_code = code;
  value.name.name = name;
  value.name.type.node_class = etree_name;


  r = exp_fold_tree_no_dot(&value,
			   (lang_output_section_statement_type *)NULL,
			   lang_first_phase_enum);
  if (r.valid) {
    return exp_intop(r.value);
  }
  new = (etree_type *)stat_alloc((bfd_size_type)(sizeof(new->name)));
  memcpy((char *)new, (char *)&value, sizeof(new->name));
  return new;

}




etree_type *
exp_assop (code, dst, src)
     int code;
     CONST char *dst;
     etree_type *src;
{
  etree_type value, *new;

  value.assign.type.node_code = code;


  value.assign.src = src;
  value.assign.dst = dst;
  value.assign.type.node_class = etree_assign;

#if 0
  if (exp_fold_tree_no_dot(&value, &result)) {
    return exp_intop(result);
  }
#endif
  new = (etree_type*)stat_alloc((bfd_size_type)(sizeof(new->assign)));
  memcpy((char *)new, (char *)&value, sizeof(new->assign));
  return new;
}

void 
exp_print_tree (tree)
     etree_type *tree;
{
  switch (tree->type.node_class) {
  case etree_value:
    print_address(tree->value.value);
    return;

  case etree_assign:
#if 0
    if (tree->assign.dst->sdefs != (asymbol *)NULL){
      fprintf(config.map_file,"%s (%x) ",tree->assign.dst->name,
	      tree->assign.dst->sdefs->value);
    }
    else {
      fprintf(config.map_file,"%s (UNDEFINED)",tree->assign.dst->name);
    }
#endif
    fprintf(config.map_file,"%s ",tree->assign.dst);
    exp_print_token(tree->type.node_code);
    exp_print_tree(tree->assign.src);
    break;
  case etree_binary:
    fprintf(config.map_file,"(");
    exp_print_tree(tree->binary.lhs);
    exp_print_token(tree->type.node_code);
    exp_print_tree(tree->binary.rhs);
    fprintf(config.map_file,")");
    break;
  case etree_trinary:
    exp_print_tree(tree->trinary.cond);
    fprintf(config.map_file,"?");
    exp_print_tree(tree->trinary.lhs);
    fprintf(config.map_file,":");
    exp_print_tree(tree->trinary.rhs);
    break;
  case etree_unary:
    exp_print_token(tree->unary.type.node_code);
    if (tree->unary.child) 
    {
      
    fprintf(config.map_file,"(");
    exp_print_tree(tree->unary.child);
    fprintf(config.map_file,")");
  }
    
    break;
  case etree_undef:
    fprintf(config.map_file,"????????");
    break;
  case etree_name:
    if (tree->type.node_code == NAME) {
      fprintf(config.map_file,"%s", tree->name.name);
    }
    else {
      exp_print_token(tree->type.node_code);
      if (tree->name.name)
      fprintf(config.map_file,"(%s)", tree->name.name);
    }
    break;
  default:
    FAIL();
    break;
  }
}




bfd_vma
exp_get_vma (tree, def, name, allocation_done)
     etree_type *tree;
      bfd_vma def;
     char *name;
     lang_phase_type allocation_done;
{
  etree_value_type r;

  if (tree != (etree_type *)NULL) {
    r = exp_fold_tree_no_dot(tree,
		 abs_output_section,
		      allocation_done);
    if (r.valid == false && name) {
      einfo("%F%S nonconstant expression for %s\n",name);
    }
    return r.value;
  }
  else {
    return def;
  }
}

int 
exp_get_value_int (tree,def,name, allocation_done)
     etree_type *tree;
     int def;
     char *name;
     lang_phase_type allocation_done;
{
  return (int)exp_get_vma(tree,(bfd_vma)def,name, allocation_done);
}

