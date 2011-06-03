/* Blackfin Distributed DMA Engine (DDE) model.

   Copyright (C) 2010-2016 Free Software Foundation, Inc.
   Contributed by Analog Devices, Inc.

   This file is part of simulators.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#include "config.h"

#include "sim-main.h"
#include "devices.h"
#include "hw-device.h"
#include "dv-bfin_dde.h"

/* Note: This DMA implementation requires the producer to be the master when
         the peer is MDMA.  The source is always a slave.  This way we don't
         have the two DMA devices thrashing each other with one trying to
         write and the other trying to read.  */

struct bfin_dde
{
  /* This top portion matches common dv_bfin struct.  */
  bu32 base;
  struct hw *dma_master;
  bool acked;

  struct hw_event *handler;
  unsigned psize, msize;
  struct hw *peer;

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 descpnext;
  bu32 addrstart;
  bu32 config;
  bu32 xcount;
  bu32 xmodify;
  bu32 ycount;
  bu32 ymodify;
  bu32 _pad0[2];
  bu32 descpcurr;
  bu32 descpprev;
  bu32 addrcurr;
  bu32 status;
  bu32 xcountcurr;
  bu32 ycountcurr;
  bu32 _pad1;
  bu32 bwlcount;
  bu32 bwlcountcur;
  bu32 bwmcount;
  bu32 bwmcountcur;
};
#define mmr_base()      offsetof(struct bfin_dde, descpnext)
#define mmr_offset(mmr) (offsetof(struct bfin_dde, mmr) - mmr_base())
#define mmr_idx(mmr)    (mmr_offset (mmr) / 4)

static const char * const mmr_names[] =
{
  "DESCPNEXT", "ADDRSTART", "CONFIG", "XCOUNT", "XMODIFY", "YCOUNT", "YMODIFY",
  [mmr_idx (descpcurr)] = "DESCPCURR", "DESCPPREV", "ADDRCURR", "STATUS",
  "XCOUNTCURR", "YCOUNTCURR",
  [mmr_idx (bwlcount)] = "BWLCOUNT", "BWLCOUNTCUR", "BWMCOUNT", "BWMCOUNTCUR",
};
#define mmr_name(off) (mmr_names[(off) / 4] ? : "<INV>")

static bool
bfin_dde_enabled (struct bfin_dde *dde)
{
  return (dde->config & DMAEN);
}

static bool
bfin_dde_running (struct bfin_dde *dde)
{
  return (dde->status & DMA_RUN);
}

static void
bfin_dde_process_desc (struct hw *me, struct bfin_dde *dde)
{
  unsigned psize, msize;
  bu8 ndsize = (dde->config & NDSIZE) >> NDSIZE_P;
  bu16 _flows[9], *flows = _flows;

  HW_TRACE ((me, "dma starting up %#x", dde->config));

  /* Figure out transfer size to memory.  */
  dde->msize = (dde->config & MSIZE) >> MSIZE_P;
  if (dde->msize > DDE_MSIZE_MAX)
    hw_abort (me, "DMA config error: invalid MSIZE %#x", dde->msize);
  msize = (dde->status & DMA_MBWIDTH) >> DMA_MBWIDTH_P;
  if (dde->msize > msize)
    hw_abort (me, "DMA config error: MSIZE %#x is larger than hw %#x",
	      dde->msize, msize);
  dde->msize = 1 << dde->msize;

  /* Figure out transfer size to peripheral peer.  */
  dde->psize = (dde->config & PSIZE) >> PSIZE_P;
  if (dde->psize > DDE_PSIZE_MAX)
    hw_abort (me, "DMA config error: invalid PSIZE %#x", dde->psize);
  psize = (dde->status & DMA_PBWIDTH) >> DMA_PBWIDTH_P;
  if (dde->psize > psize)
    hw_abort (me, "DMA config error: PSIZE %#x is larger than hw %#x",
	      dde->psize, psize);
  dde->psize = 1 << dde->psize;

  /* Address has to be mutiple of transfer size.  */
  if (dde->addrstart & (dde->msize - 1))
    dde->status |= DMA_ERR;

  /* Verify 2D sanity.  */
  if ((dde->config & TWOD) && dde->ycount == 0)
    hw_abort (me, "DMA config error: 2D dma requires non-zero ycount");

  if (dde->msize != (unsigned) abs (dde->xmodify))
    hw_abort (me, "DMA config (striding) %#x not supported (xmodify: %d)",
	      dde->config, dde->xmodify);

  switch (dde->config & FLOW)
    {
    case FLOW_AUTO:
    case FLOW_STOP:
      if (ndsize)
	hw_abort (me, "DMA config error: FLOW_{AUTO,STOP} requires NDSIZE_0");
      break;
    case FLOW_DSCL:
    case FLOW_DSCA:
    case FLOW_DSDL:
    case FLOW_DSDA:
      hw_abort (me, "DMA flow (%#x) not supported yet", dde->config & FLOW);
      break;
    default:
      hw_abort (me, "DMA config error: invalid FLOW %#x", dde->config);
    }

  if (ndsize)
    {
#if 0
      bu8 idx;
      bu16 *stores[] = {
	&dde->sal,
	&dde->sah,
	&dde->config,
	&dde->xcount,
	(void *) &dde->xmodify,
	&dde->ycount,
	(void *) &dde->ymodify,
      };

      switch (dde->config & FLOW)
	{
	case DMAFLOW_LARGE:
	  dde->ndph = _flows[1];
	  --ndsize;
	  ++flows;
	case DMAFLOW_SMALL:
	  dde->ndpl = _flows[0];
	  --ndsize;
	  ++flows;
	  break;
	}

      for (idx = 0; idx < ndsize; ++idx)
	*stores[idx] = flows[idx];
#endif
    }

  dde->descpcurr = dde->descpnext;
  dde->addrcurr = dde->addrstart;
  dde->xcountcurr = dde->xcount;
  dde->ycountcurr = dde->ycount;
}

