/* Test GDB's character set support
   Jim Blandy <jimb@cygnus.com> --- December 2001 */

#include <stdio.h>


/* X_string is a null-terminated string in the X charset whose
   elements are as follows.  X should be the name the `set charset'
   command uses for the character set, in lower-case, with any
   non-identifier characters replaced with underscores.  Where a
   character set doesn't have the given character, the string should
   contain the character 'x'.

   [0] --- the `alert' character, '\a'
   [1] --- the `backspace' character, '\b'
   [2] --- the `escape' character, '\e'
   [3] --- the `form feed' character, '\f'
   [4] --- the `line feed' character, '\n'
   [5] --- the `carriage return' character, '\r'
   [6] --- the `horizontal tab' character, '\t'
   [7] --- the `vertical tab' character, '\v'
   [8  .. 33] --- the uppercase letters A-Z
   [34 .. 59] --- the lowercase letters a-z
   [60 .. 69] --- the digits 0-9
   [70] --- the `cent' character
   [71] --- a control character with no defined backslash escape

   Feel free to extend these as you like.  */

#define NUM_CHARS (72)

char ascii_string[NUM_CHARS];
char iso_8859_1_string[NUM_CHARS];
char ebcdic_us_string[NUM_CHARS];
char ibm1047_string[NUM_CHARS];


void
init_string (char string[],
             char x,
             char alert, char backspace, char escape, char form_feed,
             char line_feed, char carriage_return, char horizontal_tab,
             char vertical_tab, char cent, char misc_ctrl)
{
  memset (string, x, NUM_CHARS);
  string[0] = alert;
  string[1] = backspace;
  string[2] = escape;
  string[3] = form_feed;
  string[4] = line_feed;
  string[5] = carriage_return;
  string[6] = horizontal_tab;
  string[7] = vertical_tab;
  string[70] = cent;
  string[71] = misc_ctrl;
}


void
fill_run (char string[], int start, int len, int first)
{
  int i;

  for (i = 0; i < len; i++)
    string[start + i] = first + i;
}


int main ()
{
#ifdef usestubs
  set_debug_traps();
  breakpoint();
#endif
  (void) malloc (1);
  /* Initialize ascii_string.  */
  init_string (ascii_string,
               120,
               7, 8, 27, 12,
               10, 13, 9,
               11, 120, 17);
  fill_run (ascii_string, 8, 26, 65);
  fill_run (ascii_string, 34, 26, 97);
  fill_run (ascii_string, 60, 10, 48);

  /* Initialize iso_8859_1_string.  */
  init_string (iso_8859_1_string,
               120,
               7, 8, 27, 12,
               10, 13, 9,
               11, 162, 17);
  fill_run (iso_8859_1_string, 8, 26, 65);
  fill_run (iso_8859_1_string, 34, 26, 97);
  fill_run (iso_8859_1_string, 60, 10, 48);

  /* Initialize ebcdic_us_string.  */
  init_string (ebcdic_us_string,
               167,
               47, 22, 39, 12,
               37, 13, 5,
               11, 74, 17);
  /* In EBCDIC, the upper-case letters are broken into three separate runs.  */
  fill_run (ebcdic_us_string, 8, 9, 193);
  fill_run (ebcdic_us_string, 17, 9, 209);
  fill_run (ebcdic_us_string, 26, 8, 226);
  /* The lower-case letters are, too.  */
  fill_run (ebcdic_us_string, 34, 9, 129);
  fill_run (ebcdic_us_string, 43, 9, 145);
  fill_run (ebcdic_us_string, 52, 8, 162);
  /* The digits, at least, are contiguous.  */
  fill_run (ebcdic_us_string, 60, 10, 240);

  /* Initialize ibm1047_string.  */
  init_string (ibm1047_string,
               167,
               47, 22, 39, 12,
               37, 13, 5,
               11, 74, 17);
  /* In EBCDIC, the upper-case letters are broken into three separate runs.  */
  fill_run (ibm1047_string, 8, 9, 193);
  fill_run (ibm1047_string, 17, 9, 209);
  fill_run (ibm1047_string, 26, 8, 226);
  /* The lower-case letters are, too.  */
  fill_run (ibm1047_string, 34, 9, 129);
  fill_run (ibm1047_string, 43, 9, 145);
  fill_run (ibm1047_string, 52, 8, 162);
  /* The digits, at least, are contiguous.  */
  fill_run (ibm1047_string, 60, 10, 240);

  puts ("All set!");            /* all strings initialized */
}
