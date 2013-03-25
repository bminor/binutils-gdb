/* CTF format support.

   Copyright (C) 2012-2013 Free Software Foundation, Inc.
   Contributed by Hui Zhu <hui_zhu@mentor.com>
   Contributed by Yao Qi <yao@codesourcery.com>

   This file is part of GDB.

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

#include "defs.h"
#include "ctf.h"
#include "tracepoint.h"
#include "regcache.h"
#include "gdb_stat.h"

#include <ctype.h>

/* GDB saves trace buffers and other information (such as trace
   status) got from the remote target into Common Trace Format (CTF).
   The following types of information are expected to save in CTF:

   1. The length (in bytes) of register cache.  Event "register" will
   be defined in metadata, which includes the length.

   2. Trace status.  Not implemented yet in CTF writer.

   3. Uploaded trace variables and tracepoints.  Not implemented yet
   in CTF writer.

   4. Trace frames.  Each trace frame is composed by several blocks
   of different types ('R', 'M', 'V').  One trace frame is saved in
   one CTF packet and the blocks of this frame are saved as events.
   4.1: The trace frame related information (such as the number of
   tracepoint associated with this frame) is saved in the packet
   context.
   4.2: The block 'M', 'R' and 'V' are saved in event "memory",
   "register" and "tsv" respectively.
   4.3: When iterating over events, babeltrace can't tell iterator
   goes to a new packet, so we need a marker or anchor to tell GDB
   that iterator goes into a new packet or frame.  We define event
   "frame".  */

#define CTF_MAGIC		0xC1FC1FC1
#define CTF_SAVE_MAJOR		1
#define CTF_SAVE_MINOR		8

#define CTF_METADATA_NAME	"metadata"
#define CTF_DATASTREAM_NAME	"datastream"

/* Reserved event id.  */

#define CTF_EVENT_ID_REGISTER 0
#define CTF_EVENT_ID_TSV 1
#define CTF_EVENT_ID_MEMORY 2
#define CTF_EVENT_ID_FRAME 3

/* The state kept while writing the CTF datastream file.  */

struct trace_write_handler
{
  /* File descriptor of metadata.  */
  FILE *metadata_fd;
  /* File descriptor of traceframes.  */
  FILE *datastream_fd;

  /* This is the content size of the current packet.  */
  size_t content_size;

  /* This is the start offset of current packet.  */
  long packet_start;
};

/* Write metadata in FORMAT.  */

static void
ctf_save_write_metadata (struct trace_write_handler *handler,
			 const char *format, ...)
{
  va_list args;

  va_start (args, format);
  if (vfprintf (handler->metadata_fd, format, args) < 0)
    error (_("Unable to write metadata file (%s)"),
	     safe_strerror (errno));
  va_end (args);
}

/* Write BUF of length SIZE to datastream file represented by
   HANDLER.  */

static int
ctf_save_write (struct trace_write_handler *handler,
		const gdb_byte *buf, size_t size)
{
  if (fwrite (buf, size, 1, handler->datastream_fd) != 1)
    error (_("Unable to write file for saving trace data (%s)"),
	   safe_strerror (errno));

  handler->content_size += size;

  return 0;
}

/* Write a unsigned 32-bit integer to datastream file represented by
   HANDLER.  */

#define ctf_save_write_uint32(HANDLER, U32) \
  ctf_save_write (HANDLER, (gdb_byte *) &U32, 4)

/* Set datastream file position.  Update HANDLER->content_size
   if WHENCE is SEEK_CUR.  */

static int
ctf_save_fseek (struct trace_write_handler *handler, long offset,
		int whence)
{
  gdb_assert (whence != SEEK_END);
  gdb_assert (whence != SEEK_SET
	      || offset <= handler->content_size + handler->packet_start);

  if (fseek (handler->datastream_fd, offset, whence))
    error (_("Unable to seek file for saving trace data (%s)"),
	   safe_strerror (errno));

  if (whence == SEEK_CUR)
    handler->content_size += offset;

  return 0;
}

