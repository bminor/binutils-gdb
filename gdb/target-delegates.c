/* THIS FILE IS GENERATED -*- buffer-read-only: t -*- */
/* vi:set ro: */

/* To regenerate this file, run:*/
/*      make-target-delegates target.h > target-delegates.c */
static void
delegate_attach (struct target_ops *self, char *arg1, int arg2)
{
  self = self->beneath;
  self->to_attach (self, arg1, arg2);
}

static void
delegate_post_attach (struct target_ops *self, int arg1)
{
  self = self->beneath;
  self->to_post_attach (self, arg1);
}

static void
tdefault_post_attach (struct target_ops *self, int arg1)
{
}

static void
delegate_detach (struct target_ops *self, const char *arg1, int arg2)
{
  self = self->beneath;
  self->to_detach (self, arg1, arg2);
}

static void
tdefault_detach (struct target_ops *self, const char *arg1, int arg2)
{
}

static void
delegate_resume (struct target_ops *self, ptid_t arg1, int arg2, enum gdb_signal arg3)
{
  self = self->beneath;
  self->to_resume (self, arg1, arg2, arg3);
}

static void
tdefault_resume (struct target_ops *self, ptid_t arg1, int arg2, enum gdb_signal arg3)
{
  noprocess ();
}

static ptid_t
delegate_wait (struct target_ops *self, ptid_t arg1, struct target_waitstatus *arg2, int arg3)
{
  self = self->beneath;
  return self->to_wait (self, arg1, arg2, arg3);
}

static ptid_t
tdefault_wait (struct target_ops *self, ptid_t arg1, struct target_waitstatus *arg2, int arg3)
{
  noprocess ();
}

static void
delegate_store_registers (struct target_ops *self, struct regcache *arg1, int arg2)
{
  self = self->beneath;
  self->to_store_registers (self, arg1, arg2);
}

static void
tdefault_store_registers (struct target_ops *self, struct regcache *arg1, int arg2)
{
  noprocess ();
}

static void
delegate_prepare_to_store (struct target_ops *self, struct regcache *arg1)
{
  self = self->beneath;
  self->to_prepare_to_store (self, arg1);
}

static void
tdefault_prepare_to_store (struct target_ops *self, struct regcache *arg1)
{
  noprocess ();
}

static void
delegate_files_info (struct target_ops *self)
{
  self = self->beneath;
  self->to_files_info (self);
}

static void
tdefault_files_info (struct target_ops *self)
{
}

static int
delegate_insert_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  self = self->beneath;
  return self->to_insert_breakpoint (self, arg1, arg2);
}

static int
delegate_remove_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  self = self->beneath;
  return self->to_remove_breakpoint (self, arg1, arg2);
}

static int
delegate_can_use_hw_breakpoint (struct target_ops *self, int arg1, int arg2, int arg3)
{
  self = self->beneath;
  return self->to_can_use_hw_breakpoint (self, arg1, arg2, arg3);
}

static int
tdefault_can_use_hw_breakpoint (struct target_ops *self, int arg1, int arg2, int arg3)
{
  return 0;
}

static int
delegate_insert_hw_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  self = self->beneath;
  return self->to_insert_hw_breakpoint (self, arg1, arg2);
}

static int
tdefault_insert_hw_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  return -1;
}

static int
delegate_remove_hw_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  self = self->beneath;
  return self->to_remove_hw_breakpoint (self, arg1, arg2);
}

static int
tdefault_remove_hw_breakpoint (struct target_ops *self, struct gdbarch *arg1, struct bp_target_info *arg2)
{
  return -1;
}

static int
delegate_remove_watchpoint (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  self = self->beneath;
  return self->to_remove_watchpoint (self, arg1, arg2, arg3, arg4);
}

static int
tdefault_remove_watchpoint (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  return -1;
}

static int
delegate_insert_watchpoint (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  self = self->beneath;
  return self->to_insert_watchpoint (self, arg1, arg2, arg3, arg4);
}

static int
tdefault_insert_watchpoint (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  return -1;
}

static int
delegate_stopped_by_watchpoint (struct target_ops *self)
{
  self = self->beneath;
  return self->to_stopped_by_watchpoint (self);
}

