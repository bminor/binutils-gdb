/* Header file for command-reading library command.c.
   Copyright (C) 1986, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _COMMAND_H_INCLUDED
#define _COMMAND_H_INCLUDED

/* Not a set/show command.  Note that some commands which begin with
   "set" or "show" might be in this category, if their syntax does
   not fall into one of the following categories.  */
typedef enum cmd_types {
  not_set_cmd,
  set_cmd,
  show_cmd,
} cmd_types;

/* Types of "set" or "show" command.  */
typedef enum var_types {
  /* "on" or "off".  *VAR is an integer which is nonzero for on,
     zero for off.  */
  var_boolean,
  /* Unsigned Integer.  *VAR is an unsigned int.  The user can type 0
     to mean "unlimited", which is stored in *VAR as UINT_MAX.  */
  var_uinteger,
  /* String which the user enters with escapes (e.g. the user types \n and
     it is a real newline in the stored string).
     *VAR is a malloc'd string, or NULL if the string is empty.  */
  var_string,
  /* String which stores what the user types verbatim.
     *VAR is a malloc'd string, or NULL if the string is empty.  */
  var_string_noescape,
  /* String which stores a filename.
     *VAR is a malloc'd string, or NULL if the string is empty.  */
  var_filename,
  /* ZeroableInteger.  *VAR is an int.  Like Unsigned Integer except
     that zero really means zero.  */
  var_zinteger,
} var_types;

/* This structure records one command'd definition.  */

struct cmd_list_element
  {
    /* Points to next command in this list.  */
    struct cmd_list_element *next;

    /* Name of this command.  */
    char *name;

    /* Command class; class values are chosen by application program.  */
    enum command_class class;

    /* Function definition of this command.
       Zero for command class names and for help topics that
       are not really commands.  */
    void (*function) ();
#   define NO_FUNCTION ((void (*)()) 0 )

    /* Documentation of this command (or help topic).
       First line is brief documentation; remaining lines form, with it,
       the full documentation.  First line should end with a period.
       Entire string should also end with a period, not a newline.  */
    char *doc;

    /* Auxiliary information.
       It is up to the calling program to decide what this means.  */
    char *aux;

    /* Nonzero identifies a prefix command.  For them, the address
       of the variable containing the list of subcommands.  */
    struct cmd_list_element **prefixlist;

    /* For prefix commands only:
       String containing prefix commands to get here: this one
       plus any others needed to get to it.  Should end in a space.
       It is used before the word "command" in describing the
       commands reached through this prefix.  */
    char *prefixname;

    /* For prefix commands only:
       nonzero means do not get an error if subcommand is not
       recognized; call the prefix's own function in that case.  */
    char allow_unknown;

    /* Nonzero says this is an abbreviation, and should not
       be mentioned in lists of commands.
       This allows "br<tab>" to complete to "break", which it
       otherwise wouldn't.  */
    char abbrev_flag;

    /* Completion routine for this command.  */
    char **(*completer)();

    /* Type of "set" or "show" command (or SET_NOT_SET if not "set"
       or "show").  */
    cmd_types type;

    /* Pointer to variable affected by "set" and "show".  Doesn't matter
       if type is not_set.  */
    char *var;

    /* What kind of variable is *VAR?  */
    var_types var_type;

    /* Pointer to command strings of user-defined commands */
    struct command_line *user_commands;
  };

/* Forward-declarations of the entry-points of command.c.  */

extern struct cmd_list_element *add_cmd ();
extern struct cmd_list_element *add_alias_cmd ();
extern struct cmd_list_element *add_prefix_cmd ();
extern struct cmd_list_element *add_abbrev_prefix_cmd ();
extern struct cmd_list_element *lookup_cmd (), *lookup_cmd_1 ();
extern void add_com ();
extern void add_com_alias ();
extern void add_info ();
extern void add_info_alias ();
extern char **complete_on_cmdlist ();
extern void delete_cmd ();
extern void help_cmd ();
extern struct cmd_list_element *add_set_cmd ();
extern struct cmd_list_element *add_show_from_set ();

/* Do a "set" or "show" command.  ARG is NULL if no argument, or the text
   of the argument, and FROM_TTY is nonzero if this command is being entered
   directly by the user (i.e. these are just like any other
   command).  C is the command list element for the command.  */
extern void do_setshow_command ();

/* Do a "show" command for each thing on a command list.  */
extern void cmd_show_list ();

extern void error_no_arg ();		/* Print error for missing argument */
extern void dont_repeat ();		/* Avoid auto-repeat of command */

#endif /* _COMMAND_H_INCLUDED */
