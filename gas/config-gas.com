$!
$! This file sets things up to build gas on a VMS system to generate object
$! files for a VMS system.  We do not use the configure script, since we
$! do not have /bin/sh to execute it.
$!
$! If you are running this file, then obviously the host is vax-dec-vms.
$!
$gas_host="vms"
$!
$cpu_type="vax"
$emulation="generic"
$obj_format="vms"
$atof="vax"
$!
$! host specific information
$call link host.h	[.config]ho-'gas_host'.h
$!
$! Target specific information
$call link targ-cpu.c	[.config]tc-'cpu_type'.c
$call link targ-cpu.h	[.config]tc-'cpu_type'.h
$call link targ-env.h	[.config]te-'emulation'.h
$!
$! Code to handle the object file format.
$call link obj-format.h	[.config]obj-'obj_format'.h
$call link obj-format.c	[.config]obj-'obj_format'.c
$!
$! Code to handle floating point.
$call link atof-targ.c	[.config]atof-'atof'.c
$!
$!
$! Create the file version.opt, which helps identify the executalbe.
$!
$search Makefile.in "VERSION="/match=and/output=t.tmp
$open ifile$ t.tmp
$read ifile$ line
$close ifile$
$delete/nolog t.tmp;*
$! Discard "VERSION=" and "\n" parts.
$ijk=f$locate("=",line)+1
$line=f$extract(ijk,f$length(line)-ijk,line)
$ijk=f$locate("\n",line)
$line=f$extract(0,ijk,line)
$!
$ if f$search("version.opt").nes."" then delete/noconfirm version.opt;*
$open ifile$ version.opt/write
$write ifile$ "ident="+""""+line+""""
$close ifile$
$! Now write config.h.
$ if f$search("config.h").nes."" then delete/noconfirm config.h;*
$open ifile$ config.h/write
$write ifile$ "#define TARGET_CPU       """,cpu_type,"""
$write ifile$ "#define TARGET_ALIAS     ""vms"""
$write ifile$ "#define TARGET_CANONICAL ""vax-dec-vms"""
$write ifile$ "#define GAS_VERSION      """,line,""""
$close ifile$
$!
$ if f$search("config.status") .nes. "" then delete config.status.*
$ open/write file config.status
$ write file "Links are now set up for use with a vax running VMS."
$ close file
$ type config.status
$exit
$!
$!
$link:
$subroutine
$if f$search(p1).nes."" then delete/nolog 'p1';
$copy 'p2' 'p1'
$write sys$output "Linked ''p2' to ''p1'."
$endsubroutine