static int
tdefault_stopped_by_watchpoint (struct target_ops *self)
{
  return 0;
}

static int
delegate_stopped_data_address (struct target_ops *self, CORE_ADDR *arg1)
{
  self = self->beneath;
  return self->to_stopped_data_address (self, arg1);
}

static int
tdefault_stopped_data_address (struct target_ops *self, CORE_ADDR *arg1)
{
  return 0;
}

static int
delegate_watchpoint_addr_within_range (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2, int arg3)
{
  self = self->beneath;
  return self->to_watchpoint_addr_within_range (self, arg1, arg2, arg3);
}

static int
delegate_region_ok_for_hw_watchpoint (struct target_ops *self, CORE_ADDR arg1, int arg2)
{
  self = self->beneath;
  return self->to_region_ok_for_hw_watchpoint (self, arg1, arg2);
}

static int
delegate_can_accel_watchpoint_condition (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  self = self->beneath;
  return self->to_can_accel_watchpoint_condition (self, arg1, arg2, arg3, arg4);
}

static int
tdefault_can_accel_watchpoint_condition (struct target_ops *self, CORE_ADDR arg1, int arg2, int arg3, struct expression *arg4)
{
  return 0;
}

static void
delegate_terminal_init (struct target_ops *self)
{
  self = self->beneath;
  self->to_terminal_init (self);
}

static void
tdefault_terminal_init (struct target_ops *self)
{
}

static void
delegate_terminal_inferior (struct target_ops *self)
{
  self = self->beneath;
  self->to_terminal_inferior (self);
}

static void
tdefault_terminal_inferior (struct target_ops *self)
{
}

static void
delegate_terminal_ours_for_output (struct target_ops *self)
{
  self = self->beneath;
  self->to_terminal_ours_for_output (self);
}

static void
tdefault_terminal_ours_for_output (struct target_ops *self)
{
}

static void
delegate_terminal_ours (struct target_ops *self)
{
  self = self->beneath;
  self->to_terminal_ours (self);
}

static void
tdefault_terminal_ours (struct target_ops *self)
{
}

static void
delegate_terminal_save_ours (struct target_ops *self)
{
  self = self->beneath;
  self->to_terminal_save_ours (self);
}

static void
tdefault_terminal_save_ours (struct target_ops *self)
{
}

static void
delegate_terminal_info (struct target_ops *self, const char *arg1, int arg2)
{
  self = self->beneath;
  self->to_terminal_info (self, arg1, arg2);
}

static void
delegate_load (struct target_ops *self, char *arg1, int arg2)
{
  self = self->beneath;
  self->to_load (self, arg1, arg2);
}

static void
tdefault_load (struct target_ops *self, char *arg1, int arg2)
{
  tcomplain ();
}

static void
delegate_post_startup_inferior (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  self->to_post_startup_inferior (self, arg1);
}

static void
tdefault_post_startup_inferior (struct target_ops *self, ptid_t arg1)
{
}

static int
delegate_insert_fork_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_insert_fork_catchpoint (self, arg1);
}

static int
tdefault_insert_fork_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_remove_fork_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_remove_fork_catchpoint (self, arg1);
}

static int
tdefault_remove_fork_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_insert_vfork_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_insert_vfork_catchpoint (self, arg1);
}

static int
tdefault_insert_vfork_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_remove_vfork_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_remove_vfork_catchpoint (self, arg1);
}

static int
tdefault_remove_vfork_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_insert_exec_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_insert_exec_catchpoint (self, arg1);
}

static int
tdefault_insert_exec_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_remove_exec_catchpoint (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_remove_exec_catchpoint (self, arg1);
}

static int
tdefault_remove_exec_catchpoint (struct target_ops *self, int arg1)
{
  return 1;
}

static int
delegate_set_syscall_catchpoint (struct target_ops *self, int arg1, int arg2, int arg3, int arg4, int *arg5)
{
  self = self->beneath;
  return self->to_set_syscall_catchpoint (self, arg1, arg2, arg3, arg4, arg5);
}

static int
tdefault_set_syscall_catchpoint (struct target_ops *self, int arg1, int arg2, int arg3, int arg4, int *arg5)
{
  return 1;
}

