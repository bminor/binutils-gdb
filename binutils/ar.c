/* ar.c - Archive modify and extract. */
/*
   Bugs: should use getopt the way tar does (complete w/optional -) and
   should have long options too. GNU ar used to check file against filesystem
   in quick_update and replace operations (would check mtime). Doesn't warn
   when name truncated. No way to specify pos_end. Error messages should be
   more consistant.
*/
#include "sysdep.h"
#include "bfd.h"
#include "ar.h"
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#define BUFSIZE 8192
/* Not great to have these here.  Should they be exported or not? */
PROTO(size_t, bfd_read, (void *ptr, size_t size, size_t nitems, bfd * abfd));
PROTO(size_t, bfd_write, (void *ptr, size_t size, size_t nitems, bfd * abfd));
/* PROTO (void, open_inarch, (char *archive_filename)); */
#ifdef __STDC__
static void     open_inarch(char *archive_filename);
#else
static void     open_inarch();
#endif				/* __STDC__ */

PROTO(void, map_over_members, (void (*function) (), char **files, int count));
PROTO(void, print_contents, (bfd * member));
PROTO(void, extract_file, (bfd * abfd));
PROTO(void, delete_members, (char **files_to_delete));
PROTO(void, do_quick_append, (char *archive_filename, char **files_to_append));
PROTO(void, move_members, (char **files_to_move));
PROTO(void, replace_members, (char **files_to_replace));
PROTO(void, print_descr, (bfd * abfd));
PROTO(void, ranlib_only, (char *archname));

/** Globals and flags */

char           *program_name = NULL;
bfd             bogus_archive;
bfd            *inarch;		/* The input arch we're manipulating */

/* Nonzero means don't warn about creating the archive file if necessary.  */
int             silent_create = 0;
/* Nonzero means describe each action performed.  */
int             verbose = 0;
/* Nonzero means preserve dates of members when extracting them.  */
int             preserve_dates = 0;
/*
   Nonzero means don't replace existing members whose dates are more recent
   than the corresponding files.
*/
int             newer_only = 0;
/* write a __.SYMDEF member into the modified archive.  */
boolean         write_armap = false;
/*
   Nonzero means don't update __.SYMDEF unless command line explicitly
   requested it
*/
int             ignore_symdef = 0;
/*
   Nonzero means it's the name of an existing member; position new or moved
   files with respect to this one.
*/
char           *posname = NULL;
/*
   Sez how to use `posname': pos_before means position before that member.
   pos_after means position after that member. pos_end means always at end.
   pos_default means default appropriately. For the latter two, `posname'
   should also be zero.
*/
enum pos {
    pos_default, pos_before, pos_after, pos_end
}               postype = pos_default;

#ifdef GNU960
	char *default_target;

	void
	gnu960_verify_target(abfd)
	bfd *abfd;
	{
	    if ( abfd->format == bfd_unknown ){
		bfd_check_format(abfd, bfd_object);
		/* Don't really care if it's an object --
		 * just want to get the correct xvec.
		 */
	    }
	    if ( !BFD_COFF_FILE_P(abfd) ){
		fatal( "'%s' not a COFF file -- operation aborted",
							abfd->filename );
	    }
	}
#endif



boolean operation_alters_arch = false;

