_dnl__								-*- Texinfo -*-
_dnl__ Copyright (c) 1990 1991 Free Software Foundation, Inc.
_dnl__ This file is part of the source for the GDB manual.
@c M4 FRAGMENT $Id$
@c This text diverted to "Remote Debugging" section in general case;
@c however, if we're doing a manual specifically for one of these, it
@c belongs up front (in "Getting In and Out" chapter).
_if__(_I960__)
_if__(!_GENERIC__)
@node i960-Nindy Remote, EB29K Remote, Mode Options, Starting _GDBN__
_fi__(!_GENERIC__)
_if__(_GENERIC__)
@node i960-Nindy Remote, EB29K Remote, Remote, Remote
_fi__(_GENERIC__)
@subsection _GDBN__ with a Remote i960 (Nindy)

@cindex Nindy
@cindex i960
@dfn{Nindy} is a ROM Monitor program for Intel 960 target systems.  When
_GDBN__ is configured to control a remote Intel 960 using Nindy, you can
tell _GDBN__ how to connect to the 960 in several ways:

@itemize @bullet
@item
Through command line options specifying serial port, version of the
Nindy protocol, and communications speed;

@item
By responding to a prompt on startup;

@item
By using the @code{target} command at any point during your _GDBN__
session.  @xref{Target Commands}.

@end itemize

@menu
* Nindy Startup::		Startup with Nindy
* Nindy Options::		Options for Nindy
* Nindy reset::			Nindy Reset Command
@end menu

@node Nindy Startup, Nindy Options, i960-Nindy Remote, i960-Nindy Remote
@subsubsection Startup with Nindy

If you simply start @code{_GDBN__} without using any command-line
options, you are prompted for what serial port to use, @emph{before} you
reach the ordinary _GDBN__ prompt:
@example
Attach /dev/ttyNN -- specify NN, or "quit" to quit:  
@end example
@noindent
Respond to the prompt with whatever suffix (after @samp{/dev/tty})
identifies the serial port you want to use.  You can, if you choose,
simply start up with no Nindy connection by responding to the prompt
with an empty line.  If you do this, and later wish to attach to Nindy,
use @code{target} (@pxref{Target Commands}).

@node Nindy Options, Nindy reset, Nindy Startup, i960-Nindy Remote
@subsubsection Options for Nindy

These are the startup options for beginning your _GDBN__ session with a
Nindy-960 board attached:

@table @code
@item -r @var{port}
Specify the serial port name of a serial interface to be used to connect
to the target system.  This option is only available when _GDBN__ is
configured for the Intel 960 target architecture.  You may specify
@var{port} as any of: a full pathname (e.g. @samp{-r /dev/ttya}), a
device name in @file{/dev} (e.g. @samp{-r ttya}), or simply the unique
suffix for a specific @code{tty} (e.g. @samp{-r a}).

@item -O
(An uppercase letter ``O'', not a zero.)  Specify that _GDBN__ should use
the ``old'' Nindy monitor protocol to connect to the target system.
This option is only available when _GDBN__ is configured for the Intel 960
target architecture.

@quotation
@emph{Warning:} if you specify @samp{-O}, but are actually trying to
connect to a target system that expects the newer protocol, the connection
will fail, appearing to be a speed mismatch.  _GDBN__ will repeatedly
attempt to reconnect at several different line speeds.  You can abort
this process with an interrupt.
@end quotation

@item -brk
Specify that _GDBN__ should first send a @code{BREAK} signal to the target
system, in an attempt to reset it, before connecting to a Nindy target.

@quotation
@emph{Warning:} Many target systems do not have the hardware that this
requires; it only works with a few boards.
@end quotation

@end table

The standard @samp{-b} option controls the line speed used on the serial
port. 

@node Nindy reset,  , Nindy Options, i960-Nindy Remote
@c @group
@subsubsection Nindy Reset Command
@table @code
@item reset
@kindex reset
For a Nindy target, this command sends a ``break'' to the remote target
system; this is only useful if the target has been equipped with a
circuit to perform a hard reset (or some other interesting action) when
a break is detected.
@end table
@c @end group
_fi__(_I960__)

