/* Target dependent code for ARC processor family, for GDB, the GNU debugger.

   Copyright 2008, 2009 Free Software Foundation, Inc.

   Contributed by ARC International (www.arc.com)

   Author:
      Richard Stuckey <richard.stuckey@arc.com>

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

/******************************************************************************/
/*                                                                            */
/* Outline:                                                                   */
/*     This module implements operations for manipulating the ARC processor   */
/*     core registers and auxiliary registers.                                */
/*                                                                            */
/******************************************************************************/

/* system header files */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

/* gdb header files */
#include "defs.h"
#include "gdbcmd.h"
#include "regcache.h"
#include "objfiles.h"
#include "inferior.h"
#include "target.h"
#include "xml-support.h"
#include "gdb_assert.h"

/* ARC header files */
#include "arc-registers.h"
#include "arc-architecture.h"
#include "arc-tdep.h"
#include "arc-elf32-tdep.h"
#include "config/arc/tm-embed.h"

/* ARC simulator header files */
#include "sim/arc/arc-sim-registers.h"


/* -------------------------------------------------------------------------- */
/*                               local types                                  */
/* -------------------------------------------------------------------------- */

typedef struct field_meaning
{
    char                *description;
    ARC_RegisterContents value;
} ARC_FieldMeaning;


typedef struct field_definition
{
    char                *name;
    char                *description;
    unsigned int         offset;
    unsigned int         size;
    RegisterAccess       access;
    ARC_RegisterContents value_for_write;
    Boolean              fixed;
    ARC_FieldMeaning    *meanings;
    unsigned int         meaning_count;
} ARC_FieldDefinition;


/* Complete the type (declared in the header file) here.  */
struct aux_register_definition
{
    char                *name;
    char                *description;
    ARC_RegisterNumber   number;    /* The number in the auxiliary register space.  */
    int                  gdb_regno;
    Boolean              is_BCR;
    ARC_Word             mask;
    RegisterAccess       access;
    ARC_FieldDefinition *fields;
    unsigned int         field_count;
    unsigned int         longest_field_name;
    unsigned int         max_bits_in_field;
};


/* An instance of this structure is used to pass state data
   between the parsing routines.

   The information is accumulated into the 'info' * field;
   as each register or field description is parsed, the
   information is held in the 'reg' or 'field' field before
   validation is performed.  If the information is valid,
   it is copied into the 'info' structure.  */
typedef struct parsing_data
{
    const char                *filename;
    ARC_RegisterInfo          *info;
    ARC_AuxRegisterDefinition *currentRegister;
    ARC_FieldDefinition       *currentField;
    ARC_RegisterContents       maxFieldContents;
    ARC_AuxRegisterDefinition  reg;
    ARC_FieldDefinition        field;
} ParsingData;


/* -------------------------------------------------------------------------- */
/*                               forward declarations                         */
/* -------------------------------------------------------------------------- */

static gdb_xml_element_start_handler start_architecture;
static gdb_xml_element_end_handler   end_architecture;
static gdb_xml_element_start_handler start_feature;
static gdb_xml_element_start_handler start_target;
static gdb_xml_element_start_handler start_auxregister;
static gdb_xml_element_start_handler start_bcr;
static gdb_xml_element_start_handler start_ecr;
static gdb_xml_element_start_handler start_field;
static gdb_xml_element_start_handler start_bcrfield;
static gdb_xml_element_start_handler start_meaning;


/* -------------------------------------------------------------------------- */
/*                               externally visible data                      */
/* -------------------------------------------------------------------------- */

/* This indicates whether a 'register architecture changed' event must be sent,
   but has not yet been sent.  */
Boolean arc_pending_register_architecture_change_event;


/* -------------------------------------------------------------------------- */
/*                               local data                                   */
/* -------------------------------------------------------------------------- */

#define REG_READ_FILE_COMMAND               "arc-reg-read-file"
#define REG_READ_EXTRA_FILE_COMMAND         "arc-reg-read-extra-file"
#define AUX_REG_READ_COMMAND                "arc-aux-read"
#define AUX_REG_WRITE_COMMAND               "arc-aux-write"
#define AUX_REG_SHOW_COMMAND                "arc-aux-show"
#define AUX_LIST_REGISTER_COMMAND           "arc-aux-list"
#define ARC_BCR_COMMAND                     "arc-bcr-registers"

#define REG_READ_FILE_COMMAND_USAGE         "Usage: "      REG_READ_FILE_COMMAND       " <FILE>\n"
#define REG_READ_EXTRA_FILE_COMMAND_USAGE   "Usage: "      REG_READ_EXTRA_FILE_COMMAND " <FILE>\n"
#define AUX_REG_READ_COMMAND_USAGE          "Usage: "      AUX_REG_READ_COMMAND        " <REG-FROM> [ <REG-TO> ]\n"
#define AUX_REG_WRITE_COMMAND_USAGE         "Usage: "      AUX_REG_WRITE_COMMAND       " <REG> = <VALUE>\n"
#define AUX_REG_SHOW_COMMAND_USAGE          "Usage: "      AUX_REG_SHOW_COMMAND        " [ <REG> ] \n"
#define AUX_LIST_REGISTER_COMMAND_USAGE     "Usage: "      AUX_LIST_REGISTER_COMMAND   " [ <REG> ]\n"
#define ARC_BCR_COMMAND_USAGE               "Usage: info " ARC_BCR_COMMAND             "\n"


#define ELEMENT_END_MARKER       { NULL, NULL, NULL, GDB_XML_EF_NONE, NULL, NULL }
#define ATTRIBUTE_END_MARKER     { NULL,             GDB_XML_AF_NONE, NULL, NULL }


/* Special values for the name and description fields.  */
static const char *NO_DESCRIPTION = "";
static const char *RESERVED       = "<reserved>";
static const char *UNUSED         = "<unused>";


/* The elements and attributes of an XML target description.  */

/* A handler_data for access values.  */

static const struct gdb_xml_enum enums_access[] =
{
    { "RO", READ_ONLY  },
    { "RW", READ_WRITE },
    { "WO", WRITE_ONLY },
    { NULL, 0          }
};