/*
   The option parsing should be in its own function.  It will be when I have
   getopt working.
*/
int
main(argc, argv)
    int             argc;
    char          **argv;
{
    char           *arg_ptr;
    char            c;
    enum {
	none = 0, delete, replace, print_table,
	print_files, extract, move, quick_append
    }               operation = none;
    int             arg_index;
    char          **files;
    char           *inarch_filename;
    char           *temp;

#ifdef GNU960
    check_v960( argc, argv );
    default_target = bfd_make_targ_name(BFD_COFF_FORMAT,HOST_BYTE_ORDER_BIG_P);
#endif

    program_name = argv[0];

    temp = strrchr(program_name, '/');
    if (temp == (char *) NULL)
	temp = program_name;	/* shouldn't happen, but... */
    else
	++temp;
    if (!strcmp(temp, "ranlib")) {
	if (argc < 2)
	    fatal("Too few command arguments.");
	ranlib_only(argv[1]);
    }


    if (argc < 3)
	fatal("Too few command arguments.");

    arg_ptr = argv[1];

    if (*arg_ptr == '-')
	++arg_ptr;		/* compatibility */

    while (c = *arg_ptr++) {
	switch (c) {
	case 'd':
	case 'm':
	case 'p':
	case 'q':
	case 'r':
	case 't':
	case 'x':
	    if (operation != none)
		fatal("two different operation switches specified");
	    switch (c) {
	    case 'd':
		operation = delete;
		operation_alters_arch = true;
		break;
	    case 'm':
		operation = move;
		operation_alters_arch = true;
		break;
	    case 'p':
		operation = print_files;
		break;
	    case 'q':
		operation = quick_append;
		operation_alters_arch = true;
		break;
	    case 'r':
		operation = replace;
		operation_alters_arch = true;
		break;
	    case 't':
		operation = print_table;
		break;
	    case 'x':
		operation = extract;
		break;
	    }
	case 'l':
	    break;
	case 'c':
	    silent_create = 1;
	    break;
	case 'o':
	    preserve_dates = 1;
	    break;
	case 's':
	    write_armap = true;
	    break;
	case 'u':
	    newer_only = 1;
	    break;
	case 'v':
	    verbose = 1;
	    break;
	case 'a':
	    postype = pos_after;
	    break;
	case 'b':
	    postype = pos_before;
	    break;
	case 'i':
	    postype = pos_before;
	    break;
	default:
	    fatal("invalid option %c", c);
	}
    }

    if (operation == none && write_armap)
	ranlib_only(argv[2]);

    if (operation == none)
	fatal("no operation specified");

    if (newer_only && operation != replace)
	fatal("'u' only meaningful with 'r' option.");

    arg_index = 2;

    if (postype != pos_default)
	posname = argv[arg_index++];

    inarch_filename = argv[arg_index++];

    if (arg_index < argc) {
	files = argv + arg_index;
	while (arg_index < argc)
	    if (!strcmp(argv[arg_index++], "__.SYMDEF")) {
		ignore_symdef = 1;
		break;
	    }
    }
    else
	files = NULL;

    if (operation == quick_append) {
	if (files != NULL)
	    do_quick_append(inarch_filename, files);
	exit(0);
    }


    open_inarch(inarch_filename);
    /*
       If we have no archive, and we've been asked to replace then create one
    */
#if 0
    if (operation == replace && inarch == &bogus_archive) {
	silent_create = 1;
	do_quick_append(inarch_filename, 0);
	open_inarch(inarch_filename);
    }
#endif
    switch (operation) {

    case print_table:
	map_over_members(print_descr, files, argc - 3);
	break;

    case print_files:
	map_over_members(print_contents, files, argc - 3);
	break;

    case extract:
	map_over_members(extract_file, files, argc - 3);
	break;

    case delete:
	if (files != NULL)
	    delete_members(files);
	break;

    case move:
	if (files != NULL)
	    move_members(files);
	break;

    case replace:
	if (files != NULL || write_armap)
	    replace_members(files);
	break;

	/* Shouldn't happen! */
    default:
	fprintf(stderr, "Sorry; this option not implemented.\n");
    }

    return (0);
}				/* main() */

static
char *normalize(file)
char *file;
{
    char *    filename = strrchr(file, '/');
    if (filename != (char *)NULL) {
	filename ++;
    }
    else {
	filename = file;
    }
    return filename;
}

