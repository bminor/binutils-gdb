/*
 * Library: libcrc
 * Author:  Lammert Bies, Ulf Samuelsson
 *
 * This file is licensed under the MIT License as stated below
 *
 * Copyright (c) 2016 Lammert Bies
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

#ifndef LDDIGEST_H
#define LDDIGEST_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "bfd.h"

#define	CRC_POLY_64_ECMA	0x42F0E1EBA9EA3693ull	/* Normal */
#define	CRC_POLY_64_ECMA_EXP	0x6C40DF5F0B497347ull	/* CRC when testing "123456789" */
#define	CRC_POLY_64_ECMA_R	0xC96C5795D7870F42ull	/* Reversed */

#define	CRC_POLY_64_ISO		0x000000000000001Bull	/* Normal */
#define	CRC_POLY_64_ISO_EXP	0xB90956C775A41001ull	/* CRC when testing "123456789" */
#define	CRC_POLY_64_ISO_R	0xD800000000000000ull	/* Reversed */

#define	CRC_POLY_64_WE		0x42F0E1EBA9EA3693ull	/* Normal */
#define	CRC_POLY_64_WE_EXP	0x62EC59E3F1A4F00Aull	/* CRC when testing "123456789" */

#define	CRC_POLY_64_XZ		0x42F0E1EBA9EA3693ull	/* Normal */
#define	CRC_POLY_64_XZ_EXP	0x995DC9BBDF1939FAull	/* CRC when testing "123456789" */

#define	CRC_START_64		0x0000000000000000ull
#define	CRC_START_64_ECMA	0x0000000000000000ull
#define	CRC_START_64_WE		0xFFFFFFFFFFFFFFFFull
#define	CRC_START_64_INV	0xFFFFFFFFFFFFFFFFull
#define	CRC_END_64		0x0000000000000000ull
#define	CRC_END_64_INV		0xFFFFFFFFFFFFFFFFull

#define	CRC_START_32		0xFFFFFFFFul
#define	CRC_END_32		0xFFFFFFFFul

#define	CRC64_ADDRESS		"___CRC64___"
#define	CRC64_START		"___CRC64_START___"
#define	CRC64_END		"___CRC64_END___"
#define	CRC64_TABLE		"___CRC64_TABLE___"

#define	CRC_POLY_32		0x04C11DB7ul	/* Normal */
#define	CRC_POLY_32_R		0xEDB88320ul	/* Reversed */

#define	CRC_START_32_INV	0xFFFFFFFFul
#define	CRC_END_32_INV    0xFFFFFFFFul

#define	CRC32_ADDRESS		"___CRC32___"
#define	CRC32_START		"___CRC32_START___"
#define	CRC32_END		"___CRC32_END___"
#define	CRC32_TABLE		"___CRC32_TABLE___"

typedef enum algorithm
{
  no_algo = 0,
  sha_algo_1 = 1,
  crc_algo_32 = 4,
  crc_algo_64 = 8,
  sha_algo_256 = 256,
  sha_algo_512
} algo_t;

typedef enum
{
  CRC64_ECMA,
  CRC64_WE,
  CRC64_XZ,
  CRC64_ISO,
  CRC64_ISO_R,
  CRC32,
  MAXALGO
} poly_t;

typedef struct
{
  uint8_t _0;
  uint8_t _1;
  uint8_t _2;
  uint8_t _3;
  uint8_t _4;
  uint8_t _5;
  uint8_t _6;
  uint8_t _7;
} uint8_ut;

typedef struct
{
  uint16_t _0;
  uint16_t _1;
  uint16_t _2;
  uint16_t _3;
} uint16_ut;

typedef struct
{
  uint32_t _0;
  uint32_t _1;
} uint32_ut;

typedef union
{
  uint64_t d64;
  uint32_ut d32;
  uint16_ut d16;
  uint8_ut d8;
} int_t;

typedef struct
{
  algo_t crc_algo;
  uint32_t crc_size;
  const char *name;
  int_t poly;
  int_t initial;
  int_t xor_val;
  bool ireflect;
  bool oreflect;
  bool reciprocal;
  void *crc_tab;
  int_t expected;
  bool big_endian;
} algorithm_desc_t;

extern char *CRC_ADDRESS;
extern char *CRC_START;
extern char *CRC_END;
extern char *CRC_TABLE;
extern const algorithm_desc_t algorithms[];
extern algorithm_desc_t algorithm;

extern const char *digest_label;
extern bool digest_big_endian;
extern bool polynome_valid;

/* CRC-32 */
extern uint32_t *init_crc32_tab (algorithm_desc_t * dsc);
extern uint32_t calc_crc32
  (algorithm_desc_t * dsc, const unsigned char *input_str, size_t num_bytes);
extern void lang_add_crc32_syndrome (algorithm_desc_t * a);

/* CR-64 */
extern bfd_vma *init_crc64_tab (algorithm_desc_t * dsc);
extern bfd_vma calc_crc64
  (algorithm_desc_t * dsc, const unsigned char *input_str, size_t num_bytes);
extern void lang_add_crc64_syndrome (algorithm_desc_t * a);

extern void lang_add_digest (bfd_vma size,
			     bfd_vma poly,
			     bfd_vma initial,
			     bfd_vma xor_val,
			     bfd_vma ireflect,
			     bfd_vma oreflect, bfd_vma reciprocal);
extern bool lang_set_digest (char *digest);
extern void lang_add_digest_table (bool big_endian);
extern const char *lang_get_label (const char *label, bool *big_endian);
extern void lang_generate_crc (void);
extern void lang_generate_digest (void);

#endif /* LDDIGEST_H */