static int
bfin_dde_finish_x (struct hw *me, struct bfin_dde *dde)
{
  /* XXX: This would be the time to process the next descriptor.  */
  /* XXX: Should this toggle Enable in dde->config ?  */

  if ((dde->config & INT) == INT_X)
    hw_port_event (me, 0, 1);

  if (dde->config & TWOD)
    {
      if (dde->ycountcurr > 1)
	{
	  dde->ycountcurr -= 1;
	  dde->xcountcurr = dde->xcount;

	  /* With 2D, last X transfer does not modify addrcurr.  */
	  dde->addrcurr = dde->addrcurr - dde->xmodify + dde->ymodify;

	  return 1;
	}
      else if ((dde->config & INT) == INT_Y)
	hw_port_event (me, 0, 1);
    }

  switch (dde->config & FLOW)
    {
    case FLOW_STOP:
      HW_TRACE ((me, "dma is complete"));
      dde->status = (dde->status & ~DMA_RUN) | DMA_DONE;
      return 0;
    default:
      bfin_dde_process_desc (me, dde);
      return 1;
    }
}

static void bfin_dde_hw_event_callback (struct hw *, void *);

static void
bfin_dde_reschedule (struct hw *me, unsigned delay)
{
  struct bfin_dde *dde = hw_data (me);
  if (dde->handler)
    {
      hw_event_queue_deschedule (me, dde->handler);
      dde->handler = NULL;
    }
  if (!delay)
    return;
  HW_TRACE ((me, "scheduling next process in %u", delay));
  dde->handler = hw_event_queue_schedule (me, delay,
					  bfin_dde_hw_event_callback, dde);
}

/* Chew through the DMA over and over.  */
static void
bfin_dde_hw_event_callback (struct hw *me, void *data)
{
  struct bfin_dde *dde = data;
  struct hw *peer;
  struct dv_bfin *bfin_peer;
  bu8 buf[4096];
  unsigned ret, nr_bytes, ele_count, ele_size;

  dde->handler = NULL;
  ret = 0;

  peer = dde->peer;
  if (!peer)
    hw_abort (me, "unable to locate peer");
  bfin_peer = hw_data (peer);

  /* XXX: This isn't exactly right, but close enough.  */
  ele_size = (dde->config & WNR) ? dde->psize : dde->msize;
  if (dde->xmodify < 0)
    /* XXX: This sucks performance wise.  */
    nr_bytes = ele_size;
  else
    nr_bytes = min (sizeof (buf), dde->xcountcurr * ele_size);

  /* Pumping a chunk!  */
  bfin_peer->dma_master = me;
  bfin_peer->acked = false;
  if (dde->config & WNR)
    {
      HW_TRACE ((me, "dma transfer to 0x%08lx length %u",
		 (unsigned long) dde->addrcurr, nr_bytes));

      ret = hw_dma_read_buffer (peer, buf, 0, dde->addrcurr, nr_bytes);
      /* Has the DMA stalled ?  abort for now.  */
      if (ret == 0)
	goto reschedule;
      /* XXX: How to handle partial DMA transfers ?  */
      if (ret % ele_size)
	goto error;
      ret = sim_write (hw_system (me), dde->addrcurr, buf, ret);
    }
  else
    {
      HW_TRACE ((me, "dma transfer from 0x%08lx length %u",
		 (unsigned long) dde->addrcurr, nr_bytes));

      ret = sim_read (hw_system (me), dde->addrcurr, buf, nr_bytes);
      if (ret == 0)
	goto reschedule;
      /* XXX: How to handle partial DMA transfers ?  */
      if (ret % ele_size)
	goto error;
      ret = hw_dma_write_buffer (peer, buf, 0, dde->addrcurr, ret, 0);
      if (ret == 0)
	goto reschedule;
    }

  /* Ignore partial writes.  */
  ele_count = ret / ele_size;
  dde->addrcurr += ele_count * dde->xmodify;
  dde->xcountcurr -= ele_count;

  if ((!dde->acked && dde->xcountcurr) || bfin_dde_finish_x (me, dde))
    /* Still got work to do, so schedule again.  */
 reschedule:
    bfin_dde_reschedule (me, ret ? 1 : 5000);

  return;

 error:
  /* Don't reschedule on errors ...  */
  dde->status |= DMA_ERR;
}

