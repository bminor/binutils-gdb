/* Compressed section support (intended for debug sections).
   Copyright (C) 2008-2022 Free Software Foundation, Inc.

   This file is part of BFD, the Binary File Descriptor library.

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
#include <zlib.h>
#ifdef HAVE_ZSTD
#include <zstd.h>
#endif
#include "bfd.h"
#include "elf-bfd.h"
#include "libbfd.h"
#include "safe-ctype.h"

#define MAX_COMPRESSION_HEADER_SIZE 24

static bool
decompress_contents (bool is_zstd, bfd_byte *compressed_buffer,
		     bfd_size_type compressed_size,
		     bfd_byte *uncompressed_buffer,
		     bfd_size_type uncompressed_size)
{
  if (is_zstd)
    {
#ifdef HAVE_ZSTD
      size_t ret = ZSTD_decompress (uncompressed_buffer, uncompressed_size,
				    compressed_buffer, compressed_size);
      return !ZSTD_isError (ret);
#endif
    }

  z_stream strm;
  int rc;

  /* It is possible the section consists of several compressed
     buffers concatenated together, so we uncompress in a loop.  */
  /* PR 18313: The state field in the z_stream structure is supposed
     to be invisible to the user (ie us), but some compilers will
     still complain about it being used without initialisation.  So
     we first zero the entire z_stream structure and then set the fields
     that we need.  */
  memset (& strm, 0, sizeof strm);
  strm.avail_in = compressed_size;
  strm.next_in = (Bytef*) compressed_buffer;
  strm.avail_out = uncompressed_size;
  /* FIXME: strm.avail_in and strm.avail_out are typically unsigned
     int.  Supporting sizes that don't fit in an unsigned int is
     possible but will require some rewriting of this function.  */
  if (strm.avail_in != compressed_size || strm.avail_out != uncompressed_size)
    return false;

  BFD_ASSERT (Z_OK == 0);
  rc = inflateInit (&strm);
  while (strm.avail_in > 0 && strm.avail_out > 0)
    {
      if (rc != Z_OK)
	break;
      strm.next_out = ((Bytef*) uncompressed_buffer
		       + (uncompressed_size - strm.avail_out));
      rc = inflate (&strm, Z_FINISH);
      if (rc != Z_STREAM_END)
	break;
      rc = inflateReset (&strm);
    }
  return inflateEnd (&strm) == Z_OK && rc == Z_OK && strm.avail_out == 0;
}

/* Compress section contents using zlib/zstd and store
   as the contents field.  This function assumes the contents
   field was allocated using bfd_malloc() or equivalent.

   Return the uncompressed size if the full section contents is
   compressed successfully.  Otherwise return 0.  */