static void
open_inarch(archive_filename)
    char           *archive_filename;
{
    bfd           **last_one;
    bfd            *next_one;
    struct stat     sbuf;
    bfd_error = no_error;
    if (stat(archive_filename, &sbuf) != 0) {
	if (errno != ENOENT)
	    bfd_fatal(archive_filename);
	if (!operation_alters_arch) {
	  fprintf (stderr, "%s: %s not found.\n", program_name,
		   archive_filename);
	  exit (1);
	}	
	if (!silent_create)
	    fprintf(stderr,
		    "%s: creating %s\n", program_name, archive_filename);

	inarch = &bogus_archive;
	inarch->filename = archive_filename;
	inarch->has_armap = true;

    }
    else {
#ifdef GNU960
	inarch = bfd_openr(archive_filename, default_target);
#else
	inarch = bfd_openr(archive_filename, NULL);
#endif
	if (inarch == NULL) {
    bloser:
	    bfd_perror(archive_filename);
	    exit(1);
	}

	if (bfd_check_format(inarch, bfd_archive) != true)
	    fatal("File %s is not an archive.", archive_filename);
#ifdef GNU960
	gnu960_verify_target(inarch);	/* Exits on failure */
#endif
	last_one = &(inarch->next);
	/* Read all the contents right away, regardless. */
	for (next_one = bfd_openr_next_archived_file(inarch, NULL);
	     next_one;
	     next_one = bfd_openr_next_archived_file(inarch, next_one)) {
	    *last_one = next_one;
	    last_one = &next_one->next;
	}
	*last_one = (bfd *) NULL;
	if (bfd_error != no_more_archived_files)
	    goto bloser;
    }
}



/*
   If count is 0, then function is called once on each entry. if nonzero,
   count is the length of the files chain; function is called on each entry
   whose name matches one in files
*/
void
map_over_members(function, files, count)
    void            (*function) ();
    char          **files;
    int             count;
{
    bfd            *head;




    if (count == 0) {
	for (head = inarch->next; head; head = head->next)
	    function(head);
	return;
    }
    /*
       This may appear to be a baroque way of accomplishing what we want.
       however we have to iterate over the filenames in order to notice where
       a filename is requested but does not exist in the archive.  Ditto
       mapping over each file each time -- we want to hack multiple
       references.
    */

    for (; count > 0; files++, count--) {
	boolean         found = false;
	for (head = inarch->next; head; head = head->next)
	    if ((head->filename != NULL) &&
		(!strcmp(*files, head->filename))) {
		found = true;
		function(head);
	    }
	if (!found)
	    fprintf(stderr, "No entry %s in archive.\n", *files);
    }
}


/* Things which are interesting to map over all or some of the files: */

void
print_descr(abfd)
    bfd            *abfd;
{
    print_arelt_descr(abfd, verbose);
}

void
print_contents(abfd)
    bfd            *abfd;
{
    int             ncopied = 0;
    struct stat     buf;
    long            size;
    if (bfd_stat_arch_elt(abfd, &buf) != 0)
	fatal("Internal stat error on %s", abfd->filename);

    if (verbose)
	printf("\n<member %s>\n\n", abfd->filename);

    bfd_seek(abfd, 0, SEEK_SET);

    size = buf.st_size;
    while (ncopied < size) {
	char            cbuf[BUFSIZE];
	int             nread;
	int             tocopy = size - ncopied;
	if (tocopy > BUFSIZE)
	    tocopy = BUFSIZE;

	nread = bfd_read(cbuf, 1, tocopy, abfd);	/* oops -- broke
							   abstraction!  */

	if (nread != tocopy)
	    fatal("file %s not a valid archive", abfd->my_archive->filename);
	fwrite(cbuf, 1, nread, stdout);
	ncopied += tocopy;
    }
}


/*
   Extract a member of the archive into its own file.

We defer opening the new file until after we have read a BUFSIZ chunk of the
   old one, since we know we have just read the archive header for the old
   one.  Since most members are shorter than BUFSIZ, this means we will read
   the old header, read the old data, write a new inode for the new file, and
   write the new data, and be done. This 'optimization' is what comes from
   sitting next to a bare disk and hearing it every time it seeks.  -- Gnu
   Gilmore
*/

