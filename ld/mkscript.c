#include <stdio.h>

int
main() 
{
  int ch;
  ch = getchar();
  printf("/* Generated through mkscript */\n");
  printf("\"{ \\\n");
  while (ch != EOF) {
    if (ch == '\"') {
      putchar('\\');
      putchar('\"');
    }
else { if (ch == '\n') {
      putchar(' ');
      putchar('\\');
    }
    putchar(ch);
     }
    ch = getchar();
  }
  printf("}\"\n");
  return 0;
}
