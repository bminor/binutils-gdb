

typedef struct {
  asymbol symbol;
} oasys_symbol_type;

typedef struct oasys_reloc_struct {
  arelent relent;
  struct oasys_reloc_struct *next;
  unsigned int symbol;
} oasys_reloc_type;


#define oasys_symbol(x) ((oasys_symbol_type *)(x))
#define oasys_per_section(x) ((oasys_per_section_type *)(x->used_by_bfd))
typedef struct oasys_per_section_struct
{
  asection *section;
  bfd_byte *data;
  bfd_vma offset;

  oasys_reloc_type **reloc_tail_ptr;
  bfd_vma pc;
  /* For output */
  struct obstack reloc_obstack;
  file_ptr current_pos;
  unsigned int current_byte;
  boolean initialized;
} oasys_per_section_type;

#define NSECTIONS 10

  



typedef struct {
  file_ptr file_offset;
  bfd *abfd;
} oasys_ar_obstack_type;


typedef struct {
  file_ptr pos;
  unsigned int size;
  bfd *abfd;
char *name;

} oasys_module_info_type;

typedef struct {
  oasys_module_info_type *module;
  unsigned int module_count;
  unsigned int module_index;
} oasys_ar_data_type;

typedef struct {

  char *strings;
  asymbol *symbols;
  unsigned int symbol_string_length;
  asection *sections[OASYS_MAX_SEC_COUNT];
  file_ptr first_data_record;
} oasys_data_type;

#define oasys_data(abfd) ((oasys_data_type *)((abfd)->tdata))
#define oasys_ar_data(abfd) ((oasys_ar_data_type *)((abfd)->tdata))



