
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define DEBUGIFY
#include "debugify.h"

#define REDIRECT

static FILE *fout=NULL;
static char fname[128];
static int file_cnt=0;	/* count number of open files */

void puts_dbg(const char *data)
{
    FILE *fdbg;

    fdbg=fopen("dbg.log","a+");
    if (fdbg==NULL) 
	return;
    fprintf(fdbg,data);
    fclose(fdbg);
}

/* Can't easily get the message back to gdb... write to a log instead. */
void fputs_dbg (const char *data, FILE * fakestream)
{
#ifdef REDIRECT
    puts_dbg(data);
#else /* REDIRECT */
      
      //CIOLogView_output (s);
      if (fakestream==stdout || fakestream==stderr || fakestream==NULL)
      {
          if (fout==NULL) 
	  {
	      for (file_cnt=0; file_cnt<20; file_cnt++)
	      {
		  sprintf(fname,"dbg%d.log",file_cnt);
		  if ((fout=fopen(fname),"r")!=NULL)
		      fclose(fout);
		  else
		      break;
	      }
	      fout=fopen(fname,"w");
	      if (fout==NULL) 
		  return;
	  }
	  fakestream=fout;
      }
      fprintf(fakestream,data);
      fflush(fakestream);
#endif /* REDIRECT */
}

void printf_dbg(const char* format,...)
{
  va_list args;
  char buf[256];
  va_start (args, format);
  vsprintf (buf, format, args);
  puts_dbg(buf);
  va_end (args);
}
