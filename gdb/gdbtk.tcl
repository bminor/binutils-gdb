# GDB GUI setup

set cfile Blank
set wins($cfile) .text
set current_label {}
set screen_height 0
set screen_top 0
set screen_bot 0
set current_output_win .command.text
set cfunc NIL

proc test {} {
	update_listing {termcap.c foo /etc/termcap 200}
}

proc echo string {puts stdout $string}

if [info exists env(EDITOR)] then {
	set editor $env(EDITOR)
	} else {
	set editor emacs
}

# GDB callbacks
#
#  These functions are called by GDB (from C code) to do various things in
#  TK-land.  All start with the prefix `gdbtk_tcl_' to make them easy to find.
#

#
# GDB Callback:
#
#	gdbtk_tcl_fputs (text) - Output text to the command window
#
# Description:
#
#	GDB calls this to output TEXT to the GDB command window.  The text is
#	placed at the end of the text widget.  Note that output may not occur,
#	due to buffering.  Use gdbtk_tcl_flush to cause an immediate update.
#

proc gdbtk_tcl_fputs {arg} {
	global current_output_win

	$current_output_win insert end "$arg"
	$current_output_win yview -pickplace end
}

#
# GDB Callback:
#
#	gdbtk_tcl_flush () - Flush output to the command window
#
# Description:
#
#	GDB calls this to force all buffered text to the GDB command window.
#

proc gdbtk_tcl_flush {} {
	$current_output_win yview -pickplace end
	update idletasks
}

#
# GDB Callback:
#
#	gdbtk_tcl_query (message) - Create a yes/no query dialog box
#
# Description:
#
#	GDB calls this to create a yes/no dialog box containing MESSAGE.  GDB
#	is hung while the dialog box is active (ie: no commands will work),
#	however windows can still be refreshed in case of damage or exposure.
#

proc gdbtk_tcl_query {message} {
	tk_dialog .query "gdb : query" "$message" {} 1 "No" "Yes"
	}

#
# GDB Callback:
#
#	gdbtk_start_variable_annotation (args ...) - 
#
# Description:
#
#	Not yet implemented.
#

proc gdbtk_tcl_start_variable_annotation {valaddr ref_type stor_cl cum_expr field type_cast} {
	echo "gdbtk_tcl_start_variable_annotation $valaddr $ref_type $stor_cl $cum_expr $field $type_cast"
}

#
# GDB Callback:
#
#	gdbtk_end_variable_annotation (args ...) - 
#
# Description:
#
#	Not yet implemented.
#

proc gdbtk_tcl_end_variable_annotation {} {
	echo gdbtk_tcl_end_variable_annotation
}

#
# GDB Callback:
#
#	gdbtk_tcl_breakpoint (action bpnum file line) - Notify the TK
#	interface of changes to breakpoints.
#
# Description:
#
#	GDB calls this to notify TK of changes to breakpoints.  ACTION is one
#	of:
#		create		- Notify of breakpoint creation
#		delete		- Notify of breakpoint deletion
#		enable		- Notify of breakpoint enabling
#		disable		- Notify of breakpoint disabling
#
#	All actions take the same set of arguments:  BPNUM is the breakpoint
#	number,  FILE is the source file and LINE is the line number, and PC is
#	the pc of the affected breakpoint.
#

proc gdbtk_tcl_breakpoint {action bpnum file line pc} {
	${action}_breakpoint $bpnum $file $line $pc
}

#
# Local procedure:
#
#	create_breakpoint (bpnum file line pc) - Record breakpoint info in TK land
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to notify TK
#	land of breakpoint creation.  This consists of recording the file and
#	line number in the breakpoint_file and breakpoint_line arrays.  Also,
#	if there is already a window associated with FILE, it is updated with
#	a breakpoint tag.
#