static int
delegate_has_exited (struct target_ops *self, int arg1, int arg2, int *arg3)
{
  self = self->beneath;
  return self->to_has_exited (self, arg1, arg2, arg3);
}

static int
tdefault_has_exited (struct target_ops *self, int arg1, int arg2, int *arg3)
{
  return 0;
}

static char *
delegate_extra_thread_info (struct target_ops *self, struct thread_info *arg1)
{
  self = self->beneath;
  return self->to_extra_thread_info (self, arg1);
}

static char *
tdefault_extra_thread_info (struct target_ops *self, struct thread_info *arg1)
{
  return 0;
}

static char *
delegate_thread_name (struct target_ops *self, struct thread_info *arg1)
{
  self = self->beneath;
  return self->to_thread_name (self, arg1);
}

static char *
tdefault_thread_name (struct target_ops *self, struct thread_info *arg1)
{
  return 0;
}

static void
delegate_rcmd (struct target_ops *self, char *arg1, struct ui_file *arg2)
{
  self = self->beneath;
  self->to_rcmd (self, arg1, arg2);
}

static char *
delegate_pid_to_exec_file (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_pid_to_exec_file (self, arg1);
}

static char *
tdefault_pid_to_exec_file (struct target_ops *self, int arg1)
{
  return 0;
}

static void
delegate_log_command (struct target_ops *self, const char *arg1)
{
  self = self->beneath;
  self->to_log_command (self, arg1);
}

static void
tdefault_log_command (struct target_ops *self, const char *arg1)
{
}

static int
delegate_can_async_p (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_async_p (self);
}

static int
delegate_is_async_p (struct target_ops *self)
{
  self = self->beneath;
  return self->to_is_async_p (self);
}

static void
delegate_async (struct target_ops *self, async_callback_ftype *arg1, void *arg2)
{
  self = self->beneath;
  self->to_async (self, arg1, arg2);
}

static void
tdefault_async (struct target_ops *self, async_callback_ftype *arg1, void *arg2)
{
  tcomplain ();
}

static int
delegate_find_memory_regions (struct target_ops *self, find_memory_region_ftype arg1, void *arg2)
{
  self = self->beneath;
  return self->to_find_memory_regions (self, arg1, arg2);
}

static char * 
delegate_make_corefile_notes (struct target_ops *self, bfd *arg1, int *arg2)
{
  self = self->beneath;
  return self->to_make_corefile_notes (self, arg1, arg2);
}

static gdb_byte * 
delegate_get_bookmark (struct target_ops *self, char *arg1, int arg2)
{
  self = self->beneath;
  return self->to_get_bookmark (self, arg1, arg2);
}

static gdb_byte * 
tdefault_get_bookmark (struct target_ops *self, char *arg1, int arg2)
{
  tcomplain ();
}

static void
delegate_goto_bookmark (struct target_ops *self, gdb_byte *arg1, int arg2)
{
  self = self->beneath;
  self->to_goto_bookmark (self, arg1, arg2);
}

static void
tdefault_goto_bookmark (struct target_ops *self, gdb_byte *arg1, int arg2)
{
  tcomplain ();
}

