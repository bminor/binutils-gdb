/* THIS FILE IS GENERATED -*- buffer-read-only: t -*- */
/* vi:set ro: */

/* To regenerate this file, run:*/
/*      make-target-delegates target.h > target-delegates.c */
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
delegate_disconnect (struct target_ops *self, char *arg1, int arg2)
{
  self = self->beneath;
  self->to_disconnect (self, arg1, arg2);
}

static void
tdefault_disconnect (struct target_ops *self, char *arg1, int arg2)
{
  tcomplain ();
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
delegate_fetch_registers (struct target_ops *self, struct regcache *arg1, int arg2)
{
  self = self->beneath;
  self->to_fetch_registers (self, arg1, arg2);
}

static void
tdefault_fetch_registers (struct target_ops *self, struct regcache *arg1, int arg2)
{
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
delegate_ranged_break_num_registers (struct target_ops *self)
{
  self = self->beneath;
  return self->to_ranged_break_num_registers (self);
}

static int
tdefault_ranged_break_num_registers (struct target_ops *self)
{
  return -1;
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
delegate_insert_mask_watchpoint (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2, int arg3)
{
  self = self->beneath;
  return self->to_insert_mask_watchpoint (self, arg1, arg2, arg3);
}

static int
tdefault_insert_mask_watchpoint (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2, int arg3)
{
  return 1;
}

static int
delegate_remove_mask_watchpoint (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2, int arg3)
{
  self = self->beneath;
  return self->to_remove_mask_watchpoint (self, arg1, arg2, arg3);
}

static int
tdefault_remove_mask_watchpoint (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2, int arg3)
{
  return 1;
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

static int
delegate_masked_watch_num_registers (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2)
{
  self = self->beneath;
  return self->to_masked_watch_num_registers (self, arg1, arg2);
}

static int
tdefault_masked_watch_num_registers (struct target_ops *self, CORE_ADDR arg1, CORE_ADDR arg2)
{
  return -1;
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
delegate_kill (struct target_ops *self)
{
  self = self->beneath;
  self->to_kill (self);
}

static void
tdefault_kill (struct target_ops *self)
{
  noprocess ();
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
delegate_follow_fork (struct target_ops *self, int arg1, int arg2)
{
  self = self->beneath;
  return self->to_follow_fork (self, arg1, arg2);
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

static void
delegate_mourn_inferior (struct target_ops *self)
{
  self = self->beneath;
  self->to_mourn_inferior (self);
}

static int
delegate_can_run (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_run (self);
}

static int
tdefault_can_run (struct target_ops *self)
{
  return 0;
}

static void
delegate_pass_signals (struct target_ops *self, int arg1, unsigned char *arg2)
{
  self = self->beneath;
  self->to_pass_signals (self, arg1, arg2);
}

static void
tdefault_pass_signals (struct target_ops *self, int arg1, unsigned char *arg2)
{
}

static void
delegate_program_signals (struct target_ops *self, int arg1, unsigned char *arg2)
{
  self = self->beneath;
  self->to_program_signals (self, arg1, arg2);
}

static void
tdefault_program_signals (struct target_ops *self, int arg1, unsigned char *arg2)
{
}

static int
delegate_thread_alive (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_thread_alive (self, arg1);
}

static int
tdefault_thread_alive (struct target_ops *self, ptid_t arg1)
{
  return 0;
}

static void
delegate_find_new_threads (struct target_ops *self)
{
  self = self->beneath;
  self->to_find_new_threads (self);
}

static void
tdefault_find_new_threads (struct target_ops *self)
{
}

static char *
delegate_pid_to_str (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_pid_to_str (self, arg1);
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
  return NULL;
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
  return NULL;
}

static void
delegate_stop (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  self->to_stop (self, arg1);
}

static void
tdefault_stop (struct target_ops *self, ptid_t arg1)
{
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
  return NULL;
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

static struct target_section_table *
delegate_get_section_table (struct target_ops *self)
{
  self = self->beneath;
  return self->to_get_section_table (self);
}

static struct target_section_table *
tdefault_get_section_table (struct target_ops *self)
{
  return NULL;
}

static int
delegate_can_async_p (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_async_p (self);
}

static int
tdefault_can_async_p (struct target_ops *self)
{
  return 0;
}

static int
delegate_is_async_p (struct target_ops *self)
{
  self = self->beneath;
  return self->to_is_async_p (self);
}

static int
tdefault_is_async_p (struct target_ops *self)
{
  return 0;
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
delegate_supports_non_stop (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_non_stop (self);
}

static int
tdefault_supports_non_stop (struct target_ops *self)
{
  return 0;
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

static VEC(mem_region_s) *
delegate_memory_map (struct target_ops *self)
{
  self = self->beneath;
  return self->to_memory_map (self);
}

static VEC(mem_region_s) *
tdefault_memory_map (struct target_ops *self)
{
  return NULL;
}

static void
delegate_flash_erase (struct target_ops *self, ULONGEST arg1, LONGEST arg2)
{
  self = self->beneath;
  self->to_flash_erase (self, arg1, arg2);
}

static void
tdefault_flash_erase (struct target_ops *self, ULONGEST arg1, LONGEST arg2)
{
  tcomplain ();
}

static void
delegate_flash_done (struct target_ops *self)
{
  self = self->beneath;
  self->to_flash_done (self);
}

static void
tdefault_flash_done (struct target_ops *self)
{
  tcomplain ();
}

static const struct target_desc *
delegate_read_description (struct target_ops *self)
{
  self = self->beneath;
  return self->to_read_description (self);
}

static const struct target_desc *
tdefault_read_description (struct target_ops *self)
{
  return NULL;
}

static ptid_t
delegate_get_ada_task_ptid (struct target_ops *self, long arg1, long arg2)
{
  self = self->beneath;
  return self->to_get_ada_task_ptid (self, arg1, arg2);
}

static int
delegate_auxv_parse (struct target_ops *self, gdb_byte **arg1, gdb_byte *arg2, CORE_ADDR *arg3, CORE_ADDR *arg4)
{
  self = self->beneath;
  return self->to_auxv_parse (self, arg1, arg2, arg3, arg4);
}

static int
delegate_search_memory (struct target_ops *self, CORE_ADDR arg1, ULONGEST arg2, const gdb_byte *arg3, ULONGEST arg4, CORE_ADDR *arg5)
{
  self = self->beneath;
  return self->to_search_memory (self, arg1, arg2, arg3, arg4, arg5);
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

static int
delegate_supports_evaluation_of_breakpoint_conditions (struct target_ops *self)
{
  self = self->beneath;
  return self->to_supports_evaluation_of_breakpoint_conditions (self);
}

static int
tdefault_supports_evaluation_of_breakpoint_conditions (struct target_ops *self)
{
  return 0;
}

static int
delegate_can_run_breakpoint_commands (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_run_breakpoint_commands (self);
}

static int
tdefault_can_run_breakpoint_commands (struct target_ops *self)
{
  return 0;
}

static struct gdbarch *
delegate_thread_architecture (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_thread_architecture (self, arg1);
}

static struct address_space *
delegate_thread_address_space (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_thread_address_space (self, arg1);
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

static void
delegate_trace_stop (struct target_ops *self)
{
  self = self->beneath;
  self->to_trace_stop (self);
}

static void
tdefault_trace_stop (struct target_ops *self)
{
  tcomplain ();
}

static int
delegate_trace_find (struct target_ops *self, enum trace_find_type  arg1, int arg2, CORE_ADDR arg3, CORE_ADDR arg4, int *arg5)
{
  self = self->beneath;
  return self->to_trace_find (self, arg1, arg2, arg3, arg4, arg5);
}

static int
tdefault_trace_find (struct target_ops *self, enum trace_find_type  arg1, int arg2, CORE_ADDR arg3, CORE_ADDR arg4, int *arg5)
{
  return -1;
}

static int
delegate_get_trace_state_variable_value (struct target_ops *self, int arg1, LONGEST *arg2)
{
  self = self->beneath;
  return self->to_get_trace_state_variable_value (self, arg1, arg2);
}

static int
tdefault_get_trace_state_variable_value (struct target_ops *self, int arg1, LONGEST *arg2)
{
  return 0;
}

static int
delegate_save_trace_data (struct target_ops *self, const char *arg1)
{
  self = self->beneath;
  return self->to_save_trace_data (self, arg1);
}

static int
tdefault_save_trace_data (struct target_ops *self, const char *arg1)
{
  tcomplain ();
}

static int
delegate_upload_tracepoints (struct target_ops *self, struct uploaded_tp **arg1)
{
  self = self->beneath;
  return self->to_upload_tracepoints (self, arg1);
}

static int
tdefault_upload_tracepoints (struct target_ops *self, struct uploaded_tp **arg1)
{
  return 0;
}

static int
delegate_upload_trace_state_variables (struct target_ops *self, struct uploaded_tsv **arg1)
{
  self = self->beneath;
  return self->to_upload_trace_state_variables (self, arg1);
}

static int
tdefault_upload_trace_state_variables (struct target_ops *self, struct uploaded_tsv **arg1)
{
  return 0;
}

static LONGEST
delegate_get_raw_trace_data (struct target_ops *self, gdb_byte *arg1, ULONGEST arg2, LONGEST arg3)
{
  self = self->beneath;
  return self->to_get_raw_trace_data (self, arg1, arg2, arg3);
}

static LONGEST
tdefault_get_raw_trace_data (struct target_ops *self, gdb_byte *arg1, ULONGEST arg2, LONGEST arg3)
{
  tcomplain ();
}

static int
delegate_get_min_fast_tracepoint_insn_len (struct target_ops *self)
{
  self = self->beneath;
  return self->to_get_min_fast_tracepoint_insn_len (self);
}

static int
tdefault_get_min_fast_tracepoint_insn_len (struct target_ops *self)
{
  return -1;
}

static void
delegate_set_disconnected_tracing (struct target_ops *self, int arg1)
{
  self = self->beneath;
  self->to_set_disconnected_tracing (self, arg1);
}

static void
tdefault_set_disconnected_tracing (struct target_ops *self, int arg1)
{
}

static void
delegate_set_circular_trace_buffer (struct target_ops *self, int arg1)
{
  self = self->beneath;
  self->to_set_circular_trace_buffer (self, arg1);
}

static void
tdefault_set_circular_trace_buffer (struct target_ops *self, int arg1)
{
}

static void
delegate_set_trace_buffer_size (struct target_ops *self, LONGEST arg1)
{
  self = self->beneath;
  self->to_set_trace_buffer_size (self, arg1);
}

static void
tdefault_set_trace_buffer_size (struct target_ops *self, LONGEST arg1)
{
}

static int
delegate_set_trace_notes (struct target_ops *self, const char *arg1, const char *arg2, const char *arg3)
{
  self = self->beneath;
  return self->to_set_trace_notes (self, arg1, arg2, arg3);
}

static int
tdefault_set_trace_notes (struct target_ops *self, const char *arg1, const char *arg2, const char *arg3)
{
  return 0;
}

static int
delegate_core_of_thread (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_core_of_thread (self, arg1);
}

static int
tdefault_core_of_thread (struct target_ops *self, ptid_t arg1)
{
  return -1;
}

static int
delegate_verify_memory (struct target_ops *self, const gdb_byte *arg1, CORE_ADDR arg2, ULONGEST arg3)
{
  self = self->beneath;
  return self->to_verify_memory (self, arg1, arg2, arg3);
}

static int
delegate_get_tib_address (struct target_ops *self, ptid_t arg1, CORE_ADDR *arg2)
{
  self = self->beneath;
  return self->to_get_tib_address (self, arg1, arg2);
}

static int
tdefault_get_tib_address (struct target_ops *self, ptid_t arg1, CORE_ADDR *arg2)
{
  tcomplain ();
}

static void
delegate_set_permissions (struct target_ops *self)
{
  self = self->beneath;
  self->to_set_permissions (self);
}

static void
tdefault_set_permissions (struct target_ops *self)
{
}

static int
delegate_static_tracepoint_marker_at (struct target_ops *self, CORE_ADDR arg1, struct static_tracepoint_marker *arg2)
{
  self = self->beneath;
  return self->to_static_tracepoint_marker_at (self, arg1, arg2);
}

static int
tdefault_static_tracepoint_marker_at (struct target_ops *self, CORE_ADDR arg1, struct static_tracepoint_marker *arg2)
{
  return 0;
}

static VEC(static_tracepoint_marker_p) *
delegate_static_tracepoint_markers_by_strid (struct target_ops *self, const char *arg1)
{
  self = self->beneath;
  return self->to_static_tracepoint_markers_by_strid (self, arg1);
}

static VEC(static_tracepoint_marker_p) *
tdefault_static_tracepoint_markers_by_strid (struct target_ops *self, const char *arg1)
{
  tcomplain ();
}

static struct traceframe_info *
delegate_traceframe_info (struct target_ops *self)
{
  self = self->beneath;
  return self->to_traceframe_info (self);
}

static struct traceframe_info *
tdefault_traceframe_info (struct target_ops *self)
{
  tcomplain ();
}

static int
delegate_use_agent (struct target_ops *self, int arg1)
{
  self = self->beneath;
  return self->to_use_agent (self, arg1);
}

static int
tdefault_use_agent (struct target_ops *self, int arg1)
{
  tcomplain ();
}

static int
delegate_can_use_agent (struct target_ops *self)
{
  self = self->beneath;
  return self->to_can_use_agent (self);
}

static int
tdefault_can_use_agent (struct target_ops *self)
{
  return 0;
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

static struct btrace_target_info *
delegate_enable_btrace (struct target_ops *self, ptid_t arg1)
{
  self = self->beneath;
  return self->to_enable_btrace (self, arg1);
}

static struct btrace_target_info *
tdefault_enable_btrace (struct target_ops *self, ptid_t arg1)
{
  tcomplain ();
}

static void
delegate_disable_btrace (struct target_ops *self, struct btrace_target_info *arg1)
{
  self = self->beneath;
  self->to_disable_btrace (self, arg1);
}

static void
tdefault_disable_btrace (struct target_ops *self, struct btrace_target_info *arg1)
{
  tcomplain ();
}

static void
delegate_teardown_btrace (struct target_ops *self, struct btrace_target_info *arg1)
{
  self = self->beneath;
  self->to_teardown_btrace (self, arg1);
}

static void
tdefault_teardown_btrace (struct target_ops *self, struct btrace_target_info *arg1)
{
  tcomplain ();
}

static enum btrace_error 
delegate_read_btrace (struct target_ops *self, VEC (btrace_block_s) **arg1, struct btrace_target_info *arg2, enum btrace_read_type  arg3)
{
  self = self->beneath;
  return self->to_read_btrace (self, arg1, arg2, arg3);
}

static enum btrace_error 
tdefault_read_btrace (struct target_ops *self, VEC (btrace_block_s) **arg1, struct btrace_target_info *arg2, enum btrace_read_type  arg3)
{
  tcomplain ();
}

static void
delegate_stop_recording (struct target_ops *self)
{
  self = self->beneath;
  self->to_stop_recording (self);
}

static void
tdefault_stop_recording (struct target_ops *self)
{
}

static void
delegate_save_record (struct target_ops *self, const char *arg1)
{
  self = self->beneath;
  self->to_save_record (self, arg1);
}

static void
tdefault_save_record (struct target_ops *self, const char *arg1)
{
  tcomplain ();
}

static void
delegate_delete_record (struct target_ops *self)
{
  self = self->beneath;
  self->to_delete_record (self);
}

static void
tdefault_delete_record (struct target_ops *self)
{
  tcomplain ();
}

static int
delegate_record_is_replaying (struct target_ops *self)
{
  self = self->beneath;
  return self->to_record_is_replaying (self);
}

static int
tdefault_record_is_replaying (struct target_ops *self)
{
  return 0;
}

static void
delegate_goto_record_begin (struct target_ops *self)
{
  self = self->beneath;
  self->to_goto_record_begin (self);
}

static void
tdefault_goto_record_begin (struct target_ops *self)
{
  tcomplain ();
}

static void
delegate_goto_record_end (struct target_ops *self)
{
  self = self->beneath;
  self->to_goto_record_end (self);
}

static void
tdefault_goto_record_end (struct target_ops *self)
{
  tcomplain ();
}

static void
delegate_goto_record (struct target_ops *self, ULONGEST arg1)
{
  self = self->beneath;
  self->to_goto_record (self, arg1);
}

static void
tdefault_goto_record (struct target_ops *self, ULONGEST arg1)
{
  tcomplain ();
}

static void
delegate_insn_history (struct target_ops *self, int arg1, int arg2)
{
  self = self->beneath;
  self->to_insn_history (self, arg1, arg2);
}

static void
tdefault_insn_history (struct target_ops *self, int arg1, int arg2)
{
  tcomplain ();
}

static void
delegate_insn_history_from (struct target_ops *self, ULONGEST arg1, int arg2, int arg3)
{
  self = self->beneath;
  self->to_insn_history_from (self, arg1, arg2, arg3);
}

static void
tdefault_insn_history_from (struct target_ops *self, ULONGEST arg1, int arg2, int arg3)
{
  tcomplain ();
}

static void
delegate_insn_history_range (struct target_ops *self, ULONGEST arg1, ULONGEST arg2, int arg3)
{
  self = self->beneath;
  self->to_insn_history_range (self, arg1, arg2, arg3);
}

static void
tdefault_insn_history_range (struct target_ops *self, ULONGEST arg1, ULONGEST arg2, int arg3)
{
  tcomplain ();
}

static void
delegate_call_history (struct target_ops *self, int arg1, int arg2)
{
  self = self->beneath;
  self->to_call_history (self, arg1, arg2);
}

static void
tdefault_call_history (struct target_ops *self, int arg1, int arg2)
{
  tcomplain ();
}

static void
delegate_call_history_from (struct target_ops *self, ULONGEST arg1, int arg2, int arg3)
{
  self = self->beneath;
  self->to_call_history_from (self, arg1, arg2, arg3);
}

static void
tdefault_call_history_from (struct target_ops *self, ULONGEST arg1, int arg2, int arg3)
{
  tcomplain ();
}

static void
delegate_call_history_range (struct target_ops *self, ULONGEST arg1, ULONGEST arg2, int arg3)
{
  self = self->beneath;
  self->to_call_history_range (self, arg1, arg2, arg3);
}

static void
tdefault_call_history_range (struct target_ops *self, ULONGEST arg1, ULONGEST arg2, int arg3)
{
  tcomplain ();
}

static int
delegate_augmented_libraries_svr4_read (struct target_ops *self)
{
  self = self->beneath;
  return self->to_augmented_libraries_svr4_read (self);
}

static int
tdefault_augmented_libraries_svr4_read (struct target_ops *self)
{
  return 0;
}

static const struct frame_unwind *
delegate_get_unwinder (struct target_ops *self)
{
  self = self->beneath;
  return self->to_get_unwinder (self);
}

static const struct frame_unwind *
tdefault_get_unwinder (struct target_ops *self)
{
  return NULL;
}

static const struct frame_unwind *
delegate_get_tailcall_unwinder (struct target_ops *self)
{
  self = self->beneath;
  return self->to_get_tailcall_unwinder (self);
}

static const struct frame_unwind *
tdefault_get_tailcall_unwinder (struct target_ops *self)
{
  return NULL;
}

static CORE_ADDR
delegate_decr_pc_after_break (struct target_ops *self, struct gdbarch *arg1)
{
  self = self->beneath;
  return self->to_decr_pc_after_break (self, arg1);
}

static void
install_delegators (struct target_ops *ops)
{
  if (ops->to_post_attach == NULL)
    ops->to_post_attach = delegate_post_attach;
  if (ops->to_detach == NULL)
    ops->to_detach = delegate_detach;
  if (ops->to_disconnect == NULL)
    ops->to_disconnect = delegate_disconnect;
  if (ops->to_resume == NULL)
    ops->to_resume = delegate_resume;
  if (ops->to_wait == NULL)
    ops->to_wait = delegate_wait;
  if (ops->to_fetch_registers == NULL)
    ops->to_fetch_registers = delegate_fetch_registers;
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
  if (ops->to_ranged_break_num_registers == NULL)
    ops->to_ranged_break_num_registers = delegate_ranged_break_num_registers;
  if (ops->to_insert_hw_breakpoint == NULL)
    ops->to_insert_hw_breakpoint = delegate_insert_hw_breakpoint;
  if (ops->to_remove_hw_breakpoint == NULL)
    ops->to_remove_hw_breakpoint = delegate_remove_hw_breakpoint;
  if (ops->to_remove_watchpoint == NULL)
    ops->to_remove_watchpoint = delegate_remove_watchpoint;
  if (ops->to_insert_watchpoint == NULL)
    ops->to_insert_watchpoint = delegate_insert_watchpoint;
  if (ops->to_insert_mask_watchpoint == NULL)
    ops->to_insert_mask_watchpoint = delegate_insert_mask_watchpoint;
  if (ops->to_remove_mask_watchpoint == NULL)
    ops->to_remove_mask_watchpoint = delegate_remove_mask_watchpoint;
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
  if (ops->to_masked_watch_num_registers == NULL)
    ops->to_masked_watch_num_registers = delegate_masked_watch_num_registers;
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
  if (ops->to_kill == NULL)
    ops->to_kill = delegate_kill;
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
  if (ops->to_follow_fork == NULL)
    ops->to_follow_fork = delegate_follow_fork;
  if (ops->to_insert_exec_catchpoint == NULL)
    ops->to_insert_exec_catchpoint = delegate_insert_exec_catchpoint;
  if (ops->to_remove_exec_catchpoint == NULL)
    ops->to_remove_exec_catchpoint = delegate_remove_exec_catchpoint;
  if (ops->to_set_syscall_catchpoint == NULL)
    ops->to_set_syscall_catchpoint = delegate_set_syscall_catchpoint;
  if (ops->to_has_exited == NULL)
    ops->to_has_exited = delegate_has_exited;
  if (ops->to_mourn_inferior == NULL)
    ops->to_mourn_inferior = delegate_mourn_inferior;
  if (ops->to_can_run == NULL)
    ops->to_can_run = delegate_can_run;
  if (ops->to_pass_signals == NULL)
    ops->to_pass_signals = delegate_pass_signals;
  if (ops->to_program_signals == NULL)
    ops->to_program_signals = delegate_program_signals;
  if (ops->to_thread_alive == NULL)
    ops->to_thread_alive = delegate_thread_alive;
  if (ops->to_find_new_threads == NULL)
    ops->to_find_new_threads = delegate_find_new_threads;
  if (ops->to_pid_to_str == NULL)
    ops->to_pid_to_str = delegate_pid_to_str;
  if (ops->to_extra_thread_info == NULL)
    ops->to_extra_thread_info = delegate_extra_thread_info;
  if (ops->to_thread_name == NULL)
    ops->to_thread_name = delegate_thread_name;
  if (ops->to_stop == NULL)
    ops->to_stop = delegate_stop;
  if (ops->to_rcmd == NULL)
    ops->to_rcmd = delegate_rcmd;
  if (ops->to_pid_to_exec_file == NULL)
    ops->to_pid_to_exec_file = delegate_pid_to_exec_file;
  if (ops->to_log_command == NULL)
    ops->to_log_command = delegate_log_command;
  if (ops->to_get_section_table == NULL)
    ops->to_get_section_table = delegate_get_section_table;
  if (ops->to_can_async_p == NULL)
    ops->to_can_async_p = delegate_can_async_p;
  if (ops->to_is_async_p == NULL)
    ops->to_is_async_p = delegate_is_async_p;
  if (ops->to_async == NULL)
    ops->to_async = delegate_async;
  if (ops->to_supports_non_stop == NULL)
    ops->to_supports_non_stop = delegate_supports_non_stop;
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
  if (ops->to_memory_map == NULL)
    ops->to_memory_map = delegate_memory_map;
  if (ops->to_flash_erase == NULL)
    ops->to_flash_erase = delegate_flash_erase;
  if (ops->to_flash_done == NULL)
    ops->to_flash_done = delegate_flash_done;
  if (ops->to_read_description == NULL)
    ops->to_read_description = delegate_read_description;
  if (ops->to_get_ada_task_ptid == NULL)
    ops->to_get_ada_task_ptid = delegate_get_ada_task_ptid;
  if (ops->to_auxv_parse == NULL)
    ops->to_auxv_parse = delegate_auxv_parse;
  if (ops->to_search_memory == NULL)
    ops->to_search_memory = delegate_search_memory;
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
  if (ops->to_supports_evaluation_of_breakpoint_conditions == NULL)
    ops->to_supports_evaluation_of_breakpoint_conditions = delegate_supports_evaluation_of_breakpoint_conditions;
  if (ops->to_can_run_breakpoint_commands == NULL)
    ops->to_can_run_breakpoint_commands = delegate_can_run_breakpoint_commands;
  if (ops->to_thread_architecture == NULL)
    ops->to_thread_architecture = delegate_thread_architecture;
  if (ops->to_thread_address_space == NULL)
    ops->to_thread_address_space = delegate_thread_address_space;
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
  if (ops->to_trace_stop == NULL)
    ops->to_trace_stop = delegate_trace_stop;
  if (ops->to_trace_find == NULL)
    ops->to_trace_find = delegate_trace_find;
  if (ops->to_get_trace_state_variable_value == NULL)
    ops->to_get_trace_state_variable_value = delegate_get_trace_state_variable_value;
  if (ops->to_save_trace_data == NULL)
    ops->to_save_trace_data = delegate_save_trace_data;
  if (ops->to_upload_tracepoints == NULL)
    ops->to_upload_tracepoints = delegate_upload_tracepoints;
  if (ops->to_upload_trace_state_variables == NULL)
    ops->to_upload_trace_state_variables = delegate_upload_trace_state_variables;
  if (ops->to_get_raw_trace_data == NULL)
    ops->to_get_raw_trace_data = delegate_get_raw_trace_data;
  if (ops->to_get_min_fast_tracepoint_insn_len == NULL)
    ops->to_get_min_fast_tracepoint_insn_len = delegate_get_min_fast_tracepoint_insn_len;
  if (ops->to_set_disconnected_tracing == NULL)
    ops->to_set_disconnected_tracing = delegate_set_disconnected_tracing;
  if (ops->to_set_circular_trace_buffer == NULL)
    ops->to_set_circular_trace_buffer = delegate_set_circular_trace_buffer;
  if (ops->to_set_trace_buffer_size == NULL)
    ops->to_set_trace_buffer_size = delegate_set_trace_buffer_size;
  if (ops->to_set_trace_notes == NULL)
    ops->to_set_trace_notes = delegate_set_trace_notes;
  if (ops->to_core_of_thread == NULL)
    ops->to_core_of_thread = delegate_core_of_thread;
  if (ops->to_verify_memory == NULL)
    ops->to_verify_memory = delegate_verify_memory;
  if (ops->to_get_tib_address == NULL)
    ops->to_get_tib_address = delegate_get_tib_address;
  if (ops->to_set_permissions == NULL)
    ops->to_set_permissions = delegate_set_permissions;
  if (ops->to_static_tracepoint_marker_at == NULL)
    ops->to_static_tracepoint_marker_at = delegate_static_tracepoint_marker_at;
  if (ops->to_static_tracepoint_markers_by_strid == NULL)
    ops->to_static_tracepoint_markers_by_strid = delegate_static_tracepoint_markers_by_strid;
  if (ops->to_traceframe_info == NULL)
    ops->to_traceframe_info = delegate_traceframe_info;
  if (ops->to_use_agent == NULL)
    ops->to_use_agent = delegate_use_agent;
  if (ops->to_can_use_agent == NULL)
    ops->to_can_use_agent = delegate_can_use_agent;
  if (ops->to_supports_btrace == NULL)
    ops->to_supports_btrace = delegate_supports_btrace;
  if (ops->to_enable_btrace == NULL)
    ops->to_enable_btrace = delegate_enable_btrace;
  if (ops->to_disable_btrace == NULL)
    ops->to_disable_btrace = delegate_disable_btrace;
  if (ops->to_teardown_btrace == NULL)
    ops->to_teardown_btrace = delegate_teardown_btrace;
  if (ops->to_read_btrace == NULL)
    ops->to_read_btrace = delegate_read_btrace;
  if (ops->to_stop_recording == NULL)
    ops->to_stop_recording = delegate_stop_recording;
  if (ops->to_save_record == NULL)
    ops->to_save_record = delegate_save_record;
  if (ops->to_delete_record == NULL)
    ops->to_delete_record = delegate_delete_record;
  if (ops->to_record_is_replaying == NULL)
    ops->to_record_is_replaying = delegate_record_is_replaying;
  if (ops->to_goto_record_begin == NULL)
    ops->to_goto_record_begin = delegate_goto_record_begin;
  if (ops->to_goto_record_end == NULL)
    ops->to_goto_record_end = delegate_goto_record_end;
  if (ops->to_goto_record == NULL)
    ops->to_goto_record = delegate_goto_record;
  if (ops->to_insn_history == NULL)
    ops->to_insn_history = delegate_insn_history;
  if (ops->to_insn_history_from == NULL)
    ops->to_insn_history_from = delegate_insn_history_from;
  if (ops->to_insn_history_range == NULL)
    ops->to_insn_history_range = delegate_insn_history_range;
  if (ops->to_call_history == NULL)
    ops->to_call_history = delegate_call_history;
  if (ops->to_call_history_from == NULL)
    ops->to_call_history_from = delegate_call_history_from;
  if (ops->to_call_history_range == NULL)
    ops->to_call_history_range = delegate_call_history_range;
  if (ops->to_augmented_libraries_svr4_read == NULL)
    ops->to_augmented_libraries_svr4_read = delegate_augmented_libraries_svr4_read;
  if (ops->to_get_unwinder == NULL)
    ops->to_get_unwinder = delegate_get_unwinder;
  if (ops->to_get_tailcall_unwinder == NULL)
    ops->to_get_tailcall_unwinder = delegate_get_tailcall_unwinder;
  if (ops->to_decr_pc_after_break == NULL)
    ops->to_decr_pc_after_break = delegate_decr_pc_after_break;
}

static void
install_dummy_methods (struct target_ops *ops)
{
  ops->to_post_attach = tdefault_post_attach;
  ops->to_detach = tdefault_detach;
  ops->to_disconnect = tdefault_disconnect;
  ops->to_resume = tdefault_resume;
  ops->to_wait = tdefault_wait;
  ops->to_fetch_registers = tdefault_fetch_registers;
  ops->to_store_registers = tdefault_store_registers;
  ops->to_prepare_to_store = tdefault_prepare_to_store;
  ops->to_files_info = tdefault_files_info;
  ops->to_insert_breakpoint = memory_insert_breakpoint;
  ops->to_remove_breakpoint = memory_remove_breakpoint;
  ops->to_can_use_hw_breakpoint = tdefault_can_use_hw_breakpoint;
  ops->to_ranged_break_num_registers = tdefault_ranged_break_num_registers;
  ops->to_insert_hw_breakpoint = tdefault_insert_hw_breakpoint;
  ops->to_remove_hw_breakpoint = tdefault_remove_hw_breakpoint;
  ops->to_remove_watchpoint = tdefault_remove_watchpoint;
  ops->to_insert_watchpoint = tdefault_insert_watchpoint;
  ops->to_insert_mask_watchpoint = tdefault_insert_mask_watchpoint;
  ops->to_remove_mask_watchpoint = tdefault_remove_mask_watchpoint;
  ops->to_stopped_by_watchpoint = tdefault_stopped_by_watchpoint;
  ops->to_stopped_data_address = tdefault_stopped_data_address;
  ops->to_watchpoint_addr_within_range = default_watchpoint_addr_within_range;
  ops->to_region_ok_for_hw_watchpoint = default_region_ok_for_hw_watchpoint;
  ops->to_can_accel_watchpoint_condition = tdefault_can_accel_watchpoint_condition;
  ops->to_masked_watch_num_registers = tdefault_masked_watch_num_registers;
  ops->to_terminal_init = tdefault_terminal_init;
  ops->to_terminal_inferior = tdefault_terminal_inferior;
  ops->to_terminal_ours_for_output = tdefault_terminal_ours_for_output;
  ops->to_terminal_ours = tdefault_terminal_ours;
  ops->to_terminal_save_ours = tdefault_terminal_save_ours;
  ops->to_terminal_info = default_terminal_info;
  ops->to_kill = tdefault_kill;
  ops->to_load = tdefault_load;
  ops->to_post_startup_inferior = tdefault_post_startup_inferior;
  ops->to_insert_fork_catchpoint = tdefault_insert_fork_catchpoint;
  ops->to_remove_fork_catchpoint = tdefault_remove_fork_catchpoint;
  ops->to_insert_vfork_catchpoint = tdefault_insert_vfork_catchpoint;
  ops->to_remove_vfork_catchpoint = tdefault_remove_vfork_catchpoint;
  ops->to_follow_fork = default_follow_fork;
  ops->to_insert_exec_catchpoint = tdefault_insert_exec_catchpoint;
  ops->to_remove_exec_catchpoint = tdefault_remove_exec_catchpoint;
  ops->to_set_syscall_catchpoint = tdefault_set_syscall_catchpoint;
  ops->to_has_exited = tdefault_has_exited;
  ops->to_mourn_inferior = default_mourn_inferior;
  ops->to_can_run = tdefault_can_run;
  ops->to_pass_signals = tdefault_pass_signals;
  ops->to_program_signals = tdefault_program_signals;
  ops->to_thread_alive = tdefault_thread_alive;
  ops->to_find_new_threads = tdefault_find_new_threads;
  ops->to_pid_to_str = default_pid_to_str;
  ops->to_extra_thread_info = tdefault_extra_thread_info;
  ops->to_thread_name = tdefault_thread_name;
  ops->to_stop = tdefault_stop;
  ops->to_rcmd = default_rcmd;
  ops->to_pid_to_exec_file = tdefault_pid_to_exec_file;
  ops->to_log_command = tdefault_log_command;
  ops->to_get_section_table = tdefault_get_section_table;
  ops->to_can_async_p = tdefault_can_async_p;
  ops->to_is_async_p = tdefault_is_async_p;
  ops->to_async = tdefault_async;
  ops->to_supports_non_stop = tdefault_supports_non_stop;
  ops->to_find_memory_regions = dummy_find_memory_regions;
  ops->to_make_corefile_notes = dummy_make_corefile_notes;
  ops->to_get_bookmark = tdefault_get_bookmark;
  ops->to_goto_bookmark = tdefault_goto_bookmark;
  ops->to_xfer_partial = tdefault_xfer_partial;
  ops->to_memory_map = tdefault_memory_map;
  ops->to_flash_erase = tdefault_flash_erase;
  ops->to_flash_done = tdefault_flash_done;
  ops->to_read_description = tdefault_read_description;
  ops->to_get_ada_task_ptid = default_get_ada_task_ptid;
  ops->to_auxv_parse = default_auxv_parse;
  ops->to_search_memory = default_search_memory;
  ops->to_can_execute_reverse = tdefault_can_execute_reverse;
  ops->to_execution_direction = default_execution_direction;
  ops->to_supports_multi_process = tdefault_supports_multi_process;
  ops->to_supports_enable_disable_tracepoint = tdefault_supports_enable_disable_tracepoint;
  ops->to_supports_string_tracing = tdefault_supports_string_tracing;
  ops->to_supports_evaluation_of_breakpoint_conditions = tdefault_supports_evaluation_of_breakpoint_conditions;
  ops->to_can_run_breakpoint_commands = tdefault_can_run_breakpoint_commands;
  ops->to_thread_architecture = default_thread_architecture;
  ops->to_thread_address_space = default_thread_address_space;
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
  ops->to_trace_stop = tdefault_trace_stop;
  ops->to_trace_find = tdefault_trace_find;
  ops->to_get_trace_state_variable_value = tdefault_get_trace_state_variable_value;
  ops->to_save_trace_data = tdefault_save_trace_data;
  ops->to_upload_tracepoints = tdefault_upload_tracepoints;
  ops->to_upload_trace_state_variables = tdefault_upload_trace_state_variables;
  ops->to_get_raw_trace_data = tdefault_get_raw_trace_data;
  ops->to_get_min_fast_tracepoint_insn_len = tdefault_get_min_fast_tracepoint_insn_len;
  ops->to_set_disconnected_tracing = tdefault_set_disconnected_tracing;
  ops->to_set_circular_trace_buffer = tdefault_set_circular_trace_buffer;
  ops->to_set_trace_buffer_size = tdefault_set_trace_buffer_size;
  ops->to_set_trace_notes = tdefault_set_trace_notes;
  ops->to_core_of_thread = tdefault_core_of_thread;
  ops->to_verify_memory = default_verify_memory;
  ops->to_get_tib_address = tdefault_get_tib_address;
  ops->to_set_permissions = tdefault_set_permissions;
  ops->to_static_tracepoint_marker_at = tdefault_static_tracepoint_marker_at;
  ops->to_static_tracepoint_markers_by_strid = tdefault_static_tracepoint_markers_by_strid;
  ops->to_traceframe_info = tdefault_traceframe_info;
  ops->to_use_agent = tdefault_use_agent;
  ops->to_can_use_agent = tdefault_can_use_agent;
  ops->to_supports_btrace = tdefault_supports_btrace;
  ops->to_enable_btrace = tdefault_enable_btrace;
  ops->to_disable_btrace = tdefault_disable_btrace;
  ops->to_teardown_btrace = tdefault_teardown_btrace;
  ops->to_read_btrace = tdefault_read_btrace;
  ops->to_stop_recording = tdefault_stop_recording;
  ops->to_save_record = tdefault_save_record;
  ops->to_delete_record = tdefault_delete_record;
  ops->to_record_is_replaying = tdefault_record_is_replaying;
  ops->to_goto_record_begin = tdefault_goto_record_begin;
  ops->to_goto_record_end = tdefault_goto_record_end;
  ops->to_goto_record = tdefault_goto_record;
  ops->to_insn_history = tdefault_insn_history;
  ops->to_insn_history_from = tdefault_insn_history_from;
  ops->to_insn_history_range = tdefault_insn_history_range;
  ops->to_call_history = tdefault_call_history;
  ops->to_call_history_from = tdefault_call_history_from;
  ops->to_call_history_range = tdefault_call_history_range;
  ops->to_augmented_libraries_svr4_read = tdefault_augmented_libraries_svr4_read;
  ops->to_get_unwinder = tdefault_get_unwinder;
  ops->to_get_tailcall_unwinder = tdefault_get_tailcall_unwinder;
  ops->to_decr_pc_after_break = default_target_decr_pc_after_break;
}
