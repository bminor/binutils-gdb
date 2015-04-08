/* Compressed section support (intended for debug sections).
   Copyright (C) 2008-2015 Free Software Foundation, Inc.

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
#include "bfd.h"
#include "libbfd.h"
#include "safe-ctype.h"

#define MAX_COMPRESSION_HEADER_SIZE 24

static bfd_boolean
decompress_contents (bfd_byte *compressed_buffer,
		     bfd_size_type compressed_size,
		     bfd_byte *uncompressed_buffer,
		     bfd_size_type uncompressed_size)
{
  z_stream strm;
  int rc;

  /* It is possible the section consists of several compressed
     buffers concatenated together, so we uncompress in a loop.  */
  strm.zalloc = NULL;
  strm.zfree = NULL;
  strm.opaque = NULL;
  strm.avail_in = compressed_size - 12;
  strm.next_in = (Bytef*) compressed_buffer + 12;
  strm.avail_out = uncompressed_size;

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
  rc |= inflateEnd (&strm);
  return rc == Z_OK && strm.avail_out == 0;
}

/* Compress data of the size specified in @var{uncompressed_size}
   and pointed to by @var{uncompressed_buffer} using zlib and store
   as the contents field.  This function assumes the contents
   field was allocated using bfd_malloc() or equivalent.  If zlib
   is not installed on this machine, the input is unmodified.

   Return the uncompressed size if the full section contents is
   compressed successfully.  Otherwise return 0.  */

static bfd_size_type
bfd_compress_section_contents (bfd *abfd, sec_ptr sec,
			       bfd_byte *uncompressed_buffer,
			       bfd_size_type uncompressed_size)
{
  uLong compressed_size;
  bfd_byte *buffer;
  bfd_size_type buffer_size;
  bfd_boolean decompress;
  int zlib_size;
  int orig_compression_header_size;
  int compression_header_size
    = bfd_get_compression_header_size (abfd, NULL);
  bfd_boolean compressed
    = bfd_is_section_compressed_with_header (abfd, sec,
					     &orig_compression_header_size);

  if (compressed)
    {
      /* We shouldn't decompress unsupported compressed section.  */
      if (orig_compression_header_size < 0)
	abort ();

      /* Different compression schemes.  Just move the compressed section
	 contents to the right position. */
      if (orig_compression_header_size == 0)
	{
	  /* Convert it from .zdebug* section.  Get the uncompressed
	     size first.  */
	  zlib_size = uncompressed_size;
	  compressed_size = zlib_size + compression_header_size;
	  uncompressed_size = bfd_getb64 (uncompressed_buffer + 4);
	}
      else
	{
	  /* Convert it to .zdebug* section. */
	  zlib_size = uncompressed_size - orig_compression_header_size;
	  compressed_size = zlib_size;
	}
    }
  else
    compressed_size = compressBound (uncompressed_size) + 12;

  /* When converting from .zdebug* section, uncompress if it leads to
     smaller size.  */
  if (compressed
      && orig_compression_header_size == 0
      && compressed_size > uncompressed_size)
    {
      decompress = TRUE;
      buffer_size = uncompressed_size;
    }
  else
    {
      decompress = FALSE;
      buffer_size = compressed_size + compression_header_size;
    }
  buffer = (bfd_byte *) bfd_malloc (buffer_size);
  if (buffer == NULL)
    return 0;

  if (compressed)
    {
      sec->size = uncompressed_size;
      if (decompress)
	{
	  if (!decompress_contents (uncompressed_buffer, zlib_size,
				    buffer, uncompressed_size))
	    {
	      bfd_set_error (bfd_error_bad_value);
	      free (buffer);
	      return 0;
	    }
	  free (uncompressed_buffer);
	  sec->contents = buffer;
	  sec->compress_status = COMPRESS_SECTION_DONE;
	  return uncompressed_size;
	}
      else
	{
	  bfd_update_compression_header (abfd, buffer, sec);
	  memmove (buffer + compression_header_size,
		   uncompressed_buffer + orig_compression_header_size,
		   zlib_size);
	}
    }
  else
    {
      bfd_size_type size = uncompressed_size;
      int header_size = 12 + compression_header_size;
      if (compress ((Bytef*) buffer + header_size,
		    &compressed_size,
		    (const Bytef*) uncompressed_buffer,
		    uncompressed_size) != Z_OK)
	{
	  free (buffer);
	  bfd_set_error (bfd_error_bad_value);
	  return 0;
	}

      compressed_size += header_size;
      /* PR binutils/18087: If compression didn't make the section smaller,
	 just keep it uncompressed.  */
      if (compressed_size < uncompressed_size)
	{
	  bfd_update_compression_header (abfd, buffer, sec);

	  /* Write the zlib header.  In this case, it should be "ZLIB"
	     followed by the uncompressed section size, 8 bytes in
	     big-endian order.  */
	  memcpy (buffer + compression_header_size, "ZLIB", 4);
	  bfd_putb64 (size, buffer + compression_header_size + 4);
	}
      else
	{
	  sec->contents = uncompressed_buffer;
	  sec->compress_status = COMPRESS_SECTION_NONE;
	  return uncompressed_size;
	}
    }

  free (uncompressed_buffer);
  sec->contents = buffer;
  sec->size = compressed_size;
  sec->compress_status = COMPRESS_SECTION_DONE;

  return uncompressed_size;
}