static bfd_size_type
bfd_compress_section_contents (bfd *abfd, sec_ptr sec)
{
  bfd_byte *input_buffer;
  uLong compressed_size;
  bfd_byte *buffer;
  bfd_size_type buffer_size;
  int zlib_size = 0;
  int orig_header_size;
  bfd_size_type uncompressed_size;
  unsigned int uncompressed_alignment_pow;
  unsigned int ch_type = 0;
  int new_header_size = bfd_get_compression_header_size (abfd, NULL);
  bool compressed
    = bfd_is_section_compressed_info (abfd, sec,
				      &orig_header_size,
				      &uncompressed_size,
				      &uncompressed_alignment_pow,
				      &ch_type);
  bool update = false;

  /* We shouldn't be trying to decompress unsupported compressed sections.  */
  if (compressed && orig_header_size < 0)
    abort ();

  /* Either ELF compression header or the 12-byte, "ZLIB" + 8-byte size,
     overhead in .zdebug* section.  */
  if (!new_header_size)
    new_header_size = 12;
  if (ch_type == 0)
    orig_header_size = 12;

  input_buffer = sec->contents;
  if (compressed)
    {
      zlib_size = sec->size - orig_header_size;
      compressed_size = zlib_size + new_header_size;

      /* If we are converting between zlib-gnu and zlib-gabi then the
	 compressed contents just need to be moved.  */
      update = (ch_type < ELFCOMPRESS_ZSTD
		&& (abfd->flags & BFD_COMPRESS_ZSTD) == 0);

      /* Uncompress when not just moving contents or when compressed
	 is not smaller than uncompressed.  */
      if (!update || compressed_size >= uncompressed_size)
	{
	  buffer_size = uncompressed_size;
	  buffer = bfd_malloc (buffer_size);
	  if (buffer == NULL)
	    return 0;

	  if (!decompress_contents (ch_type == ELFCOMPRESS_ZSTD,
				    input_buffer + orig_header_size,
				    zlib_size, buffer, buffer_size))
	    {
	      bfd_set_error (bfd_error_bad_value);
	      free (buffer);
	      return 0;
	    }
	  free (input_buffer);
	  bfd_set_section_alignment (sec, uncompressed_alignment_pow);
	  sec->contents = buffer;
	  sec->flags |= SEC_IN_MEMORY;
	  sec->compress_status = COMPRESS_SECTION_NONE;
	  sec->size = uncompressed_size;
	  input_buffer = buffer;
	}
    }

  if (!update)
    compressed_size = compressBound (uncompressed_size) + new_header_size;

  buffer_size = compressed_size;
  buffer = bfd_alloc (abfd, buffer_size);
  if (buffer == NULL)
    return 0;

  if (update)
    {
      if (compressed_size < uncompressed_size)
	memcpy (buffer + new_header_size,
		input_buffer + orig_header_size,
		zlib_size);
    }
  else
    {
      if (abfd->flags & BFD_COMPRESS_ZSTD)
	{
#if HAVE_ZSTD
	  compressed_size = ZSTD_compress (buffer + new_header_size,
					   compressed_size,
					   input_buffer,
					   uncompressed_size,
					   ZSTD_CLEVEL_DEFAULT);
	  if (ZSTD_isError (compressed_size))
	    {
	      bfd_release (abfd, buffer);
	      bfd_set_error (bfd_error_bad_value);
	      return 0;
	    }
#endif
	}
      else if (compress ((Bytef *) buffer + new_header_size, &compressed_size,
			 (const Bytef *) input_buffer, uncompressed_size)
	       != Z_OK)
	{
	  bfd_release (abfd, buffer);
	  bfd_set_error (bfd_error_bad_value);
	  return 0;
	}

      compressed_size += new_header_size;
    }

  /* If compression didn't make the section smaller, keep it uncompressed.  */
  if (compressed_size >= uncompressed_size)
    {
      memcpy (buffer, input_buffer, uncompressed_size);
      elf_section_flags (sec) &= ~SHF_COMPRESSED;
      sec->compress_status = COMPRESS_SECTION_NONE;
    }
  else
    {
      sec->size = uncompressed_size;
      bfd_update_compression_header (abfd, buffer, sec);
      sec->size = compressed_size;
      sec->compress_status = COMPRESS_SECTION_DONE;
    }
  sec->contents = buffer;
  sec->flags |= SEC_IN_MEMORY;
  free (input_buffer);
  return uncompressed_size;
}

/*
FUNCTION
	bfd_get_full_section_contents

SYNOPSIS
	bool bfd_get_full_section_contents
	  (bfd *abfd, asection *section, bfd_byte **ptr);

DESCRIPTION
	Read all data from @var{section} in BFD @var{abfd}, decompress
	if needed, and store in @var{*ptr}.  If @var{*ptr} is NULL,
	return @var{*ptr} with memory malloc'd by this function.

	Return @code{TRUE} if the full section contents is retrieved
	successfully.  If the section has no contents then this function
	returns @code{TRUE} but @var{*ptr} is set to NULL.
*/