void
extract_file(abfd)
    bfd            *abfd;
{
    FILE           *ostream;
    char            cbuf[BUFSIZE];
    int             nread,
                    tocopy;
    int             ncopied = 0;
    long            size;
    struct stat     buf;
    if (bfd_stat_arch_elt(abfd, &buf) != 0)
	fatal("Internal stat error on %s", abfd->filename);
    size = buf.st_size;

    if (verbose)
	printf("x - %s\n", abfd->filename);

    bfd_seek(abfd, 0, SEEK_SET);

    ostream = 0;
    if (size == 0) {
      /* Seems like an abstraction violation, eh?  Well it's OK! */
      ostream = fopen(abfd->filename, "w");
      if (!ostream) {
	perror(abfd->filename);
	exit(1);
      }
    } else
    while (ncopied < size) {
	tocopy = size - ncopied;
	if (tocopy > BUFSIZE)
	    tocopy = BUFSIZE;

	nread = bfd_read(cbuf, 1, tocopy, abfd);
	if (nread != tocopy)
	    fatal("file %s not a valid archive", abfd->my_archive->filename);

	/* See comment above; this saves disk arm motion */
	if (!ostream) {
	    /* Seems like an abstraction violation, eh?  Well it's OK! */
	    ostream = fopen(abfd->filename, "w");
	    if (!ostream) {
		perror(abfd->filename);
		exit(1);
	    }
	}
	/* no need to byte-swap; the two formats are presumably compatible(!) */
	fwrite(cbuf, 1, nread, ostream);
	ncopied += tocopy;
    }

    fclose(ostream);
    chmod(abfd->filename, buf.st_mode);

    if (preserve_dates) {
#ifdef USG
	long            tb[2];
	tb[0] = buf.st_mtime;
	tb[1] = buf.st_mtime;
	utime(abfd->filename, tb);	/* FIXME check result */
#else
	struct timeval  tv[2];
	tv[0].tv_sec = buf.st_mtime;
	tv[0].tv_usec = 0;
	tv[1].tv_sec = buf.st_mtime;
	tv[1].tv_usec = 0;
	utimes(abfd->filename, tv);	/* FIXME check result */
#endif
    }
}


/* Just do it quickly; don't worry about dups, armap, or anything like that */

/* This is ugly! XXX */

PROTO(struct ar_hdr *, bfd_special_undocumented_glue, (char *filename));

void
do_quick_append(archive_filename, files_to_append)
    char           *archive_filename;
    char          **files_to_append;

{
    FILE           *ofile,
                   *ifile;
    char            buf[BUFSIZE];
    long            tocopy,
                    thistime;
    bfd            *temp;
    struct stat     sbuf;
    boolean         newfile = false;
    bfd_error = no_error;

    if (stat(archive_filename, &sbuf) != 0) {
	if (errno != ENOENT)
	    bfd_fatal(archive_filename);
	newfile = true;
    }


    ofile = fopen(archive_filename, "a+");
    if (ofile == NULL) {
	perror(program_name);
	exit(1);
    }

    /* bletch */
#ifdef GNU960
    temp = bfd_openr(archive_filename, default_target);
#else
    temp = bfd_openr(archive_filename, NULL);
#endif
    if (temp == NULL) {
	bfd_perror(archive_filename);
	exit(1);
    }
    if (newfile == false) {
	if (bfd_check_format(temp, bfd_archive) != true)
	    fatal("File %s is not an archive.", archive_filename);
#ifdef GNU960
	gnu960_verify_target(temp);	/* Exits on failure */
#endif
    }
    else {
	fwrite(ARMAG, 1, SARMAG, ofile);
	if (!silent_create)
	    fprintf(stderr, "%s: creating %s\n", program_name, archive_filename);
    }

    /* assume it's an achive, go straight to the end, sans $200 */
    fseek(ofile, 0, 2);

    for (; files_to_append && *files_to_append; ++files_to_append) {
	struct ar_hdr  *hdr = bfd_special_undocumented_glue(*files_to_append);
	if (hdr == NULL) {
	    bfd_perror(*files_to_append);
	    exit(1);
	}

	BFD_SEND(temp, _bfd_truncate_arname, (temp, *files_to_append, (char *) hdr));

	ifile = fopen(*files_to_append, "r");
	if (ifile == NULL)
	    bfd_perror(program_name);

	if (stat(*files_to_append, &sbuf) != 0)
	    bfd_perror(*files_to_append);

	tocopy = sbuf.st_size;

	/* XXX should do error-checking! */
	fwrite(hdr, 1, sizeof(struct ar_hdr), ofile);


	while (tocopy > 0) {
	    thistime = tocopy;
	    if (thistime > BUFSIZE)
		thistime = BUFSIZE;
	    fread(buf, 1, thistime, ifile);
	    fwrite(buf, 1, thistime, ofile);
	    tocopy -= thistime;
	}
	fclose(ifile);
	if ((sbuf.st_size % 2) == 1)
	    putc('\n', ofile);
    }
    fclose(ofile);
    bfd_close(temp);
}