proc create_breakpoint {bpnum file line pc} {
	global wins
	global breakpoint_file
	global breakpoint_line
	global pos_to_breakpoint
	global cfunc
	global pclist

# Record breakpoint locations

	set breakpoint_file($bpnum) $file
	set breakpoint_line($bpnum) $line
	set pos_to_breakpoint($file:$line) $bpnum
	
# If there's a window for this file, update it

	if [info exists wins($file)] {
		insert_breakpoint_tag $wins($file) $line
	}

# If there's an assembly window, update that too

	set win .asm.func_${cfunc}
	if [winfo exists $win] {
		set line [lsearch -exact $pclist($cfunc) $pc]
		incr line
		insert_breakpoint_tag $win $line
	}
}

#
# Local procedure:
#
#	delete_breakpoint (bpnum file line pc) - Delete breakpoint info from TK land
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to notify TK
#	land of breakpoint destruction.  This consists of removing the file and
#	line number from the breakpoint_file and breakpoint_line arrays.  Also,
#	if there is already a window associated with FILE, the tags are removed
#	from it.
#

proc delete_breakpoint {bpnum file line pc} {
	global wins
	global breakpoint_file
	global breakpoint_line
	global pos_to_breakpoint

# Save line number and file for later

	set line $breakpoint_line($bpnum)

	set file $breakpoint_file($bpnum)

# Reset breakpoint annotation info

	unset pos_to_breakpoint($file:$line)
	unset breakpoint_file($bpnum)
	unset breakpoint_line($bpnum)

# If there's a window for this file, update it

	if [info exists wins($file)] {
		delete_breakpoint_tag $wins($file) $line
	}
}

#
# Local procedure:
#
#	enable_breakpoint (bpnum file line pc) - Record breakpoint info in TK land
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to notify TK
#	land of a breakpoint being enabled.  This consists of unstippling the
#	specified breakpoint indicator.
#

proc enable_breakpoint {bpnum file line pc} {
	global wins

	$wins($file) tag configure $line -fgstipple {}
}

#
# Local procedure:
#
#	disable_breakpoint (bpnum file line pc) - Record breakpoint info in TK land
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to notify TK
#	land of a breakpoint being disabled.  This consists of stippling the
#	specified breakpoint indicator.
#

proc disable_breakpoint {bpnum file line pc} {
	global wins

	$wins($file) tag configure $line -fgstipple gray50
}

#
# Local procedure:
#
#	insert_breakpoint_tag (win line) - Insert a breakpoint tag in WIN.
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to insert a
#	breakpoint tag into window WIN at line LINE.
#

proc insert_breakpoint_tag {win line} {
	$win configure -state normal
	$win delete $line.0
	$win insert $line.0 "B"
	$win tag add $line $line.0

	$win configure -state disabled
}

#
# Local procedure:
#
#	delete_breakpoint_tag (win line) - Remove a breakpoint tag from WIN.
#
# Description:
#
#	GDB calls this indirectly (through gdbtk_tcl_breakpoint) to remove a
#	breakpoint tag from window WIN at line LINE.
#

proc delete_breakpoint_tag {win line} {
	$win configure -state normal
	$win delete $line.0
	$win insert $line.0 " "
	$win tag delete $line
	$win configure -state disabled
}

#
# Menu:
#
#	file popup menu - Define the file popup menu.
#
# Description:
#
#	This menu just contains a bunch of buttons that do various things to
#	the line under the cursor.
#
# Items:
#
#	Edit - Run the editor (specified by the environment variable EDITOR) on
#	       this file, at the current line.
#	Breakpoint - Set a breakpoint at the current line.  This just shoves
#		a `break' command at GDB with the appropriate file and line
#		number.  Eventually, GDB calls us back (at gdbtk_tcl_breakpoint)
#		to notify us of where the breakpoint needs to show up.
#

menu .file_popup -cursor hand2
.file_popup add command -label "Not yet set" -state disabled
.file_popup add separator
.file_popup add command -label "Edit" -command {exec $editor +$selected_line $selected_file &}
.file_popup add command -label "Set breakpoint" -command {gdb_cmd "break $selected_file:$selected_line"}

