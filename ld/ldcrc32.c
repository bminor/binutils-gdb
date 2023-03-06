/*
 * Library: ldlibcrc
 * Author:  Lammert Bies, Bastian Molkenthin, Ulf Samuelsson
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 2016 Lammert Bies
 * Copyright (c) 2013 Ulf Samuelsson (ulf@emagii.com)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include "sysdep.h"
#include "bfd.h"
#include "lddigest.h"
#include "ldreflect.h"

#define SHIFT(t)   ((sizeof(t)-1)*8)
/* ============ CRC-32 LIBCRC functions ======================================*/

/*
 * void _init_crc32_tab (uint32_t *crc_tab,  uint32_t poly);
 *
 * For optimal speed, the CRC32 calculation uses a table with pre-calculated
 * bit patterns which are used in the XOR operations in the program.
 * _init_crc32_tab is copyright (c) 2016 Lammert Bies
 */
static void
_init_crc32_tab (uint32_t * crc_tab, uint32_t poly)
{
  uint32_t crc;
  uint32_t shift = SHIFT (uint32_t);
  for (uint32_t i = 0; i < 256; i++)
    {
      crc = i << shift;
      for (uint32_t j = 0; j < 8; j++)
	{
	  if ((crc & 0x80000000u) != 0)
	    {
	      crc = (crc << 1) ^ poly;
	    }
	  else
	    {
	      crc = crc << 1;
	    }
	}
      crc_tab[i] = crc;
    }
}				/* _init_crc32__tab */

/*
 * void _init_crc32_reciprocal_tab (uint32_t *crc_tab,  uint32_t poly);
 *
 * For optimal speed, the CRC32 calculation uses a table with pre-calculated
 * bit patterns which are used in the XOR operations in the program.
 * _init_crc32_reciprocal_tab is copyright (c) 2021 Bastian Molkenthin
 */
static void
_init_crc32_reciprocal_tab (uint32_t * crc_tab, uint32_t poly)
{
  uint32_t crc;
  uint32_t reflected_poly = reflect32 (poly);
  for (uint32_t i = 0; i < 256; i++)
    {
      crc = i;
      for (uint32_t j = 0; j < 8; j++)
	{
	  if (crc & 0x00000001U)
	    {
	      crc >>= 1;
	      crc ^= reflected_poly;
	    }
	  else
	    {
	      crc = crc >> 1;
	    }
	}
      crc_tab[i] = crc;
    }
}				/* _init_crc32_reciprocal_tab */

/*
 * void init_crc32_tab( void );
 *
 * For optimal speed, the CRC32 calculation uses a table with pre-calculated
 * bit patterns which are used in the XOR operations in the program.
 * The table can be calculated in a normal or a reciprocal version.
 * init_crc64_tab is copyright (c) 2023 Ulf Samuelsson
 */
uint32_t *
init_crc32_tab (algorithm_desc_t * dsc)
{
  uint32_t *crc_tab = malloc (256 * sizeof (uint32_t));

  if (crc_tab == NULL)
    return NULL;

  if (dsc->reciprocal)
    {
      _init_crc32_reciprocal_tab (crc_tab, dsc->poly.d32._0);
    }
  else
    {
      _init_crc32_tab (crc_tab, dsc->poly.d32._0);
    }

  return crc_tab;
}				/* init_crc32_tab */

/*
 * uint32_t calc_crc32(algorithm_desc_t *dsc, const unsigned char *input_str, size_t num_bytes);
 *
 * The function calc_crc32() calculates in one pass the common 32 bit CRC value for
 * a byte string that is passed to the function together with a parameter
 * indicating the length.
 * calc_crc32 is
 *   copyright (c) 2016 Lammert Bies
 *   copyright (c) 2021 Bastian Molkenthin
 *   copyright (c) 2023 Ulf Samuelsson
 */
uint32_t
calc_crc32 (algorithm_desc_t * dsc, const unsigned char *input_str,
	    size_t num_bytes)
{
  uint32_t crc;
  const unsigned char *ptr;
  uint32_t index;
  uint32_t *crc_tab = dsc->crc_tab;

  if ((ptr = input_str) == NULL)
    return 0;

  if (crc_tab == NULL)
    return 0;

  crc = dsc->initial.d32._0;

  if (dsc->reciprocal)
    {
      for (uint32_t i = 0; i < num_bytes; i++)
	{
	  index = ((crc >> 0) ^ (uint32_t) * ptr++) & 0x000000FFul;
	  crc = (crc >> 8) ^ crc_tab[index];
	}
    }
  else
    {
      uint32_t shift = SHIFT (uint32_t);
      for (uint32_t i = 0; i < num_bytes; i++)
	{
	  const unsigned char c = *ptr++;
	  uint32_t rc = (uint32_t) (dsc->ireflect ? reflect8 (c) : c);
	  crc = (crc ^ (rc << shift));
	  index = (uint32_t) (crc >> shift);
	  crc = (crc << 8);
	  crc = (crc ^ (crc_tab[index]));
	}
    }
  crc = (dsc->oreflect ? reflect32 (crc) : crc);
  crc = crc ^ dsc->xor_val.d32._0;
  return crc;
}				/* calc_crc32 */