void
write_archive()
{
    bfd            *obfd;
    char           *xmalloc();
    int             namelen = strlen(inarch->filename);
    char           *new_name = xmalloc(namelen + 6);
    bfd            *contents_head = inarch->next;
#if 0
    if (inarch == &bogus_archive) {
	/* How can this be ? */
	return;
    }
    else {
#endif
	strcpy(new_name, inarch->filename);
	strcpy(new_name + namelen, ".art");
	obfd = bfd_openw(new_name,
			 /* violates abstraction; need a better protocol */
			 (inarch->xvec ? bfd_get_target(inarch) : NULL));

	if (obfd == NULL)
	    bfd_fatal(inarch->filename);

	bfd_set_format(obfd, bfd_archive);
	obfd->has_armap = write_armap;

	if (bfd_set_archive_head(obfd, contents_head) != true)
	    bfd_fatal(inarch->filename);

	if (!bfd_close(obfd))
	    bfd_fatal(inarch->filename);
	if (rename(new_name, inarch->filename) != 0)
	    bfd_fatal(inarch->filename);
#if 0
    }
#endif
}



/*
   returns a pointer to the pointer to the entry which should be rplacd'd
   into when altering.  default_pos should be how to interpret pos_default,
   and should be a pos value.
*/

bfd **
get_pos_bfd(contents, default_pos)
    bfd           **contents;
    enum pos        default_pos;
{
    bfd           **after_bfd;

    enum pos        realpos = (postype == pos_default ? default_pos : postype);
    switch (realpos) {

    case pos_end:
	after_bfd = contents;
	while (*after_bfd) {
	    after_bfd = &((*after_bfd)->next);
	}

	break;
#if 0
    case pos_after:
	for (after_bfd = contents; after_bfd; after_bfd = after_bfd->next)
	    if (!strcpy(after_bfd->filename, posname))
		break;
	break;
    case pos_before:
	for (after_bfd = contents; after_bfd; after_bfd = after_bfd->next)
	    if (after_bfd->next && (!strcpy(after_bfd->next->filename, posname)))
		break;
#endif
    }

    return after_bfd;
}


void
delete_members(files_to_delete)
    char          **files_to_delete;
{
    bfd           **current_ptr_ptr;
    boolean         found;
    boolean         something_changed = false;
    for (; *files_to_delete != NULL; ++files_to_delete) {
	/*
	   In a.out systems, the armap is optional.  It's also called
	   __.SYMDEF.  So if the user asked to delete it, we should remember
	   that fact. The name is NULL in COFF archives, so using this as a
	   key is as good as anything I suppose
	*/
	if (!strcmp(*files_to_delete, "__.SYMDEF")) {
	    inarch->has_armap = false;
	    write_armap = false;
	    continue;
	}

	found = false;
	current_ptr_ptr = &(inarch->next);
	while (*current_ptr_ptr) {
	    if (strcmp(*files_to_delete, (*current_ptr_ptr)->filename) == 0) {
		found = true;
		something_changed = true;
		if (verbose)
		    printf("d - %s\n",
			   *files_to_delete);
		*current_ptr_ptr = ((*current_ptr_ptr)->next);
		goto next_file;

	    }
	    else {
		current_ptr_ptr = &((*current_ptr_ptr)->next);
	    }
	}

	if (verbose && found == false) {
	    printf("No member named `%s'\n", *files_to_delete);
	}
next_file:;

    }

    if (something_changed == true) {
	write_archive();
    }
}