static enum target_xfer_status 
delegate_xfer_partial (struct target_ops *self, enum target_object  arg1, const char *arg2, gdb_byte *arg3, const gdb_byte *arg4, ULONGEST arg5, ULONGEST arg6, ULONGEST *arg7)
{
  self = self->beneath;
  return self->to_xfer_partial (self, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
}

static enum target_xfer_status 
tdefault_xfer_partial (struct target_ops *self, enum target_object  arg1, const char *arg2, gdb_byte *arg3, const gdb_byte *arg4, ULONGEST arg5, ULONGEST arg6, ULONGEST *arg7)
{
  return TARGET_XFER_E_IO;
}

static ptid_t
delegate_get_ada_task_ptid (struct target_ops *self, long arg1, long arg2)
{
  self = self->beneath;
  return self->to_get_ada_task_ptid (self, arg1, arg2);
}

static int
delegate_can_execute_reverse (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_execute_reverse (self);
}

static int
tdefault_can_execute_reverse (struct target_ops *self)
{
  return 0;
}

static enum exec_direction_kind 
delegate_execution_direction (struct target_ops *self)
{
  self = self->beneath;
  return self->to_execution_direction (self);
}

static int
delegate_supports_multi_process (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_multi_process (self);
}

static int
tdefault_supports_multi_process (struct target_ops *self)
{
  return 0;
}

static int
delegate_supports_enable_disable_tracepoint (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_enable_disable_tracepoint (self);
}

static int
tdefault_supports_enable_disable_tracepoint (struct target_ops *self)
{
  return 0;
}

static int
delegate_supports_string_tracing (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_string_tracing (self);
}

static int
tdefault_supports_string_tracing (struct target_ops *self)
{
  return 0;
}

static struct gdbarch *
delegate_thread_architecture (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_thread_architecture (self, arg1);
}

static void
delegate_trace_init (struct target_ops *self)
{
  self = self->beneath;
  self->to_trace_init (self);
}

static void
tdefault_trace_init (struct target_ops *self)
{
  tcomplain ();
}

static void
delegate_download_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  self = self->beneath;
  self->to_download_tracepoint (self, arg1);
}

static void
tdefault_download_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  tcomplain ();
}

static int
delegate_can_download_tracepoint (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_download_tracepoint (self);
}

static int
tdefault_can_download_tracepoint (struct target_ops *self)
{
  return 0;
}

static void
delegate_download_trace_state_variable (struct target_ops *self, struct trace_state_variable *arg1)
{
  self = self->beneath;
  self->to_download_trace_state_variable (self, arg1);
}

static void
tdefault_download_trace_state_variable (struct target_ops *self, struct trace_state_variable *arg1)
{
  tcomplain ();
}

static void
delegate_enable_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  self = self->beneath;
  self->to_enable_tracepoint (self, arg1);
}

static void
tdefault_enable_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  tcomplain ();
}

static void
delegate_disable_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  self = self->beneath;
  self->to_disable_tracepoint (self, arg1);
}

static void
tdefault_disable_tracepoint (struct target_ops *self, struct bp_location *arg1)
{
  tcomplain ();
}

static void
delegate_trace_set_readonly_regions (struct target_ops *self)
{
  self = self->beneath;
  self->to_trace_set_readonly_regions (self);
}

static void
tdefault_trace_set_readonly_regions (struct target_ops *self)
{
  tcomplain ();
}

static void
delegate_trace_start (struct target_ops *self)
{
  self = self->beneath;
  self->to_trace_start (self);
}

static void
tdefault_trace_start (struct target_ops *self)
{
  tcomplain ();
}

static int
delegate_get_trace_status (struct target_ops *self, struct trace_status *arg1)
{
  self = self->beneath;
  return self->to_get_trace_status (self, arg1);
}

static int
tdefault_get_trace_status (struct target_ops *self, struct trace_status *arg1)
{
  return -1;
}

static void
delegate_get_tracepoint_status (struct target_ops *self, struct breakpoint *arg1, struct uploaded_tp *arg2)
{
  self = self->beneath;
  self->to_get_tracepoint_status (self, arg1, arg2);
}

static void
tdefault_get_tracepoint_status (struct target_ops *self, struct breakpoint *arg1, struct uploaded_tp *arg2)
{
  tcomplain ();
}

static int
delegate_supports_btrace (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_btrace (self);
}

static int
tdefault_supports_btrace (struct target_ops *self)
{
  return 0;
}