#
# Bindings:
#
#	file popup menu - Define the file popup menu bindings.
#
# Description:
#
#	This defines the binding for the file popup menu.  Currently, there is
#	only one, which is activated when Button-1 is released.  This causes
#	the menu to be unposted, releases the grab for the menu, and then
#	unhighlights the line under the cursor.  After that, the selected menu
#	item is invoked.
#

bind .file_popup <Any-ButtonRelease-1> {
	global selected_win

# First, remove the menu, and release the pointer

	.file_popup unpost
	grab release .file_popup

# Unhighlight the selected line

	$selected_win tag delete breaktag

# Actually invoke the menubutton here!

	tk_invokeMenu %W
}

#
# Local procedure:
#
#	file_popup_menu (win x y xrel yrel) - Popup the file popup menu.
#
# Description:
#
#	This procedure is invoked as a result of a command binding in the
#	listing window.  It does several things:
#		o - It highlights the line under the cursor.
#		o - It pops up the file popup menu which is intended to do
#		    various things to the aforementioned line.
#		o - Grabs the mouse for the file popup menu.
#

# Button 1 has been pressed in a listing window.  Pop up a menu.

proc file_popup_menu {win x y xrel yrel} {
	global wins
	global win_to_file
	global file_to_debug_file
	global highlight
	global selected_line
	global selected_file
	global selected_win

# Map TK window name back to file name.

	set file $win_to_file($win)

	set pos [$win index @$xrel,$yrel]

# Record selected file and line for menu button actions

	set selected_file $file_to_debug_file($file)
	set selected_line [lindex [split $pos .] 0]
	set selected_win $win

# Highlight the selected line

	eval $win tag config breaktag $highlight
	$win tag add breaktag "$pos linestart" "$pos linestart + 1l"

# Post the menu near the pointer, (and grab it)

	.file_popup entryconfigure 0 -label "$selected_file:$selected_line"
	.file_popup post [expr $x-[winfo width .file_popup]/2] [expr $y-10]
	grab .file_popup
}

#
# Local procedure:
#
#	listing_window_button_1 (win x y xrel yrel) - Handle button 1 in listing window
#
# Description:
#
#	This procedure is invoked as a result of holding down button 1 in the
#	listing window.  The action taken depends upon where the button was
#	pressed.  If it was in the left margin (the breakpoint column), it
#	sets or clears a breakpoint.  In the main text area, it will pop up a
#	menu.
#

proc listing_window_button_1 {win x y xrel yrel} {
	global wins
	global win_to_file
	global file_to_debug_file
	global highlight
	global selected_line
	global selected_file
	global selected_win
	global pos_to_breakpoint

# Map TK window name back to file name.

	set file $win_to_file($win)

	set pos [split [$win index @$xrel,$yrel] .]

# Record selected file and line for menu button actions

	set selected_file $file_to_debug_file($file)
	set selected_line [lindex $pos 0]
	set selected_col [lindex $pos 1]
	set selected_win $win

# If we're in the margin, then toggle the breakpoint

	if {$selected_col < 8} {
		set pos_break $selected_file:$selected_line
		set pos $file:$selected_line
		set tmp pos_to_breakpoint($pos)
		if [info exists $tmp] {
			set bpnum [set $tmp]
			gdb_cmd "delete	$bpnum"
		} else {
			gdb_cmd "break $pos_break"
		}
		return
	}

# Post the menu near the pointer, (and grab it)

	.file_popup entryconfigure 0 -label "$selected_file:$selected_line"
	.file_popup post [expr $x-[winfo width .file_popup]/2] [expr $y-10]
	grab .file_popup
}