/* Reposition existing members within an archive */

void
move_members(files_to_move)
    char          **files_to_move;
{
    bfd           **after_bfd;	/* New entries go after this one */
    bfd           **current_ptr_ptr;	/* cdr pointer into contents */




    for (; *files_to_move; ++files_to_move) {
	current_ptr_ptr = &(inarch->next);
	while (*current_ptr_ptr) {
	    bfd            *current_ptr = *current_ptr_ptr;
	    if (strcmp(normalize(*files_to_move), current_ptr->filename) == 0) {
		/*
		   Move this file to the end of the list - first cut from
		   where it is.
		*/
		*current_ptr_ptr = current_ptr->next;

		/* Now glue to end */
		after_bfd = get_pos_bfd(&inarch->next, pos_end);
		*after_bfd = current_ptr;
		current_ptr->next = (bfd *) NULL;

		if (verbose)
		    printf("m - %s\n", *files_to_move);

		goto next_file;
	    }
	    current_ptr_ptr = &((*current_ptr_ptr)->next);
	}
	fprintf(stderr, "No entry %s in archive %s!\n",
		*files_to_move, inarch->filename);
	exit(1);
next_file:;
    }

    write_archive();
}


/* Ought to default to replacing in place, but this is existing practice! */

void
replace_members(files_to_move)
    char          **files_to_move;
{
    bfd           **after_bfd;	/* New entries go after this one */
    bfd            *current;
    bfd           **current_ptr;
    bfd            *temp;
    /*
       If the first item in the archive is an __.SYMDEF then remove it
    */
    if (inarch->next &&
	strcmp(inarch->next->filename, "__.SYMDEF") == 0) {
	inarch->next = inarch->next->next;
    }



    while (files_to_move && *files_to_move) {
	current_ptr = &inarch->next;
	while (*current_ptr) {
	    current = *current_ptr;
	    
	    if (!strcmp(normalize(*files_to_move), current->filename)) {
		/* snip out this entry from the chain */
		*current_ptr = current->next;
		if (newer_only) {
		    struct stat     fsbuf,
		                    asbuf;
		    if (stat(*files_to_move, &fsbuf) != 0) {
			if (errno != ENOENT)
			    bfd_fatal(*files_to_move);
			goto next_file;
		    }
		    if (bfd_stat_arch_elt(current, &asbuf) != 0)
			fatal("Internal stat error on %s", current->filename);

		    if (fsbuf.st_mtime <= asbuf.st_mtime)
			goto next_file;
		}


		after_bfd = get_pos_bfd(&inarch->next, pos_end);
		temp = *after_bfd;
		*after_bfd = bfd_openr(*files_to_move, NULL);
		if (*after_bfd == (bfd *) NULL) {
		    fprintf(stderr, "Can't open file %s\n", *files_to_move);
		    exit(1);
		}
#ifdef GNU960
		gnu960_verify_target(*after_bfd);	/* Exits on failure */
#endif
		(*after_bfd)->next = temp;

		if (verbose) {
		    printf("%c - %s\n", (postype == pos_after ? 'r' : 'a'),
			   *files_to_move);
		}
		goto next_file;
	    }
	    current_ptr = &(current->next);
	}

	/* It isn't in there, so add to end */

	after_bfd = get_pos_bfd(&inarch->next, pos_end);
	temp = *after_bfd;
	*after_bfd = bfd_openr(*files_to_move, NULL);
	if (*after_bfd == (bfd *) NULL) {
	    fprintf(stderr, "Can't open file %s\n", *files_to_move);
	    exit(1);
	}
#ifdef GNU960
	gnu960_verify_target(*after_bfd);	/* Exits on failure */
#endif
	if (verbose) {
	    printf("c - %s\n", *files_to_move);
	}

	(*after_bfd)->next = temp;

next_file:;

	files_to_move++;
    }


    write_archive();
}

void
ranlib_only(archname)
    char           *archname;
{
    write_armap = true;
    open_inarch(archname);
    write_archive();
    exit(0);
}