static void
install_delegators (struct target_ops *ops)
{
  if (ops->to_attach == NULL)
    ops->to_attach = delegate_attach;
  if (ops->to_post_attach == NULL)
    ops->to_post_attach = delegate_post_attach;
  if (ops->to_detach == NULL)
    ops->to_detach = delegate_detach;
  if (ops->to_resume == NULL)
    ops->to_resume = delegate_resume;
  if (ops->to_wait == NULL)
    ops->to_wait = delegate_wait;
  if (ops->to_store_registers == NULL)
    ops->to_store_registers = delegate_store_registers;
  if (ops->to_prepare_to_store == NULL)
    ops->to_prepare_to_store = delegate_prepare_to_store;
  if (ops->to_files_info == NULL)
    ops->to_files_info = delegate_files_info;
  if (ops->to_insert_breakpoint == NULL)
    ops->to_insert_breakpoint = delegate_insert_breakpoint;
  if (ops->to_remove_breakpoint == NULL)
    ops->to_remove_breakpoint = delegate_remove_breakpoint;
  if (ops->to_can_use_hw_breakpoint == NULL)
    ops->to_can_use_hw_breakpoint = delegate_can_use_hw_breakpoint;
  if (ops->to_insert_hw_breakpoint == NULL)
    ops->to_insert_hw_breakpoint = delegate_insert_hw_breakpoint;
  if (ops->to_remove_hw_breakpoint == NULL)
    ops->to_remove_hw_breakpoint = delegate_remove_hw_breakpoint;
  if (ops->to_remove_watchpoint == NULL)
    ops->to_remove_watchpoint = delegate_remove_watchpoint;
  if (ops->to_insert_watchpoint == NULL)
    ops->to_insert_watchpoint = delegate_insert_watchpoint;
  if (ops->to_stopped_by_watchpoint == NULL)
    ops->to_stopped_by_watchpoint = delegate_stopped_by_watchpoint;
  if (ops->to_stopped_data_address == NULL)
    ops->to_stopped_data_address = delegate_stopped_data_address;
  if (ops->to_watchpoint_addr_within_range == NULL)
    ops->to_watchpoint_addr_within_range = delegate_watchpoint_addr_within_range;
  if (ops->to_region_ok_for_hw_watchpoint == NULL)
    ops->to_region_ok_for_hw_watchpoint = delegate_region_ok_for_hw_watchpoint;
  if (ops->to_can_accel_watchpoint_condition == NULL)
    ops->to_can_accel_watchpoint_condition = delegate_can_accel_watchpoint_condition;
  if (ops->to_terminal_init == NULL)
    ops->to_terminal_init = delegate_terminal_init;
  if (ops->to_terminal_inferior == NULL)
    ops->to_terminal_inferior = delegate_terminal_inferior;
  if (ops->to_terminal_ours_for_output == NULL)
    ops->to_terminal_ours_for_output = delegate_terminal_ours_for_output;
  if (ops->to_terminal_ours == NULL)
    ops->to_terminal_ours = delegate_terminal_ours;
  if (ops->to_terminal_save_ours == NULL)
    ops->to_terminal_save_ours = delegate_terminal_save_ours;
  if (ops->to_terminal_info == NULL)
    ops->to_terminal_info = delegate_terminal_info;
  if (ops->to_load == NULL)
    ops->to_load = delegate_load;
  if (ops->to_post_startup_inferior == NULL)
    ops->to_post_startup_inferior = delegate_post_startup_inferior;
  if (ops->to_insert_fork_catchpoint == NULL)
    ops->to_insert_fork_catchpoint = delegate_insert_fork_catchpoint;
  if (ops->to_remove_fork_catchpoint == NULL)
    ops->to_remove_fork_catchpoint = delegate_remove_fork_catchpoint;
  if (ops->to_insert_vfork_catchpoint == NULL)
    ops->to_insert_vfork_catchpoint = delegate_insert_vfork_catchpoint;
  if (ops->to_remove_vfork_catchpoint == NULL)
    ops->to_remove_vfork_catchpoint = delegate_remove_vfork_catchpoint;
  if (ops->to_insert_exec_catchpoint == NULL)
    ops->to_insert_exec_catchpoint = delegate_insert_exec_catchpoint;
  if (ops->to_remove_exec_catchpoint == NULL)
    ops->to_remove_exec_catchpoint = delegate_remove_exec_catchpoint;
  if (ops->to_set_syscall_catchpoint == NULL)
    ops->to_set_syscall_catchpoint = delegate_set_syscall_catchpoint;
  if (ops->to_has_exited == NULL)
    ops->to_has_exited = delegate_has_exited;
  if (ops->to_extra_thread_info == NULL)
    ops->to_extra_thread_info = delegate_extra_thread_info;
  if (ops->to_thread_name == NULL)
    ops->to_thread_name = delegate_thread_name;
  if (ops->to_rcmd == NULL)
    ops->to_rcmd = delegate_rcmd;
  if (ops->to_pid_to_exec_file == NULL)
    ops->to_pid_to_exec_file = delegate_pid_to_exec_file;
  if (ops->to_log_command == NULL)
    ops->to_log_command = delegate_log_command;
  if (ops->to_can_async_p == NULL)
    ops->to_can_async_p = delegate_can_async_p;
  if (ops->to_is_async_p == NULL)
    ops->to_is_async_p = delegate_is_async_p;
  if (ops->to_async == NULL)
    ops->to_async = delegate_async;
  if (ops->to_find_memory_regions == NULL)
    ops->to_find_memory_regions = delegate_find_memory_regions;
  if (ops->to_make_corefile_notes == NULL)
    ops->to_make_corefile_notes = delegate_make_corefile_notes;
  if (ops->to_get_bookmark == NULL)
    ops->to_get_bookmark = delegate_get_bookmark;
  if (ops->to_goto_bookmark == NULL)
    ops->to_goto_bookmark = delegate_goto_bookmark;
  if (ops->to_xfer_partial == NULL)
    ops->to_xfer_partial = delegate_xfer_partial;
  if (ops->to_get_ada_task_ptid == NULL)
    ops->to_get_ada_task_ptid = delegate_get_ada_task_ptid;
  if (ops->to_can_execute_reverse == NULL)
    ops->to_can_execute_reverse = delegate_can_execute_reverse;
  if (ops->to_execution_direction == NULL)
    ops->to_execution_direction = delegate_execution_direction;
  if (ops->to_supports_multi_process == NULL)
    ops->to_supports_multi_process = delegate_supports_multi_process;
  if (ops->to_supports_enable_disable_tracepoint == NULL)
    ops->to_supports_enable_disable_tracepoint = delegate_supports_enable_disable_tracepoint;
  if (ops->to_supports_string_tracing == NULL)
    ops->to_supports_string_tracing = delegate_supports_string_tracing;
  if (ops->to_thread_architecture == NULL)
    ops->to_thread_architecture = delegate_thread_architecture;
  if (ops->to_trace_init == NULL)
    ops->to_trace_init = delegate_trace_init;
  if (ops->to_download_tracepoint == NULL)
    ops->to_download_tracepoint = delegate_download_tracepoint;
  if (ops->to_can_download_tracepoint == NULL)
    ops->to_can_download_tracepoint = delegate_can_download_tracepoint;
  if (ops->to_download_trace_state_variable == NULL)
    ops->to_download_trace_state_variable = delegate_download_trace_state_variable;
  if (ops->to_enable_tracepoint == NULL)
    ops->to_enable_tracepoint = delegate_enable_tracepoint;
  if (ops->to_disable_tracepoint == NULL)
    ops->to_disable_tracepoint = delegate_disable_tracepoint;
  if (ops->to_trace_set_readonly_regions == NULL)
    ops->to_trace_set_readonly_regions = delegate_trace_set_readonly_regions;
  if (ops->to_trace_start == NULL)
    ops->to_trace_start = delegate_trace_start;
  if (ops->to_get_trace_status == NULL)
    ops->to_get_trace_status = delegate_get_trace_status;
  if (ops->to_get_tracepoint_status == NULL)
    ops->to_get_tracepoint_status = delegate_get_tracepoint_status;
  if (ops->to_supports_btrace == NULL)
    ops->to_supports_btrace = delegate_supports_btrace;
}

