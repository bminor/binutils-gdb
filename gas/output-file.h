

#ifdef __STDC__

void output_file_append(char *where, long length, char *filename);
void output_file_close(char *filename);
void output_file_create(char *name);

#else /* __STDC__ */

void output_file_append();
void output_file_close();
void output_file_create();

#endif /* __STDC__ */


/* end of output-file.h */
