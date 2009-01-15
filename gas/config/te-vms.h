#define TE_VMS
#include "obj-format.h"

extern int vms_file_stats_name (const char *, long long *, long *, char *, int *);

/* VMS debugger expects a separator.  */
#define DWARF2_DIR_SHOULD_END_WITH_SEPARATOR 1

/* VMS debugger needs the file timestamp.  */
#define DWARF2_FILE_TIME_NAME(FILENAME,DIRNAME)                       \
 ({                                                                   \
   long long cdt;                                                     \
   char *filename = (char *) alloca (strlen (FILENAME) + strlen (DIRNAME) + 1);\
   strcpy (filename, DIRNAME);                                        \
   strcat (filename, FILENAME);                                       \
   ((vms_file_stats_name (filename, &cdt, 0, 0, 0) == 0) ? cdt : 0);  \
 })

/* VMS debugger needs the file size.  */
#define DWARF2_FILE_SIZE_NAME(FILENAME,DIRNAME)                       \
 ({                                                                   \
   long siz;                                                          \
   char *filename = (char *) alloca (strlen (FILENAME) + strlen (DIRNAME) + 1);\
   strcpy (filename, DIRNAME);                                        \
   strcat (filename, FILENAME);                                       \
   ((vms_file_stats_name (filename, 0, &siz, 0, 0) == 0) ? siz : 0);  \
 })

/* VMS debugger needs the filename with version appended.  */
/* Longest filename on VMS is 255 characters. Largest version is 32768.  */
#define DWARF2_FILE_NAME(FILENAME,DIRNAME)                            \
 ({                                                                   \
   int ver;                                                           \
   char buff [255 + 7];                                               \
   char *filename = (char *) alloca (strlen (FILENAME) + strlen (DIRNAME) + 1);\
   strcpy (filename, DIRNAME);                                        \
   strcat (filename, FILENAME);                                       \
   vms_file_stats_name (filename, 0, 0, 0, &ver);                     \
   snprintf (buff, 255 + 7, "%s;%d", FILENAME, ver);                  \
   buff;                                                              \
 })

