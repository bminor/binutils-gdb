/* NLM (NetWare Loadable Module) support for BFD.
   Copyright (C) 1993 Free Software Foundation, Inc.

   Written by Fred Fish @ Cygnus Support

This file is part of BFD, the Binary File Descriptor library.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


/* This file is part of NLM support for BFD, and contains the portions
   that describe how NLM is represented externally by the BFD library.
   I.E. it describes the in-file representation of NLM.  It requires
   the nlm/common.h file which contains the portions that are common to
   both the internal and external representations.

   Note that an NLM header consists of three parts:

   (1)	A fixed length header that has specific fields of known length,
	at specific offsets in the file. 

   (2)  A variable length header that has specific fields in a specific
        order, but some fields may be variable length.

   (3)	A auxiliary header that has various optional fields in no specific
        order.  There is no way to identify the end of the auxiliary headers
	except by finding a header without a recognized 'stamp'.
*/
   
#define TARGET_LONG_SIZE 4
#define TARGET_ADDRESS_SIZE 4

/* NLM Header (32-bit implementations) */

typedef struct nlm32_external_fixed_header
{

  /* The signature field identifies the file as an NLM.  It must contain
     the string defined by NLM_SIGNATURE. */

  unsigned char signature[NLM_SIGNATURE_SIZE];

  /* The version of the header.  At this time, the highest version number
     is 4. */

  unsigned char version[TARGET_LONG_SIZE];

  /* The name of the module, which must be a DOS name (1-8 characters followed
     by a period and a 1-3 character extension).  The first byte is the byte
     length of the name and the last byte is a null terminator byte.  This
     field is fixed length, and any unused bytes should be null bytes.  The
     value is set by the OUTPUT keyword to NLMLINK. */

  unsigned char moduleName[NLM_MODULE_NAME_SIZE];

  /* The byte offset of the code image from the start of the file. */

  unsigned char codeImageOffset[TARGET_LONG_SIZE];

  /* The size of the code image, in bytes. */

  unsigned char codeImageSize[TARGET_LONG_SIZE];

  /* The byte offset of the data image from the start of the file. */

  unsigned char dataImageOffset[TARGET_LONG_SIZE];

  /* The size of the data image, in bytes. */

  unsigned char dataImageSize[TARGET_LONG_SIZE];

  /* The size of the uninitialized data region that the loader is to be
     allocated at load time.  Uninitialized data follows the initialized
     data in the NLM address space. */

  unsigned char uninitializedDataSize[TARGET_LONG_SIZE];

  /* The byte offset of the custom data from the start of the file.  The
     custom data is set by the CUSTOM keyword to NLMLINK.  It is possible
     for this to be EOF if there is no custom data. */

  unsigned char customDataOffset[TARGET_LONG_SIZE];

  /* The size of the custom data, in bytes. */

  unsigned char customDataSize[TARGET_LONG_SIZE];

  /* The byte offset of the module dependencies from the start of the file.
     The module dependencies are determined by the MODULE keyword in
     NLMLINK. */

  unsigned char moduleDependencyOffset[TARGET_LONG_SIZE];

  /* The number of module dependencies at the moduleDependencyOffset. */

  unsigned char numberOfModuleDependencies[TARGET_LONG_SIZE];

  /* The byte offset of the relocation fixup data from the start of the file */
     
  unsigned char relocationFixupOffset[TARGET_LONG_SIZE];

  unsigned char numberOfRelocationFixups[TARGET_LONG_SIZE];

  unsigned char externalReferencesOffset[TARGET_LONG_SIZE];

  unsigned char numberOfExternalReferences[TARGET_LONG_SIZE];

  unsigned char publicsOffset[TARGET_LONG_SIZE];

  unsigned char numberOfPublics[TARGET_LONG_SIZE];

  /* The byte offset of the internal debug info from the start of the file.
     It is possible for this to be EOF if there is no debug info. */

  unsigned char debugInfoOffset[TARGET_LONG_SIZE];

  unsigned char numberOfDebugRecords[TARGET_LONG_SIZE];

  unsigned char codeStartOffset[TARGET_LONG_SIZE];

  unsigned char exitProcedureOffset[TARGET_LONG_SIZE];

  unsigned char checkUnloadProcedureOffset[TARGET_LONG_SIZE];

  unsigned char moduleType[TARGET_LONG_SIZE];

  unsigned char flags[TARGET_LONG_SIZE];

} Nlm32_External_Fixed_Header;

