# GDB GUI setup

set cfile Blank
set wins($cfile) .src.text
set current_label {}
set screen_height 0
set screen_top 0
set screen_bot 0
set current_output_win .cmd.text
set cfunc NIL
set line_numbers 1
set breakpoint_file(-1) {[garbage]}

#option add *Foreground Black
#option add *Background White
#option add *Font -*-*-medium-r-normal--18-*-*-*-m-*-*-1
tk colormodel . monochrome

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

proc gdbtk_tcl_fputs_error {arg} {
	.cmd.text insert end "$arg"
	.cmd.text yview -pickplace end
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
	global current_output_win

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

proc asm_win_name {funcname} {
	if {$funcname == "*None*"} {return .asm.text}

	regsub -all {\.} $funcname _ temp

	return .asm.func_${temp}
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
	global pos_to_bpcount
	global cfunc
	global pclist

# Record breakpoint locations

	set breakpoint_file($bpnum) $file
	set breakpoint_line($bpnum) $line
	set pos_to_breakpoint($file:$line) $bpnum
	if ![info exists pos_to_bpcount($file:$line)] {
		set pos_to_bpcount($file:$line) 0
	}
	incr pos_to_bpcount($file:$line)
	set pos_to_breakpoint($pc) $bpnum
	if ![info exists pos_to_bpcount($pc)] {
		set pos_to_bpcount($pc) 0
	}
	incr pos_to_bpcount($pc)
	
# If there's a window for this file, update it

	if [info exists wins($file)] {
		insert_breakpoint_tag $wins($file) $line
	}

# If there's an assembly window, update that too

	set win [asm_win_name $cfunc]
	if [winfo exists $win] {
		insert_breakpoint_tag $win [pc_to_line $pclist($cfunc) $pc]
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
	global pos_to_bpcount
	global cfunc pclist

# Save line number and file for later

	set line $breakpoint_line($bpnum)

	set file $breakpoint_file($bpnum)

# Reset breakpoint annotation info

	if {$pos_to_bpcount($file:$line) > 0} {
		decr pos_to_bpcount($file:$line)

		if {$pos_to_bpcount($file:$line) == 0} {
			catch "unset pos_to_breakpoint($file:$line)"

			unset breakpoint_file($bpnum)
			unset breakpoint_line($bpnum)

# If there's a window for this file, update it

			if [info exists wins($file)] {
				delete_breakpoint_tag $wins($file) $line
			}
		}
	}

# If there's an assembly window, update that too

	if {$pos_to_bpcount($pc) > 0} {
		decr pos_to_bpcount($pc)

		if {$pos_to_bpcount($pc) == 0} {
			catch "unset pos_to_breakpoint($pc)"

			set win [asm_win_name $cfunc]
			if [winfo exists $win] {
				delete_breakpoint_tag $win [pc_to_line $pclist($cfunc) $pc]
			}
		}
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
	global cfunc pclist

	if [info exists wins($file)] {
		$wins($file) tag configure $line -fgstipple {}
	}

# If there's an assembly window, update that too

	set win [asm_win_name $cfunc]
	if [winfo exists $win] {
		$win tag configure [pc_to_line $pclist($cfunc) $pc] -fgstipple {}
	}
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
	global cfunc pclist

	if [info exists wins($file)] {
		$wins($file) tag configure $line -fgstipple gray50
	}

# If there's an assembly window, update that too

	set win [asm_win_name $cfunc]
	if [winfo exists $win] {
		$win tag configure [pc_to_line $pclist($cfunc) $pc] -fgstipple gray50
	}
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
	$win tag add delete $line.0 "$line.0 lineend"
	$win tag add margin $line.0 "$line.0 lineend"

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
	if {[string range $win 0 3] == ".src"} then {
		$win insert $line.0 "\xa4"
	} else {
		$win insert $line.0 " "
	}
	$win tag delete $line
	$win tag add delete $line.0 "$line.0 lineend"
	$win tag add margin $line.0 "$line.0 lineend"
	$win configure -state disabled
}

proc gdbtk_tcl_busy {} {
	if [winfo exists .src] {
		catch {.src.start configure -state disabled}
		catch {.src.stop configure -state normal}
		catch {.src.step configure -state disabled}
		catch {.src.next configure -state disabled}
		catch {.src.continue configure -state disabled}
		catch {.src.finish configure -state disabled}
		catch {.src.up configure -state disabled}
		catch {.src.down configure -state disabled}
		catch {.src.bottom configure -state disabled}
	}
	if [winfo exists .asm] {
		catch {.asm.stepi configure -state disabled}
		catch {.asm.nexti configure -state disabled}
		catch {.asm.continue configure -state disabled}
		catch {.asm.finish configure -state disabled}
		catch {.asm.up configure -state disabled}
		catch {.asm.down configure -state disabled}
		catch {.asm.bottom configure -state disabled}
		catch {.asm.close configure -state disabled}
	}
}

proc gdbtk_tcl_idle {} {
	if [winfo exists .src] {
		catch {.src.start configure -state normal}
		catch {.src.stop configure -state disabled}
		catch {.src.step configure -state normal}
		catch {.src.next configure -state normal}
		catch {.src.continue configure -state normal}
		catch {.src.finish configure -state normal}
		catch {.src.up configure -state normal}
		catch {.src.down configure -state normal}
		catch {.src.bottom configure -state normal}
	}

	if [winfo exists .asm] {
		catch {.asm.stepi configure -state normal}
		catch {.asm.nexti configure -state normal}
		catch {.asm.continue configure -state normal}
		catch {.asm.finish configure -state normal}
		catch {.asm.up configure -state normal}
		catch {.asm.down configure -state normal}
		catch {.asm.bottom configure -state normal}
		catch {.asm.close configure -state normal}
	}
}

#
# Local procedure:
#
#	decr (var val) - compliment to incr
#
# Description:
#
#
proc decr {var {val 1}} {
	upvar $var num
	set num [expr $num - $val]
	return $num
}

#
# Local procedure:
#
#	pc_to_line (pclist pc) - convert PC to a line number.
#
# Description:
#
#	Convert PC to a line number from PCLIST.  If exact line isn't found,
#	we return the first line that starts before PC.
#
proc pc_to_line {pclist pc} {
	set line [lsearch -exact $pclist $pc]

	if {$line >= 1} { return $line }

	set line 1
	foreach linepc [lrange $pclist 1 end] {
		if {$pc < $linepc} { decr line ; return $line }
		incr line
	}
	return [expr $line - 1]
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

	if {$selected_col < 11} {
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
#	create_expr_win - Creat expression display window
#
# Description:
#
#	Create the expression display window.
#

proc create_expr_win {} {
	toplevel .expr
	wm minsize .expr 1 1
	wm title .expr Expression
	canvas .expr.c -yscrollcommand {.expr.scroll set} -cursor hand2 \
		-borderwidth 2 -relief groove
	scrollbar .expr.scroll -orient vertical -command {.expr.c yview}
	entry .expr.entry -borderwidth 2 -relief groove

	pack .expr.entry -side bottom -fill x
	pack .expr.c -side left -fill both -expand yes
	pack .expr.scroll -side right -fill y

	.expr.c create text 100 0 -text "Text string"
	.expr.c create rectangle 245 195 255 205 -outline black -fill white
}

#
# Local procedure:
#
#	display_expression (expression) - Display EXPRESSION in display window
#
# Description:
#
#	Display EXPRESSION and it's value in the expression display window.
#

proc display_expression {expression} {
	if ![winfo exists .expr] {create_expr_win}


}

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

proc create_file_win {filename debug_file} {
	global breakpoint_file
	global breakpoint_line
	global line_numbers

# Replace all the dirty characters in $filename with clean ones, and generate
# a unique name for the text widget.

	regsub -all {\.} $filename {} temp
	set win .src.text$temp

# Open the file, and read it into the text widget

	if [catch "open $filename" fh] {
# File can't be read.  Put error message into .src.nofile window and return.

		catch {destroy .src.nofile}
		text .src.nofile -height 25 -width 88 -relief raised \
			-borderwidth 2 -yscrollcommand textscrollproc \
			-setgrid true -cursor hand2
		.src.nofile insert 0.0 $fh
		.src.nofile configure -state disabled
		bind .src.nofile <1> do_nothing
		bind .src.nofile <B1-Motion> do_nothing
		return .src.nofile
	}

# Actually create and do basic configuration on the text widget.

	text $win -height 25 -width 88 -relief raised -borderwidth 2 \
		-yscrollcommand textscrollproc -setgrid true -cursor hand2

# Setup all the bindings

	bind $win <Enter> {focus %W}
#	bind $win <1> {listing_window_button_1 %W %X %Y %x %y}
	bind $win <1> do_nothing
	bind $win <B1-Motion> do_nothing

	bind $win n {catch {gdb_cmd next} ; update_ptr}
	bind $win s {catch {gdb_cmd step} ; update_ptr}
	bind $win c {catch {gdb_cmd continue} ; update_ptr}
	bind $win f {catch {gdb_cmd finish} ; update_ptr}
	bind $win u {catch {gdb_cmd up} ; update_ptr}
	bind $win d {catch {gdb_cmd down} ; update_ptr}

	$win delete 0.0 end
	$win insert 0.0 [read $fh]
	close $fh

# Add margins (for annotations) and a line number to each line (if requested)

	set numlines [$win index end]
	set numlines [lindex [split $numlines .] 0]
	if $line_numbers {
		for {set i 1} {$i <= $numlines} {incr i} {
			$win insert $i.0 [format "   %4d " $i]
			$win tag add source $i.8 "$i.0 lineend"
			}
	} else {
		for {set i 1} {$i <= $numlines} {incr i} {
			$win insert $i.0 "        "
			$win tag add source $i.8 "$i.0 lineend"
			}
	}

# Add the breakdots

	foreach i [gdb_sourcelines $debug_file] {
		$win delete $i.0
		$win insert $i.0 "\xa4"
		$win tag add margin $i.0 $i.8
		}

	$win tag bind margin <1> {listing_window_button_1 %W %X %Y %x %y}
	$win tag bind source <1> {
		%W mark set anchor "@%x,%y wordstart"
		set last [%W index "@%x,%y wordend"]
		%W tag remove sel 0.0 anchor
		%W tag remove sel $last end
		%W tag add sel anchor $last
		}
#	$win tag bind source <Double-Button-1> {
#		%W mark set anchor "@%x,%y wordstart"
#		set last [%W index "@%x,%y wordend"]
#		%W tag remove sel 0.0 anchor
#		%W tag remove sel $last end
#		%W tag add sel anchor $last
#		echo "Selected [selection get]"
#		}
	$win tag bind source <B1-Motion> {
		%W tag remove sel 0.0 anchor
		%W tag remove sel $last end
		%W tag add sel anchor @%x,%y
		}
	$win tag bind sel <1> do_nothing
	$win tag bind sel <Double-Button-1> {display_expression [selection get]}
	$win tag raise sel


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
#	create_asm_win (funcname pc) - Create an assembly win for FUNCNAME.
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

proc create_asm_win {funcname pc} {
	global breakpoint_file
	global breakpoint_line
	global current_output_win
	global pclist

# Replace all the dirty characters in $filename with clean ones, and generate
# a unique name for the text widget.

	set win [asm_win_name $funcname]

# Actually create and do basic configuration on the text widget.

	text $win -height 25 -width 88 -relief raised -borderwidth 2 \
		-setgrid true -cursor hand2 -yscrollcommand asmscrollproc

# Setup all the bindings

	bind $win <Enter> {focus %W}
	bind $win <1> {asm_window_button_1 %W %X %Y %x %y}
	bind $win <B1-Motion> do_nothing
	bind $win n {catch {gdb_cmd nexti} ; update_ptr}
	bind $win s {catch {gdb_cmd stepi} ; update_ptr}
	bind $win c {catch {gdb_cmd continue} ; update_ptr}
	bind $win f {catch {gdb_cmd finish} ; update_ptr}
	bind $win u {catch {gdb_cmd up} ; update_ptr}
	bind $win d {catch {gdb_cmd down} ; update_ptr}

# Disassemble the code, and read it into the new text widget

	set temp $current_output_win
	set current_output_win $win
	gdb_cmd "disassemble $pc"
	set current_output_win $temp

	set numlines [$win index end]
	set numlines [lindex [split $numlines .] 0]
	decr numlines

# Delete the first and last lines, cuz these contain useless info

	$win delete 1.0 2.0
	$win delete {end - 1 lines} end
	decr numlines 2

# Add margins (for annotations) and note the PC for each line

	catch "unset pclist($funcname)"
	lappend pclist($funcname) Unused
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
	global .src.label

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
			set wins($cfile) [create_file_win $cfile $debug_file]
			if {$wins($cfile) != ".src.nofile"} {
				set win_to_file($wins($cfile)) $cfile
				set file_to_debug_file($cfile) $debug_file
				set pointers($cfile) 1.1
				}
			}

# Pack the text widget into the listing widget, and scroll to the right place

		pack $wins($cfile) -side left -expand yes -in .src.info \
			-fill both -after .src.scroll

# Make the scrollbar point at the new text widget

		.src.scroll configure -command "$wins($cfile) yview"

		$wins($cfile) yview [expr $line - $screen_height / 2]
		}

# Update the label widget in case the filename or function name has changed

	if {$current_label != "$filename.$funcname"} then {
		set tail [expr [string last / $filename] + 1]
		set .src.label "[string range $filename $tail end] : ${funcname}()"
#		.src.label configure -text "[string range $filename $tail end] : ${funcname}()"
		set current_label $filename.$funcname
		}

# Update the pointer, scrolling the text widget if necessary to keep the
# pointer in an acceptable part of the screen.

	if [info exists pointers($cfile)] then {
		$wins($cfile) configure -state normal
		set pointer_pos $pointers($cfile)
		$wins($cfile) configure -state normal
		$wins($cfile) delete $pointer_pos "$pointer_pos + 2 char"
		$wins($cfile) insert $pointer_pos "  "

		set pointer_pos [$wins($cfile) index $line.1]
		set pointers($cfile) $pointer_pos

		$wins($cfile) delete $pointer_pos "$pointer_pos + 2 char"
		$wins($cfile) insert $pointer_pos "->"

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
#	create_asm_window - Open up the assembly window.
#
# Description:
#
#	Create an assembly window if it doesn't exist.
#

proc create_asm_window {} {
	global cfunc

	if ![winfo exists .asm] {
		set cfunc *None*
		set win [asm_win_name $cfunc]

		build_framework .asm Assembly "*NIL*"

		.asm.text configure -yscrollcommand asmscrollproc

		frame .asm.row1
		frame .asm.row2

		button .asm.stepi -width 6 -text Stepi \
			-command {catch {gdb_cmd stepi} ; update_ptr}
		button .asm.nexti -width 6 -text Nexti \
			-command {catch {gdb_cmd nexti} ; update_ptr}
		button .asm.continue -width 6 -text Cont \
			-command {catch {gdb_cmd continue} ; update_ptr}
		button .asm.finish -width 6 -text Finish \
			-command {catch {gdb_cmd finish} ; update_ptr}
		button .asm.up -width 6 -text Up -command {catch {gdb_cmd up} ; update_ptr}
		button .asm.down -width 6 -text Down \
			-command {catch {gdb_cmd down} ; update_ptr}
		button .asm.bottom -width 6 -text Bottom \
			-command {catch {gdb_cmd {frame 0}} ; update_ptr}

		pack .asm.stepi .asm.continue .asm.up .asm.bottom -side left -padx 3 -pady 5 -in .asm.row1
		pack .asm.nexti .asm.finish .asm.down -side left -padx 3 -pady 5 -in .asm.row2

		pack .asm.row2 .asm.row1 -side bottom -anchor w -before .asm.info

		update

		update_assembly [gdb_loc]
	}
}

proc reg_config_menu {} {
    	catch {destroy .reg.config}
	toplevel .reg.config
	wm geometry .reg.config +300+300
	wm title .reg.config "Register configuration"
	wm iconname .reg.config "Reg config"
	set regnames [gdb_regnames]
	set num_regs [llength $regnames]

	frame .reg.config.buts

	button .reg.config.done -text " Done " -command "
		recompute_reg_display_list $num_regs
		populate_reg_window
		update_registers all
		destroy .reg.config "

	button .reg.config.update -text Update -command "
		recompute_reg_display_list $num_regs
		populate_reg_window
		update_registers all "

	pack .reg.config.buts -side bottom -fill x

	pack .reg.config.done -side left -fill x -expand yes -in .reg.config.buts
	pack .reg.config.update -side right -fill x -expand yes -in .reg.config.buts

# Since there can be lots of registers, we build the window with no more than
# 32 rows, and as many columns as needed.

# First, figure out how many columns we need and create that many column frame
# widgets

	set ncols [expr ($num_regs + 31) / 32]

	for {set col 0} {$col < $ncols} {incr col} {
		frame .reg.config.col$col
		pack .reg.config.col$col -side left -anchor n
	}

# Now, create the checkbutton widgets and pack them in the appropriate columns

	set col 0
	set row 0
	for {set regnum 0} {$regnum < $num_regs} {incr regnum} {
		set regname [lindex $regnames $regnum]
		checkbutton .reg.config.col$col.$row -text $regname -pady 0 \
			-variable regena($regnum) -relief flat -anchor w -bd 1

		pack .reg.config.col$col.$row -side top -fill both

		incr row
		if {$row >= 32} {
			incr col
			set row 0
		}
	}
}

#
# Local procedure:
#
#	create_registers_window - Open up the register display window.
#
# Description:
#
#	Create the register display window, with automatic updates.
#

proc create_registers_window {} {
	global reg_format

	if [winfo exists .reg] return

# Create an initial register display list consisting of all registers

	if ![info exists reg_format] {
		global reg_display_list
		global changed_reg_list
		global regena

		set reg_format {}
		set num_regs [llength [gdb_regnames]]
		for {set regnum 0} {$regnum < $num_regs} {incr regnum} {
			set regena($regnum) 1
		}
		recompute_reg_display_list $num_regs
		set changed_reg_list $reg_display_list
	}

	build_framework .reg Registers

# First, delete all the old menu entries

	.reg.menubar.view.menu delete 0 last

# Hex menu item
	.reg.menubar.view.menu add radiobutton -variable reg_format \
		-label Hex -value x -command {update_registers all}

# Decimal menu item
	.reg.menubar.view.menu add radiobutton -variable reg_format \
		-label Decimal -value d -command {update_registers all}

# Octal menu item
	.reg.menubar.view.menu add radiobutton -variable reg_format \
		-label Octal -value o -command {update_registers all}

# Natural menu item
	.reg.menubar.view.menu add radiobutton -variable reg_format \
		-label Natural -value {} -command {update_registers all}

# Config menu item
	.reg.menubar.view.menu add separator

	.reg.menubar.view.menu add command -label Config -command {
		reg_config_menu }

	destroy .reg.label

# Install the reg names

	populate_reg_window
	update_registers all
}

# Convert regena into a list of the enabled $regnums

proc recompute_reg_display_list {num_regs} {
	global reg_display_list
	global regmap
	global regena

	catch {unset reg_display_list}

	set line 1
	for {set regnum 0} {$regnum < $num_regs} {incr regnum} {

		if {[set regena($regnum)] != 0} {
			lappend reg_display_list $regnum
			set regmap($regnum) $line
			incr line
		}
	}
}

# Fill out the register window with the names of the regs specified in
# reg_display_list.

proc populate_reg_window {} {
	global max_regname_width
	global reg_display_list

	.reg.text configure -state normal

	.reg.text delete 0.0 end

	set regnames [eval gdb_regnames $reg_display_list]

# Figure out the longest register name

	set max_regname_width 0

	foreach reg $regnames {
		set len [string length $reg]
		if {$len > $max_regname_width} {set max_regname_width $len}
	}

	set width [expr $max_regname_width + 15]

	set height [llength $regnames]

	if {$height > 60} {set height 60}

	.reg.text configure -height $height -width $width

	foreach reg $regnames {
		.reg.text insert end [format "%-*s \n" $max_regname_width ${reg}]
	}

	.reg.text yview 0
	.reg.text configure -state disabled
}

#
# Local procedure:
#
#	update_registers - Update the registers window.
#
# Description:
#
#	This procedure updates the registers window.
#

proc update_registers {which} {
	global max_regname_width
	global reg_format
	global reg_display_list
	global changed_reg_list
	global highlight
	global regmap

	set margin [expr $max_regname_width + 1]
	set win .reg.text
	set winwidth [lindex [$win configure -width] 4]
	set valwidth [expr $winwidth - $margin]

	$win configure -state normal

	if {$which == "all"} {
		set lineindex 1
		foreach regnum $reg_display_list {
			set regval [gdb_fetch_registers $reg_format $regnum]
			set regval [format "%-*s" $valwidth $regval]
			$win delete $lineindex.$margin "$lineindex.0 lineend"
			$win insert $lineindex.$margin $regval
			incr lineindex
		}
		$win configure -state disabled
		return
	}

# Unhighlight the old values

	foreach regnum $changed_reg_list {
		$win tag delete $win.$regnum
	}

# Now, highlight the changed values of the interesting registers

	set changed_reg_list [eval gdb_changed_register_list $reg_display_list]

	set lineindex 1
	foreach regnum $changed_reg_list {
		set regval [gdb_fetch_registers $reg_format $regnum]
		set regval [format "%-*s" $valwidth $regval]

		set lineindex $regmap($regnum)
		$win delete $lineindex.$margin "$lineindex.0 lineend"
		$win insert $lineindex.$margin $regval
		$win tag add $win.$regnum $lineindex.0 "$lineindex.0 lineend"
		eval $win tag configure $win.$regnum $highlight
	}

	$win configure -state disabled
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
	global .asm.label

# Rip the linespec apart

	set pc [lindex $linespec 4]
	set line [lindex $linespec 3]
	set filename [lindex $linespec 2]
	set funcname [lindex $linespec 1]
	set debug_file [lindex $linespec 0]

	set win [asm_win_name $cfunc]

# Sometimes there's no source file for this location

	if {$filename == ""} {set filename Blank}

# If we want to switch funcs, we need to unpack the current text widget, and
# stick in the new one.

	if {$funcname != $cfunc } {
		set oldwin $win
		set cfunc $funcname

		set win [asm_win_name $cfunc]

# Create a text widget for this func if necessary

		if {![winfo exists $win]} {
			create_asm_win $cfunc $pc
			set asm_pointers($cfunc) 1.1
			set current_asm_label NIL
			}

# Pack the text widget, and scroll to the right place

		pack forget $oldwin
		pack $win -side left -expand yes -fill both \
			-after .asm.scroll
		.asm.scroll configure -command "$win yview"
		set line [pc_to_line $pclist($cfunc) $pc]
		update
		$win yview [expr $line - $asm_screen_height / 2]
		}

# Update the label widget in case the filename or function name has changed

	if {$current_asm_label != "$pc $funcname"} then {
		set .asm.label "$pc $funcname"
		set current_asm_label "$pc $funcname"
		}

# Update the pointer, scrolling the text widget if necessary to keep the
# pointer in an acceptable part of the screen.

	if [info exists asm_pointers($cfunc)] then {
		$win configure -state normal
		set pointer_pos $asm_pointers($cfunc)
		$win configure -state normal
		$win delete $pointer_pos "$pointer_pos + 2 char"
		$win insert $pointer_pos "  "

# Map the PC back to a line in the window		

		set line [pc_to_line $pclist($cfunc) $pc]

		if {$line == -1} {
			echo "Can't find PC $pc"
			return
			}

		set pointer_pos [$win index $line.1]
		set asm_pointers($cfunc) $pointer_pos

		$win delete $pointer_pos "$pointer_pos + 2 char"
		$win insert $pointer_pos "->"

		if {$line < $asm_screen_top + 1
		    || $line > $asm_screen_bot} then {
			$win yview [expr $line - $asm_screen_height / 2]
			}

		$win configure -state disabled
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

proc update_ptr {} {
	update_listing [gdb_loc]
	if [winfo exists .asm] {
		update_assembly [gdb_loc]
	}
	if [winfo exists .reg] {
		update_registers changed
	}
}

# Make toplevel window disappear

wm withdraw .

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

# Setup command window

proc build_framework {win {title GDBtk} {label {}}} {
	global ${win}.label

	toplevel ${win}
	wm title ${win} $title
	wm minsize ${win} 1 1

	frame ${win}.menubar

	menubutton ${win}.menubar.file -padx 12 -text File \
		-menu ${win}.menubar.file.menu -underline 0

	menu ${win}.menubar.file.menu
	${win}.menubar.file.menu add command -label Edit \
		-command {exec $editor +[expr ($screen_top + $screen_bot)/2] $cfile &}
	${win}.menubar.file.menu add command -label Close \
		-command "destroy ${win}"
	${win}.menubar.file.menu add command -label Quit \
		-command {catch {gdb_cmd quit}}

	menubutton ${win}.menubar.view -padx 12 -text View \
		-menu ${win}.menubar.view.menu -underline 0

	menu ${win}.menubar.view.menu
	${win}.menubar.view.menu add command -label Hex -command {echo Hex}
	${win}.menubar.view.menu add command -label Decimal \
		-command {echo Decimal}
	${win}.menubar.view.menu add command -label Octal -command {echo Octal}

	menubutton ${win}.menubar.window -padx 12 -text Window \
		-menu ${win}.menubar.window.menu -underline 0

	menu ${win}.menubar.window.menu
	${win}.menubar.window.menu add command -label Source \
		-command {echo Source}
	${win}.menubar.window.menu add command -label Command \
		-command {echo Command}
	${win}.menubar.window.menu add command -label Assembly \
		-command {create_asm_window ; update_ptr}
	${win}.menubar.window.menu add command -label Register \
		-command {create_registers_window ; update_ptr}

	menubutton ${win}.menubar.help -padx 12 -text Help \
		-menu ${win}.menubar.help.menu -underline 0

	menu ${win}.menubar.help.menu
	${win}.menubar.help.menu add command -label "with GDBtk" \
		-command {echo "with GDBtk"}
	${win}.menubar.help.menu add command -label "with this window" \
		-command {echo "with this window"}
	${win}.menubar.help.menu add command -label "Report bug" \
		-command {exec send-pr}

	tk_menuBar ${win}.menubar ${win}.menubar.file ${win}.menubar.view \
		${win}.menubar.window ${win}.menubar.help
	pack ${win}.menubar.file ${win}.menubar.view ${win}.menubar.window \
		-side left
	pack ${win}.menubar.help -side right

	frame ${win}.info
	text ${win}.text -height 25 -width 80 -relief raised -borderwidth 2 \
		-setgrid true -cursor hand2 -yscrollcommand "${win}.scroll set"

	set ${win}.label $label
	label ${win}.label -textvariable ${win}.label -borderwidth 2 -relief raised

	scrollbar ${win}.scroll -orient vertical -command "${win}.text yview"

	pack ${win}.label -side bottom -fill x -in ${win}.info
	pack ${win}.scroll -side right -fill y -in ${win}.info
	pack ${win}.text -side left -expand yes -fill both -in ${win}.info

	pack ${win}.menubar -side top -fill x
	pack ${win}.info -side top -fill both -expand yes
}

proc create_source_window {} {
	global wins
	global cfile

	build_framework .src Source "*No file*"

# First, delete all the old view menu entries

	.src.menubar.view.menu delete 0 last

# Source file selection
	.src.menubar.view.menu add command -label "Select source file" \
		-command files_command

# Line numbers enable/disable menu item
	.src.menubar.view.menu add checkbutton -variable line_numbers \
		-label "Line numbers" -onvalue 1 -offvalue 0 -command {
		foreach source [array names wins] {
			if {$source == "Blank"} continue
			destroy $wins($source)
			unset wins($source)
			}
		set cfile Blank
		update_listing [gdb_loc]
		}

	frame .src.row1
	frame .src.row2

	button .src.start -width 6 -text Start -command \
		{catch {gdb_cmd {break main}}
		 catch {gdb_cmd {enable delete $bpnum}}
		 catch {gdb_cmd run}
		 update_ptr }
	button .src.stop -width 6 -text Stop -fg red -activeforeground red \
		-state disabled -command gdb_stop
	button .src.step -width 6 -text Step \
		-command {catch {gdb_cmd step} ; update_ptr}
	button .src.next -width 6 -text Next \
		-command {catch {gdb_cmd next} ; update_ptr}
	button .src.continue -width 6 -text Cont \
		-command {catch {gdb_cmd continue} ; update_ptr}
	button .src.finish -width 6 -text Finish \
		-command {catch {gdb_cmd finish} ; update_ptr}
	button .src.up -width 6 -text Up \
		-command {catch {gdb_cmd up} ; update_ptr}
	button .src.down -width 6 -text Down \
		-command {catch {gdb_cmd down} ; update_ptr}
	button .src.bottom -width 6 -text Bottom \
		-command {catch {gdb_cmd {frame 0}} ; update_ptr}

	pack .src.start .src.step .src.continue .src.up .src.bottom \
		-side left -padx 3 -pady 5 -in .src.row1
	pack .src.stop .src.next .src.finish .src.down -side left -padx 3 \
		-pady 5 -in .src.row2

	pack .src.row2 .src.row1 -side bottom -anchor w -before .src.info

	$wins($cfile) insert 0.0 "  This page intentionally left blank."
	$wins($cfile) configure -width 88 -state disabled \
		-yscrollcommand textscrollproc

	proc textscrollproc {args} {global screen_height screen_top screen_bot
				    eval ".src.scroll set $args"
				    set screen_height [lindex $args 1]
				    set screen_top [lindex $args 2]
				    set screen_bot [lindex $args 3]}
}

proc create_command_window {} {
	global command_line

	build_framework .cmd Command "* Command Buffer *"

	set command_line {}

	gdb_cmd {set language c}
	gdb_cmd {set height 0}
	gdb_cmd {set width 0}

	bind .cmd.text <Enter> {focus %W}
	bind .cmd.text <Delete> {delete_char %W}
	bind .cmd.text <BackSpace> {delete_char %W}
	bind .cmd.text <Control-u> {delete_line %W}
	bind .cmd.text <Any-Key> {
		global command_line

		%W insert end %A
		%W yview -pickplace end
		append command_line %A
		}
	bind .cmd.text <Key-Return> {
		global command_line

		%W insert end \n
		%W yview -pickplace end
		catch "gdb_cmd [list $command_line]"
		set command_line {}
		update_ptr
		%W insert end "(gdb) "
		%W yview -pickplace end
		}

	proc delete_char {win} {
		global command_line

		tk_textBackspace $win
		$win yview -pickplace insert
		set tmp [expr [string length $command_line] - 2]
		set command_line [string range $command_line 0 $tmp]
	}

	proc delete_line {win} {
		global command_line

		$win delete {end linestart + 6 chars} end
		$win yview -pickplace insert
		set command_line {}
	}
}

# Setup the initial windows

create_source_window

if {[tk colormodel .src.text] == "color"} {
	set highlight "-background red2 -borderwidth 2 -relief sunk"
} else {
	set fg [lindex [.src.text config -foreground] 4]
	set bg [lindex [.src.text config -background] 4]
	set highlight "-foreground $bg -background $fg -borderwidth 0"
}

create_command_window
update