/* Change the datastream file position to align on ALIGN_SIZE,
   and write BUF to datastream file.  The size of BUF is SIZE.  */

static int
ctf_save_align_write (struct trace_write_handler *handler,
		      const gdb_byte *buf,
		      size_t size, size_t align_size)
{
  long offset
    = (align_up (handler->content_size, align_size)
       - handler->content_size);

  if (ctf_save_fseek (handler, offset, SEEK_CUR))
    return -1;

  if (ctf_save_write (handler, buf, size))
    return -1;

  return 0;
}

/* Write events to next new packet.  */

static void
ctf_save_next_packet (struct trace_write_handler *handler)
{
  handler->packet_start += (handler->content_size + 4);
  ctf_save_fseek (handler, handler->packet_start, SEEK_SET);
  handler->content_size = 0;
}

/* Write the CTF metadata header.  */

static void
ctf_save_metadata_header (struct trace_write_handler *handler)
{
  const char metadata_fmt[] =
  "\ntrace {\n"
  "	major = %u;\n"
  "	minor = %u;\n"
  "	byte_order = %s;\n"		/* be or le */
  "	packet.header := struct {\n"
  "		uint32_t magic;\n"
  "	};\n"
  "};\n"
  "\n"
  "stream {\n"
  "	packet.context := struct {\n"
  "		uint32_t content_size;\n"
  "		uint32_t packet_size;\n"
  "		uint16_t tpnum;\n"
  "	};\n"
  "	event.header := struct {\n"
  "		uint32_t id;\n"
  "	};\n"
  "};\n";

  ctf_save_write_metadata (handler, "/* CTF %d.%d */\n",
			   CTF_SAVE_MAJOR, CTF_SAVE_MINOR);
  ctf_save_write_metadata (handler,
			   "typealias integer { size = 8; align = 8; "
			   "signed = false; encoding = ascii;}"
			   " := ascii;\n");
  ctf_save_write_metadata (handler,
			   "typealias integer { size = 8; align = 8; "
			   "signed = false; }"
			   " := uint8_t;\n");
  ctf_save_write_metadata (handler,
			   "typealias integer { size = 16; align = 16;"
			   "signed = false; } := uint16_t;\n");
  ctf_save_write_metadata (handler,
			   "typealias integer { size = 32; align = 32;"
			   "signed = false; } := uint32_t;\n");
  ctf_save_write_metadata (handler,
			   "typealias integer { size = 64; align = 64;"
			   "signed = false; base = hex;}"
			   " := uint64_t;\n");
  ctf_save_write_metadata (handler, "\n");

  /* Get the byte order of the host and write CTF data in this byte
     order.  */
#if WORDS_BIGENDIAN
#define HOST_ENDIANNESS "be"
#else
#define HOST_ENDIANNESS "le"
#endif

  ctf_save_write_metadata (handler, metadata_fmt,
			   CTF_SAVE_MAJOR, CTF_SAVE_MINOR,
			   HOST_ENDIANNESS);
  ctf_save_write_metadata (handler, "\n");
}

/* CTF trace writer.  */

struct ctf_trace_file_writer
{
  struct trace_file_writer base;

  /* States related to writing CTF trace file.  */
  struct trace_write_handler tcs;
};

/* This is the implementation of trace_file_write_ops method
   dtor.  */

static void
ctf_dtor (struct trace_file_writer *self)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;

  if (writer->tcs.metadata_fd != NULL)
    fclose (writer->tcs.metadata_fd);

  if (writer->tcs.datastream_fd != NULL)
    fclose (writer->tcs.datastream_fd);

}

/* This is the implementation of trace_file_write_ops method
   target_save.  */

static int
ctf_target_save (struct trace_file_writer *self,
		 const char *dirname)
{
  /* Don't support save trace file to CTF format in the target.  */
  return 0;
}

#ifdef USE_WIN32API
#undef mkdir
#define mkdir(pathname, mode) mkdir (pathname)
#endif

