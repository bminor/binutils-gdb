/* OBSOLETE /* Low level interface to ptrace, for GDB when running under Unix. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 2000 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE    This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE    This program is free software; you can redistribute it and/or modify */
/* OBSOLETE    it under the terms of the GNU General Public License as published by */
/* OBSOLETE    the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE    (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE    This program is distributed in the hope that it will be useful, */
/* OBSOLETE    but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE    GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE    You should have received a copy of the GNU General Public License */
/* OBSOLETE    along with this program; if not, write to the Free Software */
/* OBSOLETE    Foundation, Inc., 59 Temple Place - Suite 330, */
/* OBSOLETE    Boston, MA 02111-1307, USA.  */ */
/* OBSOLETE  */
/* OBSOLETE #ifdef __GNUC__ */
/* OBSOLETE /* Bad implement execle(3). It's depend for "/bin/cc". */
/* OBSOLETE  */
/* OBSOLETE    main() */
/* OBSOLETE    { */
/* OBSOLETE    printf("execle:\n"); */
/* OBSOLETE    execle(FILE, ARGS, envp); */
/* OBSOLETE    exit(1); */
/* OBSOLETE    } */
/* OBSOLETE  */
/* OBSOLETE    GCC: */
/* OBSOLETE    link a6,#0 */
/* OBSOLETE    pea LC5      ; call printf */
/* OBSOLETE    jbsr _printf */
/* OBSOLETE    ;            ; (not popd stack) */
/* OBSOLETE    pea _envp    ; call execle */
/* OBSOLETE    clrl sp@- */
/* OBSOLETE    pea LC4 */
/* OBSOLETE    pea LC4 */
/* OBSOLETE    pea LC4 */
/* OBSOLETE    pea LC3 */
/* OBSOLETE    pea LC6 */
/* OBSOLETE    jbsr _execle */
/* OBSOLETE    addw #32,sp  ; delayed pop !! */
/* OBSOLETE  */
/* OBSOLETE    /bin/cc: */
/* OBSOLETE    link.l       fp,#L23 */
/* OBSOLETE    movem.l      #L24,(sp) */
/* OBSOLETE    pea  L26             ; call printf */
/* OBSOLETE    jbsr _printf */
/* OBSOLETE    addq.l       #4,sp   ; <--- popd stack !! */
/* OBSOLETE    pea  _envp           ; call execle */
/* OBSOLETE    clr.l        -(sp) */
/* OBSOLETE    pea  L32 */
/* OBSOLETE  */
/* OBSOLETE  */ */
/* OBSOLETE  */
/* OBSOLETE execle (char *name, char *args) */
/* OBSOLETE { */
/* OBSOLETE   register char **env = &args; */
/* OBSOLETE   while (*env++) */
/* OBSOLETE     ; */
/* OBSOLETE   execve (name, (char **) &args, (char **) *env); */
/* OBSOLETE } */
/* OBSOLETE #endif */