/* The version header is one of the optional auxiliary headers and
   follows the fixed length and variable length NLM headers. */

typedef struct nlm32_external_version_header
{

  /* The header is recognized by "VeRsIoN#" in the stamp field. */
  char stamp[8];

  unsigned char majorVersion[TARGET_LONG_SIZE];

  unsigned char minorVersion[TARGET_LONG_SIZE];

  unsigned char revision[TARGET_LONG_SIZE];

  unsigned char year[TARGET_LONG_SIZE];

  unsigned char month[TARGET_LONG_SIZE];

  unsigned char day[TARGET_LONG_SIZE];

} Nlm32_External_Version_Header;


typedef struct nlm32_external_copyright_header
{

  /* The header is recognized by "CoPyRiGhT=" in the stamp field. */

  char stamp[10];

  unsigned char copyrightMessageLength[1];

  /* There is a variable length field here called 'copyrightMessage'
     that is the length specified by copyrightMessageLength. */

} Nlm32_External_Copyright_Header;


typedef struct nlm32_external_extended_header
{

  /* The header is recognized by "MeSsAgEs" in the stamp field. */

  char stamp[8];

  unsigned char languageID[TARGET_LONG_SIZE];

  unsigned char messageFileOffset[TARGET_LONG_SIZE];

  unsigned char messageFileLength[TARGET_LONG_SIZE];

  unsigned char messageCount[TARGET_LONG_SIZE];

  unsigned char helpFileOffset[TARGET_LONG_SIZE];

  unsigned char helpFileLength[TARGET_LONG_SIZE];

  unsigned char RPCDataOffset[TARGET_LONG_SIZE];

  unsigned char RPCDataLength[TARGET_LONG_SIZE];

  unsigned char sharedCodeOffset[TARGET_LONG_SIZE];

  unsigned char sharedCodeLength[TARGET_LONG_SIZE];

  unsigned char sharedDataOffset[TARGET_LONG_SIZE];

  unsigned char sharedDataLength[TARGET_LONG_SIZE];

  unsigned char sharedRelocationFixupOffset[TARGET_LONG_SIZE];

  unsigned char sharedRelocationFixupCount[TARGET_LONG_SIZE];

  unsigned char sharedExternalReferenceOffset[TARGET_LONG_SIZE];

  unsigned char sharedExternalReferenceCount[TARGET_LONG_SIZE];

  unsigned char sharedPublicsOffset[TARGET_LONG_SIZE];

  unsigned char sharedPublicsCount[TARGET_LONG_SIZE];

  unsigned char sharedInitializationOffset[TARGET_ADDRESS_SIZE];

  unsigned char SharedExitProcedureOffset[TARGET_ADDRESS_SIZE];

  unsigned char productID[TARGET_LONG_SIZE];

  unsigned char reserved0[TARGET_LONG_SIZE];

  unsigned char reserved1[TARGET_LONG_SIZE];

  unsigned char reserved2[TARGET_LONG_SIZE];

  unsigned char reserved3[TARGET_LONG_SIZE];

  unsigned char reserved4[TARGET_LONG_SIZE];

  unsigned char reserved5[TARGET_LONG_SIZE];

} Nlm32_External_Extended_Header;


typedef struct nlm32_external_custom_header
{

  /* The header is recognized by "CuStHeAd" in the stamp field. */
  char stamp[8];

  unsigned char dataLength[TARGET_LONG_SIZE];

  unsigned char debugRecOffset[TARGET_LONG_SIZE];

  unsigned char debugRecLength[TARGET_LONG_SIZE];

} Nlm32_External_Custom_Header;