#
# Local procedure:
#
#	asm_window_button_1 (win x y xrel yrel) - Handle button 1 in asm window
#
# Description:
#
#	This procedure is invoked as a result of holding down button 1 in the
#	assembly window.  The action taken depends upon where the button was
#	pressed.  If it was in the left margin (the breakpoint column), it
#	sets or clears a breakpoint.  In the main text area, it will pop up a
#	menu.
#

proc asm_window_button_1 {win x y xrel yrel} {
	global wins
	global win_to_file
	global file_to_debug_file
	global highlight
	global selected_line
	global selected_file
	global selected_win
	global pos_to_breakpoint
	global pclist
	global cfunc

	set pos [split [$win index @$xrel,$yrel] .]

# Record selected file and line for menu button actions

	set selected_line [lindex $pos 0]
	set selected_col [lindex $pos 1]
	set selected_win $win

# Figure out the PC

	set pc [lindex $pclist($cfunc) $selected_line]

# If we're in the margin, then toggle the breakpoint

	if {$selected_col < 8} {
		set tmp pos_to_breakpoint($pc)
		if [info exists $tmp] {
			set bpnum [set $tmp]
			gdb_cmd "delete	$bpnum"
		} else {
			gdb_cmd "break *$pc"
		}
		return
	}

# Post the menu near the pointer, (and grab it)

#	.file_popup entryconfigure 0 -label "$selected_file:$selected_line"
#	.file_popup post [expr $x-[winfo width .file_popup]/2] [expr $y-10]
#	grab .file_popup
}

#
# Local procedure:
#
#	do_nothing - Does absoultely nothing.
#
# Description:
#
#	This procedure does nothing.  It is used as a placeholder to allow
#	the disabling of bindings that would normally be inherited from the
#	parent widget.  I can't think of any other way to do this.
#

proc do_nothing {} {}

#
# Local procedure:
#
#	create_file_win (filename) - Create a win for FILENAME.
#
# Return value:
#
#	The new text widget.
#
# Description:
#
#	This procedure creates a text widget for FILENAME.  It returns the
#	newly created widget.  First, a text widget is created, and given basic
#	configuration info.  Second, all the bindings are setup.  Third, the
#	file FILENAME is read into the text widget.  Fourth, margins and line
#	numbers are added.
#

proc create_file_win {filename} {
	global breakpoint_file
	global breakpoint_line

# Replace all the dirty characters in $filename with clean ones, and generate
# a unique name for the text widget.

	regsub -all {\.|/} $filename {} temp
	set win .text$temp

# Actually create and do basic configuration on the text widget.

	text $win -height 25 -width 80 -relief raised -borderwidth 2 -yscrollcommand textscrollproc -setgrid true -cursor hand2

# Setup all the bindings

	bind $win <Enter> {focus %W}
	bind $win <1> {listing_window_button_1 %W %X %Y %x %y}
	bind $win <B1-Motion> do_nothing
	bind $win n {gdb_cmd next ; update_ptr}
	bind $win s {gdb_cmd step ; update_ptr}
	bind $win c {gdb_cmd continue ; update_ptr}
	bind $win f {gdb_cmd finish ; update_ptr}
	bind $win u {gdb_cmd up ; update_ptr}
	bind $win d {gdb_cmd down ; update_ptr}

# Open the file, and read it into the text widget

	set fh [open $filename]
	$win delete 0.0 end
	$win insert 0.0 [read $fh]
	close $fh

# Add margins (for annotations) and a line number to each line

	set numlines [$win index end]
	set numlines [lindex [split $numlines .] 0]
	for {set i 1} {$i <= $numlines} {incr i} {
		$win insert $i.0 [format "   %4d " $i]
		}

# Scan though the breakpoint data base and install any destined for this file

	foreach bpnum [array names breakpoint_file] {
		if {$breakpoint_file($bpnum) == $filename} {
			insert_breakpoint_tag $win $breakpoint_line($bpnum)
			}
		}

# Disable the text widget to prevent user modifications

	$win configure -state disabled
	return $win
}

