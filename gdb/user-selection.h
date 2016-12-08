#ifndef USER_SELECTION_H
#define USER_SELECTION_H

class user_selection {
public:

  /* Default constructor, nothing is selected.  */

  user_selection ()
  : m_inferior (nullptr),
    m_thread (nullptr),
    m_frame_id (null_frame_id),
    m_frame_level (INVALID_FRAME_LEVEL)
  {}

  /* Make INF the selected inferior.  If NOTIFY is true, call the observer
     indicating a selection change.

     Return true if the newly selected inferior is different than the previously
     selected inferior.  */

  bool select_inferior (struct inferior *inf, bool notify);

  /* Make THREAD the selected thread.  If NOTIFY is true, call the observer
     indicating a selection change.

     Return true if the newly selected thread is different than the previously
     selected thread.  */

  bool select_thread (struct thread_info *thread, bool notify);

  /* Make FRAME the selected frame.  If NOTIFY is true, call the observer
     indicating a selection change.

     Return true if the newly selected frame is different than the previously
     selected frame.  */

  bool select_frame (struct frame_info *frame, bool notify);

  /* Get the selected inferior.  */

  struct inferior *inferior () const
  { return m_inferior; }

  /* Get the selected thread.  */

  struct thread_info *thread () const
  { return m_thread; }

  /* Get the selected frame.  */

  struct frame_info *
  frame ()
  {
    if (!has_valid_frame ())
      try_select_current_frame ();

    if (!has_valid_frame ())
      return NULL;

    return frame_find_by_id (m_frame_id);
  }

  frame_id
  raw_frame_id () const
  { return m_frame_id; }

  int
  raw_frame_level () const
  { return m_frame_level; }

  bool has_valid_frame () const
  { return m_frame_level != INVALID_FRAME_LEVEL; }

private:

  struct inferior *m_inferior;
  struct thread_info *m_thread;

  struct frame_id m_frame_id;
  int m_frame_level;

  void sanity_check () const;
  void try_select_current_frame ();
};

/* Get the global user selection.  */

user_selection *global_user_selection ();

/* Initialize the global user selection.  This must be called after the initial
   inferior has been created.  */

void init_global_user_selection ();

/* Apply the global user selection to core of gdb.  */

void apply_global_user_selection ();

#endif /* USER_SELECTION_H */