/* This is the implementation of trace_file_write_ops method
   start.  It creates the directory DIRNAME, metadata and datastream
   in the directory.  */

static void
ctf_start (struct trace_file_writer *self, const char *dirname)
{
  char *file_name;
  struct cleanup *old_chain;
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  int i;
  mode_t hmode = S_IRUSR | S_IWUSR | S_IXUSR
#ifdef S_IRGRP
    | S_IRGRP
#endif
#ifdef S_IXGRP
    | S_IXGRP
#endif
    | S_IROTH /* Defined in common/gdb_stat.h if not defined.  */
#ifdef S_IXOTH
    | S_IXOTH
#endif
    ;

  /* Create DIRNAME.  */
  if (mkdir (dirname, hmode) && errno != EEXIST)
    error (_("Unable to open directory '%s' for saving trace data (%s)"),
	   dirname, safe_strerror (errno));

  memset (&writer->tcs, '\0', sizeof (writer->tcs));

  file_name = xstrprintf ("%s/%s", dirname, CTF_METADATA_NAME);
  old_chain = make_cleanup (xfree, file_name);

  writer->tcs.metadata_fd = fopen (file_name, "w");
  if (writer->tcs.metadata_fd == NULL)
    error (_("Unable to open file '%s' for saving trace data (%s)"),
	   file_name, safe_strerror (errno));
  do_cleanups (old_chain);

  ctf_save_metadata_header (&writer->tcs);

  file_name = xstrprintf ("%s/%s", dirname, CTF_DATASTREAM_NAME);
  old_chain = make_cleanup (xfree, file_name);
  writer->tcs.datastream_fd = fopen (file_name, "w");
  if (writer->tcs.datastream_fd == NULL)
    error (_("Unable to open file '%s' for saving trace data (%s)"),
	   file_name, safe_strerror (errno));
  do_cleanups (old_chain);
}

/* This is the implementation of trace_file_write_ops method
   write_header.  Write the types of events on trace variable and
   frame.  */

static void
ctf_write_header (struct trace_file_writer *self)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;


  ctf_save_write_metadata (&writer->tcs, "\n");
  ctf_save_write_metadata (&writer->tcs,
			   "event {\n\tname = \"memory\";\n\tid = %u;\n"
			   "\tfields := struct { \n"
			   "\t\tuint64_t address;\n"
			   "\t\tuint16_t length;\n"
			   "\t\tuint8_t contents[length];\n"
			   "\t};\n"
			   "};\n", CTF_EVENT_ID_MEMORY);

  ctf_save_write_metadata (&writer->tcs, "\n");
  ctf_save_write_metadata (&writer->tcs,
			   "event {\n\tname = \"tsv\";\n\tid = %u;\n"
			   "\tfields := struct { \n"
			   "\t\tuint64_t val;\n"
			   "\t\tuint32_t num;\n"
			   "\t};\n"
			   "};\n", CTF_EVENT_ID_TSV);

  ctf_save_write_metadata (&writer->tcs, "\n");
  ctf_save_write_metadata (&writer->tcs,
			   "event {\n\tname = \"frame\";\n\tid = %u;\n"
			   "\tfields := struct { \n"
			   "\t};\n"
			   "};\n", CTF_EVENT_ID_FRAME);

  gdb_assert (writer->tcs.content_size == 0);
  gdb_assert (writer->tcs.packet_start == 0);
}

/* This is the implementation of trace_file_write_ops method
   write_regblock_type.  Write the type of register event in
   metadata.  */

static void
ctf_write_regblock_type (struct trace_file_writer *self, int size)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;

  ctf_save_write_metadata (&writer->tcs, "\n");

  ctf_save_write_metadata (&writer->tcs,
			   "event {\n\tname = \"register\";\n\tid = %u;\n"
			   "\tfields := struct { \n"
			   "\t\tascii contents[%d];\n"
			   "\t};\n"
			   "};\n",
			   CTF_EVENT_ID_REGISTER, size);
}

