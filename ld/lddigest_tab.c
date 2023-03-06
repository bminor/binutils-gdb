/* Linker command language support.
   Copyright (C) 1991-2023 Ulf Samuelsson <ulf@emagii.com>

   This file is part of the GNU Binutils.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street - Fifth Floor, Boston,
   MA 02110-1301, USA.  */

#include "sysdep.h"
#include "bfd.h"
#include "safe-ctype.h"
#include "obstack.h"
#include "bfdlink.h"
#include "ctf-api.h"

#include "ld.h"
#include "ldmain.h"
#include "ldexp.h"
#include "ldlang.h"
#include <ldgram.h>
#include "ldlex.h"
#include "ldmisc.h"
#include "lddigest.h"

const algorithm_desc_t algorithms[MAXALGO+1] =
{
  [CRC64_ECMA] =
	{
		crc_algo_64, 64, "CRC64-ECMA",
		.poly.d64 = CRC_POLY_64_ECMA,
		.initial.d64 = CRC_START_64,
		.xor_val.d64 = CRC_END_64,
		.ireflect = false,
		.oreflect = false,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0x6c40df5f0b497347
	},
  [CRC64_WE] =
	{
		crc_algo_64, 64, "CRC64-WE",
		.poly.d64 = CRC_POLY_64_WE,
		.initial.d64 = CRC_START_64_INV,
		.xor_val.d64 = CRC_END_64_INV,
		.ireflect = false,
		.oreflect = false,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0x62ec59e3f1a4f00a
	},
  [CRC64_XZ] =
	{
		crc_algo_64, 64, "CRC64-XZ",
		.poly.d64 = CRC_POLY_64_XZ,
		.initial.d64 = CRC_START_64_INV,
		.xor_val.d64 = CRC_END_64_INV,
		.ireflect = true,
		.oreflect = true,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0x995dc9bbdf1939fa
	},
  [CRC64_ISO] =
	{
		crc_algo_64, 64, "CRC64-GO-ISO",
		.poly.d64 = CRC_POLY_64_ISO,
		.initial.d64 = CRC_START_64_INV,
		.xor_val.d64 = CRC_END_64_INV,
		.ireflect = true,
		.oreflect = true,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0xb90956c775a41001
	},
  [CRC64_ISO_R] =
	{
		crc_algo_64, 64, "CRC64-GO-ISO-R",
		.poly.d64 = CRC_POLY_64_ISO,
		.initial.d64 = CRC_START_64_INV,
		.xor_val.d64 = CRC_END_64_INV,
		.ireflect = false,
		.oreflect = false,
		.reciprocal = true,
		.crc_tab = NULL,
		.expected.d64 = 0xb90956c775a41001
	},
  [CRC32] =
	{
		crc_algo_32, 32, "CRC32",
		.poly.d32._0 = CRC_POLY_32,
		.initial.d64 = CRC_START_32_INV,
		.xor_val.d32._0 = CRC_END_32_INV,
		.ireflect = true,
		.oreflect = true,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0xCBF43926
	},
  [MAXALGO] =
	{
		no_algo, 0, "",
		.poly.d64 = 0,
		.initial.d64 = 0,
		.xor_val.d64 = 0,
		.ireflect = false,
		.oreflect = false,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0
	}
};

algorithm_desc_t algorithm =
{
		no_algo, 0, "",
		.poly.d64 = 0,
		.initial.d64 = 0,
		.xor_val.d64 = 0,
		.ireflect = false,
		.oreflect = false,
		.reciprocal = false,
		.crc_tab = NULL,
		.expected.d64 = 0
};