/*
FUNCTION
	bfd_get_full_section_contents

SYNOPSIS
	bfd_boolean bfd_get_full_section_contents
	  (bfd *abfd, asection *section, bfd_byte **ptr);

DESCRIPTION
	Read all data from @var{section} in BFD @var{abfd}, decompress
	if needed, and store in @var{*ptr}.  If @var{*ptr} is NULL,
	return @var{*ptr} with memory malloc'd by this function.

	Return @code{TRUE} if the full section contents is retrieved
	successfully.  If the section has no contents then this function
	returns @code{TRUE} but @var{*ptr} is set to NULL.
*/

bfd_boolean
bfd_get_full_section_contents (bfd *abfd, sec_ptr sec, bfd_byte **ptr)
{
  bfd_size_type sz;
  bfd_byte *p = *ptr;
  bfd_boolean ret;
  bfd_size_type save_size;
  bfd_size_type save_rawsize;
  bfd_byte *compressed_buffer;
  unsigned int compression_header_size;

  if (abfd->direction != write_direction && sec->rawsize != 0)
    sz = sec->rawsize;
  else
    sz = sec->size;
  if (sz == 0)
    {
      *ptr = NULL;
      return TRUE;
    }

  switch (sec->compress_status)
    {
    case COMPRESS_SECTION_NONE:
      if (p == NULL)
	{
	  p = (bfd_byte *) bfd_malloc (sz);
	  if (p == NULL)
	    return FALSE;
	}

      if (!bfd_get_section_contents (abfd, sec, p, 0, sz))
	{
	  if (*ptr != p)
	    free (p);
	  return FALSE;
	}
      *ptr = p;
      return TRUE;

    case DECOMPRESS_SECTION_SIZED:
      /* Read in the full compressed section contents.  */
      compressed_buffer = (bfd_byte *) bfd_malloc (sec->compressed_size);
      if (compressed_buffer == NULL)
	return FALSE;
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
      sec->compress_status = DECOMPRESS_SECTION_SIZED;
      if (!ret)
	goto fail_compressed;

      if (p == NULL)
	p = (bfd_byte *) bfd_malloc (sz);
      if (p == NULL)
	goto fail_compressed;

      compression_header_size = bfd_get_compression_header_size (abfd, sec);
      if (!decompress_contents (compressed_buffer + compression_header_size,
				sec->compressed_size, p, sz))
	{
	  bfd_set_error (bfd_error_bad_value);
	  if (p != *ptr)
	    free (p);
	fail_compressed:
	  free (compressed_buffer);
	  return FALSE;
	}

      free (compressed_buffer);
      *ptr = p;
      return TRUE;

    case COMPRESS_SECTION_DONE:
      if (sec->contents == NULL)
	return FALSE;
      if (p == NULL)
	{
	  p = (bfd_byte *) bfd_malloc (sz);
	  if (p == NULL)
	    return FALSE;
	  *ptr = p;
	}
      /* PR 17512; file: 5bc29788.  */
      if (p != sec->contents)
	memcpy (p, sec->contents, sz);
      return TRUE;

    default:
      abort ();
    }
}

/*
FUNCTION
	bfd_cache_section_contents

SYNOPSIS
	void bfd_cache_section_contents
	  (asection *sec, void *contents);

DESCRIPTION
	Stash @var(contents) so any following reads of @var(sec) do
	not need to decompress again.
*/

void
bfd_cache_section_contents (asection *sec, void *contents)
{
  if (sec->compress_status == DECOMPRESS_SECTION_SIZED)
    sec->compress_status = COMPRESS_SECTION_DONE;
  sec->contents = contents;
  sec->flags |= SEC_IN_MEMORY;
}

/*
FUNCTION
	bfd_is_section_compressed_with_header

SYNOPSIS
	bfd_boolean bfd_is_section_compressed_with_header
	  (bfd *abfd, asection *section,
	  int *compression_header_size_p);

DESCRIPTION
	Return @code{TRUE} if @var{section} is compressed.  Compression
	header size is returned in @var{compression_header_size_p}.  If
	compression is unsupported, compression header size is returned
	with -1.
*/

