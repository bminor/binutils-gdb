# GDB GUI setup

set cfile Blank
set wins($cfile) .text
set current_label {}
set screen_height 0
set screen_top 0
set screen_bot 0

proc test {} {
	update_listing {termcap.c foo /etc/termcap 200}
}

proc echo string {puts stdout $string}

proc gdbtk_tcl_fputs {arg} {
	.command.text insert end "$arg"
	.command.text yview -pickplace end
}

proc gdbtk_tcl_flush {} {update idletasks}

proc gdbtk_tcl_query {message} {
	tk_dialog .query "gdb : query" "$message" {} 1 "No" "Yes"
	}

if [info exists env(EDITOR)] then {
	set editor $env(EDITOR)
	} else {
	set editor emacs
}

proc gdbtk_tcl_start_variable_annotation {valaddr ref_type stor_cl cum_expr field type_cast} {
	echo "gdbtk_tcl_start_variable_annotation $valaddr $ref_type $stor_cl $cum_expr $field $type_cast"
}

proc gdbtk_tcl_end_variable_annotation {} {
	echo gdbtk_tcl_end_variable_annotation
}

proc insert_breakpoint_tag {win line} {
	$win configure -state normal
	$win delete $line.0
	$win insert $line.0 "B"
	$win tag add $line $line.0
	$win tag bind $line <1> {
#		echo "tag %W %X %Y %x"
#		echo "tag names [$wins($cfile) tag names]"
	}

	$win configure -state disabled
}

proc delete_breakpoint_tag {win line} {
	$win configure -state normal
	$win delete $line.0
	$win insert $line.0 " "
	$win tag delete $line
	$win configure -state disabled
}

# Callback from GDB to notify us of breakpoint creation.

proc create_breakpoint {bpnum file line} {
	global wins
	global breakpoint_file
	global breakpoint_line

# Record breakpoint locations

	set breakpoint_file($bpnum) $file
	set breakpoint_line($bpnum) $line
	
# If there isn't a window for this file, don't try to update it

	if [info exists wins($file)] {
		insert_breakpoint_tag $wins($file) $line
	}
}

proc delete_breakpoint {bpnum file line} {
	global wins
	global breakpoint_file
	global breakpoint_line

# Save line number for later

	set line $breakpoint_line($bpnum)

# Reset breakpoint annotation info

	unset breakpoint_file($bpnum)
	unset breakpoint_line($bpnum)

# If there isn't a window for this file, don't try to update it

	if [info exists wins($file)] {
		delete_breakpoint_tag $wins($file) $line
	}
}

# This is a callback from C code to notify us of breakpoint changes.  ACTION
# can be one of create, delete, enable, or disable.

proc gdbtk_tcl_breakpoint {action bpnum file line} {
	${action}_breakpoint $bpnum $file $line
}

# Create the popup listing window menu

menu .breakpoint -cursor hand2
.breakpoint add command -label Break
.breakpoint add separator
.breakpoint add command -label "Edit" -command {exec $editor +$selected_line $selected_file &}
.breakpoint add command -label "Set breakpoint" -command {gdb_cmd "break $selected_file:$selected_line"}
#.breakpoint add command -label "Clear breakpoint" -command {echo "Clear"}
#.breakpoint add command -label "Enable breakpoint" -command {echo "Enable"}
#.breakpoint add command -label "Disable breakpoint" -command {echo "Disable"}

# Come here when button is released in the popup menu

bind .breakpoint <Any-ButtonRelease-1> {
	global selected_win

# First, remove the menu, and release the pointer

	.breakpoint unpost
	grab release .breakpoint

# Unhighlight the selected line

	$selected_win tag delete breaktag
#	echo "after deleting $selected_win [$selected_win tag names]"
#	echo "grab [grab current]"

# Actually invoke the menubutton here!

	tk_invokeMenu %W
#	destroy .breakpoint
	grab release $selected_win
}

# Button 1 has been pressed in a listing window.  Pop up a menu.

proc breakpoint_menu {win x y xrel yrel} {
	global wins
	global win_to_file
	global file_to_debug_file
	global highlight
	global selected_line
	global selected_file
	global selected_win

	grab $win

#	echo "bpm grab current [grab current]"

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

	.breakpoint post [expr $x-[winfo width .breakpoint]/2] [expr $y-10]
	grab .breakpoint
#	echo "after grab [grab current]"
}

proc do_nothing {} {}

proc create_file_win {filename} {
	global breakpoint_file
	global breakpoint_line

	regsub -all {\.|/} $filename {} temp
	set win .text$temp
	text $win -height 25 -width 80 -relief raised -borderwidth 2 -yscrollcommand textscrollproc -setgrid true -cursor hand2
	bind $win <Enter> {focus %W}
#	bind $win <1> {breakpoint_menu %W %X %Y %x %y}
	bind $win <B1-Motion> do_nothing
	bind $win n {gdb_cmd next ; update_ptr}
	bind $win s {gdb_cmd step ; update_ptr}
	bind $win c {gdb_cmd continue ; update_ptr}
	bind $win f {gdb_cmd finish ; update_ptr}
	bind $win u {gdb_cmd up ; update_ptr}
	bind $win d {gdb_cmd down ; update_ptr}
	set fh [open $filename]
	$win delete 0.0 end
	$win insert 0.0 [read $fh]
	close $fh
	set numlines [$win index end]
	set numlines [lindex [split $numlines .] 0]
	for {set i 1} {$i <= $numlines} {incr i} {
		$win insert $i.0 [format "   %4d " $i]
		}

	$win tag add wholebuf 0.0 end
	$win tag bind wholebuf <1> {breakpoint_menu %W %X %Y %x %y}
	foreach bpnum [array names breakpoint_file] {
		if {$breakpoint_file($bpnum) == $filename} {
			insert_breakpoint_tag $win $breakpoint_line($bpnum)
			}
		}

	$win configure -state disabled
	return $win
}

proc update_listing {linespec} {
	global pointers
	global screen_height
	global screen_top
	global screen_bot
	global wins cfile
	global current_label
	global win_to_file
	global file_to_debug_file

	set line [lindex $linespec 3]
	set filename [lindex $linespec 2]
	set funcname [lindex $linespec 1]
	set debug_file [lindex $linespec 0]

	if {$filename == ""} {set filename Blank}

	if {$filename != $cfile} then {
		pack forget $wins($cfile)
		set cfile $filename
		if ![info exists wins($cfile)] then {
			set wins($cfile) [create_file_win $cfile]
			set win_to_file($wins($cfile)) $cfile
			set file_to_debug_file($cfile) $debug_file
			set pointers($cfile) 1.1
			}

		pack $wins($cfile) -side left -expand yes -in .listing -fill both -after .label
		$wins($cfile) yview [expr $line - $screen_height / 2]
		}

	if {$current_label != "$filename.$funcname"} then {
		set tail [expr [string last / $filename] + 1]
		.label configure -text "[string range $filename $tail end] : ${funcname}()"
		set current_label $filename.$funcname
		}

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

proc update_ptr {} {update_listing [gdb_loc]}

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
pack .start .step .next .continue .finish .up .down .bottom .files .exit -side left
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

