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
   
/* NLM Header */

typedef struct nlmNAME(external_fixed_header)
{

  /* The signature field identifies the file as an NLM.  It must contain
     the signature string, which depends upon the NLM target. */

  unsigned char signature[NLM_SIGNATURE_SIZE];

  /* The version of the header.  At this time, the highest version number
     is 4. */

  unsigned char version[NLM_TARGET_LONG_SIZE];

  /* The name of the module, which must be a DOS name (1-8 characters followed
     by a period and a 1-3 character extension).  The first byte is the byte
     length of the name and the last byte is a null terminator byte.  This
     field is fixed length, and any unused bytes should be null bytes.  The
     value is set by the OUTPUT keyword to NLMLINK. */

  unsigned char moduleName[NLM_MODULE_NAME_SIZE];

  /* The byte offset of the code image from the start of the file. */

  unsigned char codeImageOffset[NLM_TARGET_LONG_SIZE];

  /* The size of the code image, in bytes. */

  unsigned char codeImageSize[NLM_TARGET_LONG_SIZE];

  /* The byte offset of the data image from the start of the file. */

  unsigned char dataImageOffset[NLM_TARGET_LONG_SIZE];

  /* The size of the data image, in bytes. */

  unsigned char dataImageSize[NLM_TARGET_LONG_SIZE];

  /* The size of the uninitialized data region that the loader is to be
     allocated at load time.  Uninitialized data follows the initialized
     data in the NLM address space. */

  unsigned char uninitializedDataSize[NLM_TARGET_LONG_SIZE];

  /* The byte offset of the custom data from the start of the file.  The
     custom data is set by the CUSTOM keyword to NLMLINK.  It is possible
     for this to be EOF if there is no custom data. */

  unsigned char customDataOffset[NLM_TARGET_LONG_SIZE];

  /* The size of the custom data, in bytes. */

  unsigned char customDataSize[NLM_TARGET_LONG_SIZE];

  /* The byte offset of the module dependencies from the start of the file.
     The module dependencies are determined by the MODULE keyword in
     NLMLINK. */

  unsigned char moduleDependencyOffset[NLM_TARGET_LONG_SIZE];

  /* The number of module dependencies at the moduleDependencyOffset. */

  unsigned char numberOfModuleDependencies[NLM_TARGET_LONG_SIZE];

  /* The byte offset of the relocation fixup data from the start of the file */
     
  unsigned char relocationFixupOffset[NLM_TARGET_LONG_SIZE];

  unsigned char numberOfRelocationFixups[NLM_TARGET_LONG_SIZE];

  unsigned char externalReferencesOffset[NLM_TARGET_LONG_SIZE];

  unsigned char numberOfExternalReferences[NLM_TARGET_LONG_SIZE];

  unsigned char publicsOffset[NLM_TARGET_LONG_SIZE];

  unsigned char numberOfPublics[NLM_TARGET_LONG_SIZE];

  /* The byte offset of the internal debug info from the start of the file.
     It is possible for this to be EOF if there is no debug info. */

  unsigned char debugInfoOffset[NLM_TARGET_LONG_SIZE];

  unsigned char numberOfDebugRecords[NLM_TARGET_LONG_SIZE];

  unsigned char codeStartOffset[NLM_TARGET_LONG_SIZE];

  unsigned char exitProcedureOffset[NLM_TARGET_LONG_SIZE];

  unsigned char checkUnloadProcedureOffset[NLM_TARGET_LONG_SIZE];

  unsigned char moduleType[NLM_TARGET_LONG_SIZE];

  unsigned char flags[NLM_TARGET_LONG_SIZE];

} NlmNAME(External_Fixed_Header);

/* The version header is one of the optional auxiliary headers and
   follows the fixed length and variable length NLM headers. */

typedef struct nlmNAME(external_version_header)
{

  /* The header is recognized by "VeRsIoN#" in the stamp field. */
  char stamp[8];

  unsigned char majorVersion[NLM_TARGET_LONG_SIZE];

  unsigned char minorVersion[NLM_TARGET_LONG_SIZE];

  unsigned char revision[NLM_TARGET_LONG_SIZE];

  unsigned char year[NLM_TARGET_LONG_SIZE];

  unsigned char month[NLM_TARGET_LONG_SIZE];

  unsigned char day[NLM_TARGET_LONG_SIZE];

} NlmNAME(External_Version_Header);


typedef struct nlmNAME(external_copyright_header)
{

  /* The header is recognized by "CoPyRiGhT=" in the stamp field. */

  char stamp[10];

  unsigned char copyrightMessageLength[1];

  /* There is a variable length field here called 'copyrightMessage'
     that is the length specified by copyrightMessageLength. */

} NlmNAME(External_Copyright_Header);


typedef struct nlmNAME(external_extended_header)
{

  /* The header is recognized by "MeSsAgEs" in the stamp field. */

  char stamp[8];

  unsigned char languageID[NLM_TARGET_LONG_SIZE];

  unsigned char messageFileOffset[NLM_TARGET_LONG_SIZE];

  unsigned char messageFileLength[NLM_TARGET_LONG_SIZE];

  unsigned char messageCount[NLM_TARGET_LONG_SIZE];

  unsigned char helpFileOffset[NLM_TARGET_LONG_SIZE];

  unsigned char helpFileLength[NLM_TARGET_LONG_SIZE];

  unsigned char RPCDataOffset[NLM_TARGET_LONG_SIZE];

  unsigned char RPCDataLength[NLM_TARGET_LONG_SIZE];

  unsigned char sharedCodeOffset[NLM_TARGET_LONG_SIZE];

  unsigned char sharedCodeLength[NLM_TARGET_LONG_SIZE];

  unsigned char sharedDataOffset[NLM_TARGET_LONG_SIZE];

  unsigned char sharedDataLength[NLM_TARGET_LONG_SIZE];

  unsigned char sharedRelocationFixupOffset[NLM_TARGET_LONG_SIZE];

  unsigned char sharedRelocationFixupCount[NLM_TARGET_LONG_SIZE];

  unsigned char sharedExternalReferenceOffset[NLM_TARGET_LONG_SIZE];

  unsigned char sharedExternalReferenceCount[NLM_TARGET_LONG_SIZE];

  unsigned char sharedPublicsOffset[NLM_TARGET_LONG_SIZE];

  unsigned char sharedPublicsCount[NLM_TARGET_LONG_SIZE];

  unsigned char sharedInitializationOffset[NLM_TARGET_ADDRESS_SIZE];

  unsigned char SharedExitProcedureOffset[NLM_TARGET_ADDRESS_SIZE];

  unsigned char productID[NLM_TARGET_LONG_SIZE];

  unsigned char reserved0[NLM_TARGET_LONG_SIZE];

  unsigned char reserved1[NLM_TARGET_LONG_SIZE];

  unsigned char reserved2[NLM_TARGET_LONG_SIZE];

  unsigned char reserved3[NLM_TARGET_LONG_SIZE];

  unsigned char reserved4[NLM_TARGET_LONG_SIZE];

  unsigned char reserved5[NLM_TARGET_LONG_SIZE];

} NlmNAME(External_Extended_Header);


typedef struct nlmNAME(external_custom_header)
{

  /* The header is recognized by "CuStHeAd" in the stamp field. */
  char stamp[8];

  unsigned char dataLength[NLM_TARGET_LONG_SIZE];

  unsigned char debugRecOffset[NLM_TARGET_LONG_SIZE];

  unsigned char debugRecLength[NLM_TARGET_LONG_SIZE];

} NlmNAME(External_Custom_Header);
