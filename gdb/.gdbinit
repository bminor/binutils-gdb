echo Setting up the environment for debugging gdb.\n

set complaints 1

b fatal

b info_command
commands
	silent
	return
end

dir ../mmalloc
dir ../libiberty
dir ../bfd
set prompt (top-gdb) 

define list-objfiles
set $obj = object_files
printf "objfile    bfd        msyms  name\n"
while $obj != 0
printf "0x%-8x 0x%-8x %6d %s\n", $obj, $obj->obfd, \
  $obj->minimal_symbol_count, $obj->name
set var $obj = $obj->next
end
end
