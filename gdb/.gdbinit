echo Setting up the environment for debugging gdb.\n

b fatal

b info_command
commands
	silent
	return
end

set prompt (top-gdb)
set complaints 1