static const struct gdb_xml_attribute aux_register_attributes[] =
{
    { "name",         GDB_XML_AF_NONE,     NULL,                        NULL         },
    { "description",  GDB_XML_AF_NONE,     NULL,                        NULL         },
    { "number",       GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "mask",         GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "access",       GDB_XML_AF_OPTIONAL, gdb_xml_parse_attr_enum,     enums_access },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_attribute core_register_attributes[] =
{
    { "number",       GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "mask",         GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "access",       GDB_XML_AF_OPTIONAL, gdb_xml_parse_attr_enum,     enums_access },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_attribute field_attributes[] =
{
    { "name",         GDB_XML_AF_NONE,     NULL,                        NULL         },
    { "description",  GDB_XML_AF_OPTIONAL, NULL,                        NULL         },
    { "onwrite",      GDB_XML_AF_OPTIONAL, gdb_xml_parse_attr_ulongest, NULL         },
    { "offset",       GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "size",         GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "access",       GDB_XML_AF_OPTIONAL, gdb_xml_parse_attr_enum,     enums_access },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_attribute bcrfield_attributes[] =
{
    { "name",         GDB_XML_AF_NONE,     NULL,                        NULL         },
    { "description",  GDB_XML_AF_OPTIONAL, NULL,                        NULL         },
    { "offset",       GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    { "size",         GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_attribute meaning_attributes[] =
{
    { "description",  GDB_XML_AF_NONE,     NULL,                        NULL         },
    { "value",        GDB_XML_AF_NONE,     gdb_xml_parse_attr_ulongest, NULL         },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_attribute feature_attributes[] =
{
    { "name", GDB_XML_AF_NONE, NULL, NULL },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_element field_children[] =
{
    { "meaning", meaning_attributes, NULL, GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_meaning, NULL },
    ELEMENT_END_MARKER
};


static const struct gdb_xml_element auxregister_children[] =
{
    { "field", field_attributes, field_children, GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_field, NULL },
    ELEMENT_END_MARKER
};


static const struct gdb_xml_element bcr_children[] =
{
    { "bcrfield", bcrfield_attributes, NULL, GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_bcrfield, NULL },
    ELEMENT_END_MARKER
};


static const struct gdb_xml_element feature_children[] =
{
    { "auxregister", aux_register_attributes,  auxregister_children, GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_auxregister, NULL },
    { "bcr",         aux_register_attributes,  bcr_children,         GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_bcr,         NULL },
    { "ecr",         core_register_attributes, NULL,                 GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE, start_ecr,         NULL },
    ELEMENT_END_MARKER
};


static const struct gdb_xml_attribute target_attributes[] =
{
    { "version", GDB_XML_AF_NONE, NULL, NULL },
    ATTRIBUTE_END_MARKER
};


static const struct gdb_xml_element target_children[] =
{
    { "architecture", NULL, NULL, GDB_XML_EF_OPTIONAL, start_architecture, end_architecture },
    { "feature",      feature_attributes, feature_children,
                      GDB_XML_EF_OPTIONAL | GDB_XML_EF_REPEATABLE,
                      start_feature, NULL },
    ELEMENT_END_MARKER
};


static const struct gdb_xml_element elements[] =
{
    { "target", target_attributes, target_children, GDB_XML_EF_NONE, start_target, NULL },
    ELEMENT_END_MARKER
};


/* -------------------------------------------------------------------------- */
/*                               local macros                                 */
/* -------------------------------------------------------------------------- */

#define NAME_IS(ident)    (strcmp(name, ident) == 0)

#define INFO_OF(gdbarch)  &gdbarch_tdep (gdbarch)->processor_variant_info->registers

#define EXTRACT(argument, type, result)                                    \
{                                                                          \
    struct expression *expr  = parse_expression(argument);                 \
    struct value      *val   = evaluate_expression(expr);                  \
    struct cleanup    *chain = make_cleanup(free_current_contents, &expr); \
                                                                           \
    result = *(type*) (value_contents (val));                              \
    do_cleanups (chain);                                                   \
}

#define FIND_REGISTER_DEFINITION_SUCH_THAT(condition)             \
{                                                                 \
    ARC_RegisterInfo *info = find_info(TRUE);                     \
    unsigned int      i;                                          \
                                                                  \
    for (i = 0; i < info->aux_register_count; i++)                \
    {                                                             \
        ARC_AuxRegisterDefinition *def = &info->aux_registers[i]; \
                                                                  \
        if (condition)                                            \
            return def;                                           \
    }                                                             \
                                                                  \
    return NULL;                                                  \
}


/* -------------------------------------------------------------------------- */
/*                     local functions for XML file parsing                   */
/* -------------------------------------------------------------------------- */

/* Return a string corresponding to the given access.  */

static const char*
RegisterAccess_Image (RegisterAccess val)
{
    switch (val)
    {
        case READ_ONLY : return "read-only";
        case READ_WRITE: return "read/write";
        case WRITE_ONLY: return "write-only";
        default        : return "???";
    }
}


static void
initialize_register (ARC_AuxRegisterDefinition *reg)
{
    reg->name               = NULL;
    reg->number             = 0;
    reg->gdb_regno          = 0;
    reg->is_BCR             = FALSE;
    reg->description        = (char*) NO_DESCRIPTION;
    reg->mask               = 0xFFFFFFFF;
    reg->access             = READ_WRITE;
    reg->field_count        = 0;
    reg->fields             = NULL;
    reg->longest_field_name = 0;
    reg->max_bits_in_field  = 0;
}


static void
initialize_field (ARC_FieldDefinition *field, RegisterAccess access)
{
    field->name            = NULL;
    field->description     = (char*) NO_DESCRIPTION;
    field->value_for_write = 0;
    field->fixed           = FALSE;
    field->size            = 1;
    field->offset          = 0;
    field->access          = access;
    field->meanings        = NULL;
    field->meaning_count   = 0;
}


static void
initialize_meaning (ARC_FieldMeaning *meaning)
{
    meaning->description = (char*) NO_DESCRIPTION;
    meaning->value       = 0;
}


/* Return TRUE if the given register fields overlap within the register.  */

static Boolean
overlaps(ARC_FieldDefinition *field1, ARC_FieldDefinition *field2)
{
    unsigned int field1_start = field1->offset;
    unsigned int field1_end   = field1_start + field1->size - 1;
    unsigned int field2_start = field2->offset;
    unsigned int field2_end   = field2_start + field2->size - 1;

    return !(field2_end < field1_start || field1_end < field2_start);
}


/* Release all the storage allocated to hold the given register information,
   and re-initialize it.   */

static void
free_register_set (ARC_RegisterInfo *info)
{
    if (info->aux_registers)
    {
        unsigned int i;

        for (i = 0; i < info->aux_register_count; i++)
        {
            ARC_AuxRegisterDefinition *r = &info->aux_registers[i];

            if (r->name != UNUSED)
                xfree (r->name);
            if (r->description != NO_DESCRIPTION)
                xfree (r->description);

            if (r->fields)
            {
                unsigned int j;

                for (j = 0; j < r->field_count; j++)
                {
                    ARC_FieldDefinition *f = &r->fields[j];

                    if (f->name != RESERVED)
                        xfree (f->name);

                    if (f->description != NO_DESCRIPTION)
                        xfree (f->description);

                    if (f->meanings)
                    {
                        unsigned int k;

                        for (k = 0; k < f->meaning_count; k++)
                        {
                            ARC_FieldMeaning *m = &f->meanings[k];

                            if (m->description != NO_DESCRIPTION)
                                xfree (m->description);
                        }

                        xfree(f->meanings);
                    }
                }
            }

            xfree(r->fields);
        }

        xfree(info->aux_registers);

        arc_initialize_aux_reg_info(info);
    }
}


/* This function reads the contents of a file.

   Parameters:
      filename : the name of the file to be accessed
      baton    : the path to the directory containing the file

   Result:
      If the file contents were successfully read, a pointer to a buffer
      containing those contents; otherwise NULL.  */

static char*
read_file_contents (const char *filename, void *baton)
{
    char       *name     = (char*) filename;
    const char *dirname  = baton;
    char       *contents = NULL;
    int         fd;

    if ((dirname != NULL) && (*dirname != '\0'))
    {
        name = concat (dirname, "/", filename, NULL);

        if (name == NULL)
        {
            /* N.B. this does not return */
            nomem (0);
        }
    }

    if ((fd = open (name, O_RDONLY)) != -1)
    {
        struct stat stat;
        int         status;

        if ((status = fstat(fd, &stat)) != -1)
        {
            size_t size = (size_t) stat.st_size;

            /* Allocate buffer to hold the file contents; note that this space
               is deliberately made larger than required, so that it is possible
               to add extra characters to the end of the data.  */
            if ((contents = xmalloc (size + 10)))
            {
                ssize_t bytes;

                if ((bytes = read (fd, contents, size)) == (ssize_t) size)
                {
                    /* Append an explicit end-of-line to the data, in case there
                       was not one there already; also, explicitly NUL-terminate
                       the data so that it is a valid string.  */
                    contents[bytes]     = '\n';
                    contents[bytes + 1] = '\0';
                }
                else
                {
                    warning (_("can not read contents of file '%s': %s"),
                             name, strerror (errno));

                    /* Cannot trust the contents of the buffer.  */
                    xfree (contents);
                    contents = NULL;
                }

                if ((status = close (fd)) != 0)
                    warning (_("can not close file '%s': %s"), name,
                             strerror (errno));
            }
            else
                warning (_("can not allocate buffer to hold contents of file '%s'"), name);
        }
        else
            warning (_("can not get size of file '%s': %s"), name,
                     strerror (errno));
    }
    else
        warning (_("can not open file '%s': %s"), name, strerror (errno));

    if (name != filename)
        xfree (name);

    return contents;
}


/* This function is called by the XML parser when the start of a <target> element is seen.  */

static void
start_target (struct gdb_xml_parser        *parser,
              const struct gdb_xml_element *element,
              void                         *user_data,
              VEC(gdb_xml_value_s)         *attributes)
{
//  ParsingData* data = user_data;
    char *version = VEC_index (gdb_xml_value_s, attributes, 0)->value;

    if (strcmp (version, "1.0") != 0)
        gdb_xml_error (parser,
                       _("Registers description has unsupported version \"%s\""),
                       version);

    DEBUG("target started\n");
}


/* This function is called by the XML parser when the start of an <architecture> element is seen.  */

static void
start_architecture (struct gdb_xml_parser        *parser,
                    const struct gdb_xml_element *element,
                    void                         *user_data,
                    VEC(gdb_xml_value_s)         *attributes)
{
    DEBUG("architecture started\n");
}


/* This function is called by the XML parser when the end of an <architecture> element is seen.  */

static void
end_architecture (struct gdb_xml_parser        *parser,
                  const struct gdb_xml_element *element,
                  void                         *user_data,
                  const char                   *body_text)
{
    ParsingData *data = user_data;

    if (body_text[0] != '\0')
    {
        data->info->processor = arc_version(body_text);

        if (data->info->processor == UNSUPPORTED_ARCHITECTURE)
            warning(_("unknown target architecture '%s' in XML file '%s'\n"),
                    body_text, data->filename);
    }
}


/* This function is called by the XML parser when the start of a <feature> element is seen.  */

static void
start_feature (struct gdb_xml_parser        *parser,
               const struct gdb_xml_element *element,
               void                         *user_data,
               VEC(gdb_xml_value_s)         *attributes)
{
    DEBUG("feature started\n");
}


/* This function checks the XML description of an auxiliary register for
   correctness, and if it is correct, adds a definition of this register to the
   register information that is currently being constructed.

   Parameters:
      parser    : the XML parser being used
      element   : a pointer to the parse structure
      user_data : a pointer to the parsing data
      attributes: a vector containing the attributes of the register from the XML
      is_BCR    : TRUE if the register is a Build Configuration Register
  */

static void
add_aux_register (struct gdb_xml_parser        *parser,
                  const struct gdb_xml_element *element,
                  void                         *user_data,
                  VEC(gdb_xml_value_s)         *attributes,
                  Boolean                       is_BCR)
{
    ParsingData               *data   = user_data;
    ARC_RegisterInfo          *info   = data->info;
    ARC_AuxRegisterDefinition *reg    = &data->reg;
    struct gdb_xml_value      *attrs  = VEC_address (gdb_xml_value_s, attributes);
    unsigned int               length = VEC_length  (gdb_xml_value_s, attributes);
    unsigned int               i;
    Boolean                    add    = TRUE;

    initialize_register(reg);

    /* Look at all the register attributes.  */
    for (i = 0; i < length; i++)
    {
        const char *name  = attrs[i].name;
        void       *value = attrs[i].value;

        if      (NAME_IS("name"))        reg->name        = value;
        else if (NAME_IS("description")) reg->description = xstrdup(value);
        else if (NAME_IS("number"))      reg->number      = (ARC_RegisterNumber) *(ULONGEST*) value;
        else if (NAME_IS("mask"))        reg->mask        = (ARC_Word)           *(ULONGEST*) value;
        else if (NAME_IS("access"))      reg->access      = (RegisterAccess)     *(ULONGEST*) value;
    }

    if (strcasecmp(reg->name, "unused") == 0)
        reg->name = (char*) UNUSED;
    else
        reg->name = xstrdup (reg->name);

    if (is_BCR)
    {
        reg->is_BCR = is_BCR;
        reg->access = READ_ONLY;
    }

    /* Sanity checking.  */
    for (i = 0; i < info->aux_register_count; i++)
    {
        if (reg->name != UNUSED)
            if (strcasecmp(reg->name, info->aux_registers[i].name) == 0)
            {
                warning(_("auxiliary register with name '%s' already defined in file %s"),
                        reg->name, data->filename);
                add = FALSE;
            }

        if (reg->number == info->aux_registers[i].number)
        {
            warning(_("auxiliary register with number %u already defined in file %s"),
                    reg->number, data->filename);
            add = FALSE;
        }
    }

    if (add)
    {
        unsigned int name_length = (unsigned int) strlen(reg->name);

        if (name_length > info->max_name_length)
            info->max_name_length = name_length;

        info->aux_register_count++;
        info->aux_registers = xrealloc(info->aux_registers,
                                       info->aux_register_count * sizeof(ARC_AuxRegisterDefinition));

        if (info->aux_registers == NULL)
        {
            /* N.B. this does not return.  */
            nomem (0);
        }

        /* Copy the register description into the array, and make it the current
           register that will be referred to when parsing any fields.  */
        data->currentRegister = &info->aux_registers[info->aux_register_count - 1];
        *data->currentRegister = *reg;
    }
    else
        /* Do not copy it into the array, but still make it the current register.  */
        data->currentRegister = reg;
}


/* This function is called by the XML parser when the start of a <auxregister> element is seen.  */

static void
start_auxregister (struct gdb_xml_parser        *parser,
                   const struct gdb_xml_element *element,
                   void                         *user_data,
                   VEC(gdb_xml_value_s)         *attributes)
{
    add_aux_register(parser, element, user_data, attributes, FALSE);
}


/* This function is called by the XML parser when the start of a <bcr> element is seen.  */

static void
start_bcr (struct gdb_xml_parser        *parser,
           const struct gdb_xml_element *element,
           void                         *user_data,
           VEC(gdb_xml_value_s)         *attributes)
{
    add_aux_register(parser, element, user_data, attributes, TRUE);
}


/* This function is called by the XML parser when the start of a <ecr> element
   is seen.  It checks the XML description of an extension core register for
   correctness, and if it is correct, adds a definition of this register to the
   register information that is currently being constructed.

   Parameters:
      parser    : the XML parser being used
      element   : a pointer to the parse structure
      user_data : a pointer to the parsing data
      attributes: a vector containing the attributes of the register from the XML
  */

static void
start_ecr (struct gdb_xml_parser        *parser,
           const struct gdb_xml_element *element,
           void                         *user_data,
           VEC(gdb_xml_value_s)         *attributes)
{
    ParsingData          *data   = user_data;
    ARC_RegisterInfo     *info   = data->info;
    struct gdb_xml_value *attrs  = VEC_address (gdb_xml_value_s, attributes);
    unsigned int          length = VEC_length  (gdb_xml_value_s, attributes);
    ARC_RegisterNumber    number = 0;
    ARC_Word              mask   = 0xFFFFFFFF;
    RegisterAccess        access = READ_WRITE;
    unsigned int          i;

    /* Look at all the register attributes.  */
    for (i = 0; i < length; i++)
    {
        const char *name  = attrs[i].name;
        void       *value = attrs[i].value;

        if      (NAME_IS("number")) number = (ARC_RegisterNumber) *(ULONGEST*) value;
        else if (NAME_IS("mask"))   mask   = (ARC_Word)           *(ULONGEST*) value;
        else if (NAME_IS("access")) access = (RegisterAccess)     *(ULONGEST*) value;
    }

    /* Sanity checking.  */
    if (IS_EXTENSION_CORE_REGISTER(number))
    {
        ARC_CoreRegisterDefinition *reg = &info->core_registers[number];

        if (reg->exists)
        {
            warning(_("extension core register with number %d already defined in file %s"),
                    number, data->filename);
        }
        else
        {
            reg->mask   = mask;
            reg->access = access;
            reg->exists = TRUE;
        }
    }
    else
        warning(_("extension core register with invalid number %d defined in file %s"),
                number, data->filename);
}


/* This function checks the XML description of an auxiliary register field for
   correctness, and if it is correct, adds a definition of this field to the
   register information that is currently being constructed.

   Parameters:
      parser    : the XML parser being used
      element   : a pointer to the parse structure
      user_data : a pointer to the parsing data
      attributes: a vector containing the attributes of the field from the XML
      is_BCR    : TRUE if the field is part of a Build Configuration Register
  */

static void
add_field (struct gdb_xml_parser        *parser,
           const struct gdb_xml_element *element,
           void                         *user_data,
           VEC(gdb_xml_value_s)         *attributes,
           Boolean                       is_BCR)
{
    ParsingData               *data   = user_data;
    ARC_AuxRegisterDefinition *reg    = data->currentRegister;
    ARC_FieldDefinition       *field  = &data->field;
    struct gdb_xml_value      *attrs  = VEC_address (gdb_xml_value_s, attributes);
    unsigned int               length = VEC_length  (gdb_xml_value_s, attributes);
    unsigned int               i;
    Boolean                    add    = TRUE;

    /* By default, a field has the same access as the register that contains it
       (though a particular field in a R/W register might be R/O or W/O).  */
    initialize_field(field, reg->access);

    /* Look at all the field attributes.  */
    for (i = 0; i < length; i++)
    {
        const char *name  = attrs[i].name;
        void       *value = attrs[i].value;

        if      (NAME_IS("name"))        field->name            = value;
        else if (NAME_IS("description")) field->description     = xstrdup(value);
        else if (NAME_IS("offset"))      field->offset          = (unsigned int)         *(ULONGEST*) value;
        else if (NAME_IS("size"))        field->size            = (unsigned int)         *(ULONGEST*) value;
        else if (NAME_IS("access"))      field->access          = (RegisterAccess)       *(ULONGEST*) value;
        else if (NAME_IS("onwrite"))  {  field->value_for_write = (ARC_RegisterContents) *(ULONGEST*) value;
                                         field->fixed = TRUE; }
    }

    if (strcasecmp(field->name, "reserved") == 0)
        field->name = (char*) RESERVED;
    else
        field->name = xstrdup (field->name);

    /* Sanity checking.  */
    if (is_BCR)
    {
        /* BCRs are, by definition, read-only.  */
        field->access = READ_ONLY;
    }
    else
    {
        if (field->access != WRITE_ONLY && reg->access == WRITE_ONLY)
        {
            warning (_("field '%s' is readable in write-only auxiliary register '%s' in file %s"),
                     field->name, reg->name, data->filename);
            add = FALSE;
        }
        else if (field->access != READ_ONLY && reg->access == READ_ONLY)
        {
            warning (_("field '%s' is writable in read-only auxiliary register '%s' in file %s"),
                     field->name, reg->name, data->filename);
            add = FALSE;
        }
    }

    if (field->size == 0)
    {
        warning(_("field '%s' contains no bits in auxiliary register '%s' in file %s"),
                field->name, reg->name, data->filename);
        add = FALSE;
    }

    if (field->offset > BITS_IN_REGISTER - 1)
    {
        warning(_("field '%s' offset > %u in auxiliary register '%s' in file %s"),
                field->name, BITS_IN_REGISTER - 1, reg->name, data->filename);
        add = FALSE;
    }

    if (field->offset + field->size > BITS_IN_REGISTER)
    {
        warning(_("field '%s' is too wide in auxiliary register '%s' in file %s"),
                field->name, reg->name, data->filename);
        add = FALSE;
    }

    for (i = 0; i < reg->field_count; i++)
    {
        ARC_FieldDefinition *f = &reg->fields[i];

        if (field->name != RESERVED)
            if (strcasecmp(field->name, f->name) == 0)
            {
                warning(_("field '%s' already defined in auxiliary register '%s' in file %s"),
                        field->name, reg->name, data->filename);
                add = FALSE;
            }

        if (overlaps(field, f))
        {
            warning(_("field '%s' overlaps field '%s' in auxiliary register '%s' in file %s"),
                    field->name, f->name, reg->name, data->filename);
            add = FALSE;
        }
    }

    data->maxFieldContents = 0;

    for (i = 0; i < field->size; i++)
        data->maxFieldContents = (data->maxFieldContents << 1) + 1;

    if (field->value_for_write > data->maxFieldContents)
    {
        warning(_("value on write %u is too large for %u-bit reserved field in auxiliary register '%s' in file %s"),
                field->value_for_write, field->size, reg->name, data->filename);
        add = FALSE;
    }

    if (add)
    {
        /* Keep track of the longest field name, and the most number of bits in a field.  */
        unsigned int len = (unsigned int) strlen(field->name);

        if (len > reg->longest_field_name)
            reg->longest_field_name = len;

        if (field->size > reg->max_bits_in_field)
            reg->max_bits_in_field = field->size;

        reg->field_count++;
        reg->fields = xrealloc(reg->fields,
                               reg->field_count * sizeof(ARC_FieldDefinition));

        if (reg->fields == NULL)
        {
            /* N.B. this does not return.  */
            nomem (0);
        }

        /* Copy the field description into the array, and make it the current
           field that will be referred to when parsing any meanings.  */
        data->currentField = &reg->fields[reg->field_count - 1];
        *data->currentField = *field;
    }
    else
        /* Do not copy it into the array, but still make it the current field.  */
        data->currentField = field;
}


/* This function is called by the XML parser when the start of a <field> element is seen.  */

static void
start_field (struct gdb_xml_parser        *parser,
             const struct gdb_xml_element *element,
             void                         *user_data,
             VEC(gdb_xml_value_s)         *attributes)
{
    add_field(parser, element, user_data, attributes, FALSE);
}


/* This function is called by the XML parser when the start of a <bcrfield> element is seen.  */

static void
start_bcrfield (struct gdb_xml_parser        *parser,
                const struct gdb_xml_element *element,
                void                         *user_data,
                VEC(gdb_xml_value_s)         *attributes)
{
    add_field(parser, element, user_data, attributes, TRUE);
}


/* This function is called by the XML parser when the start of a <meaning>
   element is seen.  It checks the XML description of the meaning for
   correctness, and if it is correct, adds a definition of this meaning to the
   register information that is currently being constructed.

   Parameters:
      parser    : the XML parser being used
      element   : a pointer to the parse structure
      user_data : a pointer to the parsing data
      attributes: a vector containing the attributes of the meaning from the XML
  */

static void
start_meaning (struct gdb_xml_parser        *parser,
               const struct gdb_xml_element *element,
               void                         *user_data,
               VEC(gdb_xml_value_s)         *attributes)
{
    ParsingData               *data   = user_data;
    ARC_AuxRegisterDefinition *reg    = data->currentRegister;
    ARC_FieldDefinition       *field  = data->currentField;
    ARC_FieldMeaning           meaning;
    struct gdb_xml_value      *attrs  = VEC_address (gdb_xml_value_s, attributes);
    unsigned int               length = VEC_length  (gdb_xml_value_s, attributes);
    unsigned int               i;
    Boolean                    add    = TRUE;

    initialize_meaning(&meaning);

    /* Look at all the meaning attributes.  */
    for (i = 0; i < length; i++)
    {
        const char *name  = attrs[i].name;
        void       *value = attrs[i].value;

        if      (NAME_IS("description")) meaning.description = xstrdup(value);
        else if (NAME_IS("value"))       meaning.value       = (ARC_RegisterContents) *(ULONGEST*) value;
    }

    if (meaning.value > data->maxFieldContents)
    {
        warning(_("value %u is too large for field '%s' in auxiliary register '%s' in file %s"),
                meaning.value, field->name, reg->name, data->filename);
        add = FALSE;
    }

    /* Sanity checking.  */
    for (i = 0; i < field->meaning_count; i++)
    {
        ARC_FieldMeaning *m = &field->meanings[i];

        if (strcmp(meaning.description, m->description) == 0)
        {
            warning(_("meaning '%s' already defined for field '%s' in auxiliary register '%s' in file %s"),
                    meaning.description, field->name, reg->name, data->filename);
            add = FALSE;
        }

        if (meaning.value == m->value)
        {
            warning(_("value %u already has meaning for field '%s' in auxiliary register '%s' in file %s"),
                    meaning.value, field->name, reg->name, data->filename);
            add = FALSE;
        }
    }

    if (add)
    {
        field->meaning_count++;
        field->meanings = xrealloc(field->meanings,
                                   field->meaning_count * sizeof(ARC_FieldMeaning));

        if (field->meanings == NULL)
        {
            /* N.B. this does not return.  */
            nomem (0);
        }

        field->meanings[field->meaning_count - 1] = meaning;
    }
}


/* This function is passed to qsort to sort the auxiliary registers into a
   canonical order such that BCRs come after the non-BCR aux registers, but
   otherwise, the registers are ordered by increasing number, with the
   exception that the IDENTITY register must come first.   */

static int
compare_auxiliary_registers(const void *p1, const void *p2)
{
    ARC_AuxRegisterDefinition* reg1 = (ARC_AuxRegisterDefinition*) p1;
    ARC_AuxRegisterDefinition* reg2 = (ARC_AuxRegisterDefinition*) p2;

    if (reg1->is_BCR && !reg2->is_BCR)
        return 1;

    if (!reg1->is_BCR && reg2->is_BCR)
        return -1;

    if (!reg1->is_BCR && !reg2->is_BCR)
    {
        if (strcmp(reg1->name, "IDENTITY") == 0)
            return -1;
        if (strcmp(reg2->name, "IDENTITY") == 0)
            return 1;
    }

    return (reg1->number < reg2->number) ? -1 : 1;
}


/* This function assigns the gdb register numbers to the target processor's
   registers, using the scheme defined in arc-elf32-tdep.c.  */

static void
assign_gdb_register_numbers (struct gdbarch *gdbarch, ARC_RegisterInfo *info)
{
    unsigned int i;
    int          pc = -1;

    /* Core registers first.  */
    for (i = 0; i < ELEMENTS_IN_ARRAY(info->core_registers); i++)
    {
        ARC_CoreRegisterDefinition *reg = &info->core_registers[i];

        /* If this core register exists in the target, we have one more core
           register in total; and the gdb number of this register is that number
           less 1.  */
        if (reg->exists)
            reg->gdb_regno = (int) info->core_register_count++;
    }

    info->first_aux_gdb_regno = (int) info->core_register_count;

    /* Then the auxiliary registers. */
    for (i = 0; i < info->aux_register_count; i++)
    {
        ARC_AuxRegisterDefinition *reg = &info->aux_registers[i];

        reg->gdb_regno = info->first_aux_gdb_regno + i;

        /* Is this the PC?  */
        if (strcasecmp(reg->name, "PC") == 0)
        {
            pc = reg->gdb_regno;
            info->PC_number = pc;
        }
    }

    /* We must tell gdb that the total number of registers has changed.  */
    set_gdbarch_num_regs (gdbarch, (int) (info->core_register_count +
                                          info->aux_register_count));

    /* The file may have contained a description of the PC, so we must tell gdb
       what its number is.
       N.B. if the 'replace' parameter to read_XML_file was true and the file
            did NOT contain a description of the PC, we now no longer know which
            auxiliary register is the PC, so we must set gdb's PC number back to
            -1!  */
    set_gdbarch_pc_regnum(gdbarch, pc);

    if (pc >= 0)
    {
        DEBUG("PC is reg #%d (arch %p)\n", pc, gdbarch);

        /* So PC is defined - so remove the guard.  */
        arc_aux_pc_guard(gdbarch, FALSE);
    }
}


/* Parse the XML file containing the register definitions.

   Parameters:
      document      : the XML document to be parsed (i.e. the XML text)
      fetcher       : a function which can be used to read an XML document from a file
      fetcher_baton : the directory containing the XML file(s)
      data          : a pointer to a structure used to pass state data
                      between the parsing routines.  */

static void
parse_XML (const char          *document,
           xml_fetch_another    fetcher,
           void                *fetcher_baton,
           ParsingData         *data)

{
    struct cleanup        *back_to;
    struct gdb_xml_parser *parser;
    char                  *expanded_text;

    /* Expand all XInclude directives.  */
    expanded_text = xml_process_xincludes (_("aux registers description"),
                                           document, fetcher, fetcher_baton, 0);
    if (expanded_text == NULL)
    {
        warning (_("can not load XML auxiliary registers description"));
        return;
    }

    back_to = make_cleanup (null_cleanup, NULL);
    parser = gdb_xml_create_parser_and_cleanup (_("aux registers description"),
                                                elements, data);

    /* Do the parsing; the DTD file defines the schema to be used by the XML.  */
    gdb_xml_use_dtd (parser, "arc-registers.dtd");

    (void) make_cleanup (xfree, expanded_text);

    if (gdb_xml_parse (parser, expanded_text) != 0)
        warning (_("can not load XML auxiliary registers description"));

    do_cleanups (back_to);
}


/* Read an XML file containing the register definitions.

   Parameters:
      filename : the path to the file
      gdbarch  : the architecture for which the register set is are being defined
      replace  : TRUE if any existing set of register defintions should be deleted first
      inform   : TRUE if a message should be output say that the file has been read
      check    : TRUE if an architectural check is to be performed once the file has been read

   Result: TRUE if the file was successfully read and its contents parsed.  */

static Boolean
read_XML_file (const char     *filename,
               struct gdbarch *gdbarch,
               Boolean         replace,
               Boolean         inform,
               Boolean         check)
{
    char *xml = read_file_contents(filename, NULL);

    DEBUG("reading XML file: %s\n", filename);

    if (xml)
    {
        ARC_RegisterInfo *info    = INFO_OF(gdbarch);
        struct cleanup   *back_to = make_cleanup (xfree, xml);
        char             *dirname = ldirname (filename);
        ParsingData       data;

        memset (&data, 0, sizeof (ParsingData));

        if (replace)
            free_register_set(info);

        info->processor = NO_ARCHITECTURE;

        data.info     = info;
        data.filename = filename;

        if (dirname != NULL)
            (void) make_cleanup (xfree, dirname);

        parse_XML (xml, read_file_contents, dirname, &data);
        do_cleanups (back_to);

        if (inform)
            printf_filtered(_("Register definitions read from file %s\n"), filename);

        if (check)
            ARCHITECTURE_CHECK(gdbarch,
                               (current_objfile) ? current_objfile->obfd : NULL);

        /* Sort the auxiliary registers into a canonical order: this allows
           entries in the XML file to be in any order without affecting the gdb
           register numbers assigned to them; in particular, it aids in ensuring
           that when the xISS is being used as a remote debug target gdb and the
           xISS agree upon the order in which register contents are held in the
           RSP 'G' (set all registers) packet and the 'g' (get all registers)
           response packet.  */
        qsort(info->aux_registers,
              (size_t) info->aux_register_count,
              sizeof (ARC_AuxRegisterDefinition),
              compare_auxiliary_registers);

        /* Now that we know all of the core and auxiliary registers in this
           target, we can assign gdb register numbers to them.  */
        assign_gdb_register_numbers(gdbarch, info);

        /* We can send the event now only if current_gdbarch is not NULL, or
           it could cause an error elsewhere where gdbarch_num_regs or
           gdbarch_num_pseudo_regs is used (e.g. in setup_architecture_data in
           gdbtk/generic/gdbtk-register.c).  */
        if (current_gdbarch == NULL)
            arc_pending_register_architecture_change_event = TRUE;
        else
            reg_architecture_changed_event();

        return TRUE;
    }

    return FALSE;
}


/* Try to find a file containing the default AUX register definitions; look in
      1) the current working directory
      2) the user's home directory

   Parameters:
      gdbarch  : the architecture for which the register set is are being defined
      inform   : TRUE if a message should be output say that the file has been read
      check    : TRUE if an architectural check is to be performed once the file has been read
*/

static void
read_default_file (struct gdbarch *gdbarch, Boolean inform, Boolean check)
{
    ENTERARGS("inform = %d, check = %d", inform, check);

#define DESCR  "auxiliary registers definition file "

    if (access(REGISTER_DEFINITION_FILE, F_OK) == 0)
    {
        if (!read_XML_file(REGISTER_DEFINITION_FILE, gdbarch, FALSE, inform, check))
            error(_("Can not read " DESCR REGISTER_DEFINITION_FILE));
    }
    else
    {
        const char *home_dir = getenv ("HOME");

        if (home_dir)
        {
            char *home_file = xstrprintf (_("%s/%s"), home_dir, REGISTER_DEFINITION_FILE);

            if (access(home_file, F_OK) == 0)
            {
                if (!read_XML_file(home_file, gdbarch, FALSE, inform, check))
                    error(_("Can not read " DESCR " %s"), home_file);
            }
            else
                warning(_("can not find " DESCR REGISTER_DEFINITION_FILE " in either current directory or $HOME"));

            xfree (home_file);
        }
        else
            warning(_("HOME environment variable is not set - can not find " DESCR REGISTER_DEFINITION_FILE));
    }
}


/* -------------------------------------------------------------------------- */
/*                         miscellaneous local functions                      */
/* -------------------------------------------------------------------------- */

/* This is a callback which is called from gdb when it writes the PC.
   It is used to "guard" the PC.  */

static CORE_ADDR
get_pc (struct regcache *regcache)
{
    struct gdbarch *gdbarch = get_regcache_arch (regcache);

    /* This does not return if PC is not defined.  */
    arc_aux_check_pc_defined(gdbarch);

    /* So PC is defined - so remove the guard.  */
    arc_aux_pc_guard(gdbarch, FALSE);

    /* Now read and return the PC.  */
    return read_pc();
}


/* This is a callback which is called from gdb when it reads the PC.
   It is used to "guard" the PC.  */

static void
set_pc (struct regcache *regcache, CORE_ADDR val)
{
    struct gdbarch *gdbarch = get_regcache_arch (regcache);

    /* This does not return if PC is not defined.  */
    arc_aux_check_pc_defined(gdbarch);

    /* So PC is defined - so remove the guard.  */
    arc_aux_pc_guard(gdbarch, FALSE);

    write_pc(val);
}


/* Try to find the register information for the current architecture.

   Parameter:
       must_be_defined : TRUE if the register information must be known

   Result: a pointer to the info; NULL if there is none as yet.  */

static ARC_RegisterInfo*
find_info (Boolean must_be_defined)
{
    gdb_assert (current_gdbarch != NULL);

    {
        ARC_RegisterInfo *info = INFO_OF(current_gdbarch);

        /* If we have no aux register info.  */
        if (must_be_defined && info->aux_register_count == 0)
        {
            /* Try to get it.  */
            read_default_file(current_gdbarch, FALSE, FALSE);

            /* No, could not get it.  */
            if (info->aux_register_count == 0)
                error(_("No auxiliary registers have yet been defined for this target"));
        }

        return info;
    }
}


/* Map a gdb register number to the ARC processor hardware number, and
   determine the class of the register (as known to the built-in simulator).  */

static void
simulator_mapping (int                gdb_regno,
                   int               *hw_regno,
                   ARC_RegisterClass *reg_class)
{
    /* Just in case gdb_regno is invalid.  */
    *hw_regno  = -1;
    *reg_class = ARC_UNKNOWN_REGISTER;

   if (arc_is_core_register(gdb_regno))
   {
       *hw_regno  = (int) arc_core_register_number(gdb_regno);
       *reg_class = ARC_CORE_REGISTER;
   }
   else if (gdb_regno == arc_aux_pc_number(current_gdbarch))
   {
        /* The hw_regno is irrelevant here.  */
        *reg_class = ARC_PROGRAM_COUNTER;
   }
   else
   {
        ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_gdb_number(gdb_regno);

        if (def)
        {
            *hw_regno  = (int) arc_aux_hw_register_number(def);
            *reg_class = ARC_AUX_REGISTER;
        }
   }
}


/* -------------------------------------------------------------------------- */
/*                  local functions for supporting commands                   */
/* -------------------------------------------------------------------------- */

/* This function determines the h/w register number of an auxiliary register
   from a command argument provided by a user. The argument might be a register
   name, a number, or an expression to be evaluated.  */

static ARC_RegisterNumber
extractRegisterNumber (char *arg)
{
    ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_name(arg);
    ARC_RegisterNumber         num;

    /* Is it a register name?  */
    if (def)
       num = arc_aux_hw_register_number(def);
    else
    {
        int regnum;

        /* Is it some expression?  */
        EXTRACT(arg, int, regnum)

        if (regnum < 0)
            error(_("Register number '%s < 0"), arg);

        num = (ARC_RegisterNumber) regnum;
    }

    return num;
}


/* This function finds the definition of an auxiliary register from a command
   argument provided by a user. The argument might be a register name, a number,
   or an expression to be evaluated.

   NULL is returned if the argumenmt does not identify a defined auxiliary
   register.  */

static ARC_AuxRegisterDefinition*
find_aux_register (char *arg)
{
    ARC_AuxRegisterDefinition* def = arc_find_aux_register_by_name(arg);

    /* Is it not a register name?  */
    if (def == NULL)
    {
        int regnum;

        /* Is it some expression?  */
        EXTRACT(arg, int, regnum)

        if (regnum < 0)
            error(_("Register number '%s < 0"), arg);

        def = arc_find_aux_register_by_hw_number((ARC_RegisterNumber) regnum);
    }

    return def;
}


/* This function list the description (but not the contents) of an auxiliary
   register.

   Parameters:
      r   : a pointer to the register definition
      full: TRUE if full information is to be listed.
*/

static void
list_register (ARC_AuxRegisterDefinition *r, Boolean full)
{
    printf_filtered("%s", r->name);
    if (r->is_BCR)
        printf_filtered(_(" (BCR)"));
    printf_filtered(_("\n"));

    if (r->description != NO_DESCRIPTION)
        printf_filtered(_("    description: %s\n"), r->description);
    printf_filtered(_("    number     : 0x%x\n"), r->number);

    if (full)
    {
        /* The gdb number of the register is internal information which would
           be meaningless to the user; the mask is probably not useful either.  */
        printf_filtered(_("    gdb number : %d\n"),   r->gdb_regno);
        printf_filtered(_("    mask       : %08X\n"), r->mask);
    }

    if (!r->is_BCR)
        printf_filtered(_("    access     : %s\n"), RegisterAccess_Image(r->access));

    if (r->fields)
    {
        unsigned int j;

        printf_filtered(_("    fields\n"));

        for (j = 0; j < r->field_count; j++)
        {
            ARC_FieldDefinition *f = &r->fields[j];

            printf_filtered(_("    %s\n"), f->name);
            if (f->description != NO_DESCRIPTION)
                printf_filtered(_("        description: %s\n"),    f->description);
            printf_filtered(_("        position   : %u:%u\n"), f->offset, f->size);
            printf_filtered(_("        access     : %s\n"),    RegisterAccess_Image(f->access));

            if (f->meanings)
            {
                unsigned int k;

                printf_filtered(_("        field meanings\n"));

                for (k = 0; k < f->meaning_count; k++)
                {
                    ARC_FieldMeaning *m = &f->meanings[k];

                    printf_filtered(_("          %x ==> %s\n"), m->value, m->description);
                }
            }
        }
    }

    printf_filtered(_("\n"));
}


/* This function list the descriptions (but not the contents) of all auxiliary
   registers.

   Parameter:
      full: TRUE if full information is to be listed.
*/

static void
list_registers (Boolean full)
{
    ARC_RegisterInfo *info = find_info(TRUE);
    unsigned int      i;

    for (i = 0; i < info->aux_register_count; i++)
    {
        ARC_AuxRegisterDefinition *def = info->aux_registers + i;

        if (def->name != UNUSED)
            list_register(def, full);
    }
}


/* This function reads the contents of an auxiliary register on the target.

   Parameters:
      def            : the definition of the register
      value          : set to the contents of the register
      warn_on_failure: TRUE if a warning should be issued if the read fails

   Result: TRUE if the register contents are read. */

static Boolean
read_aux_register (ARC_AuxRegisterDefinition *def,
                   ARC_RegisterContents      *value,
                   Boolean                    warn_on_failure)
{
    int              gdb_regno = arc_aux_gdb_register_number(def);
    struct regcache *regcache  = get_current_regcache();

    /* Read the register contents from the target to the register cache,
       then collect the register value from the cache.  */
    target_fetch_registers(regcache, gdb_regno);
    regcache_raw_collect (regcache, gdb_regno, value);

    /* Unfortunately, the target_fetch_registers operation does not give us an
       indication of success or failure.  */
    return TRUE;
}


/* This function writes the contents of an auxiliary register on the target.

   Parameters:
      def            : the definition of the register
      value          : the contents of the register
      warn_on_failure: TRUE if a warning should be issued if the write fails

   Result: TRUE if the register contents are written. */

static Boolean
write_aux_register (ARC_AuxRegisterDefinition *def,
                    ARC_RegisterContents       value,
                    Boolean                    warn_on_failure)
{
    int                  gdb_regno = arc_aux_gdb_register_number(def);
    struct regcache     *regcache  = get_current_regcache();
    ARC_RegisterContents written   = arc_write_value(def, value);

    /* Supply the register value to the register cache, then write it from the
       cache to the target.  */
    regcache_raw_supply (regcache, gdb_regno, &written);
    target_store_registers(regcache, gdb_regno);

    /* If the value we actually wrote to the target is not the same as the value
       we were given (because the register has fields that must have particular
       values when written).  */
    if (written != value)
    {
        DEBUG("%s auxiliary register value %08X written as %08X\n",
              arc_aux_register_name(def), value, written);

        /* Put the value we were actually given into the register cache, so if
           the user then displays the register contents he will see the
           unmodified value.  */
        regcache_raw_supply (regcache, gdb_regno, &value);
    }

    /* Unfortunately, the target_store_registers operation does not give us an
       indication of success or failure.  */
    return TRUE;
}


/* This function is passed to the arc_all_aux_registers iterator.
   It is called for each auxiliary register defined for the current architecture;
   if the register is a Build Configuration Register, and it is not unused, the
   register's contents are read from the target and printed.  */

static void
print_bcr (ARC_AuxRegisterDefinition *def, void *data)
{
    if (arc_aux_is_BCR(def) && !arc_aux_is_unused(def))
    {
        ARC_RegisterNumber   bcr = arc_aux_hw_register_number(def);
        ARC_RegisterContents bcr_value;

        if (read_aux_register (def, &bcr_value, TRUE))
            printf_filtered(_("[%02x] %-16s : 0x%02x\n"),
                            bcr, arc_aux_register_name(def), bcr_value);
    }
}


/* This function may be passed to the arc_all_aux_registers iterator.
   It reads the contents of the register whose definition is given from
   the target and prints those contents.  */

static void
show_one_aux_register (ARC_AuxRegisterDefinition *def, void *data)
{
    ARC_RegisterNumber   reg_no = arc_aux_hw_register_number(def);
    ARC_RegisterContents contents;

    DEBUG("try to read aux reg %u\n", reg_no);

    if (read_aux_register(def, &contents, TRUE))
        arc_print_aux_register(def, contents);
}


/* Read the register definitions from a file.

   Parameters:
      filename : the path to the file
      gdbarch  : the architecture for which the register set is are being defined
      replace  : TRUE if any existing set of register defintions should be deleted first
      inform   : TRUE if a message should be output say that the file has been read
      check    : TRUE if an architectural check is to be performed once the file has been read
*/
static Boolean
read_aux_regs_file (const char     *filename,
                    struct gdbarch *gdbarch,
                    Boolean         replace,
                    Boolean         inform,
                    Boolean         check)
{
    /* Try to read the register descriptions from the file.  */
    if (read_XML_file(filename, gdbarch, replace, inform, check))
        return TRUE;

    printf_filtered(_("can not read file '%s'\n"), filename);
    return FALSE;
}


/* -------------------------------------------------------------------------- */
/*                     local functions implementing commands                  */
/* -------------------------------------------------------------------------- */

/* Command: <command> <from> [ <to> ]

   Read and display a range of auxiliary registers.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_aux_reg_read_command (char *arg, int from_tty)
{
    char              *arg2;
    ARC_RegisterNumber first_regnum, last_regnum, r;
    char               format[40];

    if (!arg)
    {
        printf_filtered (_(AUX_REG_READ_COMMAND_USAGE));
        return;
    }

    /* Strip leading spaces.  */
    while (*arg == ' ')
        arg++;

    /* This assumes that the first arg cannot have spaces (the disas command
       also seems to work this way).  */
    arg2 = strchr (arg, ' ');

    /* Are there two arguments?  */
    if (arg2)
    {
        /* Split the input string up.  */
        arg2[0] = (char) 0;
        arg2++;
    }

    /* First arg.  */
    first_regnum = extractRegisterNumber(arg);

    /* So, how many regs do we want?  */
    if (arg2)
    {
        last_regnum = extractRegisterNumber(arg2);

        if (last_regnum < first_regnum)
        {
            warning(_(AUX_REG_READ_COMMAND ": %s < %s, showing one register"), arg2, arg);
            last_regnum = first_regnum;
        }
    }
    else
        last_regnum = first_regnum;

    DEBUG("try to read aux regs %d .. %d\n", first_regnum, last_regnum);

    (void) snprintf(format, sizeof(format),
                    _("0x%%08x %%-%us: %%08X\n"),
                    arc_aux_register_max_name_length() + 1);

    for (r = first_regnum; r <= last_regnum; r++)
    {
        ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_hw_number(r);

        /* If the aux register exists, and is used.  */
        if ((def != NULL) && !arc_aux_is_unused(def))
        {
            ARC_RegisterContents contents;

            DEBUG("try to read aux reg %u\n", r);

            if (read_aux_register(def, &contents, TRUE))
                printf_filtered (format, r, arc_aux_register_name(def), contents);
        }
    }
}


/* Command: <command> <reg> = <value>

   Write VALUE to auxiliary register REG.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */
static void
arc_aux_reg_write_command (char *arg, int from_tty)
{
    char                      *value_arg;
    char                      *p;
    ARC_RegisterNumber         regnum;
    ARC_RegisterContents       value;
    ARC_AuxRegisterDefinition *def;

    if (!arg)
    {
        printf_filtered (_(AUX_REG_WRITE_COMMAND_USAGE));
        return;
    }

    p = strchr(arg, '=');

    if (p == NULL)
    {
        printf_filtered (_(AUX_REG_WRITE_COMMAND ": no second argument\n" AUX_REG_WRITE_COMMAND_USAGE));
        return;
    }

    /* Split up the input string.  */
    value_arg = p + 1;
    p--;
    while (*p == ' ') p--;
    p[1] = '\0';

    /* Register expression.  */
    regnum = extractRegisterNumber(arg);

    /* Value expression.  */
    EXTRACT(value_arg, ARC_RegisterContents, value)

    def = arc_find_aux_register_by_hw_number(regnum);

    if (def == NULL)
        warning(_("no such auxiliary register: %s"), arg);
    else
    {
        DEBUG("try to write aux reg %d = 0x%08X\n", regnum, value);

        /* Write it.  */
        (void) write_aux_register(def, value, TRUE);
    }
}


/* Command: <command> [ <reg> ]

   Display the values of one or all of the auxiliary registers.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_aux_reg_show_command (char *arg, int from_tty)
{
    if (arg)
    {
        ARC_AuxRegisterDefinition *def = find_aux_register(arg);

        if (def)
            show_one_aux_register(def, NULL);
        else
            printf_filtered(_("There is no auxiliary register named '%s'\n"), arg);
    }
    else
        /* list them all */
        arc_all_aux_registers(show_one_aux_register, NULL);
}


/* Command: <command> [ <file> ]

   Read a definition of a set of auxiliary registers from an XML file.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_aux_reg_file_read_command (char *arg, int from_tty)
{
    if (!arg)
    {
        printf_filtered (REG_READ_FILE_COMMAND_USAGE);
        return;
    }

    /* The new set replaces the existing set (if any).  */
    (void) read_aux_regs_file(arg, current_gdbarch, TRUE, TRUE, TRUE);
}


/* Command: <command> <file>

   Read a definition of a set of auxiliary registers from an XML file.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_aux_reg_file_read_extra_command (char *arg, int from_tty)
{
    if (!arg)
    {
        printf_filtered (REG_READ_EXTRA_FILE_COMMAND_USAGE);
        return;
    }

    /* The new set is added to the existing set (if any).  */
    (void) read_aux_regs_file(arg, current_gdbarch, FALSE, TRUE, TRUE);
}


/* Command: <command> [ <reg> ]

   Display a description of one or all auxiliary registers.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_aux_reg_list_command (char *arg, int from_tty)
{
    if (arg)
    {
        ARC_AuxRegisterDefinition *def = find_aux_register(arg);

        if (def)
            list_register(def, FALSE);
        else
            printf_filtered(_("There is no auxiliary register named '%s'\n"), arg);
    }
    else
        /* List them all.  */
        list_registers(FALSE);
}


/* Command: <command>

   Display the Build Configuration Registers.

   We should eventually change this to use the ui_out stuff rather than
   printf_filtered.  */

static void
arc_print_bcr_regs (char *arg, int from_tty)
{
    arc_all_aux_registers(print_bcr, NULL);
}


/* -------------------------------------------------------------------------- */
/*                               externally visible functions                 */
/* -------------------------------------------------------------------------- */

/* Initialize the given register information to the base configuration:
      1) core registers exist
      2) extension core registers do not exist
      3) no aux registers
  */

void
arc_initialize_aux_reg_info (ARC_RegisterInfo *info)
{
    unsigned int i;

    info->processor           = NO_ARCHITECTURE;
    info->aux_registers       = NULL;
    info->aux_register_count  = 0;
    info->max_name_length     = 0;
    info->PC_number           = -1;
    info->first_aux_gdb_regno = 0;
    info->core_register_count = 0;

    /* All possible core registers.  */
    for (i = 0; i < ELEMENTS_IN_ARRAY(info->core_registers); i++)
    {
        ARC_CoreRegisterDefinition *reg = &info->core_registers[i];

        reg->exists  = TRUE;
        reg->mask    = 0xFFFFFFFF;
        reg->access  = READ_WRITE;
    }

    /* We do not yet know if we have any extension registers.  */
    for (i = ARC_FIRST_EXTENSION_CORE_REGISTER; i <= ARC_LAST_EXTENSION_CORE_REGISTER; i++)
        info->core_registers[i].exists = FALSE;

    /* R61 is reserved, R62 is not a real register.  */
    info->core_registers[61].exists = FALSE;
    info->core_registers[62].exists = FALSE;

    /* PCL is read-only */
    info->core_registers[63].access = READ_ONLY;
}


/* Read the XML registers definition for the given architecture from the default file.  */

void
arc_read_default_aux_registers (struct gdbarch *gdbarch)
{
    ENTERMSG;
    read_default_file(gdbarch, FALSE, FALSE);
}



/* This function sets up or remaoves a "guard" on the PC */

void
arc_aux_pc_guard (struct gdbarch *gdbarch, Boolean on)
{
    set_gdbarch_read_pc (gdbarch, (on) ? get_pc : NULL);
    set_gdbarch_write_pc(gdbarch, (on) ? set_pc : NULL);
}


/* Check whether the PC aux register is defined for the given architecture.  */

void
arc_aux_check_pc_defined (struct gdbarch *gdbarch)
{
    ENTERARGS("target %s", current_target.to_shortname);

    if (gdbarch == NULL)
        gdbarch = current_gdbarch;

    if (arc_aux_pc_number(gdbarch) < 0)
        error(_("There is no auxiliary register description for the PC (Program Counter)"));
}


/* Return the gdb register number of the PC (Program Counter); -1 if the PC is not defined.  */

int
arc_aux_pc_number (struct gdbarch *gdbarch)
{
    ARC_RegisterInfo *info = INFO_OF(gdbarch);

    return (info) ? info->PC_number : -1;
}


/* Find the register definition of the given aux register (identified by name).  */

ARC_AuxRegisterDefinition*
arc_find_aux_register_by_name (const char *name)
{
    FIND_REGISTER_DEFINITION_SUCH_THAT(strcasecmp(name, def->name) == 0)
}


/* Find the register definition of the given aux register (identified by hardware register number).  */

ARC_AuxRegisterDefinition*
arc_find_aux_register_by_hw_number (ARC_RegisterNumber hw_regno)
{
    FIND_REGISTER_DEFINITION_SUCH_THAT(hw_regno == def->number)
}


/* Find the register definition of the given aux register (identified by gdb register number).  */

ARC_AuxRegisterDefinition*
arc_find_aux_register_by_gdb_number (int gdb_regno)
{
    /* N.B. the elements in the info->aux_registers array have strictly increasing
            gdb numbers starting at info->first_aux_gdb_regno, so we can index the array
            instead of searching it.  */
    ARC_RegisterInfo *info  = find_info(TRUE);
    int               index = gdb_regno - info->first_aux_gdb_regno;

    if (0 <= index && index < (int) info->aux_register_count)
    {
        ARC_AuxRegisterDefinition *def = info->aux_registers + index;

        /* Just to be sure we have found the right element.  */
        gdb_assert(def->gdb_regno == gdb_regno);

        return def;
    }

    return NULL;
}


/* Return the hardware register number of the given named auxiliary register;
   if no register set is currently defined, or there is no register of that
   name in the set, return the given default number. */

ARC_RegisterNumber
arc_aux_find_register_number (const char        *name,
                              ARC_RegisterNumber defaultNumber)
{
    if (arc_aux_regs_defined(current_gdbarch))
    {
        ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_name(name);

        if (def != NULL)
            return arc_aux_hw_register_number(def);
    }

    warning(_("there is no auxiliary register description for the %s register - "
              "using 0x%x for the register number"), name, defaultNumber);
    return defaultNumber;
}


/* Return the hardware register number of the given core register.  */

ARC_RegisterNumber
arc_core_register_number (int gdb_regno)
{
    ARC_RegisterInfo *info  = find_info(TRUE);
    unsigned int      i;

    /* The lower-numbered set of non-extension core registers (i.e. excluding
       R60 .. R63) have fixed gdb numbers which are the same as the h/w number.  */
    if (gdb_regno < ARC_FIRST_EXTENSION_CORE_REGISTER)
        return (ARC_RegisterNumber) gdb_regno;

    /* Scan the rest of the array.  */
    for (i = ARC_FIRST_EXTENSION_CORE_REGISTER; i < ELEMENTS_IN_ARRAY(info->core_registers); i++)
    {
        ARC_CoreRegisterDefinition *def = &info->core_registers[i];

        if (def->exists)
            if (gdb_regno == def->gdb_regno)
                return (ARC_RegisterNumber) (i);
    }

    /* Too large to be the number of a core register.  */
    return ARC_MAX_CORE_REGS + 1000;
}


/* Return the gdb register number of the given core register.  */

int
arc_core_register_gdb_number (ARC_RegisterNumber hw_regno)
{
    ARC_RegisterInfo           *info = find_info(TRUE);
    ARC_CoreRegisterDefinition *def  = &info->core_registers[hw_regno];

    gdb_assert(def->exists);

    return def->gdb_regno;
}


/* Print the aux register whose definition and contents are given.
   The field and meaning information in the definition is used to
   give a detailed display of the register.  */

void
arc_print_aux_register (ARC_AuxRegisterDefinition *def,
                        ARC_RegisterContents       contents)

{
    printf_filtered (_("%s : %08x\n"), def->name, contents);

    if (def->fields)
    {
        unsigned int i;
        char         format[128];

        printf_filtered(_("    fields\n"));

        /* Create a format string such as

              "   %-10s:  %8s"

           so that the field name is left-justified, and the field value is
           right-justified.  */
        (void) snprintf(format, sizeof(format),
                                _("      %%-%us: %%%us"),
                                def->longest_field_name, def->max_bits_in_field);

        for (i = 0; i < def->field_count; i++)
        {
            ARC_FieldDefinition *f    = &def->fields[i];
            ARC_RegisterContents val  = contents >> f->offset;
            ARC_RegisterContents val2 = val;
            ARC_RegisterContents mask = 0;
            char                 bits[BITS_IN_REGISTER];
            char                *p = &bits[BITS_IN_REGISTER - 1];
            unsigned int         b;

            *p = '\0';

            /* Build up a string representing the bits of the field, starting
               with the least significant bit, which will be the rightmost digit
               displayed; at the same time, construct a mask of 1-bits of the
               same size as the field.  */
            for (b = 0; b < f->size; b++)
            {
                p--;
                *p = (val & 1) ? '1' : '0';
                val >>= 1;

                mask <<= 1;
                mask++;
            }

            printf_filtered(format, f->name, p);

            if (f->meanings)
            {
                unsigned int j;

                val2 &= mask;

                /* Look for a meaning for this particular value of the field.  */
                for (j = 0; j < f->meaning_count; j++)
                {
                    ARC_FieldMeaning *m = &f->meanings[j];

                    if (val2 == m->value)
                    {
                        printf_filtered(_("    (%s)"), m->description);
                        break;
                    }
                }
            }

            printf_filtered(_("\n"));
        }
    }

    printf_filtered(_("\n"));
}


/* Return the gdb register number of the given aux register.  */

int
arc_aux_gdb_register_number (ARC_AuxRegisterDefinition *def)
{
    return def->gdb_regno;
}


/* Return the hardware register number of the given aux register.  */

ARC_RegisterNumber
arc_aux_hw_register_number (ARC_AuxRegisterDefinition *def)
{
    return def->number;
}


/* Return the access mode (R/W, RO 0r WO) of the given aux register.  */

RegisterAccess
arc_aux_register_access (ARC_AuxRegisterDefinition *def)
{
    return def->access;
}


/* Return TRUE if the given aux register is not used in the processor architecture,
   i.e. there is a "place-holder" definition of that register (possibly a BCR) in
   the XML file, but that register does not actually exist on the target.  */

Boolean
arc_aux_is_unused (ARC_AuxRegisterDefinition *def)
{
    return (def->name == UNUSED);
}


/* Return TRUE if the given aux register is a BCR (Build Configuration Register).  */

Boolean
arc_aux_is_BCR (ARC_AuxRegisterDefinition *def)
{
    return def->is_BCR;
}


/* Return the name of the given aux register.  */

const char*
arc_aux_register_name (ARC_AuxRegisterDefinition *def)
{
    return def->name;
}


/* Return the access mode (R/W, RO 0r WO) of the given core register.  */

RegisterAccess
arc_core_register_access (ARC_RegisterNumber hw_regno)
{
    ARC_RegisterInfo           *info = find_info(TRUE);
    ARC_CoreRegisterDefinition *def  = &info->core_registers[hw_regno];

    gdb_assert(def->exists);

    return def->access;
}


/* Return the name of the given auxiliary register.  */

const char*
arc_aux_register_name_of (ARC_RegisterNumber hw_regno)
{
    ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_hw_number(hw_regno);

    return (def) ? def->name : "<no such register>";
}


/* Return TRUE if the given register is a core register.  */

Boolean
arc_is_core_register (int gdb_regno)
{
    ARC_RegisterInfo *info = find_info(TRUE);

    return (gdb_regno < info->first_aux_gdb_regno);
}


/* Iterate across all the aux registers; for each one, call the supplied
   function, passing it the defintion of the register and the supplied data.  */

void
arc_all_aux_registers (ARC_AuxRegisterFunction function, void *data)
{
    ARC_RegisterInfo *info = find_info(TRUE);
    unsigned int         i;

    for (i = 0; i < info->aux_register_count; i++)
        function(info->aux_registers + i, data);
}


/* Return the length of the longest aux register name in the current architecture.  */
unsigned int
arc_aux_register_max_name_length (void)
{
    ARC_RegisterInfo *info = find_info(TRUE);

    return info->max_name_length;
}


/* Return the number of aux registers defined for the given architecture;
   0 if no register set has yet been defined.  */

unsigned int
arc_aux_register_count (struct gdbarch *gdbarch)
{
    ARC_RegisterInfo *info = INFO_OF(gdbarch);

    return (info) ? info->aux_register_count : 0;
}


/* Return the number of core registers defined for the given architecture;
   ARC_NUM_STANDARD_CORE_REGS if no register set has yet been defined.  */

unsigned int
arc_core_register_count (struct gdbarch *gdbarch)
{
    ARC_RegisterInfo *info = INFO_OF(gdbarch);

    return (info) ? info->core_register_count : ARC_NUM_STANDARD_CORE_REGS;
}


/* Return TRUE if the register set has been defined for the given architecture.  */

Boolean
arc_aux_regs_defined (struct gdbarch *gdbarch)
{
    ARC_RegisterInfo *info = INFO_OF(gdbarch);

    return (info->aux_register_count > 0 && info->aux_registers != NULL);
}


/* Return the architectural version of the register set associated with the
   given architecture.  */

ARC_ProcessorVersion
arc_aux_architecture (struct gdbarch *gdbarch)
{
    ARC_RegisterInfo *info = INFO_OF(gdbarch);

    return info->processor;
}


/* Compute the value to be written to an auxiliary register so that any fields it
   has contain the values that they are required to have by the ARC architectural
   specification.

   Parameters:
      def   : the definition of the aux register
      value : the value to be adjusted

   Result: the adjusted register contents.  */

ARC_RegisterContents
arc_write_value (ARC_AuxRegisterDefinition *def,
                 ARC_RegisterContents       value)
{
    unsigned int i;

    for (i = 0; i < def->field_count; i++)
    {
        ARC_FieldDefinition *field = &def->fields[i];

        if (field->fixed)
        {
            value &= ~(((1 << field->size) - 1) << field->offset);
            value |=  (field->value_for_write   << field->offset);
        }
    }

    return value;
}


/* Adjust the value to be written to a register so that any fields it has contain
   the values that they are required to have by the ARC architectural specification.

   Parameters:
      gdb_regno : the number of the aux register
      buffer    : the value to be adjusted
 */

void
arc_convert_aux_contents_for_write (int gdb_regno, void *buffer)
{
    ARC_AuxRegisterDefinition *def = arc_find_aux_register_by_gdb_number(gdb_regno);

    /* Is it an auxiliary register? If not, do nothing.  */
    if (def)
    {
        ARC_RegisterContents old;
        ARC_RegisterContents new;

        memcpy(&old, buffer, BYTES_IN_REGISTER);
        new = arc_write_value(def, old);

        if (new != old)
        {
            DEBUG("*** converted register %s value from %08X to %08X\n",
                  arc_aux_register_name(def), old, new);
            memcpy(buffer, &new, BYTES_IN_REGISTER);
        }
    }
}


/* Initialize the module.  This function is called from the gdb core on start-up.  */

void
_initialize_arc_aux_regs (void)
{
    arc_pending_register_architecture_change_event = FALSE;

    /* If this module is being built with a test driver.  */
#ifdef STANDALONE_TEST
    /* N.B. it would be better to set this up in the test driver, but that
            causes problems when linking!  */
    struct gdbarch_info     info;
    static ARC_VariantsInfo variant;
    struct gdbarch_tdep    *tdep    = malloc (sizeof (struct gdbarch_tdep));
    struct gdbarch         *gdbarch = gdbarch_alloc (&info, tdep);

    tdep->processor_variant_info = &variant;
    current_gdbarch = gdbarch;
#endif

    (void) add_cmd (REG_READ_FILE_COMMAND,
                    class_files,
                    arc_aux_reg_file_read_command,
                    _("Read a file describing a set of auxiliary registers.\n"
                      REG_READ_FILE_COMMAND_USAGE
                      "<FILE> is an XML file containing the auxiliary register definitions.\n"),
                    &cmdlist);

    (void) add_cmd (REG_READ_EXTRA_FILE_COMMAND,
                    class_files,
                    arc_aux_reg_file_read_extra_command,
                    _("Read a file describing an additional set of auxiliary registers.\n"
                      REG_READ_EXTRA_FILE_COMMAND_USAGE
                      "<FILE> is an XML file containing the auxiliary register definitions.\n"),
                    &cmdlist);

    (void) add_cmd (AUX_LIST_REGISTER_COMMAND,
                    class_vars,
                    arc_aux_reg_list_command,
                    _("Show a description of one or all auxiliary registers.\n"
                      AUX_LIST_REGISTER_COMMAND_USAGE
                      "<REG> is the name of an auxiliary register.\n"),
                    &cmdlist);

    (void) add_cmd(ARC_BCR_COMMAND,
                   class_info,
                   arc_print_bcr_regs,
                   _("Show Build Configuration Registers in the ARC processor variant.\n"
                     ARC_BCR_COMMAND_USAGE),
                   &infolist);

    (void) add_cmd (AUX_REG_READ_COMMAND,
                    class_vars,
                    arc_aux_reg_read_command,
                    _("Read and show a range of auxiliary registers.\n"
                      AUX_REG_READ_COMMAND_USAGE
                      "REG-FROM and REG-TO are the names or numbers of the registers.\n"
                      "If REG-TO is not specified, one register is displayed.\n"),
                    &cmdlist);

    (void) add_cmd (AUX_REG_WRITE_COMMAND,
                    class_vars,
                    arc_aux_reg_write_command,
                    _("Write to an auxiliary register.\n"
                      AUX_REG_WRITE_COMMAND_USAGE
                      "REG is the name or number of the register.\n"
                      "VALUE can be any expression that evaluates to an integer.\n"),
                    &cmdlist);

    (void) add_cmd (AUX_REG_SHOW_COMMAND,
                    class_vars,
                    arc_aux_reg_show_command,
                    _("Read and show one or all auxiliary registers.\n"
                      AUX_REG_SHOW_COMMAND_USAGE
                      "<REG> is the name of an auxiliary register.\n"),
                    &cmdlist);

    /* Provide the built-in simulator with a functions that it can use to map
       from gdb register numbers to h/w register numbers, and set the fields
       of aux registers to any values that they may be required to have on write.  */
    arc_set_register_mapping(&simulator_mapping);
    arc_set_aux_register_conversion(arc_convert_aux_contents_for_write);
}

/******************************************************************************/
