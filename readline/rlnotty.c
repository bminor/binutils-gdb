#define READLINE_LIBRARY

#if defined (HAVE_CONFIG_H)
#  include <config.h>
#endif

#include <stdio.h>
#include "readline.h"
#include "rlprivate.h"

rl_vintfunc_t *rl_prep_term_function = rl_prep_terminal;
rl_voidfunc_t *rl_deprep_term_function = rl_deprep_terminal;

int
tgetent (buffer, termtype)
     char *buffer;
     char *termtype;
{
  return -1;
}

int
tgetnum (name)
     char *name;
{
  return -1;
}

int
tgetflag (name)
     char *name;
{
  return -1;
}

char *
tgetstr (name, area)
     char *name;
     char **area;
{
  return NULL;
}

int
tputs (string, nlines, outfun)
     char *string;
     int nlines;
     int (*outfun) ();
{
  while (*string)
    outfun (*string++);
}

char *
tgoto (cap, col, row)
     const char *cap;
     int col;
     int row;
{
  return NULL;
}
     
int
_rl_disable_tty_signals ()
{
  return 0;
}

int
_rl_restore_tty_signals ()
{
  return 0;
}

void 
rl_prep_terminal (meta_flag)
     int meta_flag;
{
  readline_echoing_p = 1;
  return;
}

void
rl_deprep_terminal ()
{
  return;
}

int
rl_restart_output (count, key)
     int count;
     int key;
{
  return 0;
}

void
rl_tty_set_default_bindings (kmap)
     Keymap kmap;
{
}