static unsigned
bfin_dde_io_write_buffer (struct hw *me, const void *source, int space,
			  address_word addr, unsigned nr_bytes)
{
  struct bfin_dde *dde = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);
  mmr_off = addr % dde->base;
  valuep = (void *)((unsigned long)dde + mmr_base() + mmr_off);

  HW_TRACE_WRITE ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, true);

  /* XXX: All registers are RO when DMA is enabled (except IRQ_STATUS).
          But does the HW discard writes or send up IVGHW ?  The sim
          simply discards atm ... */
  switch (mmr_off)
    {
    case mmr_offset(descpnext):
    case mmr_offset(addrstart):
    case mmr_offset(descpcurr):
    case mmr_offset(addrcurr):
      if (!bfin_dde_running (dde))
	*valuep = value;
      else
	HW_TRACE ((me, "discarding write while dma running"));
      break;
    case mmr_offset(xcount):
    case mmr_offset(xmodify):
    case mmr_offset(ycount):
    case mmr_offset(ymodify):
      if (!bfin_dde_running (dde))
	*valuep = value;
      break;
    case mmr_offset(config):
      /* XXX: How to handle updating CONFIG of a running channel ?  */
      *valuep = value;

      if (bfin_dde_enabled (dde))
	{
	  dde->status |= DMA_RUN;
	  bfin_dde_process_desc (me, dde);
	  /* The writer is the master.  */
	  if (dde->config & WNR)
	    bfin_dde_reschedule (me, 1);
	}
      else
	{
	  dde->status &= ~DMA_RUN;
	  bfin_dde_reschedule (me, 0);
	}
      break;
    case mmr_offset(status):
      dv_w1c_4 (valuep, value, DMA_DONE | DMA_ERR | DMA_PIRQ);
      break;
    case mmr_offset(xcountcurr):
    case mmr_offset(ycountcurr):
      if (!bfin_dde_running (dde))
	*valuep = value;
      else
	HW_TRACE ((me, "discarding write while dma running"));
      break;
    default:
      /* XXX: The HW lets the pad regions be read/written ...  */
      dv_bfin_mmr_invalid (me, addr, nr_bytes, true);
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_dde_io_read_buffer (struct hw *me, void *dest, int space,
			 address_word addr, unsigned nr_bytes)
{
  struct bfin_dde *dde = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr % dde->base;
  valuep = (void *)((unsigned long)dde + mmr_base() + mmr_off);

  HW_TRACE_READ ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, false);

  switch (mmr_off)
    {
    case mmr_offset(descpnext):
    case mmr_offset(addrstart):
    case mmr_offset(config):
    case mmr_offset(xcount):
    case mmr_offset(xmodify):
    case mmr_offset(ycount):
    case mmr_offset(ymodify):
    case mmr_offset(descpcurr):
    case mmr_offset(descpprev):
    case mmr_offset(addrcurr):
    case mmr_offset(status):
    case mmr_offset(xcountcurr):
    case mmr_offset(ycountcurr):
    case mmr_offset(bwlcount):
    case mmr_offset(bwlcountcur):
    case mmr_offset(bwmcount):
    case mmr_offset(bwmcountcur):
      dv_store_4 (dest, *valuep);
      break;
    default:
      /* XXX: The HW lets the pad regions be read/written ...  */
      dv_bfin_mmr_invalid (me, addr, nr_bytes, true);
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_dde_dma_read_buffer (struct hw *me, void *dest, int space,
			  unsigned_word addr, unsigned nr_bytes)
{
  struct bfin_dde *dde = hw_data (me);
  unsigned ret, ele_count;

  HW_TRACE_DMA_READ ();

  /* If someone is trying to read from me, I have to be enabled.  */
  if (!bfin_dde_enabled (dde) && !bfin_dde_running (dde))
    return 0;

  /* XXX: handle xmodify ...  */
  ret = sim_read (hw_system (me), dde->addrcurr, dest, nr_bytes);
  /* Ignore partial writes.  */
  ele_count = ret / dde->msize;
  /* Has the DMA stalled ?  abort for now.  */
  if (!ele_count)
    return 0;

  dde->addrcurr += ele_count * dde->xmodify;
  dde->xcountcurr -= ele_count;

  if (dde->xcountcurr == 0)
    bfin_dde_finish_x (me, dde);

  return ret;
}

static unsigned
bfin_dde_dma_write_buffer (struct hw *me, const void *source,
			   int space, unsigned_word addr,
			   unsigned nr_bytes,
			   int violate_read_only_section)
{
  struct bfin_dde *dde = hw_data (me);
  unsigned ret, ele_count;

  HW_TRACE_DMA_WRITE ();

  /* If someone is trying to write to me, I have to be enabled.  */
  if (!bfin_dde_enabled (dde) && !bfin_dde_running (dde))
    return 0;

  /* XXX: handle xmodify ...  */
  ret = sim_write (hw_system (me), dde->addrcurr, source, nr_bytes);
  /* Ignore partial writes.  */
  ele_count = ret / dde->psize;
  /* Has the DMA stalled ?  abort for now.  */
  if (!ele_count)
    return 0;

  dde->addrcurr += ele_count * dde->xmodify;
  dde->xcountcurr -= ele_count;

  if (dde->xcountcurr == 0)
    bfin_dde_finish_x (me, dde);

  return ret;
}

static const struct hw_port_descriptor bfin_dde_ports[] =
{
  { "di", 0, 0, output_port, }, /* DMA Interrupt */
  { "pi", 0, 0, input_port, }, /* Peripheral Interrupt */
  { NULL, 0, 0, 0, },
};

static void
bfin_dde_port_event (struct hw *me, int my_port, struct hw *source,
		     int source_port, int level)
{
  struct bfin_dde *dde = hw_data (me);

  /* Have the DMA channel raise its interrupt to the SEC.  */
  if ((dde->config & INT) == INT_PER)
    hw_port_event (me, 0, 1);
}

static void
attach_bfin_dde_regs (struct hw *me, struct bfin_dde *dde)
{
  address_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;
  const char *peer;
  char path[100];
  unsigned mwidth, pwidth;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (hw_find_property (me, "peer") == NULL)
    hw_abort (me, "Missing \"peer\" property");

  if (hw_find_property (me, "mwidth") == NULL)
    hw_abort (me, "Missing \"mwidth\" property");

  if (hw_find_property (me, "pwidth") == NULL)
    hw_abort (me, "Missing \"pwidth\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain three addr/size entries");

  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &attach_space, &attach_address, me);
  hw_unit_size_to_attach_size (hw_parent (me), &reg.size, &attach_size, me);

  if (attach_size != BFIN_MMR_DDE_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_DDE_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  dde->base = attach_address;

  peer = hw_find_string_property (me, "peer");
  sprintf (path, "/core/%s", peer);
  dde->peer = hw_tree_find_device (me, path);

  mwidth = hw_find_integer_property (me, "mwidth");
  if (mwidth > DDE_MWIDTH_MAX)
    hw_abort (me, "\"mwidth\" must be between 0 and %i inclusive",
	      DDE_MWIDTH_MAX);
  dde->status |= mwidth << DMA_MBWIDTH_P;

  pwidth = hw_find_integer_property (me, "pwidth");
  if (pwidth > DDE_PWIDTH_MAX)
    hw_abort (me, "\"pwidth\" must be between 0 and %i inclusive",
	      DDE_PWIDTH_MAX);
  dde->status |= pwidth << DMA_PBWIDTH_P;
}

static void
bfin_dde_finish (struct hw *me)
{
  struct bfin_dde *dde;

  dde = HW_ZALLOC (me, struct bfin_dde);

  set_hw_data (me, dde);
  set_hw_io_read_buffer (me, bfin_dde_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_dde_io_write_buffer);
  set_hw_dma_read_buffer (me, bfin_dde_dma_read_buffer);
  set_hw_dma_write_buffer (me, bfin_dde_dma_write_buffer);
  set_hw_ports (me, bfin_dde_ports);
  set_hw_port_event (me, bfin_dde_port_event);

  attach_bfin_dde_regs (me, dde);
}

const struct hw_descriptor dv_bfin_dde_descriptor[] =
{
  {"bfin_dde", bfin_dde_finish,},
  {NULL, NULL},
};
