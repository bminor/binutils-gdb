#include <stdio.h>

int
main() 
{
  int ch;
  ch = getchar();
  printf("/* Generated through mkscript */\n");
  printf("\"{ \\\n");
  while (ch != EOF) {
    if (ch == '\"' || ch == '\\' || ch == '\'') {
      putchar('\\');
      putchar(ch);
    }
    else { if (ch == '\n')
      fputs("\\n\\", stdout);
    putchar(ch);
     }
    ch = getchar();
  }
  printf("}\"\n");
  return 0;
}