_if__(_AMD29K__)
_if__(!_GENERIC__)
@node EB29K Remote, VxWorks Remote, i960-Nindy Remote, Starting _GDBN__
_fi__(!_GENERIC__)
_if__(_GENERIC__)
@node EB29K Remote, VxWorks Remote, i960-Nindy Remote, Remote
_fi__(_GENERIC__)
@subsection _GDBN__ with a Remote EB29K

@cindex EB29K board
@cindex running 29K programs

To use _GDBN__ from a Unix system to run programs on AMD's EB29K
board in a PC, you must first connect a serial cable between the PC
and a serial port on the Unix system.  In the following, we assume
you've hooked the cable between the PC's @file{COM1} port and
@file{/dev/ttya} on the Unix system.

@menu
* Comms (EB29K)::		Communications Setup
* _GDBP__-EB29K::			EB29K cross-debugging
* Remote Log::			Remote Log
@end menu

@node Comms (EB29K), _GDBP__-EB29K, EB29K Remote, EB29K Remote
@subsubsection Communications Setup
The next step is to set up the PC's port, by doing something like the
following in DOS on the PC:
_0__@example
C:\> MODE com1:9600,n,8,1,none
_1__@end example
@noindent
This example---run on an MS DOS 4.0 system---sets the PC port to 9600
bps, no parity, eight data bits, one stop bit, and no ``retry'' action;
you must match the communications parameters when establishing the Unix
end of the connection as well.
@c FIXME: Who knows what this "no retry action" crud from the DOS manual may
@c       mean?  It's optional; leave it out? ---pesch@cygnus.com, 25feb91 

To give control of the PC to the Unix side of the serial line, type
the following at the DOS console:
_0__@example
C:\> CTTY com1
_1__@end example
@noindent
(Later, if you wish to return control to the DOS console, you can use
the command @code{CTTY con}---but you must send it over the device that
had control, in our example over the @file{COM1} serial line).

From the Unix host, use a communications program such as @code{tip} or
@code{cu} to communicate with the PC; for example,
@example
cu -s 9600 -l /dev/ttya
@end example
@noindent
The @code{cu} options shown specify, respectively, the linespeed and the
serial port to use.  If you use @code{tip} instead, your command line
may look something like the following:
@example
tip -9600 /dev/ttya
@end example
@noindent
Your system may define a different name where our example uses
@file{/dev/ttya} as the argument to @code{tip}.  The communications
parameters, including what port to use, are associated with the
@code{tip} argument in the ``remote'' descriptions file---normally the
system table @file{/etc/remote}.
@c FIXME: What if anything needs doing to match the "n,8,1,none" part of
@c the DOS side's comms setup?  cu can support -o (odd
@c parity), -e (even parity)---apparently no settings for no parity or
@c for character size.  Taken from stty maybe...?  John points out tip
@c can set these as internal variables, eg ~s parity=none; man stty
@c suggests that it *might* work to stty these options with stdin or
@c stdout redirected... ---pesch@cygnus.com, 25feb91

