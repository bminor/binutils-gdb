
/* We manipulate environments represented as these structures.  */

struct environ
{
  /* Number of usable slots allocated in VECTOR.
     VECTOR always has one slot not counted here,
     to hold the terminating zero.  */
  int allocated;
  /* A vector of slots, ALLOCATED + 1 of them.
     The first few slots contain strings "VAR=VALUE"
     and the next one contains zero.
     Then come some unused slots.  */
  char **vector;
};

struct environ *make_environ ();
void free_environ ();
void init_environ ();
char *get_in_environ ();
void set_in_environ ();
void unset_in_environ ();
char **environ_vector ();
