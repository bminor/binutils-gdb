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

#ifndef DV_BFIN_UART4_H
#define DV_BFIN_UART4_H

#include "dv-bfin_uart_core.h"

/* UART_CONTROL */
#define UEN			(1 << 0)
#define LOOP_ENA		(1 << 1)
#define UMOD			(3 << 4)
#define UMOD_UART		(0 << 4)
#define UMOD_MDB		(1 << 4)
#define UMOD_IRDA		(1 << 4)
#define WLS			(3 << 8)
#define WLS_5			(0 << 8)
#define WLS_6			(1 << 8)
#define WLS_7			(2 << 8)
#define WLS_8			(3 << 8)
#define STB			(1 << 12)
#define STBH			(1 << 13)
#define PEN			(1 << 14)
#define EPS			(1 << 15)
#define STP			(1 << 16)
#define FPE			(1 << 17)
#define FFE			(1 << 18)
#define SB			(1 << 19)
#define FCPOL			(1 << 22)
#define RPOLC			(1 << 23)
#define TPOLC			(1 << 24)
#define MRTS			(1 << 25)
#define XOFF			(1 << 26)
#define ARTS			(1 << 27)
#define ACTS			(1 << 28)
#define RFIT			(1 << 29)
#define RFRT			(1 << 30)

/* UART_STATUS */
#define DR			(1 << 0)
#define OE			(1 << 1)
#define PE			(1 << 2)
#define FE			(1 << 3)
#define BI			(1 << 4)
#define THRE			(1 << 5)
#define TEMT			(1 << 7)
#define TFI			(1 << 8)
#define ASTKY			(1 << 9)
#define ADDR			(1 << 10)
#define RO			(1 << 11)
#define SCTS			(1 << 12)
#define CTS			(1 << 16)
#define RFCS			(1 << 17)

/* UART_EMASK */
#define ERBFI			(1 << 0)
#define ETBEI			(1 << 1)
#define ELSI			(1 << 2)
#define EDSSI			(1 << 3)
#define EDTPTI			(1 << 4)
#define ETFI			(1 << 5)
#define ERFCI			(1 << 6)
#define EAWI			(1 << 7)
#define ERXS			(1 << 8)
#define ETXS			(1 << 9)

#endif