/* This is the implementation of trace_file_write_ops method
   write_status.  */

static void
ctf_write_status (struct trace_file_writer *self,
		  struct trace_status *ts)
{
  /* It is not supported yet to write trace status into CTF trace
     data.  */
}

/* This is the implementation of trace_file_write_ops method
   write_uploaded_tsv.  */

static void
ctf_write_uploaded_tsv (struct trace_file_writer *self,
			struct uploaded_tsv *tsv)
{
  /* It is not supported yet to write uploaded trace variables
     into CTF trace data.  */
}

/* This is the implementation of trace_file_write_ops method
   write_uploaded_tp.  */

static void
ctf_write_uploaded_tp (struct trace_file_writer *self,
		       struct uploaded_tp *tp)
{
  /* It is not supported yet to write uploaded tracepoints
     into CTF trace data.  */
}

/* This is the implementation of trace_file_write_ops method
   write_definition_end.  */

static void
ctf_write_definition_end (struct trace_file_writer *self)
{
  /* Nothing to do for CTF.  */
}

/* The minimal file size of data stream.  It is required by
   babeltrace.  */

#define CTF_FILE_MIN_SIZE		4096

/* This is the implementation of trace_file_write_ops method
   end.  */

static void
ctf_end (struct trace_file_writer *self)
{
  struct ctf_trace_file_writer *writer = (struct ctf_trace_file_writer *) self;

  gdb_assert (writer->tcs.content_size == 0);
  /* The babeltrace requires or assumes that the size of datastream
     file is greater than 4096 bytes.  If we don't generate enough
     packets and events, create a fake packet which has zero event,
      to use up the space.  */
  if (writer->tcs.packet_start < CTF_FILE_MIN_SIZE)
    {
      uint32_t u32;

      /* magic.  */
      u32 = CTF_MAGIC;
      ctf_save_write_uint32 (&writer->tcs, u32);

      /* content_size.  */
      u32 = 0;
      ctf_save_write_uint32 (&writer->tcs, u32);

      /* packet_size.  */
      u32 = 12;
      if (writer->tcs.packet_start + u32 < CTF_FILE_MIN_SIZE)
	u32 = CTF_FILE_MIN_SIZE - writer->tcs.packet_start;

      u32 *= TARGET_CHAR_BIT;
      ctf_save_write_uint32 (&writer->tcs, u32);

      /* tpnum.  */
      u32 = 0;
      ctf_save_write (&writer->tcs, (gdb_byte *) &u32, 2);

      /* Enlarge the file to CTF_FILE_MIN_SIZE is it is still less
	 than that.  */
      if (CTF_FILE_MIN_SIZE
	  > (writer->tcs.packet_start + writer->tcs.content_size))
	{
	  gdb_byte b = 0;

	  /* Fake the content size to avoid assertion failure in
	     ctf_save_fseek.  */
	  writer->tcs.content_size = (CTF_FILE_MIN_SIZE
				      - 1 - writer->tcs.packet_start);
	  ctf_save_fseek (&writer->tcs, CTF_FILE_MIN_SIZE - 1,
			  SEEK_SET);
	  ctf_save_write (&writer->tcs, &b, 1);
	}
    }
}

/* This is the implementation of trace_frame_write_ops method
   start.  */

static void
ctf_write_frame_start (struct trace_file_writer *self, uint16_t tpnum)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  uint32_t id = CTF_EVENT_ID_FRAME;
  uint32_t u32;

  /* Step 1: Write packet context.  */
  /* magic.  */
  u32 = CTF_MAGIC;
  ctf_save_write_uint32 (&writer->tcs, u32);
  /* content_size and packet_size..  We still don't know the value,
     write it later.  */
  ctf_save_fseek (&writer->tcs, 4, SEEK_CUR);
  ctf_save_fseek (&writer->tcs, 4, SEEK_CUR);
  /* Tracepoint number.  */
  ctf_save_write (&writer->tcs, (gdb_byte *) &tpnum, 2);

  /* Step 2: Write event "frame".  */
  /* Event Id.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &id, 4, 4);
}

/* This is the implementation of trace_frame_write_ops method
   write_r_block.  */

