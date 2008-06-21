/*
 * gdbfreeplay -- interface
 */

#ifndef GDBFREEPLAY_H
#define GDBFREEPLAY_H

#include "remote-breakpoint.h"

extern int verbose;

extern enum successcode gdbfreeplay_open (char *filename);
extern void gdbfreeplay (int socket_fd);

extern unsigned long target_pc_from_T (char *tpacket);
extern unsigned long target_pc_from_G (char *gpacket);
extern unsigned long target_pc_from_g (FILE *infile);

extern int hex_to_int (int ch);

#endif