bool
bfd_get_full_section_contents (bfd *abfd, sec_ptr sec, bfd_byte **ptr)
{
  bfd_size_type sz = bfd_get_section_limit_octets (abfd, sec);
  bfd_byte *p = *ptr;
  bool ret;
  bfd_size_type save_size;
  bfd_size_type save_rawsize;
  bfd_byte *compressed_buffer;
  unsigned int compression_header_size;
  const unsigned int compress_status = sec->compress_status;

  if (sz == 0)
    {
      *ptr = NULL;
      return true;
    }

  if (p == NULL
      && compress_status != COMPRESS_SECTION_DONE
      && _bfd_section_size_insane (abfd, sec))
    {
      /* PR 24708: Avoid attempts to allocate a ridiculous amount
	 of memory.  */
      _bfd_error_handler
	/* xgettext:c-format */
	(_("error: %pB(%pA) is too large (%#" PRIx64 " bytes)"),
	 abfd, sec, (uint64_t) sz);
      return false;
    }

  switch (compress_status)
    {
    case COMPRESS_SECTION_NONE:
      if (p == NULL)
	{
	  p = (bfd_byte *) bfd_malloc (sz);
	  if (p == NULL)
	    {
	      /* PR 20801: Provide a more helpful error message.  */
	      if (bfd_get_error () == bfd_error_no_memory)
		_bfd_error_handler
		  /* xgettext:c-format */
		  (_("error: %pB(%pA) is too large (%#" PRIx64 " bytes)"),
		  abfd, sec, (uint64_t) sz);
	      return false;
	    }
	}

      if (!bfd_get_section_contents (abfd, sec, p, 0, sz))
	{
	  if (*ptr != p)
	    free (p);
	  return false;
	}
      *ptr = p;
      return true;

    case DECOMPRESS_SECTION_ZLIB:
    case DECOMPRESS_SECTION_ZSTD:
      /* Read in the full compressed section contents.  */
      compressed_buffer = (bfd_byte *) bfd_malloc (sec->compressed_size);
      if (compressed_buffer == NULL)
	return false;
      save_rawsize = sec->rawsize;
      save_size = sec->size;
      /* Clear rawsize, set size to compressed size and set compress_status
	 to COMPRESS_SECTION_NONE.  If the compressed size is bigger than
	 the uncompressed size, bfd_get_section_contents will fail.  */
      sec->rawsize = 0;
      sec->size = sec->compressed_size;
      sec->compress_status = COMPRESS_SECTION_NONE;
      ret = bfd_get_section_contents (abfd, sec, compressed_buffer,
				      0, sec->compressed_size);
      /* Restore rawsize and size.  */
      sec->rawsize = save_rawsize;
      sec->size = save_size;
      sec->compress_status = compress_status;
      if (!ret)
	goto fail_compressed;

      if (p == NULL)
	p = (bfd_byte *) bfd_malloc (sz);
      if (p == NULL)
	goto fail_compressed;

      compression_header_size = bfd_get_compression_header_size (abfd, sec);
      if (compression_header_size == 0)
	/* Set header size to the zlib header size if it is a
	   SHF_COMPRESSED section.  */
	compression_header_size = 12;
      bool is_zstd = compress_status == DECOMPRESS_SECTION_ZSTD;
      if (!decompress_contents (
	      is_zstd, compressed_buffer + compression_header_size,
	      sec->compressed_size - compression_header_size, p, sz))
	{
	  bfd_set_error (bfd_error_bad_value);
	  if (p != *ptr)
	    free (p);
	fail_compressed:
	  free (compressed_buffer);
	  return false;
	}

      free (compressed_buffer);
      *ptr = p;
      return true;

    case COMPRESS_SECTION_DONE:
      if (sec->contents == NULL)
	return false;
      if (p == NULL)
	{
	  p = (bfd_byte *) bfd_malloc (sz);
	  if (p == NULL)
	    return false;
	  *ptr = p;
	}
      /* PR 17512; file: 5bc29788.  */
      if (p != sec->contents)
	memcpy (p, sec->contents, sz);
      return true;

    default:
      abort ();
    }
}

/*
FUNCTION
	bfd_is_section_compressed_info

SYNOPSIS
	bool bfd_is_section_compressed_info
	  (bfd *abfd, asection *section,
	   int *compression_header_size_p,
	   bfd_size_type *uncompressed_size_p,
	   unsigned int *uncompressed_alignment_power_p,
	   unsigned int *ch_type);

DESCRIPTION
	Return @code{TRUE} if @var{section} is compressed.  Compression
	header size is returned in @var{compression_header_size_p},
	uncompressed size is returned in @var{uncompressed_size_p}
	and the uncompressed data alignement power is returned in
	@var{uncompressed_align_pow_p}.  If compression is
	unsupported, compression header size is returned with -1
	and uncompressed size is returned with 0.
*/