#
# Local procedure:
#
#	create_asm_win (funcname) - Create an assembly win for FUNCNAME.
#
# Return value:
#
#	The new text widget.
#
# Description:
#
#	This procedure creates a text widget for FUNCNAME.  It returns the
#	newly created widget.  First, a text widget is created, and given basic
#	configuration info.  Second, all the bindings are setup.  Third, the
#	function FUNCNAME is read into the text widget.
#

proc create_asm_win {funcname} {
	global breakpoint_file
	global breakpoint_line
	global current_output_win
	global pclist

# Replace all the dirty characters in $filename with clean ones, and generate
# a unique name for the text widget.

	set win .asm.func_${funcname}

# Actually create and do basic configuration on the text widget.

	text $win -height 25 -width 80 -relief raised -borderwidth 2 \
		-setgrid true -cursor hand2 -yscrollcommand asmscrollproc

# Setup all the bindings

	bind $win <Enter> {focus %W}
	bind $win <1> {asm_window_button_1 %W %X %Y %x %y}
	bind $win <B1-Motion> do_nothing
	bind $win n {gdb_cmd nexti ; update_ptr}
	bind $win s {gdb_cmd stepi ; update_ptr}
	bind $win c {gdb_cmd continue ; update_ptr}
	bind $win f {gdb_cmd finish ; update_ptr}
	bind $win u {gdb_cmd up ; update_ptr}
	bind $win d {gdb_cmd down ; update_ptr}

# Disassemble the code, and read it into the new text widget

	set current_output_win $win
	gdb_cmd "disassemble $funcname"
	set current_output_win .command.text

	set numlines [$win index end]
	set numlines [lindex [split $numlines .] 0]

# Delete the first and last lines, cuz these contain useless info

	$win delete 1.0 2.0
	$win delete {end - 1 lines} end

# Add margins (for annotations) and note the PC for each line

	if [info exists pclist($funcname)] { unset pclist($funcname) }
	for {set i 1} {$i <= $numlines} {incr i} {
		scan [$win get $i.0 "$i.0 lineend"] "%s " pc
		lappend pclist($funcname) $pc
		$win insert $i.0 "    "
		}


# Scan though the breakpoint data base and install any destined for this file

#	foreach bpnum [array names breakpoint_file] {
#		if {$breakpoint_file($bpnum) == $filename} {
#			insert_breakpoint_tag $win $breakpoint_line($bpnum)
#			}
#		}

# Disable the text widget to prevent user modifications

	$win configure -state disabled
	return $win
}

#
# Local procedure:
#
#	asmscrollproc (WINHEIGHT SCREENHEIGHT SCREENTOP SCREENBOT) - Update the
#	asm window scrollbar.
#
# Description:
#
#	This procedure is called to update the assembler window's scrollbar.
#

proc asmscrollproc {args} {
	global asm_screen_height asm_screen_top asm_screen_bot

	eval ".asm.scroll set $args"
	set asm_screen_height [lindex $args 1]
	set asm_screen_top [lindex $args 2]
	set asm_screen_bot [lindex $args 3]
}

#
# Local procedure:
#
#	update_listing (linespec) - Update the listing window according to
#				    LINESPEC.
#
# Description:
#
#	This procedure is called from various places to update the listing
#	window based on LINESPEC.  It is usually invoked with the result of
#	gdb_loc.
#
#	It will move the cursor, and scroll the text widget if necessary.
#	Also, it will switch to another text widget if necessary, and update
#	the label widget too.
#
#	LINESPEC is a list of the form:
#
#	{ DEBUG_FILE FUNCNAME FILENAME LINE }, where:
#
#	DEBUG_FILE - is the abbreviated form of the file name.  This is usually
#		     the file name string given to the cc command.  This is
#		     primarily needed for breakpoint commands, and when an
#		     abbreviated for of the filename is desired.
#	FUNCNAME - is the name of the function.
#	FILENAME - is the fully qualified (absolute) file name.  It is usually
#		   the same as $PWD/$DEBUG_FILE, where PWD is the working dir
#		   at the time the cc command was given.  This is used to
#		   actually locate the file to be displayed.
#	LINE - The line number to be displayed.
#
#	Usually, this procedure will just move the cursor one line down to the
#	next line to be executed.  However, if the cursor moves out of range
#	or into another file, it will scroll the text widget so that the line
#	of interest is in the middle of the viewable portion of the widget.
#

