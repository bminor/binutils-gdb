typedef struct {
  unsigned int index:24;
  char letter;
} ieee_symbol_index_type;

typedef struct ieee_symbol {
  asymbol symbol;
  struct ieee_symbol *next;

unsigned int index;
} ieee_symbol_type;


typedef struct ieee_reloc {
  arelent relent;
  struct ieee_reloc *next;
  ieee_symbol_index_type symbol;

} ieee_reloc_type;

#define ieee_symbol(x) ((ieee_symbol_type *)(x))

typedef struct ieee_per_section
{
  asection *section;
  bfd_byte *data;
  bfd_vma offset;
  bfd_vma pc;
  /* For output */
  file_ptr current_pos;
  unsigned int current_byte;
  boolean initialized;
  ieee_reloc_type **reloc_tail_ptr;
} ieee_per_section_type;

#define ieee_per_section(x) ((ieee_per_section_type *)((x)->used_by_bfd))
#define NSECTIONS 10

  

typedef struct {
  boolean read_symbols;
  boolean read_data;	
unsigned  char *input_p;
unsigned char *first_byte;
  file_ptr output_cursor;
  /* Map of section indexes to section ptrs */
  asection * section_table[NSECTIONS];
  ieee_address_descriptor_type ad;
  ieee_module_begin_type mb;
  ieee_w_variable_type w;

  unsigned int section_count;
  struct obstack ieee_obstack;


  unsigned int map_idx;
  /* List of GLOBAL EXPORT symbols */
  ieee_symbol_type *external_symbols;
  /* List of UNDEFINED symbols */
  ieee_symbol_type *external_reference;

  /* When the symbols have been canonicalized, they are in a
   * special order, we remember various bases here.. */
  unsigned int external_symbol_max_index;
  unsigned int external_symbol_min_index;
  unsigned int external_symbol_count;
  int external_symbol_base_offset;

  unsigned int external_reference_max_index;
  unsigned int external_reference_min_index;
  unsigned int external_reference_count;
  int external_reference_base_offset;


  boolean symbol_table_full;
} ieee_data_type;

typedef struct {
  file_ptr file_offset;
  bfd *abfd;
} ieee_ar_obstack_type;

typedef struct {
  ieee_ar_obstack_type *elements;
  struct  obstack element_obstack;
  unsigned  int element_index ;
  unsigned int element_count;
} ieee_ar_data_type;
#define ieee_data(abfd) ((ieee_data_type *)((abfd)->tdata))
#define ieee_ar_data(abfd) ((ieee_ar_data_type *)((abfd)->arelt_data))


#define ptr(abfd) (ieee_data(abfd)->input_p)