bool
bfd_is_section_compressed_info (bfd *abfd, sec_ptr sec,
				int *compression_header_size_p,
				bfd_size_type *uncompressed_size_p,
				unsigned int *uncompressed_align_pow_p,
				unsigned int *ch_type)
{
  bfd_byte header[MAX_COMPRESSION_HEADER_SIZE];
  int compression_header_size;
  int header_size;
  unsigned int saved = sec->compress_status;
  bool compressed;

  *uncompressed_align_pow_p = 0;

  compression_header_size = bfd_get_compression_header_size (abfd, sec);
  if (compression_header_size > MAX_COMPRESSION_HEADER_SIZE)
    abort ();
  header_size = compression_header_size ? compression_header_size : 12;

  /* Don't decompress the section.  */
  sec->compress_status = COMPRESS_SECTION_NONE;

  /* Read the header.  */
  if (bfd_get_section_contents (abfd, sec, header, 0, header_size))
    {
      if (compression_header_size == 0)
	/* In this case, it should be "ZLIB" followed by the uncompressed
	   section size, 8 bytes in big-endian order.  */
	compressed = startswith ((char*) header , "ZLIB");
      else
	compressed = true;
    }
  else
    compressed = false;

  *uncompressed_size_p = sec->size;
  if (compressed)
    {
      if (compression_header_size != 0)
	{
	  if (!bfd_check_compression_header (abfd, header, sec, ch_type,
					     uncompressed_size_p,
					     uncompressed_align_pow_p))
	    compression_header_size = -1;
	}
      /* Check for the pathalogical case of a debug string section that
	 contains the string ZLIB.... as the first entry.  We assume that
	 no uncompressed .debug_str section would ever be big enough to
	 have the first byte of its (big-endian) size be non-zero.  */
      else if (strcmp (sec->name, ".debug_str") == 0
	       && ISPRINT (header[4]))
	compressed = false;
      else
	*uncompressed_size_p = bfd_getb64 (header + 4);
    }

  /* Restore compress_status.  */
  sec->compress_status = saved;
  *compression_header_size_p = compression_header_size;
  return compressed;
}

/*
FUNCTION
	bfd_is_section_compressed

SYNOPSIS
	bool bfd_is_section_compressed
	  (bfd *abfd, asection *section);

DESCRIPTION
	Return @code{TRUE} if @var{section} is compressed.
*/

bool
bfd_is_section_compressed (bfd *abfd, sec_ptr sec)
{
  int compression_header_size;
  bfd_size_type uncompressed_size;
  unsigned int uncompressed_align_power;
  unsigned int ch_type;
  return (bfd_is_section_compressed_info (abfd, sec,
					  &compression_header_size,
					  &uncompressed_size,
					  &uncompressed_align_power,
					  &ch_type)
	  && compression_header_size >= 0
	  && uncompressed_size > 0);
}

/*
FUNCTION
	bfd_init_section_decompress_status

SYNOPSIS
	bool bfd_init_section_decompress_status
	  (bfd *abfd, asection *section);

DESCRIPTION
	Record compressed section size, update section size with
	decompressed size and set compress_status to
	DECOMPRESS_SECTION_{ZLIB,ZSTD}.

	Return @code{FALSE} if the section is not a valid compressed
	section.  Otherwise, return @code{TRUE}.
*/

