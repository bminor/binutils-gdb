/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "gpuif.h"
#include "dma.h"
#include "pke0.h"
#include "pke1.h"
#include "vu0.h"
#include "vu1.h"

#include "hardware.h"

void 
register_devices(SIM_DESC sd)
{
  /* Attach a bunch of devices... */
  gpuif_attach(sd);
  dma_attach(sd);
  pke0_attach(sd);
  vu0_attach(sd);
  vu1_attach(sd);
  pke1_attach(sd);
}
