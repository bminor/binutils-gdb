typedef enum 
{
  
  bfd_indirect_seclet,
  
} bfd_seclet_enum_type;


struct bfd_seclet_struct 
{
  struct bfd_seclet_struct *next;
  bfd_seclet_enum_type type;
  unsigned int offset;  
  unsigned int size;
  union 
  {
    struct 
    {
      asection *section;
      asymbol **symbols;
      
    } indirect;
  }
  u;
};

typedef struct bfd_seclet_struct bfd_seclet_type;

bfd_seclet_type *EXFUN(bfd_new_seclet,(bfd*,asection*));