bfd_boolean
bfd_is_section_compressed_with_header (bfd *abfd, sec_ptr sec,
				       int *compression_header_size_p)
{
  bfd_byte header[MAX_COMPRESSION_HEADER_SIZE + 12];
  int compression_header_size;
  int header_size = 12;
  unsigned int saved = sec->compress_status;
  bfd_boolean compressed;

  compression_header_size = bfd_get_compression_header_size (abfd, sec);
  if (compression_header_size > MAX_COMPRESSION_HEADER_SIZE)
    abort ();
  header_size += compression_header_size;

  /* Don't decompress the section.  */
  sec->compress_status = COMPRESS_SECTION_NONE;

  /* Read the zlib header.  In this case, it should be "ZLIB" followed
     by the uncompressed section size, 8 bytes in big-endian order.  */
  compressed = bfd_get_section_contents (abfd, sec, header, 0,
					 header_size)
		&& CONST_STRNEQ ((char*) header + compression_header_size,
				 "ZLIB");

  if (compressed)
    {
      if (compression_header_size != 0)
	{
	  bfd_size_type uncompressed_size
	    = bfd_getb64 ((bfd_byte *) header
			  + compression_header_size + 4);
	  if (!bfd_check_compression_header (abfd, header, sec,
					     uncompressed_size))
	    compression_header_size = -1;
	}
      /* Check for the pathalogical case of a debug string section that
	 contains the string ZLIB.... as the first entry.  We assume that
	 no uncompressed .debug_str section would ever be big enough to
	 have the first byte of its (big-endian) size be non-zero.  */
      else if (strcmp (sec->name, ".debug_str") == 0
	       && ISPRINT (header[compression_header_size + 4]))
	compressed = FALSE;
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
	bfd_boolean bfd_is_section_compressed
	  (bfd *abfd, asection *section);

DESCRIPTION
	Return @code{TRUE} if @var{section} is compressed.
*/

bfd_boolean
bfd_is_section_compressed (bfd *abfd, sec_ptr sec)
{
  int compression_header_size;
  return (bfd_is_section_compressed_with_header (abfd, sec,
						 &compression_header_size)
	  && compression_header_size >= 0);
}

/*
FUNCTION
	bfd_init_section_decompress_status

SYNOPSIS
	bfd_boolean bfd_init_section_decompress_status
	  (bfd *abfd, asection *section);

DESCRIPTION
	Record compressed section size, update section size with
	decompressed size and set compress_status to
	DECOMPRESS_SECTION_SIZED.

	Return @code{FALSE} if the section is not a valid compressed
	section or zlib is not installed on this machine.  Otherwise,
	return @code{TRUE}.
*/

bfd_boolean
bfd_init_section_decompress_status (bfd *abfd, sec_ptr sec)
{
  bfd_byte header[MAX_COMPRESSION_HEADER_SIZE + 12];
  int compression_header_size;
  int header_size = 12;
  bfd_size_type uncompressed_size;

  compression_header_size = bfd_get_compression_header_size (abfd, sec);
  if (compression_header_size > MAX_COMPRESSION_HEADER_SIZE)
    abort ();
  header_size += compression_header_size;

  if (sec->rawsize != 0
      || sec->contents != NULL
      || sec->compress_status != COMPRESS_SECTION_NONE
      || !bfd_get_section_contents (abfd, sec, header, 0, header_size))
    {
      bfd_set_error (bfd_error_invalid_operation);
      return FALSE;
    }

  /* Read the zlib header.  In this case, it should be "ZLIB" followed
     by the uncompressed section size, 8 bytes in big-endian order.  */
  if (! CONST_STRNEQ ((char*) header + compression_header_size, "ZLIB"))
    {
      bfd_set_error (bfd_error_wrong_format);
      return FALSE;
    }

  uncompressed_size = bfd_getb64 (header + compression_header_size + 4);
  if (compression_header_size != 0
      && !bfd_check_compression_header (abfd, header, sec,
					uncompressed_size))
    {
      bfd_set_error (bfd_error_wrong_format);
      return FALSE;
    }
  sec->compressed_size = sec->size;
  sec->size = uncompressed_size;
  sec->compress_status = DECOMPRESS_SECTION_SIZED;

  return TRUE;
}

/*
FUNCTION
	bfd_init_section_compress_status

SYNOPSIS
	bfd_boolean bfd_init_section_compress_status
	  (bfd *abfd, asection *section);

DESCRIPTION
	If open for read, compress section, update section size with
	compressed size and set compress_status to COMPRESS_SECTION_DONE.

	Return @code{FALSE} if the section is not a valid compressed
	section or zlib is not installed on this machine.  Otherwise,
	return @code{TRUE}.
*/

bfd_boolean
bfd_init_section_compress_status (bfd *abfd, sec_ptr sec)
{
  bfd_size_type uncompressed_size;
  bfd_byte *uncompressed_buffer;
  bfd_boolean ret;

  /* Error if not opened for read.  */
  if (abfd->direction != read_direction
      || sec->size == 0
      || sec->rawsize != 0
      || sec->contents != NULL
      || sec->compress_status != COMPRESS_SECTION_NONE)
    {
      bfd_set_error (bfd_error_invalid_operation);
      return FALSE;
    }

  /* Read in the full section contents and compress it.  */
  uncompressed_size = sec->size;
  uncompressed_buffer = (bfd_byte *) bfd_malloc (uncompressed_size);
  if (!bfd_get_section_contents (abfd, sec, uncompressed_buffer,
				 0, uncompressed_size))
    ret = FALSE;
  else
    {
      uncompressed_size = bfd_compress_section_contents (abfd, sec,
							 uncompressed_buffer,
							 uncompressed_size);
      ret = uncompressed_size != 0;
    }

  return ret;
}
