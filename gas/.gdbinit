break as_warn
break as_bad
break as_bad_where
break as_fatal
break as_perror
break abort

define pe
call print_expr ($)
end

document pe
Print $ as an expressionS, expanding parameters.
end

define ps
call print_symbol_value ($)
end

document ps
Print $ as a symbolS, including expression value.
end
