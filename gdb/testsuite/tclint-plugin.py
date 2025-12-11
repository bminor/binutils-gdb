from tclint.commands.checks import (
    CommandArgError,
)
from tclint.commands.schema import commands_schema


def _script_args(name, n, script_args, args, parser):
    if len(args) != n:
        raise CommandArgError(
            f"wrong # of args to {name}: got {len(args)}, expected {n}"
        )

    res = []
    arg_n = 0
    for arg in args:
        if arg_n in script_args:
            res.append(parser.parse_script(arg))
        else:
            res.append(arg)
        arg_n += 1
    return res


def _script_last_arg(name, n, args, parser):
    return _script_args(name, n, [n - 1], args, parser)


def _cond_wrap(args, parser):
    return _script_last_arg("cond_wrap", 3, args, parser)


def _foreach_gnat_encoding(args, parser):
    return _script_last_arg("foreach_gnat_encoding", 4, args, parser)


def _foreach_location_functions(args, parser):
    return _script_args("foreach_location_functions", 4, [2, 3], args, parser)


def _foreach_location_labels(args, parser):
    return _script_args("foreach_location_labels", 5, [3, 4], args, parser)


def _foreach_mi_ui_mode(args, parser):
    return _script_last_arg("foreach_mi_ui_mode", 2, args, parser)


def _foreach_with_prefix(args, parser):
    return _script_last_arg("foreach_with_prefix", 3, args, parser)


def _gdb_caching_proc(args, parser):
    return _script_last_arg("gdb_caching_proc", 3, args, parser)


def _proc_with_prefix(args, parser):
    return _script_last_arg("proc_with_prefix", 3, args, parser)


def _require(args, parser):
    res = []
    for arg in args:
        res.append(parser.parse_script(arg))
    return res


def _save_vars(args, parser):
    return _script_last_arg("save_vars", 2, args, parser)


def _save_target_board_info(args, parser):
    return _script_last_arg("save_target_board_info", 2, args, parser)


def _with_debuginfod_env(args, parser):
    return _script_last_arg("with_debuginfod_env", 2, args, parser)


def _with_shared_gdb(args, parser):
    return _script_last_arg("with_shared_gdb", 1, args, parser)


def _with_test_prefix(args, parser):
    return _script_last_arg("with_test_prefix", 2, args, parser)


def _with_cwd(args, parser):
    return _script_last_arg("with_cwd", 2, args, parser)


def _with_gdb_cwd(args, parser):
    return _script_last_arg("with_gdb_cwd", 2, args, parser)


def _with_gdb_prompt(args, parser):
    return _script_last_arg("with_gdb_prompt", 2, args, parser)


def _with_target_charset(args, parser):
    return _script_last_arg("with_target_charset", 2, args, parser)


def _with_max_value_size(args, parser):
    return _script_last_arg("with_max_value_size", 2, args, parser)


def _with_spawn_id(args, parser):
    return _script_last_arg("with_spawn_id", 2, args, parser)


def _with_timeout_factor(args, parser):
    return _script_last_arg("with_timeout_factor", 2, args, parser)


def _with_read1_timeout_factor(args, parser):
    return _script_last_arg("with_read1_timeout_factor", 2, args, parser)


def _with_multilib_flags_filtered(args, parser):
    return _script_last_arg("with_multilib_flags_filtered", 2, args, parser)


def _with_PIE_multilib_flags_filtered(args, parser):
    return _script_last_arg("with_PIE_multilib_flags_filtered", 1, args, parser)


def _with_set(args, parser):
    return _script_last_arg("with_set", 3, args, parser)


def _with_complaints(args, parser):
    return _script_last_arg("with_complaints", 2, args, parser)


def _with_override(args, parser):
    return _script_last_arg("with_override", 3, args, parser)


def _with_ansi_styling_terminal(args, parser):
    return _script_last_arg("with_ansi_styling_terminal", 1, args, parser)


def _with_lock(args, parser):
    return _script_last_arg("with_lock", 2, args, parser)


def _with_rocm_gpu_lock(args, parser):
    return _script_last_arg("with_rocm_gpu_lock", 1, args, parser)


def _ns_term_log_cur(args, parser):
    return _script_last_arg("Term::log_cur", 2, args, parser)


def _ns_term_with_term(args, parser):
    return _script_last_arg("Term::with_term", 2, args, parser)


def _ns_term_with_tuiterm(args, parser):
    return _script_last_arg("Term::with_tuiterm", 3, args, parser)


commands = commands_schema(
    {
        "cond_wrap": _cond_wrap,
        "foreach_gnat_encoding": _foreach_gnat_encoding,
        "foreach_location_functions": _foreach_location_functions,
        "foreach_location_labels": _foreach_location_labels,
        "foreach_mi_ui_mode": _foreach_mi_ui_mode,
        "foreach_with_prefix": _foreach_with_prefix,
        "gdb_caching_proc": _gdb_caching_proc,
        "proc_with_prefix": _proc_with_prefix,
        "require": _require,
        "save_vars": _save_vars,
        "save_target_board_info": _save_target_board_info,
        "with_debuginfod_env": _with_debuginfod_env,
        "with_shared_gdb": _with_shared_gdb,
        "with_test_prefix": _with_test_prefix,
        "with_cwd": _with_cwd,
        "with_gdb_cwd": _with_gdb_cwd,
        "with_gdb_prompt": _with_gdb_prompt,
        "with_target_charset": _with_target_charset,
        "with_max_value_size": _with_max_value_size,
        "with_spawn_id": _with_spawn_id,
        "with_timeout_factor": _with_timeout_factor,
        "with_read1_timeout_factor": _with_read1_timeout_factor,
        "with_multilib_flags_filtered": _with_multilib_flags_filtered,
        "with_PIE_multilib_flags_filtered": _with_PIE_multilib_flags_filtered,
        "with_set": _with_set,
        "with_complaints": _with_complaints,
        "with_override": _with_override,
        "with_ansi_styling_terminal": _with_ansi_styling_terminal,
        "with_lock": _with_lock,
        "with_rocm_gpu_lock": _with_rocm_gpu_lock,
        "Term::_log_cur": _ns_term_log_cur,
        "Term::with_term": _ns_term_with_term,
        "Term::with_tuiterm": _ns_term_with_tuiterm,
    }
)
