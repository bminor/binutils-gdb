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

#ifndef DV_BFIN_DDE_H
#define DV_BFIN_DDE_H

#define DDE_PSIZE_MAX		3
#define DDE_MSIZE_MAX		5
#define DDE_PWIDTH_MAX		3
#define DDE_MWIDTH_MAX		3

/* DMA_CONFIG Masks */
#define DMAEN			(1 << DMAEN_P)	/* DMA Channel Enable */
#define WNR			(1 << WNR_P)	/* Channel Direction (W/R*) */
#define SYNC			(1 << SYNC_P)	/* Synchronize Work Unit Transitions */
#define CADDR			(1 << CADDR_P)	/* Use Current Address */
#define PSIZE			(7 << PSIZE_P)	/* Peripheral Word Size */
#define PSIZE_1			(0 << PSIZE_P)
#define PSIZE_2			(1 << PSIZE_P)
#define PSIZE_4			(2 << PSIZE_P)
#define PSIZE_8			(3 << PSIZE_P)
#define MSIZE			(7 << MSIZE_P)	/* Memory Transfer Size */
#define MSIZE_1			(0 << MSIZE_P)
#define MSIZE_2			(1 << MSIZE_P)
#define MSIZE_4			(2 << MSIZE_P)
#define MSIZE_8			(3 << MSIZE_P)
#define MSIZE_16		(4 << MSIZE_P)
#define MSIZE_32		(5 << MSIZE_P)
#define FLOW			(7 << FLOW_P)	/* Next Operation */
#define FLOW_STOP		(0 << FLOW_P)	/* Stop Mode */
#define FLOW_AUTO		(1 << FLOW_P)	/* Autobuffer Mode */
#define FLOW_DSCL		(4 << FLOW_P)	/* Descriptor List */
#define FLOW_DSCA		(5 << FLOW_P)	/* Descriptor Array */
#define FLOW_DSDL		(6 << FLOW_P)	/* Descriptor On Demand List */
#define FLOW_DSDA		(7 << FLOW_P)	/* Descriptor On Demand Array */
#define NDSIZE			(7 << NDSIZE_P)	/* Next Descriptor Set Size */
#define NDSIZE_1		(0 << NDSIZE_P)
#define NDSIZE_2		(1 << NDSIZE_P)
#define NDSIZE_3		(2 << NDSIZE_P)
#define NDSIZE_4		(3 << NDSIZE_P)
#define NDSIZE_5		(4 << NDSIZE_P)
#define NDSIZE_6		(5 << NDSIZE_P)
#define NDSIZE_7		(6 << NDSIZE_P)
#define INT			(3 << INT_P)	/* Generate Interrupt */
#define INT_NEVER		(0 << INT_P)	/* ... never */
#define INT_X			(1 << INT_P)	/* ... when xcount is 0 */
#define INT_Y			(2 << INT_P)	/* ... when ycount is 0 */
#define INT_PER			(3 << INT_P)	/* ... forwarding peripherals */
#define TRIG			(3 << TRIG_P)	/* Generate Trigger */
#define TOVEN			(1 << TOVEN_P)
#define DESCIDCPY		(1 << DESCIDCPY_P)
#define TWOD			(1 << TWOD_P)
#define PDRF			(1 << PDRF_P)

#define DMAEN_P			0
#define WNR_P			1
#define SYNC_P			2
#define CADDR_P			3
#define PSIZE_P			4
#define MSIZE_P			8
#define FLOW_P			12
#define TWAIT_P			15
#define NDSIZE_P		16
#define INT_P			20
#define TRIG_P			22
#define TOVEN_P			24
#define DESCIDCPY_P		25
#define TWOD_P			26
#define PDRF_P			28

/* DMA_STATUS Masks */
#define DMA_DONE		(1 << DMA_DONE_P)	/* Work Unit/Row Done Interrupt */
#define DMA_ERR			(1 << DMA_ERR_P)	/* Error Interrupt */
#define DMA_PIRQ		(1 << DMA_PIRQ_P)	/* Peripheral Interrupt Request */
#define DMA_ERRC		(7 << DMA_ERRC_P)	/* Error Cause */
#define DMA_RUN			(7 << DMA_RUN_P)	/* Run Status */
#define DMA_PBWIDTH		(3 << DMA_PBWIDTH_P)	/* Peripheral Bus Width */
#define DMA_MBWIDTH		(3 << DMA_MBWIDTH_P)	/* Memory Bus Width */
#define DMA_FIFOFILL		(7 << DMA_FIFOFILL_P)	/* FIFO Fill Status */
#define DMA_TWAIT		(1 << DMA_TWAIT_P)	/* Trigger Wait Status */

#define DMA_DONE_P		0
#define DMA_ERR_P		1
#define DMA_PIRQ_P		2
#define DMA_ERRC_P		4
#define DMA_RUN_P		8
#define DMA_PBWIDTH_P		12
#define DMA_MBWIDTH_P		14
#define DMA_FIFOFILL_P		16
#define DMA_TWAIT_P		20

#endif