bool
bfd_init_section_decompress_status (bfd *abfd, sec_ptr sec)
{
  bfd_byte header[MAX_COMPRESSION_HEADER_SIZE];
  int compression_header_size;
  int header_size;
  bfd_size_type uncompressed_size;
  unsigned int uncompressed_alignment_power = 0;
  unsigned int ch_type;
  z_stream strm;

  compression_header_size = bfd_get_compression_header_size (abfd, sec);
  if (compression_header_size > MAX_COMPRESSION_HEADER_SIZE)
    abort ();
  header_size = compression_header_size ? compression_header_size : 12;

  /* Read the header.  */
  if (sec->rawsize != 0
      || sec->contents != NULL
      || sec->compress_status != COMPRESS_SECTION_NONE
      || !bfd_get_section_contents (abfd, sec, header, 0, header_size))
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  if (compression_header_size == 0)
    {
      /* In this case, it should be "ZLIB" followed by the uncompressed
	 section size, 8 bytes in big-endian order.  */
      if (! startswith ((char*) header, "ZLIB"))
	{
	  bfd_set_error (bfd_error_wrong_format);
	  return false;
	}
      uncompressed_size = bfd_getb64 (header + 4);
    }
  else if (!bfd_check_compression_header (abfd, header, sec,
					  &ch_type,
					  &uncompressed_size,
					  &uncompressed_alignment_power))
    {
      bfd_set_error (bfd_error_wrong_format);
      return false;
    }

  /* PR28530, reject sizes unsupported by decompress_contents.  */
  strm.avail_in = sec->size;
  strm.avail_out = uncompressed_size;
  if (strm.avail_in != sec->size || strm.avail_out != uncompressed_size)
    {
      bfd_set_error (bfd_error_nonrepresentable_section);
      return false;
    }

  sec->compressed_size = sec->size;
  sec->size = uncompressed_size;
  bfd_set_section_alignment (sec, uncompressed_alignment_power);
  sec->compress_status = (ch_type == ELFCOMPRESS_ZSTD
			  ? DECOMPRESS_SECTION_ZSTD : DECOMPRESS_SECTION_ZLIB);

  return true;
}

/*
FUNCTION
	bfd_init_section_compress_status

SYNOPSIS
	bool bfd_init_section_compress_status
	  (bfd *abfd, asection *section);

DESCRIPTION
	If open for read, compress section, update section size with
	compressed size and set compress_status to COMPRESS_SECTION_DONE.

	Return @code{FALSE} if the section is not a valid compressed
	section.  Otherwise, return @code{TRUE}.
*/

bool
bfd_init_section_compress_status (bfd *abfd, sec_ptr sec)
{
  bfd_size_type uncompressed_size;
  bfd_byte *uncompressed_buffer;

  /* Error if not opened for read.  */
  if (abfd->direction != read_direction
      || sec->size == 0
      || sec->rawsize != 0
      || sec->contents != NULL
      || sec->compress_status != COMPRESS_SECTION_NONE)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  /* Read in the full section contents and compress it.  */
  uncompressed_size = sec->size;
  uncompressed_buffer = (bfd_byte *) bfd_malloc (uncompressed_size);
  /* PR 21431 */
  if (uncompressed_buffer == NULL)
    return false;

  if (!bfd_get_section_contents (abfd, sec, uncompressed_buffer,
				 0, uncompressed_size))
    return false;

  sec->contents = uncompressed_buffer;
  if (bfd_compress_section_contents (abfd, sec) == 0)
    {
      free (sec->contents);
      sec->contents = NULL;
      return false;
    }
  return true;
}

/*
FUNCTION
	bfd_compress_section

SYNOPSIS
	bool bfd_compress_section
	  (bfd *abfd, asection *section, bfd_byte *uncompressed_buffer);

DESCRIPTION
	If open for write, compress section, update section size with
	compressed size and set compress_status to COMPRESS_SECTION_DONE.

	Return @code{FALSE} if compression fail.  Otherwise, return
	@code{TRUE}.  UNCOMPRESSED_BUFFER is freed in both cases.
*/

bool
bfd_compress_section (bfd *abfd, sec_ptr sec, bfd_byte *uncompressed_buffer)
{
  bfd_size_type uncompressed_size = sec->size;

  /* Error if not opened for write.  */
  if (abfd->direction != write_direction
      || uncompressed_size == 0
      || uncompressed_buffer == NULL
      || sec->contents != NULL
      || sec->compressed_size != 0
      || sec->compress_status != COMPRESS_SECTION_NONE)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return false;
    }

  sec->contents = uncompressed_buffer;
  if (bfd_compress_section_contents (abfd, sec) == 0)
    {
      free (sec->contents);
      sec->contents = NULL;
      return false;
    }
  return true;
}