proc update_listing {linespec} {
	global pointers
	global screen_height
	global screen_top
	global screen_bot
	global wins cfile
	global current_label
	global win_to_file
	global file_to_debug_file

# Rip the linespec apart

	set line [lindex $linespec 3]
	set filename [lindex $linespec 2]
	set funcname [lindex $linespec 1]
	set debug_file [lindex $linespec 0]

# Sometimes there's no source file for this location

	if {$filename == ""} {set filename Blank}

# If we want to switch files, we need to unpack the current text widget, and
# stick in the new one.

	if {$filename != $cfile} then {
		pack forget $wins($cfile)
		set cfile $filename

# Create a text widget for this file if necessary

		if ![info exists wins($cfile)] then {
			set wins($cfile) [create_file_win $cfile]
			set win_to_file($wins($cfile)) $cfile
			set file_to_debug_file($cfile) $debug_file
			set pointers($cfile) 1.1
			}

# Pack the text widget into the listing widget, and scroll to the right place

		pack $wins($cfile) -side left -expand yes -in .listing -fill both -after .label
		$wins($cfile) yview [expr $line - $screen_height / 2]
		}

# Update the label widget in case the filename or function name has changed

	if {$current_label != "$filename.$funcname"} then {
		set tail [expr [string last / $filename] + 1]
		.label configure -text "[string range $filename $tail end] : ${funcname}()"
		set current_label $filename.$funcname
		}

# Update the pointer, scrolling the text widget if necessary to keep the
# pointer in an acceptable part of the screen.

	if [info exists pointers($cfile)] then {
		$wins($cfile) configure -state normal
		set pointer_pos $pointers($cfile)
		$wins($cfile) configure -state normal
		$wins($cfile) delete $pointer_pos
		$wins($cfile) insert $pointer_pos " "

		set pointer_pos [$wins($cfile) index $line.1]
		set pointers($cfile) $pointer_pos

		$wins($cfile) delete $pointer_pos
		$wins($cfile) insert $pointer_pos "\xbb"

		if {$line < $screen_top + 1
		    || $line > $screen_bot} then {
			$wins($cfile) yview [expr $line - $screen_height / 2]
			}

		$wins($cfile) configure -state disabled
		}
}

#
# Local procedure:
#
#	update_ptr - Update the listing window.
#
# Description:
#
#	This routine will update the listing window using the result of
#	gdb_loc.
#

proc update_ptr {} {update_listing [gdb_loc]}

#
# Local procedure:
#
#	asm_command - Open up the assembly window.
#
# Description:
#
#	Create an assembly window if it doesn't exist.
#

proc asm_command {} {
	global cfunc

	if ![winfo exists .asm] {
		set cfunc *None*
		set win .asm.func_${cfunc}
		toplevel .asm
		wm minsize .asm 1 1

		label .asm.label -text "*NIL*" -borderwidth 2 -relief raised
		text $win -height 25 -width 80 -relief raised -borderwidth 2 \
			-setgrid true -cursor hand2 \
			-yscrollcommand asmscrollproc
		scrollbar .asm.scroll -orient vertical -command {$win yview}
		frame .asm.buts

		button .asm.stepi -text Stepi \
			-command {gdb_cmd stepi ; update_ptr}
		button .asm.nexti -text Nexti \
			-command {gdb_cmd nexti ; update_ptr}
		button .asm.continue -text Continue \
			-command {gdb_cmd continue ; update_ptr}
		button .asm.finish -text Finish \
			-command {gdb_cmd finish ; update_ptr}
		button .asm.up -text Up -command {gdb_cmd up ; update_ptr}
		button .asm.down -text Down \
			-command {gdb_cmd down ; update_ptr}
		button .asm.bottom -text Bottom \
			-command {gdb_cmd {frame 0} ; update_ptr}
		button .asm.close -text Close -command {destroy .asm}

		pack .asm.label -side top -fill x
		pack .asm.stepi .asm.nexti .asm.continue .asm.finish .asm.up \
		     .asm.down .asm.bottom .asm.close -side left -in .asm.buts
		pack .asm.buts -side top -fill x
		pack $win -side left -expand yes -fill both
		pack .asm.scroll -side left -fill y

		update
	}
}

