/************************************************************
(C) Copyright 1987-1992
Lynx Real-Time Systems, Inc.
Los Gatos, CA
All rights reserved.

$Date$
$Revision$

************************************************************/

#ifndef THREAD_H
#define THREAD_H

extern void init_thread_list PARAMS ((void));

extern void add_thread PARAMS ((int));

extern int in_thread_list PARAMS ((int));

#if 0
#ifdef __STDC__
struct _bfd;
#endif

extern void bfd_get_core_threads PARAMS ((struct _bfd *));
#endif

#endif	/* THREAD_H */
