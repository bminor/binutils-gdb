/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*      VPE1 simulator (part of VU1) global variables                       */
/*                                                                          */
/****************************************************************************/

#ifndef LIBVPE_H_
#define LIBVPE_H_

extern unsigned long _ITOP;
extern unsigned long _TOP;
extern unsigned long _vpepc;
extern int	_is_dbg;
extern int	_is_verb;
extern int	_is_dump;
extern int	_pgpuif;
extern FILE *_fp_gpu;

extern int _GIF_SIM_OFF;
extern int _GIF_BUSY;
extern int _GIF_VUCALL;
extern int _GIF_VUADDR;

#include "vu.h"

void initvpe(VectorUnitState *state);

void vpecallms_init(VectorUnitState *state);
void vpecallms_cycle(VectorUnitState* state);


#endif
