/* Blackfin Universal Asynchronous Receiver/Transmitter (UART) model.
   For "newer style" UARTs on BF60x parts.

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
#include "dv-bfin_uart4.h"

/* XXX: Should we bother emulating the TX/RX FIFOs ?  */

/* Internal state needs to be the same as bfin_uart.  */
struct bfin_uart
{
  /* This top portion matches common dv_bfin struct.  */
  bu32 base;
  struct hw *dma_master;
  bool acked;

  struct hw_event *handler;
  char saved_byte;
  int saved_count;

  /* Order after here is important -- matches hardware MMR layout.  */
  bu32 revid;
  bu32 ctl;
  bu32 stat;
  bu32 scr;
  bu32 clk;
  bu32 imsk;
  bu32 imsk_set;
  bu32 imsk_clr;
  bu32 rbr;
  bu32 thr;
  bu32 taip;
  bu32 tsr;
  bu32 rsr;
  bu32 txcnt;
  bu32 rxcnt;
};
#define mmr_base()      offsetof(struct bfin_uart, revid)
#define mmr_offset(mmr) (offsetof(struct bfin_uart, mmr) - mmr_base())

static const char * const mmr_names[] =
{
  "UART_REVID",
  "UART_CTL",
  "UART_STAT",
  "UART_SCR",
  "UART_CLK",
  "UART_IMSK",
  "UART_IMSK_SET",
  "UART_IMSK_CLR",
  "UART_RBR",
  "UART_THR",
  "UART_TAIP",
  "UART_TSR",
  "UART_RSR",
  "UART_TXCNT",
  "UART_RXCNT",
};
#define mmr_name(off) mmr_names[(off) / 4]

static unsigned
bfin_uart_io_write_buffer (struct hw *me, const void *source,
			   int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_uart *uart = hw_data (me);
  bu32 mmr_off;
  bu32 value;
  bu32 *valuep;

  value = dv_load_4 (source);
  mmr_off = addr - uart->base;
  valuep = (void *)((unsigned long)uart + mmr_base() + mmr_off);

  HW_TRACE_WRITE ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, true);

  switch (mmr_off)
    {
    case mmr_offset(thr):
      uart->thr = bfin_uart_write_byte (me, value, 0);
      if (uart->imsk & ETBEI)
	hw_port_event (me, DV_PORT_TX, 1);
      break;
    case mmr_offset(imsk_set):
      uart->imsk |= value;
      break;
    case mmr_offset(imsk_clr):
      dv_w1c_4 (&uart->imsk, value, -1);
      break;
    case mmr_offset(stat):
      dv_w1c_4 (valuep, value, SCTS | ASTKY | TFI | BI | FE | PE | OE);
      break;
    default:
      *valuep = value;
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_uart_io_read_buffer (struct hw *me, void *dest,
			  int space, address_word addr, unsigned nr_bytes)
{
  struct bfin_uart *uart = hw_data (me);
  bu32 mmr_off;
  bu32 *valuep;

  mmr_off = addr - uart->base;
  valuep = (void *)((unsigned long)uart + mmr_base() + mmr_off);

  HW_TRACE_READ ();

  dv_bfin_mmr_require_32 (me, addr, nr_bytes, false);

  switch (mmr_off)
    {
    case mmr_offset(rbr):
      uart->rbr = bfin_uart_get_next_byte (me, uart->rbr, 0, NULL);
      dv_store_4 (dest, uart->rbr);
      break;
    case mmr_offset(stat):
      uart->stat &= ~(DR | THRE | TEMT | RO | CTS | RFCS);
      uart->stat |= bfin_uart_get_status (me, TEMT | THRE);
      dv_store_4 (dest, *valuep);
      break;
    case mmr_offset(imsk):
    case mmr_offset(imsk_set):
    case mmr_offset(imsk_clr):
      dv_store_4 (dest, uart->imsk);
      break;
    default:
      dv_store_4 (dest, *valuep);
      break;
    }

  return nr_bytes;
}

static unsigned
bfin_uart_dma_read_buffer (struct hw *me, void *dest, int space,
			   unsigned_word addr, unsigned nr_bytes)
{
  HW_TRACE_DMA_READ ();
  return bfin_uart_read_buffer (me, dest, nr_bytes);
}

static unsigned
bfin_uart_dma_write_buffer (struct hw *me, const void *source,
			    int space, unsigned_word addr,
			    unsigned nr_bytes,
			    int violate_read_only_section)
{
  struct bfin_uart *uart = hw_data (me);
  unsigned ret;

  HW_TRACE_DMA_WRITE ();

  ret = bfin_uart_write_buffer (me, source, nr_bytes);

  hw_port_event (me, DV_PORT_TX, 1);

  return ret;
}

static const struct hw_port_descriptor bfin_uart_ports[] =
{
  { "tx",   DV_PORT_TX,   0, output_port, },
  { "rx",   DV_PORT_RX,   0, output_port, },
  { "stat", DV_PORT_STAT, 0, output_port, },
  { NULL, 0, 0, 0, },
};

static void
attach_bfin_uart_regs (struct hw *me, struct bfin_uart *uart)
{
  address_word attach_address;
  int attach_space;
  unsigned attach_size;
  reg_property_spec reg;

  if (hw_find_property (me, "reg") == NULL)
    hw_abort (me, "Missing \"reg\" property");

  if (!hw_find_reg_array_property (me, "reg", 0, &reg))
    hw_abort (me, "\"reg\" property must contain three addr/size entries");

  hw_unit_address_to_attach_address (hw_parent (me),
				     &reg.address,
				     &attach_space, &attach_address, me);
  hw_unit_size_to_attach_size (hw_parent (me), &reg.size, &attach_size, me);

  if (attach_size != BFIN_MMR_UART4_SIZE)
    hw_abort (me, "\"reg\" size must be %#x", BFIN_MMR_UART4_SIZE);

  hw_attach_address (hw_parent (me),
		     0, attach_space, attach_address, attach_size, me);

  uart->base = attach_address;
}

static void
bfin_uart_finish (struct hw *me)
{
  struct bfin_uart *uart;

  uart = HW_ZALLOC (me, struct bfin_uart);

  set_hw_data (me, uart);
  set_hw_io_read_buffer (me, bfin_uart_io_read_buffer);
  set_hw_io_write_buffer (me, bfin_uart_io_write_buffer);
  set_hw_dma_read_buffer (me, bfin_uart_dma_read_buffer);
  set_hw_dma_write_buffer (me, bfin_uart_dma_write_buffer);
  set_hw_ports (me, bfin_uart_ports);

  attach_bfin_uart_regs (me, uart);

  /* Initialize the UART.  */
  uart->revid = 0x00000004;
  uart->stat  = 0x000000A0;
  uart->clk   = 0x0000FFFF;
  uart->tsr   = 0x000007FF;
}

const struct hw_descriptor dv_bfin_uart4_descriptor[] =
{
  {"bfin_uart4", bfin_uart_finish,},
  {NULL, NULL},
};
