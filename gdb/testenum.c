/* Source file for showing ENUM lossage in GDB.
   Compile with "cc -o foo -g foo.c".  */

enum bar { value1, value2, value3 };

struct foo {
  enum bar enum_value;
  int int_value;
  char *pointer_value;
};

struct foo foo_instance;
struct foo *foo_instance_pointer;

main ()
{
  foo_instance_pointer = &foo_instance;
  foo_instance.enum_value = value2;
  foo_instance.int_value = 1;
  foo_instance.pointer_value = "Text to make a char *";  

  /* In GDB, set a breakpoint at this line.  Then try to change the
     value of foo_instance.enum_value in any way.  I can't do it. */
}