#
# Local procedure:
#
#	update_assembly - Update the assembly window.
#
# Description:
#
#	This procedure updates the assembly window.
#

proc update_assembly {linespec} {
	global asm_pointers
	global screen_height
	global screen_top
	global screen_bot
	global wins cfunc
	global current_label
	global win_to_file
	global file_to_debug_file
	global current_asm_label
	global pclist
	global asm_screen_height asm_screen_top asm_screen_bot

# Rip the linespec apart

	set pc [lindex $linespec 4]
	set line [lindex $linespec 3]
	set filename [lindex $linespec 2]
	set funcname [lindex $linespec 1]
	set debug_file [lindex $linespec 0]

	set win .asm.func_${cfunc}

# Sometimes there's no source file for this location

	if {$filename == ""} {set filename Blank}

# If we want to switch funcs, we need to unpack the current text widget, and
# stick in the new one.

	if {$funcname != $cfunc} then {
		pack forget $win
		set cfunc $funcname

		set win .asm.func_${cfunc}

# Create a text widget for this func if necessary

		if ![winfo exists $win] then {
			create_asm_win $cfunc
			set asm_pointers($cfunc) 1.1
			set current_asm_label NIL
			}

# Pack the text widget, and scroll to the right place

		pack $win -side left -expand yes -fill both \
			-after .asm.buts
		set line [lsearch -exact $pclist($cfunc) $pc]
		incr line
		$win yview [expr $line - $asm_screen_height / 2]
		}

# Update the label widget in case the filename or function name has changed

	if {$current_asm_label != $funcname} then {
		.asm.label configure -text $funcname
		set current_asm_label $funcname
		}

# Update the pointer, scrolling the text widget if necessary to keep the
# pointer in an acceptable part of the screen.

	if [info exists asm_pointers($cfunc)] then {
		$win configure -state normal
		set pointer_pos $asm_pointers($cfunc)
		$win configure -state normal
		$win delete $pointer_pos
		$win insert $pointer_pos " "

# Map the PC back to a line in the window		

		set line [lsearch -exact $pclist($cfunc) $pc]

		if {$line == -1} {
			echo "Can't find PC $pc"
			return
			}

		incr line

		set pointer_pos [$win index $line.1]
		set asm_pointers($cfunc) $pointer_pos

		$win delete $pointer_pos
		$win insert $pointer_pos "\xbb"

		if {$line < $asm_screen_top + 1
		    || $line > $asm_screen_bot} then {
			$win yview [expr $line - $asm_screen_height / 2]
			}

#		echo "Picking line $line"
#		$win yview -pickplace $line

		$win configure -state disabled
		}
}

proc update_ptr {} {
	update_listing [gdb_loc]
	if [winfo exists .asm] {
		update_assembly [gdb_loc]
	}
}

#
# Window:
#
#	listing window - Define the listing window.
#
# Description:
#
#

# Setup listing window

frame .listing

wm minsize . 1 1

label .label -text "*No file*" -borderwidth 2 -relief raised
text $wins($cfile) -height 25 -width 80 -relief raised -borderwidth 2 -yscrollcommand textscrollproc -setgrid true -cursor hand2
scrollbar .scroll -orient vertical -command {$wins($cfile) yview}

