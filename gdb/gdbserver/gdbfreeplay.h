/*
 * gdbfreeplay -- interface
 */

#ifndef GDBFREEPLAY_H
#define GDBFREEPLAY_H

#include "remote-breakpoint.h"

extern int verbose;

extern enum successcode gdbfreeplay_open (char *filename);
extern void gdbfreeplay (int socket_fd);

extern unsigned long long target_pc_from_T (char *tpacket);
extern unsigned long long target_pc_from_G (char *gpacket);
extern unsigned long long target_pc_from_g (char *gpacket);

extern char *target_compose_T_packet (char *origTpacket, 
				      unsigned long long pc,
				      int breakpoint_p);
extern char *target_compose_g_packet (char *origTpacket);

extern int hex_to_int (int ch);

#endif