static void
ctf_write_frame_r_block (struct trace_file_writer *self,
			 gdb_byte *buf, int32_t size)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  uint32_t id = CTF_EVENT_ID_REGISTER;

  /* Event Id.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &id, 4, 4);

  /* array contents.  */
  ctf_save_align_write (&writer->tcs, buf, size, 1);
}

/* This is the implementation of trace_frame_write_ops method
   write_m_block_header.  */

static void
ctf_write_frame_m_block_header (struct trace_file_writer *self,
				uint64_t addr, uint16_t length)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  uint32_t event_id = CTF_EVENT_ID_MEMORY;

  /* Event Id.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &event_id, 4, 4);

  /* Address.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &addr, 8, 8);

  /* Length.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &length, 2, 2);
}

/* This is the implementation of trace_frame_write_ops method
   write_m_block_memory.  */

static void
ctf_write_frame_m_block_memory (struct trace_file_writer *self,
				gdb_byte *buf, uint16_t length)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;

  /* Contents.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) buf, length, 1);
}

/* This is the implementation of trace_frame_write_ops method
   write_v_block.  */

static void
ctf_write_frame_v_block (struct trace_file_writer *self,
			 int32_t num, uint64_t val)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  uint32_t id = CTF_EVENT_ID_TSV;

  /* Event Id.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &id, 4, 4);

  /* val.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &val, 8, 8);
  /* num.  */
  ctf_save_align_write (&writer->tcs, (gdb_byte *) &num, 4, 4);
}

/* This is the implementation of trace_frame_write_ops method
   end.  */

static void
ctf_write_frame_end (struct trace_file_writer *self)
{
  struct ctf_trace_file_writer *writer
    = (struct ctf_trace_file_writer *) self;
  uint32_t u32;
  uint32_t t;

  /* Write the content size to packet header.  */
  ctf_save_fseek (&writer->tcs, writer->tcs.packet_start + 4,
		  SEEK_SET);
  u32 = writer->tcs.content_size * TARGET_CHAR_BIT;

  t = writer->tcs.content_size;
  ctf_save_write_uint32 (&writer->tcs, u32);

  /* Write the packet size.  */
  u32 += 4 * TARGET_CHAR_BIT;
  ctf_save_write_uint32 (&writer->tcs, u32);

  writer->tcs.content_size = t;

  /* Write zero at the end of the packet.  */
  ctf_save_fseek (&writer->tcs, writer->tcs.packet_start + t,
		  SEEK_SET);
  u32 = 0;
  ctf_save_write_uint32 (&writer->tcs, u32);
  writer->tcs.content_size = t;

  ctf_save_next_packet (&writer->tcs);
}

/* Operations to write various types of trace frames into CTF
   format.  */

static const struct trace_frame_write_ops ctf_write_frame_ops =
{
  ctf_write_frame_start,
  ctf_write_frame_r_block,
  ctf_write_frame_m_block_header,
  ctf_write_frame_m_block_memory,
  ctf_write_frame_v_block,
  ctf_write_frame_end,
};

/* Operations to write trace buffers into CTF format.  */

static const struct trace_file_write_ops ctf_write_ops =
{
  ctf_dtor,
  ctf_target_save,
  ctf_start,
  ctf_write_header,
  ctf_write_regblock_type,
  ctf_write_status,
  ctf_write_uploaded_tsv,
  ctf_write_uploaded_tp,
  ctf_write_definition_end,
  NULL,
  &ctf_write_frame_ops,
  ctf_end,
};

/* Return a trace writer for CTF format.  */

struct trace_file_writer *
ctf_trace_file_writer_new (void)
{
  struct ctf_trace_file_writer *writer
    = xmalloc (sizeof (struct ctf_trace_file_writer));

  writer->base.ops = &ctf_write_ops;

  return (struct trace_file_writer *) writer;
}