if {[tk colormodel .text] == "color"} {
	set highlight "-background red2 -borderwidth 2 -relief sunk"
} else {
	set fg [lindex [.text config -foreground] 4]
	set bg [lindex [.text config -background] 4]
	set highlight "-foreground $bg -background $fg -borderwidth 0"
}

proc textscrollproc {args} {global screen_height screen_top screen_bot
			    eval ".scroll set $args"
			    set screen_height [lindex $args 1]
			    set screen_top [lindex $args 2]
			    set screen_bot [lindex $args 3]}

$wins($cfile) insert 0.0 "  This page intentionally left blank."
$wins($cfile) configure -state disabled

pack .label -side bottom -fill x -in .listing
pack $wins($cfile) -side left -expand yes -in .listing -fill both
pack .scroll -side left -fill y -in .listing

button .start -text Start -command \
	{gdb_cmd {break main}
	 gdb_cmd {enable delete $bpnum}
	 gdb_cmd run
	 update_ptr }
button .step -text Step -command {gdb_cmd step ; update_ptr}
button .next -text Next -command {gdb_cmd next ; update_ptr}
button .continue -text Continue -command {gdb_cmd continue ; update_ptr}
button .finish -text Finish -command {gdb_cmd finish ; update_ptr}
#button .test -text Test -command {echo [info var]}
button .exit -text Exit -command {gdb_cmd quit}
button .up -text Up -command {gdb_cmd up ; update_ptr}
button .down -text Down -command {gdb_cmd down ; update_ptr}
button .bottom -text "Bottom" -command {gdb_cmd {frame 0} ; update_ptr}
button .asm_but -text "Asm" -command {asm_command ; update_ptr}

proc files_command {} {
	toplevel .files_window

	wm minsize .files_window 1 1
#	wm overrideredirect .files_window true
	listbox .files_window.list -geometry 30x20 -setgrid true
	button .files_window.close -text Close -command {destroy .files_window}
	tk_listboxSingleSelect .files_window.list
	eval .files_window.list insert 0 [lsort [gdb_listfiles]]
	pack .files_window.list -side top -fill both -expand yes
	pack .files_window.close -side bottom -fill x -expand no -anchor s
	bind .files_window.list <Any-ButtonRelease-1> {
		set file [%W get [%W curselection]]
		gdb_cmd "list $file:1,0"
		update_listing [gdb_loc $file:1]
		destroy .files_window}
}

button .files -text Files -command files_command

pack .listing -side bottom -fill both -expand yes
#pack .test -side bottom -fill x
pack .start .step .next .continue .finish .up .down .bottom .asm_but .files .exit -side left
toplevel .command

# Setup command window

label .command.label -text "* Command Buffer *" -borderwidth 2 -relief raised
text .command.text -height 25 -width 80 -relief raised -borderwidth 2 -setgrid true -cursor hand2

pack .command.label -side top -fill x
pack .command.text -side top -expand yes -fill both

set command_line {}

gdb_cmd {set language c}
gdb_cmd {set height 0}
gdb_cmd {set width 0}

bind .command.text <Any-Key> {
	global command_line

	%W insert end %A
	%W yview -pickplace end
	append command_line %A
	}
bind .command.text <Key-Return> {
	global command_line

	%W insert end \n
	%W yview -pickplace end
	gdb_cmd $command_line
	set command_line {}
	update_ptr
	%W insert end "(gdb) "
	%W yview -pickplace end
	}
bind .command.text <Enter> {focus %W}
bind .command.text <Delete> {delete_char %W}
bind .command.text <BackSpace> {delete_char %W}
proc delete_char {win} {
	global command_line

	tk_textBackspace $win
	$win yview -pickplace insert
	set tmp [expr [string length $command_line] - 2]
	set command_line [string range $command_line 0 $tmp]
}

wm minsize .command 1 1
