#ifndef EMUL_DEFS
#define EMUL_DEFS

struct emulation {
  void (*match) PARAMS ((const char *));
  const char *name;
  void (*init) ();
  const char *(*bfd_name) ();
  unsigned char local_labels_fb : 1;
  unsigned char local_labels_dollar : 1;
  unsigned char leading_underscore : 2;
  unsigned char strip_underscore : 1;
  unsigned char default_endian : 2;
  const char *fake_label_name;
  const struct format_ops *format;
};

COMMON struct emulation *this_emulation;

extern const char *default_emul_bfd_name ();
extern void common_emul_init ();

#endif