static void
install_dummy_methods (struct target_ops *ops)
{
  ops->to_attach = find_default_attach;
  ops->to_post_attach = tdefault_post_attach;
  ops->to_detach = tdefault_detach;
  ops->to_resume = tdefault_resume;
  ops->to_wait = tdefault_wait;
  ops->to_store_registers = tdefault_store_registers;
  ops->to_prepare_to_store = tdefault_prepare_to_store;
  ops->to_files_info = tdefault_files_info;
  ops->to_insert_breakpoint = memory_insert_breakpoint;
  ops->to_remove_breakpoint = memory_remove_breakpoint;
  ops->to_can_use_hw_breakpoint = tdefault_can_use_hw_breakpoint;
  ops->to_insert_hw_breakpoint = tdefault_insert_hw_breakpoint;
  ops->to_remove_hw_breakpoint = tdefault_remove_hw_breakpoint;
  ops->to_remove_watchpoint = tdefault_remove_watchpoint;
  ops->to_insert_watchpoint = tdefault_insert_watchpoint;
  ops->to_stopped_by_watchpoint = tdefault_stopped_by_watchpoint;
  ops->to_stopped_data_address = tdefault_stopped_data_address;
  ops->to_watchpoint_addr_within_range = default_watchpoint_addr_within_range;
  ops->to_region_ok_for_hw_watchpoint = default_region_ok_for_hw_watchpoint;
  ops->to_can_accel_watchpoint_condition = tdefault_can_accel_watchpoint_condition;
  ops->to_terminal_init = tdefault_terminal_init;
  ops->to_terminal_inferior = tdefault_terminal_inferior;
  ops->to_terminal_ours_for_output = tdefault_terminal_ours_for_output;
  ops->to_terminal_ours = tdefault_terminal_ours;
  ops->to_terminal_save_ours = tdefault_terminal_save_ours;
  ops->to_terminal_info = default_terminal_info;
  ops->to_load = tdefault_load;
  ops->to_post_startup_inferior = tdefault_post_startup_inferior;
  ops->to_insert_fork_catchpoint = tdefault_insert_fork_catchpoint;
  ops->to_remove_fork_catchpoint = tdefault_remove_fork_catchpoint;
  ops->to_insert_vfork_catchpoint = tdefault_insert_vfork_catchpoint;
  ops->to_remove_vfork_catchpoint = tdefault_remove_vfork_catchpoint;
  ops->to_insert_exec_catchpoint = tdefault_insert_exec_catchpoint;
  ops->to_remove_exec_catchpoint = tdefault_remove_exec_catchpoint;
  ops->to_set_syscall_catchpoint = tdefault_set_syscall_catchpoint;
  ops->to_has_exited = tdefault_has_exited;
  ops->to_extra_thread_info = tdefault_extra_thread_info;
  ops->to_thread_name = tdefault_thread_name;
  ops->to_rcmd = default_rcmd;
  ops->to_pid_to_exec_file = tdefault_pid_to_exec_file;
  ops->to_log_command = tdefault_log_command;
  ops->to_can_async_p = find_default_can_async_p;
  ops->to_is_async_p = find_default_is_async_p;
  ops->to_async = tdefault_async;
  ops->to_find_memory_regions = dummy_find_memory_regions;
  ops->to_make_corefile_notes = dummy_make_corefile_notes;
  ops->to_get_bookmark = tdefault_get_bookmark;
  ops->to_goto_bookmark = tdefault_goto_bookmark;
  ops->to_xfer_partial = tdefault_xfer_partial;
  ops->to_get_ada_task_ptid = default_get_ada_task_ptid;
  ops->to_can_execute_reverse = tdefault_can_execute_reverse;
  ops->to_execution_direction = default_execution_direction;
  ops->to_supports_multi_process = tdefault_supports_multi_process;
  ops->to_supports_enable_disable_tracepoint = tdefault_supports_enable_disable_tracepoint;
  ops->to_supports_string_tracing = tdefault_supports_string_tracing;
  ops->to_thread_architecture = default_thread_architecture;
  ops->to_trace_init = tdefault_trace_init;
  ops->to_download_tracepoint = tdefault_download_tracepoint;
  ops->to_can_download_tracepoint = tdefault_can_download_tracepoint;
  ops->to_download_trace_state_variable = tdefault_download_trace_state_variable;
  ops->to_enable_tracepoint = tdefault_enable_tracepoint;
  ops->to_disable_tracepoint = tdefault_disable_tracepoint;
  ops->to_trace_set_readonly_regions = tdefault_trace_set_readonly_regions;
  ops->to_trace_start = tdefault_trace_start;
  ops->to_get_trace_status = tdefault_get_trace_status;
  ops->to_get_tracepoint_status = tdefault_get_tracepoint_status;
  ops->to_supports_btrace = tdefault_supports_btrace;
}