@kindex EBMON
Using the @code{tip} or @code{cu} connection, change the DOS working
directory to the directory containing a copy of your 29K program, then
start the PC program @code{EBMON} (an EB29K control program supplied
with your board by AMD).  You should see an initial display from
@code{EBMON} similar to the one that follows, ending with the
@code{EBMON} prompt @samp{#}---
_0__@example
C:\> G:

G:\> CD \usr\joe\work29k

G:\USR\JOE\WORK29K> EBMON
Am29000 PC Coprocessor Board Monitor, version 3.0-18
Copyright 1990 Advanced Micro Devices, Inc.
Written by Gibbons and Associates, Inc.

Enter '?' or 'H' for help

PC Coprocessor Type   = EB29K
I/O Base              = 0x208
Memory Base           = 0xd0000

Data Memory Size      = 2048KB
Available I-RAM Range = 0x8000 to 0x1fffff
Available D-RAM Range = 0x80002000 to 0x801fffff

PageSize              = 0x400
Register Stack Size   = 0x800
Memory Stack Size     = 0x1800

CPU PRL               = 0x3
Am29027 Available     = No
Byte Write Available  = Yes

# ~.
_1__@end example

Then exit the @code{cu} or @code{tip} program (done in the example by
typing @code{~.} at the @code{EBMON} prompt).  @code{EBMON} will keep
running, ready for _GDBN__ to take over.

For this example, we've assumed what is probably the most convenient
way to make sure the same 29K program is on both the PC and the Unix
system: a PC/NFS connection that establishes ``drive @code{G:}'' on the
PC as a file system on the Unix host.  If you don't have PC/NFS or
something similar connecting the two systems, you must arrange some
other way---perhaps floppy-disk transfer---of getting the 29K program
from the Unix system to the PC; _GDBN__ will @emph{not} download it over the
serial line.

@node _GDBP__-EB29K, Remote Log, Comms (EB29K), EB29K Remote
@subsubsection EB29K cross-debugging
Finally, @code{cd} to the directory containing an image of your 29K
program on the Unix system, and start _GDBN__---specifying as argument the
name of your 29K program:
@example
cd /usr/joe/work29k
_GDBP__ myfoo
@end example
Now you can use the @code{target} command:
@example
target amd-eb /dev/ttya 9600 MYFOO
@end example
@c FIXME: test above 'target amd-eb' as spelled, with caps!  caps are meant to
@c emphasize that this is the name as seen by DOS (since I think DOS is
@c single-minded about case of letters).  ---pesch@cygnus.com, 25feb91

@noindent
In this example, we've assumed your program is in a file called
@file{myfoo}.  Note that the filename given as the last argument to
@code{target amd-eb} should be the name of the program as it appears to DOS.
In our example this is simply @code{MYFOO}, but in general it can include
a DOS path, and depending on your transfer mechanism may not resemble
the name on the Unix side.

At this point, you can set any breakpoints you wish; when you're ready
to see your program run on the 29K board, use the _GDBN__ command
@code{run}.

To stop debugging the remote program, use the _GDBN__ @code{detach}
command.  

To return control of the PC to its console, use @code{tip} or @code{cu}
once again, after your _GDBN__ session has concluded, to attach to
@code{EBMON}.  You can then type the command @code{q} to shut down
@code{EBMON}, returning control to the DOS command-line interpreter.
Type @code{CTTY con} to return command input to the main DOS console,
and type @kbd{~.} to leave @code{tip} or @code{cu}.

@node Remote Log,  , _GDBP__-EB29K, EB29K Remote
@subsubsection Remote Log
@kindex eb.log
@cindex log file for EB29K
The @code{target amd-eb} command creates a file @file{eb.log} in the
current working directory, to help debug problems with the connection.
@file{eb.log} records all the output from @code{EBMON}, including echoes
of the commands sent to it.  Running @samp{tail -f} on this file in
another window often helps to understand trouble with @code{EBMON}, or
unexpected events on the PC side of the connection.
_fi__(_AMD29K__)

_if__(_VXWORKS__)
_if__(!_GENERIC__)
@node VxWorks Remote,  , EB29K Remote, Starting _GDBN__
_fi__(!_GENERIC__)
_if__(_GENERIC__)
@node VxWorks Remote,  , EB29K Remote, Remote
_fi__(_GENERIC__)
@subsection _GDBN__ and VxWorks
@cindex VxWorks
_GDBN__ enables developers to spawn and debug tasks running on networked
VxWorks targets from a Unix host.  Already-running tasks spawned from
the VxWorks shell can also be debugged.  _GDBN__ uses code that runs on
both the UNIX host and on the VxWorks target.  The program
@code{_GDBP__} is installed and executed on the UNIX host.  

The remote debugging interface (RDB) routines are installed and executed
on the VxWorks target.  These routines are included in the VxWorks library
@file{rdb.a} and are incorporated into the system image when source-level
debugging is enabled in the VxWorks configuration.

@kindex INCLUDE_RDB
Defining @code{INCLUDE_RDB} in the VxWorks configuration file
@file{configAll.h} includes the RDB interface routines and spawns the
source debugging task @code{tRdbTask} when VxWorks is booted.  For more
information on configuring and remaking VxWorks, see the @cite{VxWorks
Programmer's Guide}.

Once you have included the RDB interface in your VxWorks system image
and set your Unix execution search path to find _GDBN__, you are ready
to run _GDBN__.  From your UNIX host, type:

@smallexample
% _GDBP__
@end smallexample

_GDBN__ will come up showing the prompt:

@smallexample
(_GDBP__)
@end smallexample

@menu
* VxWorks connection::		Connecting to VxWorks
* VxWorks download::		VxWorks Download
* VxWorks attach::		Running Tasks
@end menu

@node VxWorks connection, VxWorks download, VxWorks Remote, VxWorks Remote
@subsubsection Connecting to VxWorks

The _GDBN__ command @code{target} lets you connect to a VxWorks target on the
network.  To connect to a target whose host name is ``@code{tt}'', type:

@smallexample
(_GDBP__) target vxworks tt
@end smallexample

_GDBN__ will display a message similar to the following:

@smallexample
Attaching remote machine across net... Success!
@end smallexample

_GDBN__ will then attempt to read the symbol tables of any object
modules loaded into the VxWorks target since it was last booted.
_GDBN__ locates these files by searching the directories listed in the
command search path (@pxref{Environment}); if it fails to find an
object file, it will display a message such as:

@smallexample
prog.o: No such file or directory.
@end smallexample

This will cause the @code{target} command to abort.  When this happens,
you should add the appropriate directory to the search path, with the
_GDBN__ command @code{path}, and execute the @code{target} command
again.

@node VxWorks download, VxWorks attach, VxWorks connection, VxWorks Remote
@subsubsection VxWorks Download

@cindex download to VxWorks
If you have connected to the VxWorks target and you want to debug an
object that has not yet been loaded, you can use the _GDBN__ @code{load}
command to download a file from UNIX to VxWorks incrementally.  The
object file given as an argument to the @code{load} command is actually
opened twice: first by the VxWorks target in order to download the code,
then by _GDBN__ in order to read the symbol table.  This can lead to
problems if the current working directories on the two systems differ.
It is simplest to set the working directory on both systems to the
directory in which the object file resides, and then to reference the
file by its name, without any path.  Thus, to load a program
@file{prog.o}, residing in @file{wherever/vw/demo/rdb}, on VxWorks type:

@smallexample
-> cd "wherever/vw/demo/rdb"
@end smallexample

On _GDBN__ type:

@smallexample
(_GDBP__) cd wherever/vw/demo/rdb 
(_GDBP__) load prog.o
@end smallexample

_GDBN__ will display a response similar to the following:

@smallexample
Reading symbol data from wherever/vw/demo/rdb/prog.o... done.
@end smallexample

You can also use the @code{load} command to reload an object module
after editing and recompiling the corresponding source file.  Note that
this will cause _GDBN__ to delete all currently-defined breakpoints,
auto-displays, and convenience variables, and to clear the value
history.  (This is necessary in order to preserve the integrity of
debugger data structures that reference the target system's symbol
table.)

@node VxWorks attach,  , VxWorks download, VxWorks Remote
@subsubsection Running Tasks

@cindex running VxWorks tasks
You can also attach to an existing task using the @code{attach} command as
follows:

@smallexample
(_GDBP__) attach @var{task}
@end smallexample

where @var{task} is the VxWorks hexadecimal task ID.  The task can be running
or suspended when you attach to it.  If running, it will be suspended at
the time of attachment.

_fi__(_VXWORKS__)
