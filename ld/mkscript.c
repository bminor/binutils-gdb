#include <stdio.h>

int
main() 
{
  int ch;
  ch = getchar();
  printf("/* Generated through mkscript */\n");
  printf("\"SCRIPT \\\n");
  while (ch != EOF) {
    if (ch == '\n') {
      putchar(' ');
      putchar('\\');
    }
    putchar(ch);
    ch = getchar();
  }
  printf("ENDSCRIPT\"\n");
  return 0;
}
