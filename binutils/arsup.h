



void EXFUN( map_over_members, (void (*function) (), char **files, int count));





	struct list {
		char *name;	
		struct list *next;
	} ;

void EXFUN(maybequit,(void));
void EXFUN(prompt,(void));
void EXFUN(ar_clear,(void));
void EXFUN(ar_replace,(struct list *));
void EXFUN(ar_delete,(struct list *));
void EXFUN(ar_save, (void));
void EXFUN(ar_list, (void));
void EXFUN(ar_open, (char *, int));
void EXFUN(ar_directory,(char *, struct list *, char *));
void EXFUN(ar_addmod, (struct list *));
void EXFUN(ar_addlib, (char *, struct list *));
int interactive;

int yyparse();
