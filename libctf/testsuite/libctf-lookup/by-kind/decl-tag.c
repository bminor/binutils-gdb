int foo __attribute__((btf_decl_tag("dtag")));

struct
{
  char a;
  char b __attribute__((btf_decl_tag("dtag2")));
} bar __attribute__((used));
