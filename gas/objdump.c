/* objdump -- dump information about an object file.
   Copyright (C) 1988, 1991 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* $Id$ */

/*
 * objdump
 * 
 * dump information about an object file.  Until there is other documentation,
 * refer to the manual page dump(1) in the system 5 program's reference manual
 */
#include <stdio.h>
#include <assert.h>

#include "getopt.h"

#include "as.h"

/* #define COFF_ENCAPSULATE 1 */

typedef FILHDR fileheader;
typedef struct exec fileheader; 

#ifdef __STDC__
static char *sym_pname(SYMENT *s);
static char *xmalloc(unsigned size);
static char *xrealloc(char *p, unsigned size);
static void doit(char *filename);
static void dump_data(fileheader *execp, FILE *f){};
static void dump_header(fileheader *execp, FILE *f);
static void dump_lnno(fileheader *execp, FILE *f);
static void dump_nstuff(fileheader *execp){};
static void dump_reloc(fileheader *execp, FILE *f);
static void dump_section_contents(fileheader *execp, FILE *f);
static void dump_section_headers(fileheader *execp, FILE *f);
static void dump_sym(fileheader *execp, FILE *f);
static void dump_text(fileheader *execp, FILE *f){};
static void hex_dump(void *buffer, int size);
#endif /* __STDC__ */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
static void read_symbols (execp, f)
#else
read_symbols (execp, f)
#endif /* OBJ_BOUT */
struct exec *execp;
#else
static void read_section_headers(execp, f)
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
{
#ifndef OBJ_COFF
	int i;
	struct nlist *sp;
	if (symtbl)
		return;
	nsyms = execp->a_syms / sizeof (struct nlist);
	if (nsyms == 0)
#else
	if (section_headers || execp->f_nscns == 0) {
#endif /* OBJ_COFF */
		return;
#ifdef OBJ_COFF
	} /* already read them, or don't need to */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
	symtbl = (struct nlist *)xmalloc (nsyms * sizeof (struct nlist));
#else
	fseek(f, sizeof(*execp) + execp->f_opthdr, 0);
	section_headers = (struct scnhdr *) xmalloc(execp->f_nscns * sizeof(*section_headers));
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	fseek(f, N_STROFF(*execp), 0);
	if (fread((char *)&strsize, sizeof strsize, 1, f) != 1) {
		fprintf(stderr, "%s: can not read string table size\n",
#else
	fseek (f, N_STROFF(*execp), 0);
	if (fread ((char *)&strsize, sizeof strsize, 1, f) != 1) {
		fprintf (stderr, "%s: can not read string table size\n",
#endif /* OBJ_BOUT */
			 program_name);
		exit (1);
	}
	strtbl = xmalloc (strsize);
#ifndef OBJ_BOUT
	fseek(f, N_STROFF (*execp), 0);
	if (fread(strtbl, 1, strsize, f) != strsize) {
		fprintf(stderr, "%s: error reading string table\n",
#else
	fseek (f, N_STROFF (*execp), 0);
	if (fread (strtbl, 1, strsize, f) != strsize) {
		fprintf (stderr, "%s: error reading string table\n",
#endif /* OBJ_BOUT */
			 program_name);
		exit (1);
	}
#else
	if (fread(section_headers, execp->f_nscns * sizeof(*section_headers), 1, f) != 1) {
		perror("error reading section headers");
		abort();
	} /* on error */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	fseek(f, N_SYMOFF (*execp), 0);
	if (fread((char *)symtbl, sizeof (struct nlist), nsyms, f) != nsyms) {
		fprintf(stderr, "%s: error reading symbol table\n",
#else
	fseek (f, N_SYMOFF (*execp), 0);
	if (fread ((char *)symtbl, sizeof (struct nlist), nsyms, f) != nsyms) {
		fprintf (stderr, "%s: error reading symbol table\n",
#endif /* OBJ_BOUT */
			 program_name);
		exit (1);
	}
#else
	return;
} /* read_section_headers() */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
	for (i = 0, sp = symtbl; i < nsyms; i++, sp++) {
		if (sp->n_un.n_strx == 0)
			sp->n_un.n_name = "";
		else if (sp->n_un.n_strx < 0 || sp->n_un.n_strx > strsize)
			sp->n_un.n_name = "<bad string table index>";
		else
			sp->n_un.n_name = strtbl + sp->n_un.n_strx;
	}
#ifndef OBJ_BOUT
} /* read_symbols() */
#else
}
#endif /* OBJ_BOUT */
#else
static SYMENT *symbols = NULL;
static int longest_symbol_name = SYMNMLEN;
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
static void free_symbols ()
#else
free_symbols ()
#endif /* OBJ_BOUT */
#else
static void read_symbols(execp, f)
fileheader *execp;
FILE *f;
#endif /* OBJ_COFF */
{
#ifdef OBJ_COFF
	long here;
	int bufsiz = execp->f_nsyms * sizeof(struct syment);
	SYMENT *s;
	
	if (symbols || bufsiz == 0) {
		return;
	} /* already read, or don't need to */

	symbols = (SYMENT *) xmalloc(bufsiz);
	
	/* read symbols */
	fseek(f, execp->f_symptr, 0);
	if (fread(symbols, bufsiz, 1, f) != 1) {
		fprintf(stderr, "error reading symbol table.\n");
		abort();
	} /* on error */
	
	here = ftell(f);
	fseek(f, 0, 2); /* find end of file */

	if (here != ftell(f)) {
		/* find string table size */
		fseek(f, here, 0);
		if (fread(&strsize, sizeof(strsize), 1, f) != 1) {
			perror("error reading string table size");
			abort();
		} /* on error */
		
		/* read string table if there is one */
		if (strsize > 0) {
			strtbl = xmalloc(strsize);
			fseek(f, -sizeof(strsize), 1); /* backup over size count */
			
			if (fread(strtbl, strsize, 1, f) != 1) {
				perror("error reading string table");
				abort();
			} /* on error */
			
			/* then connect the dots. */
			for (s = symbols; s < symbols + execp->f_nsyms; ++s) {
				if (!s->n_zeroes) {
					int l;
					
					s->n_offset = (long) strtbl + s->n_offset;
					l = strlen((char *) s->n_offset);
					if (l > longest_symbol_name) {
						longest_symbol_name = l;
					} /* keep max */
				} /* "long" name */
				
				s += s->n_numaux; /* skip aux entries */
			} /* walk the symbol table */
		} else {
			fprintf(stderr, "Well, now that's weird.  I have a string table whose size is zero?\n");
		} /* if there is a string table */
	} /* if there is a string table */
	return;
} /* read_symbols() */

#ifdef comment
static void free_symbols() {
#endif /* OBJ_COFF */
	if (symtbl)
		free (symtbl);
	symtbl = NULL;
	if (strtbl)
		free (strtbl);
	strtbl = NULL;
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
} /* free_symbols() */
#ifndef OBJ_COFF
#else
}
#endif /* OBJ_BOUT */
#else
#endif /* comment */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF

#ifndef OBJ_BOUT
static void usage ()
#else
usage ()
#endif /* OBJ_BOUT */
{
#ifndef OBJ_BOUT
#else
static void usage() {
#endif /* OBJ_COFF */
	(void) fprintf(stderr, "Usage: %s\n", program_name);
	(void) fprintf(stderr, "\t[-ahnrt] [+all] [+header] [+nstuff]\n");
	(void) fprintf(stderr, "\t[+reloc] [+symbols] [+text] [+data]\n");
	(void) fprintf(stderr, "\t[+omit-symbol-numbers] [+omit-reloc-numbers]\n");
	(void) fprintf(stderr, "\tfile...\n");
#ifndef OBJ_COFF
#else
	fprintf (stderr, "\
Usage: %s [-hnrt] [+header] [+nstuff] [+reloc] [+symbols] file...\n",
		 program_name);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	exit (1);
#ifndef OBJ_COFF
}
#else
} /* usage() */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
static int aflag = 0;
static int hflag = 0;
#ifdef OBJ_COFF
static int lflag = 0;
#endif /* OBJ_COFF */
static int nflag = 0;
static int rflag = 0;
#ifdef OBJ_COFF
static int sflag = 0;
#endif /* OBJ_COFF */
static int tflag = 0;
static int Dflag = 0;
static int Tflag = 0;
static int omit_reloc_numbers_flag = 0;
static int omit_sym_numbers_flag = 0;
#ifndef OBJ_COFF
#else
int hflag;
int nflag;
int rflag;
int tflag;
#endif /* OBJ_BOUT */
#else
static int section_headers_flag = 0;
static int section_contents_flag = 0;
#endif /* OBJ_COFF */

/* Size of a page.  Required by N_DATADDR in a.out.gnu.h [VAX].  */
int page_size;

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
int main (argc, argv)
#else
int main(argc, argv)
#endif /* OBJ_COFF */
int argc;
#ifndef OBJ_COFF
#else
main (argc, argv)
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
char **argv;
{
	int c;
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
/* 	extern char *optarg; */
#ifndef OBJ_COFF
#else
	extern char *optarg;
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	extern int optind;
	int seenflag = 0;
	int ind = 0;
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	static struct option long_options[] = {
#ifdef OBJ_COFF
		{"line-numbers",   0, &lflag, 1},
		{"section-contents",   0, &section_contents_flag, 1},
		{"section-headers",   0, &section_headers_flag, 1},
#endif /* OBJ_COFF */
		{"symbols",   0, &tflag, 1},
		{"reloc",  0, &rflag, 1},
		{"nstuff", 0, &nflag, 1},
		{"header", 0, &hflag, 1},
		{"data", 0, &Dflag, 1},
		{"text", 0, &Tflag, 1},
		{"omit-relocation-numbers", 0, &omit_reloc_numbers_flag, 1},
		{"omit-symbol-numbers", 0, &omit_sym_numbers_flag, 1},
		{"all", 0, &aflag, 1},
		{NULL, 0, NULL, 0},
	};
#ifndef OBJ_COFF
#else
	static struct option long_options[] = 
	  {
	    {"symbols",   0, &tflag, 1},
	    {"reloc",  0, &rflag, 1},
	    {"nstuff", 0, &nflag, 1},
	    {"header", 0, &hflag, 1},
	    {NULL, 0, NULL, 0}
	  };
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
	page_size = getpagesize ();

#endif /* OBJ_COFF */
	program_name = argv[0];
		 		 
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	while ((c = getopt_long (argc, argv, "ahnrt", long_options, &ind)) != EOF) {
#else
	while ((c = getopt_long (argc, argv, "hnrt", long_options, &ind))
 		      != EOF) {
#endif /* OBJ_BOUT */
#else
	while ((c = getopt_long (argc, argv, "ahlonrt", long_options, &ind)) != EOF) {
#endif /* OBJ_COFF */
		seenflag = 1;
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		switch (c) {
		case  0 : break; /* we've been given a long option */
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		case 'a': aflag = 1; break;
		case 'h': hflag = 1; break;
#ifdef OBJ_COFF
		case 'o': hflag = 1; break;
		case 'l': lflag = 1; break;
#endif /* OBJ_COFF */
		case 'n': nflag = 1; break;
		case 'r': rflag = 1; break;
#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		case 't': tflag = 1; break;
#ifndef OBJ_COFF
#ifdef OBJ_BOUT
		case 'r': rflag = 1; break;
		case 'n': nflag = 1; break;
		case 'h': hflag = 1; break;
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		default:
			usage ();
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		} /* switch on option */
	} /* while there are options */
#ifndef OBJ_COFF
#else
		}
	}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

	if (seenflag == 0 || optind == argc)
		usage ();

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	if (aflag) {
		hflag = 1;
#ifdef OBJ_COFF
		lflag = 1;
#endif /* OBJ_COFF */
		nflag = 1;
		rflag = 1;
		tflag = 1;
		Dflag = 1;
		Tflag = 1;
#ifdef OBJ_COFF
		section_headers_flag = 1;
		section_contents_flag = 1;
#endif /* OBJ_COFF */
	} /* if all */

#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	while (optind < argc)
#ifndef OBJ_COFF
		doit (argv[optind++]);
#ifndef OBJ_BOUT
#else
		doit(argv[optind++]);
#endif /* OBJ_COFF */

	return(0);
} /* main() */
#ifndef OBJ_COFF
#else
}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
static void doit (name)
#else
doit (name)
#endif /* OBJ_BOUT */
#else
static void doit(name)
#endif /* OBJ_COFF */
char *name;
{
	FILE *f;
#ifndef OBJ_COFF
	struct exec exec;
#ifndef OBJ_BOUT
#else
	fileheader exec;

	if (section_headers) {
		free(section_headers);
		section_headers = NULL;
	} /* free section headers */

	if (symbols) {
		free(symbols);
		symbols = NULL;
	} /* free symbols */

#endif /* OBJ_COFF */
	printf("%s:\n", name);
#ifndef OBJ_COFF
#else
	printf ("%s:\n", name);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	f = fopen (name, "r");
	if (f == NULL) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		fprintf(stderr, "%s: can not open ", program_name);
#ifndef OBJ_COFF
#else
		fprintf (stderr, "%s: can not open ", program_name);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		perror (name);
		return;
	}
#ifdef HEADER_SEEK
	HEADER_SEEK (f);
#endif
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	if (fread((char *)&exec, sizeof exec, 1, f) != 1) {
#else
	if (fread((char *)&exec, sizeof(exec), 1, f) != 1) {
#endif /* OBJ_COFF */
		fprintf(stderr, "%s: can not read header for %s\n",
#ifndef OBJ_COFF
#else
	if (fread ((char *)&exec, sizeof exec, 1, f) != 1) {
		fprintf (stderr, "%s: can not read header for %s\n",
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
			 program_name, name);
		return;
	}

#ifdef OBJ_COFF
#ifdef I960ROMAGIC
#define N_BADMAG I960BADMAG
#endif /* I960ROMAGIC */

#endif /* OBJ_COFF */
	if (N_BADMAG (exec)) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		fprintf(stderr, "%s: %s is not a%s object file\n",
			 program_name, name,
#ifdef B_OUT
			 " b.out"
#else
			 "n a.out"
#endif /* B_OUT */
			 );
#ifndef OBJ_COFF
#else
		fprintf (stderr, "%s: %s is not an object file\n",
			 program_name, name);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		return;
	}

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	if (hflag) dump_header(&exec, f);
#ifdef OBJ_COFF
	if (lflag) dump_lnno(&exec, f);
#endif /* OBJ_COFF */
	if (nflag) dump_nstuff(&exec);
#ifdef OBJ_COFF
	if (section_headers_flag) dump_section_headers(&exec, f);
	if (section_contents_flag) dump_section_contents(&exec, f);
	if (sflag) dump_section_contents(&exec, f);
#endif /* OBJ_COFF */
	if (Tflag) dump_text(&exec, f);
	if (Dflag) dump_data(&exec, f);
	if (tflag) dump_sym(&exec, f);
	if (rflag) dump_reloc(&exec, f);
#ifndef OBJ_COFF
#else
	if (hflag)
		dump_header (&exec);
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
	if (nflag)
		dump_nstuff (&exec);
#endif /* OBJ_BOUT */
#else
	printf("\n");
	fclose(f);
	return;
} /* doit() */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	free_symbols();
#else
	if (tflag)
		dump_sym (&exec, f);
#endif /* OBJ_BOUT */
#else
static void dump_lnno(execp, f)
fileheader *execp;
FILE *f;
{
	int i = execp->f_nscns;
	struct scnhdr *section;
	char *buffer;
	int bufsiz = 0;

	if (i) {
		printf("Line numbers:\n");
		read_section_headers(execp, f);
		read_symbols(execp, f);

		for (section = section_headers; i; ++section, --i) {
			int size = section->s_nlnno * LINESZ;
			LINENO *r;

			if (size > bufsiz) {
				if (bufsiz) {
					buffer = xrealloc(buffer, bufsiz = size);
				} else {
					buffer = xmalloc(bufsiz = size);
				} /* if we had allocated anything before */
			} /* if bigger than our old buffer */
			
			printf("%8.8s:", section->s_name);
			fseek(f, section->s_lnnoptr, 0);

			if (size) {
				int j;

				if (fread(buffer, size, 1, f) != 1) {
					printf(" (error reading lnno)\n");
					continue;
				} /* on read error */

				printf("\n");
				
				for (r = (LINENO *) buffer, j = 0; j < section->s_nlnno; ++j, ++r) {
					printf("lnno = %d,", r->l_lnno);

					if (r->l_lnno) {
						printf(" paddr = 0x%lx", (unsigned long) r->l_addr.l_paddr);
					} else {
						printf(" symndx = %ld, \"%s\"",
						       r->l_addr.l_symndx,
						       sym_pname(symbols + r->l_addr.l_symndx));
					} /* if not symbol'd */

					if (r->padding[0] || r->padding[1]) {
						printf(" (padding = %2x %2x)",
						       (unsigned) r->padding[0],
						       (unsigned) r->padding[1]);
					} /* if padding not zero'd */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#else
					printf("\n");
				} /* for each lnno record */
			} else {
				printf(" (section has no line numbers.)\n");
			} /* if there really is something in the section */
		} /* for each section */
	} else {
		printf("No Sections.\n");
	} /* if there are sections */

	free(buffer);
	printf("\n");
#endif /* OBJ_COFF */
	return;
#ifndef OBJ_COFF
} /* doit() */
#else
	if (rflag)
		dump_reloc (&exec, f);
#endif /* OBJ_BOUT */
#else
} /* dump_lnno() */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
static void dump_header(execp, f)
#else
	free_symbols ();
#else
static void dump_reloc(execp, f)
fileheader *execp;
FILE *f;
{
	int i = execp->f_nscns;
	struct scnhdr *section;
	char *buffer;
	int bufsiz = 0;

	if (i) {
		read_section_headers(execp, f);

		printf("Relocations:\n");
		for (section = section_headers; i; ++section, --i) {
			int size = section->s_nreloc * RELSZ;
			RELOC *r;

			if (size > bufsiz) {
				if (bufsiz) {
					buffer = xrealloc(buffer, bufsiz = size);
				} else {
					buffer = xmalloc(bufsiz = size);
				} /* if we had allocated anything before */
			} /* if bigger than our old buffer */
			
			printf("%8.8s:", section->s_name);
			fseek(f, section->s_relptr, 0);

			if (size) {
				int j;

				if (fread(buffer, size, 1, f) != 1) {
					printf(" (error reading reloc)\n");
					continue;
				} /* on read error */

				printf("\n");
				
				for (r = (RELOC *) buffer, j = 0; j < section->s_nreloc; ++j, ++r) {
					printf("vaddr = 0x%lx, symndx = %ld, r_type = ",
					       (unsigned long) r->r_vaddr,
					       r->r_symndx);

					switch (r->r_type) {
					case R_RELLONG:		printf(" RELLONG"); break;
					case R_IPRSHORT:	printf("IPRSHORT"); break;
					case R_IPRMED:		printf("  IPRMED"); break;
					case R_IPRLONG:		printf(" IPRLONG"); break;
					case R_OPTCALL:		printf(" OPTCALL"); break;
					case R_OPTCALLX:	printf("OPTCALLX"); break;
					case R_GETSEG:		printf("  GETSEG"); break;
					case R_GETPA:		printf("   GETPA"); break;
					case R_TAGWORD:		printf(" TAGWORD"); break;
					default: printf("unrecognized"); break;
					} /* switch on reloc type */

					printf(".");

					if (r->pad[0] || r->pad[1]) {
						printf(" (padding = %2x %2x)",
						       (unsigned) r->pad[0],
						       (unsigned) r->pad[1]);
					} /* if padding isn't zero */

					printf("\n");
				} /* for each reloc record */
			} else {
				printf(" (section has no relocations.)\n");
			} /* if there really is something in the section */
		} /* for each section */
	} else {
		printf("No Sections.\n");
	} /* if there are sections */

	/* free(buffer); */
	printf("\n");
	return;
} /* dump_reloc() */

static void dump_section_contents(execp, f)
fileheader *execp;
FILE *f;
{
	int i = execp->f_nscns;
	struct scnhdr *section;
	char *buffer;
	int bufsiz = 0;

	if (i) {
		read_section_headers(execp, f);
		printf("Section Contents:\n");

		for (section = section_headers; i; ++section, --i) {
			if (section->s_size > bufsiz) {
				if (bufsiz) {
					buffer = xrealloc(buffer, bufsiz = section->s_size);
				} else {
					buffer = xmalloc(bufsiz = section->s_size);
				} /* if we had allocated anything before */
			} /* if bigger than our old buffer */
			
			printf("%8.8s:", section->s_name);
			
			if (section->s_flags & STYP_BSS) {
				printf(" bss sections have no contents.\n");
			} else {
				fseek(f, section->s_scnptr, 0);
				
				if (section->s_size) {
					if (fread(buffer, section->s_size, 1, f) != 1) {
						printf(" (error reading section contents)\n");
					} /* on read error */
					
					printf("\n");
					hex_dump(buffer, section->s_size);
					printf("\n");
				} else {
					printf(" (section has a size of zero.)\n");
				} /* if there really is a section */
			} /* if bss else dump */
		} /* for each section */
	} else {
		printf("No Sections.\n");
	} /* if there are sections */

	free(buffer);
	printf("\n");
	return;
} /* dump_section_contents() */

static void dump_section_headers(execp, f)
fileheader *execp;
FILE *f;
{
	int i = execp->f_nscns;
	struct scnhdr *section;

	if (i > 0) {
		read_section_headers(execp, f);
		printf("Section Headers:\n");

		for (section = section_headers; i; ++section, --i) {
			long flags = section->s_flags;

			printf("\"%8.8s\"", section->s_name);
			
			printf(" physical address: 0x%x vma: 0x%x size: 0x%x (%ld)",
			       (unsigned) section->s_paddr,
			       (unsigned) section->s_vaddr,
			       (unsigned) section->s_size,
			       section->s_size);
			
			printf(" relocs: %d linenos: %d alignment: 0x%lx (%ld)",
			       section->s_nreloc,
			       section->s_nlnno,
			       section->s_align,
			       (long) section->s_align);
			
			printf(" flags: 0x%x = ", (unsigned) section->s_flags);
			
			if (flags & STYP_REG) {
				printf(" REG");
				flags &= ~STYP_REG;
			} /* STYP_REG */
			
			if (flags & STYP_DSECT) {
				printf(" DSECT");
				flags &= ~STYP_DSECT;
			} /* STYP_DSECT */
			
			if (flags & STYP_NOLOAD) {
				printf(" NOLOAD");
				flags &= ~STYP_NOLOAD;
			} /* STYP_NOLOAD */
			
			if (flags & STYP_GROUP) {
				printf(" GROUP");
				flags &= ~STYP_GROUP;
			} /* STYP_GROUP */
			
			if (flags & STYP_PAD) {
				printf(" PAD");
				flags &= ~STYP_PAD;
			} /* STYP_PAD */
			
			if (flags & STYP_COPY) {
				printf(" COPY");
				flags &= ~STYP_COPY;
			} /* STYP_COPY */
			
			if (flags & STYP_TEXT) {
				printf(" TEXT");
				flags &= ~STYP_TEXT;
			} /* STYP_TEXT */
			
			if (flags & S_SHRSEG) {
				printf(" SHRSEG");
				flags &= ~S_SHRSEG;
			} /* S_SHRSEG */
			
			if (flags & STYP_DATA) {
				printf(" DATA");
				flags &= ~STYP_DATA;
			} /* STYP_DATA */
			
			if (flags & STYP_BSS) {
				printf(" BSS");
				flags &= ~STYP_BSS;
			} /* STYP_BSS */
			
			if (flags & S_NEWFCN) {
				printf(" NEWFCN");
				flags &= ~S_NEWFCN;
			} /* S_NEWFCN */
			
			if (flags & STYP_INFO) {
				printf(" INFO");
				flags &= ~STYP_INFO;
			} /* STYP_INFO */
			
			if (flags & STYP_OVER) {
				printf(" OVER");
				flags &= ~STYP_OVER;
			} /* STYP_OVER */
			
			if (flags & STYP_LIB) {
				printf(" LIB");
				flags &= ~STYP_LIB;
			} /* STYP_LIB */
			
			if (flags & STYP_MERGE) {
				printf(" MERGE");
				flags &= ~STYP_MERGE;
			} /* STYP_MERGE */
			
			if (flags & STYP_REVERSE_PAD) {
				printf(" REVERSE_PAD");
				flags &= ~STYP_REVERSE_PAD;
			} /* STYP_REVERSE_PAD */
			
			if (flags) {
				printf(" +unknown");
			} /* foo */
			
			printf("\n");
		} /* for each section header */
	} else {
		printf("No section headers.\n");
	} /* if there are any sections */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
}
#else
	printf("\n");
	return;
} /* dump_section_headers() */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
dump_header (execp)
#endif /* OBJ_BOUT */
struct exec *execp;
#ifndef OBJ_BOUT
#else
static void dump_header(execp, f)
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
{
#ifdef OBJ_COFF
#ifdef COFF
	printf("magic: 0x%x (%o) ", (unsigned) execp->f_magic, (unsigned) execp->f_magic);
	printf("number of sections: %d number of syms: %ld ", execp->f_nscns, execp->f_nsyms);
	printf("time stamp: %s", ctime(&(execp->f_timdat)));
	printf("flags:");

	if (execp->f_flags & F_RELFLG) {
		printf(" RELFLG");
	} /* relflg */

	if (execp->f_flags & F_EXEC) {
		printf(" EXEC");
	} /* exec */

	if (execp->f_flags & F_LNNO) {
		printf(" LNNO");
	} /* lnno */

	if (execp->f_flags & F_LSYMS) {
		printf(" LSYMS");
	} /* lsyms */

	if (execp->f_flags & F_AR32WR) {
		printf(" AR32WR");
	} /* ar32wr */

	assert(F_I960KB == F_I960SB);
	assert(F_I960KA == F_I960SA);

	switch (execp->f_flags & F_I960TYPE) {
	case F_I960CORE:	printf(" I960CORE"); break;
	case F_I960KB:		printf(" I960KB (== I960SB)"); break;
	case F_I960MC:		printf(" I960MC"); break;
	case F_I960XA:		printf(" I960XA"); break;
	case F_I960CA:		printf(" I960CA"); break;
	case F_I960KA:		printf(" I960KA (== I960SA)"); break;
	default:		printf(" I960Unknown"); break;
	} /* switch on i960 type */

	if (execp->f_flags & ~(F_RELFLG | F_EXEC | F_LNNO | F_LSYMS | F_AR32WR | F_I960TYPE)) {
		printf(" +unrecognized");
	} /* unrecognized */

	printf("\n\n");

	if (execp->f_opthdr) {
		if (execp->f_opthdr == sizeof(AOUTHDR)) {
			AOUTHDR hdr;
			
			fseek(f, sizeof(*execp), 0);
			if (fread(&hdr, sizeof(AOUTHDR), 1, f) == 1) {
				printf("aouthdr:\n");
				printf("magic: 0x%x (%o)", (unsigned) hdr.magic, (unsigned) hdr.magic);
				printf(" vstamp: 0x%ld\n", (long) hdr.vstamp);

				printf("sizes: text 0x%lx (%ld), data 0x%lx (%ld), bss 0x%lx (%ld)\n",
				       hdr.tsize,
				       (long) hdr.tsize,
				       hdr.dsize,
				       (long) hdr.dsize,
				       hdr.bsize,
				       (long) hdr.bsize);
				
				printf("entry point: 0x%lx, starts: text 0x%lx (%ld), data 0x%lx (%ld)\n",
				       hdr.entry,
				       hdr.text_start,
				       (long) hdr.text_start,
				       hdr.data_start,
				       (long) hdr.data_start);
				
				printf("tag entries: %ld\n",
				       (long) hdr.tagentries);
			} else {
				fprintf(stderr, "%s: error reading optional header", program_name);
				perror(NULL);
			} /* on error */
			
		} else {
			printf("opthder != sizeof aouthdr?");
		} /* size mismatch */

	} else {
		printf("No optional header.");
	} /* print optional header */
		
		
#else /* COFF */
#endif /* OBJ_COFF */
	int x;

#if defined (__GNU_EXEC_MACROS__) && !defined (__STRUCT_EXEC_OVERRIDE__)
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	printf("magic: 0x%x (%o)", N_MAGIC(*execp), N_MAGIC(*execp));
	printf("machine type: %d", N_MACHTYPE(*execp));
	printf("flags: 0x%x", N_FLAGS(*execp));
#ifndef OBJ_COFF
#else
	printf ("magic: 0x%x (%o)", N_MAGIC(*execp), N_MAGIC(*execp));
	printf ("machine type: %d", N_MACHTYPE(*execp));
	printf ("flags: 0x%x", N_FLAGS(*execp));
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
#else /* non-gnu struct exec.  */
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	printf("magic: 0x%x (%o) ", (unsigned) execp->a_magic, (unsigned) execp->a_magic);
#ifndef OBJ_COFF
#else
	printf ("magic: 0x%x (%o) ", execp->a_magic, execp->a_magic);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
#endif /* non-gnu struct exec.  */
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	printf("text 0x%x ", (unsigned) execp->a_text);
	printf("data 0x%x ", (unsigned) execp->a_data);
	printf("bss 0x%x\n", (unsigned) execp->a_bss);
	printf("nsyms %ld",  (long) (execp->a_syms / sizeof(struct nlist)));
	x = execp->a_syms % sizeof(struct nlist);
#ifndef OBJ_COFF
#else
	printf ("text 0x%x ", execp->a_text);
	printf ("data 0x%x ", execp->a_data);
	printf ("bss 0x%x\n", execp->a_bss);
	printf ("nsyms %d", execp->a_syms / sizeof (struct nlist));
	x = execp->a_syms % sizeof (struct nlist);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	if (x) 
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		printf(" (+ %d bytes)", x);
	printf(" entry 0x%lx ", execp->a_entry);

#ifdef B_OUT
	printf(" talign 0x%x", (unsigned) execp->a_talign);
	printf(" dalign 0x%x", (unsigned) execp->a_dalign);
	printf(" balign 0x%x", (unsigned) execp->a_balign);
	printf(" unused 0x%x", (unsigned) execp->unused);
#endif /* B_OUT */

	printf(" trsize 0x%lx", execp->a_trsize);
	printf(" drsize 0x%lx", execp->a_drsize);

	if (N_TXTOFF(*execp) != 0 && N_TXTOFF(*execp) != sizeof(*execp)) {
		char *buffer;
		char *i;
		int size = N_TXTOFF(*execp) - sizeof(*execp);
		
		buffer = xmalloc(size);
		
		fseek(f, sizeof(*execp), 0);
		if (fread(buffer, size, 1, f) != 1) {
			fprintf(stderr, "%s: error reading between header and text", program_name);
			perror(NULL);
		} /* on error */
		
		for (i = buffer; i < (buffer + size); ++i) {
			if (*i != '\0') {
				printf(" (garbage follows header)");
				break;
			} /* non null */
		} /* walk the buffer looking for garbage */
	} /* check for garbage following header */
#ifdef OBJ_COFF
#endif /* COFF */
#endif /* OBJ_COFF */

	printf("\n");
	return;
} /* dump_header() */

#ifdef OBJ_COFF
#ifdef comment
#endif /* OBJ_COFF */
static void dump_nstuff(execp)
#ifndef OBJ_COFF
struct exec *execp;
#else
fileheader *execp;
#endif /* OBJ_COFF */
{
	printf("N_BADMAG %d\n", N_BADMAG(*execp));
	printf("N_TXTOFF 0x%x\n", N_TXTOFF(*execp));
	printf("N_SYMOFF 0x%lx\n", N_SYMOFF(*execp));
	printf("N_STROFF 0x%lx\n", N_STROFF(*execp));
	printf("N_TXTADDR 0x%x\n", (unsigned) N_TXTADDR(*execp));
	printf("N_DATADDR 0x%lx\n", N_DATADDR(*execp));

	return;
} /* dump_nstuff() */
#ifndef OBJ_COFF
#else
		printf (" (+ %d bytes)", x);
	printf (" entry 0x%x ", execp->a_entry);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
static void dump_text(execp, f)
#ifndef OBJ_COFF
struct exec *execp;
#else
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
{
	void *buffer;

	if (execp->a_text) {
		buffer = xmalloc(execp->a_text);
		fseek(f, N_TXTOFF(*execp), 0);

		if (fread(buffer, execp->a_text, 1, f) != 1) {
			fprintf(stderr, "%s: error reading text section.\n", program_name);
			return;
		} /* on error */
#ifndef OBJ_COFF
#else
	printf (" talign 0x%x ", execp->a_talign);
	printf (" dalign 0x%x ", execp->a_dalign);
	printf (" balign 0x%x ", execp->a_balign);
	printf (" unused 0x%x ", execp->unused);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		hex_dump(buffer, execp->a_text);
		free(buffer);
	} else {
		printf("No text section.\n");
	} /* if there is text */

	return;
} /* dump_text() */
#ifndef OBJ_COFF
#else
	printf ("trsize 0x%x ", execp->a_trsize);
	printf ("drsize 0x%x\n", execp->a_drsize);
}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
static void dump_data(execp, f)
#ifndef OBJ_COFF
#else
dump_nstuff (execp)
#endif /* OBJ_BOUT */
struct exec *execp;
#ifndef OBJ_BOUT
#else
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
{
	void *buffer;

	if (execp->a_data) {
		buffer = xmalloc(execp->a_data);
		fseek(f, N_TXTOFF(*execp), 0);

		if (fread(buffer, execp->a_data, 1, f) != 1) {
			fprintf(stderr, "%s: error reading data section.\n", program_name);
			return;
		} /* on error */

		hex_dump(buffer, execp->a_data);
		free(buffer);
	} else {
		printf("No data section.\n");
	} /* if there is data */

	return;
} /* dump_data() */
#ifdef OBJ_COFF
#endif /* comment */
#endif /* OBJ_COFF */

static void hex_dump(buffer, size)
void *buffer;
int size;
#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
{
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	FILE *f;

#ifndef OBJ_COFF
	if ((f = popen("od -x +0x0", "w")) != NULL) {
#else
	fflush(stdout);

	if ((f = popen("hexl", "w")) != NULL) {
#endif /* OBJ_COFF */
		if (fwrite(buffer, size, 1, f) != 1) {
			(void) fprintf(stderr, "%s: error writing to od(1) pipe:", program_name);
			perror(NULL);
		} /* on error */
	} else {
		(void) fprintf(stderr, "%s: error opening pipe to od(1):", program_name);
		perror(NULL);
	} /* on successful popen */

	(void) pclose(f);
#ifdef OBJ_COFF
	fflush(stdout);
#endif /* OBJ_COFF */
	return;
} /* hex_dump() */
#ifndef OBJ_COFF
#else
	printf ("N_BADMAG %d\n", N_BADMAG (*execp));
	printf ("N_TXTOFF 0x%x\n", N_TXTOFF (*execp));
	printf ("N_SYMOFF 0x%x\n", N_SYMOFF (*execp));
	printf ("N_STROFF 0x%x\n", N_STROFF (*execp));
	printf ("N_TXTADDR 0x%x\n", N_TXTADDR (*execp));
	printf ("N_DATADDR 0x%x\n", N_DATADDR (*execp));
}
#endif /* OBJ_BOUT */
#else

char *sym_class_pname(class)
char class;
{
	switch (class) {
	case C_EFCN:	return("EFCN");
	case C_NULL:	return("NULL");
	case C_AUTO:	return("AUTO");
	case C_EXT:	return("EXT");
	case C_STAT:	return("STAT");
	case C_REG:	return("REG");
	case C_EXTDEF:	return("EXTDEF");
	case C_LABEL:	return("LABEL");
	case C_ULABEL:	return("ULABEL");
	case C_MOS:	return("MOS");
	case C_ARG:	return("ARG");
	case C_STRTAG:	return("STRTAG");
	case C_MOU:	return("MOU");
	case C_UNTAG:	return("UNTAG");
	case C_TPDEF:	return("TPDEF");
	case C_USTATIC:	return("USTATIC");
	case C_ENTAG:	return("ENTAG");
	case C_MOE:	return("MOE");
	case C_REGPARM:	return("REGPARM");
	case C_FIELD:	return("FIELD");
	case C_BLOCK:	return("BLOCK");
	case C_FCN:	return("FCN");
	case C_EOS:	return("EOS");
	case C_FILE:	return("FILE");
	case C_LINE:	return("LINE");
	case C_ALIAS:	return("ALIAS");
	case C_HIDDEN:	return("HIDDEN");
		
	case C_SCALL:	return("SCALL");
	case C_LEAFEXT:	return("LEAFEXT");
	case C_OPTVAR:	return("OPTVAR");
	case C_DEFINE:	return("DEFINE");
	case C_PRAGMA:	return("PRAGMA");
	case C_SEGMENT:	return("SEGMENT");
	case C_LEAFSTAT:return("LEAFSTAT");
	case C_AUTOARG:	return("AUTOARG");
		
	default:	return("(???)");
	} /* switch on class */
} /* sym_class_pname() */

char *sym_type_pname(type)
unsigned long type;
{
	switch (type) {
	case T_NULL:	return("NULL");
	case T_VOID:	return("VOID");
	case T_CHAR:	return("CHAR");
	case T_SHORT:	return("SHORT");
	case T_INT:	return("INT");
	case T_LONG:	return("LONG");
	case T_FLOAT:	return("FLOAT");
	case T_DOUBLE:	return("DOUBLE");
	case T_STRUCT:	return("STRUCT");
	case T_UNION:	return("UNION");
	case T_ENUM:	return("ENUM");
	case T_MOE:	return("MOE");
	case T_UCHAR:	return("UCHAR");
	case T_USHORT:	return("USHORT");
	case T_UINT:	return("UINT");
	case T_ULONG:	return("ULONG");
	case T_LNGDBL:	return("LNGDBL");

	default:	return("(???)");
	} /* switch on type */
} /* sym_type_pname() */

char *sym_section_pname(scnum, execp)
short scnum;
fileheader *execp;
{
	switch (scnum) {
	case N_UNDEF: return("UNDEF");
	case N_ABS: return("ABS");
	case N_DEBUG: return("DEBUG");
	case N_TV: return("NTV");
	case P_TV: return("PTV");
		
	default:
		assert(0 <= (scnum-1));
		assert((scnum-1) < execp->f_nscns);
		return(section_headers[scnum-1].s_name);
	} /* switch on scnum */
} /* sym_section_pname() */

static char *sym_pname(s)
SYMENT *s;
{
	static char buffer[SYMNMLEN + 1];
	if (s->n_zeroes) {
		bzero(buffer, SYMNMLEN + 1);
		bcopy(s->n_name, buffer, SYMNMLEN);
		return(buffer);
	} else {
		return((char *) s->n_offset);
	} /* if "short" name */
} /* sym_pname() */

/*
 * Notes:  .file must be first, .text, .data, .bss must be last.
 */

static void dump_aux_fcn(aux)
AUXENT *aux;
{
	/* function symbol */
	printf(" tagndx %ld,", aux->x_sym.x_tagndx);
	printf(" size %ld,", aux->x_sym.x_misc.x_fsize);
	printf(" lnnoptr 0x%lx,", (unsigned long) aux->x_sym.x_fcnary.x_fcn.x_lnnoptr);
	printf(" endndx %ld", aux->x_sym.x_fcnary.x_fcn.x_endndx);
	printf(" tvndx 0x%x,", (unsigned) aux->x_sym.x_tvndx);
	return;
} /* dump_aux_fcn() */

static void dump_aux_tagmember(aux)
AUXENT *aux;
{
	printf(" tagndx %ld,", aux->x_sym.x_tagndx);
	printf(" size %d,", aux->x_sym.x_misc.x_lnsz.x_size);
	return;
} /* dump_aux_tagmember() */

static void dump_aux_array(aux)
AUXENT *aux;
{
	int i;
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#else
	printf(" size %d, ", aux->x_sym.x_misc.x_lnsz.x_size);

	for (i = 0; i < 4; ++i) {
		printf("[%d]", aux->x_sym.x_fcnary.x_ary.x_dimen[i]);
	} /* four dimensions */

	return;
} /* dump_aux_array() */

#endif /* OBJ_COFF */
static void dump_sym(execp, f)
#ifndef OBJ_COFF
#else
dump_sym (execp, f)
#endif /* OBJ_BOUT */
struct exec *execp;
#else
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
{
	int i;
#ifndef OBJ_COFF
	struct nlist *sp;
#else
	SYMENT *sp;
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
	read_symbols(execp, f);
#else
	read_symbols (execp, f);
#endif /* OBJ_BOUT */
	if (nsyms == 0) {
#ifndef OBJ_BOUT
#else
	read_section_headers(execp, f);

	if (execp->f_nsyms == 0) {
#endif /* OBJ_COFF */
		printf("no symbols\n");
#ifndef OBJ_COFF
#else
		printf ("no symbols\n");
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		return;
#ifndef OBJ_COFF
	}
#else
	} /* if there are any */

	read_symbols(execp, f);
	printf("Symbols:\n");
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	if (!omit_sym_numbers_flag) {
#ifndef OBJ_COFF
		printf("%3s: ", "#");
#else
		printf("%3s:", "#");
#endif /* OBJ_COFF */
	} /* printing symbol numbers */

#ifndef OBJ_COFF
	printf("%4s %5s %4s %8s\n",
		"type", "other", "desc", "val");
#else
	printf(" %*.*s %8.8s %3.3s %8.8s %7.7s %3.3s %s\n",
	       SYMNMLEN, SYMNMLEN, "name",
	       "value", "num", "sec-name", "class", "aux", "type");
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#else
	printf ("%3s: %4s %5s %4s %8s\n",
		"#", "type", "other", "desc", "val");
#endif /* OBJ_BOUT */
	for (i = 0, sp = symtbl; i < nsyms; i++, sp++) {
#ifndef OBJ_BOUT
#else
	for (i = 0, sp = symbols; sp < symbols + execp->f_nsyms; ++sp, ++i) {
#endif /* OBJ_COFF */
		if (!omit_sym_numbers_flag) {
#ifndef OBJ_COFF
			printf("%3d: ", i);
#else
			printf("%3d:", i);
#endif /* OBJ_COFF */
		} /* printing symbol numbers */
		
#ifndef OBJ_COFF
		printf("%4x %5x %4x %8lx %s",
			(unsigned) (sp->n_type & 0xff),
			(unsigned) (sp->n_other & 0xff),
			(unsigned) (sp->n_desc & 0xffff),
#else
		printf ("%3d: %4x %5x %4x %8x %s",
			i,
			sp->n_type & 0xff,
			sp->n_other & 0xff,
			sp->n_desc & 0xffff,
#endif /* OBJ_BOUT */
			sp->n_value,
			sp->n_un.n_name);
#ifndef OBJ_BOUT
#else
		printf(" %*.*s", SYMNMLEN, SYMNMLEN, (sp->n_zeroes) ? sp->n_name : "");
#endif /* OBJ_COFF */
		
#ifndef OBJ_COFF
#else

#endif /* OBJ_BOUT */
		if (sp->n_type & N_EXT) printf(" N_EXT");
		if (sp->n_type & N_STAB) printf(" N_STAB");
#ifndef OBJ_BOUT
#else
		printf(" %8lx", (unsigned long) sp->n_value);
		printf(" %3d", sp->n_scnum);
		printf(" %8.8s", sym_section_pname(sp->n_scnum, execp));
		printf(" %7.7s", sym_class_pname(sp->n_sclass));
		printf(" %1d", sp->n_numaux);
#endif /* OBJ_COFF */
		
#ifndef OBJ_COFF
#else

#endif /* OBJ_BOUT */
		if ((sp->n_type & N_TYPE) == N_UNDF) {
			printf(" N_UNDF");
		} else {
			if (sp->n_type & N_ABS) printf(" N_ABS");
			if (sp->n_type & N_TEXT) printf(" N_TEXT");
			if (sp->n_type & N_DATA) printf(" N_DATA");
			if (sp->n_type & N_BSS) printf(" N_BSS");
			if (sp->n_type & N_FN) printf(" N_FN");
		} /* if not undefined */
#ifndef OBJ_BOUT
#else
		printf(" %s", sym_type_pname(BTYPE(sp->n_type)));
#endif /* OBJ_COFF */
		
#ifndef OBJ_COFF
#ifdef B_OUT
#else

#endif /* OBJ_BOUT */
		if (sp->n_other) {
			printf(" [");
#ifndef OBJ_BOUT
#else
		/* derived type */
		printf("%s", (ISPTR(sp->n_type)
				 ? "(PTR)"
				 : (ISFCN(sp->n_type)
				    ? "(FCN)"
				    : (ISARY(sp->n_type)
				       ? "(ARY)"
				       : ""))));
		
		if (sp->n_type & ~(N_BTMASK | N_TMASK)) {
			printf("+");
		} /* if type isn't all */
		
		if (!sp->n_zeroes) {
			printf(" \"%s\"", sym_pname(sp));
		} /* if "long" name */
		
		/* FIXME do something with the flags field */
#ifdef comment
		if (sp->pad1[0] != 0 || sp->pad1[1] != 0) {
			printf(" (pad1 %2.2x%2.2x)", (unsigned) sp->pad1[0], (unsigned) sp->pad1[1]);
		} /* if padding not zeroed */
#endif /* comment */
		
		if (sp->pad2[0] != 0 || sp->pad2[1] != 0) {
			printf(" (pad2 %2.2x%2.2x)", (unsigned) sp->pad2[0], (unsigned) sp->pad2[1]);
		} /* if padding not zeroed */
		
#define DTYPE(x) (((x) & N_TMASK) >> N_BTSHFT)
		
		if (sp->n_numaux > 0) {
			int auxcountshouldbe = 1;
			AUXENT *aux = (AUXENT *) (sp + 1);
			AUXENT *aux2 = (AUXENT *) (sp + 2);
#endif /* OBJ_COFF */
			
#ifndef OBJ_COFF
#else
#else
			switch (sp->n_sclass) {
				
			case C_FILE: /* file symbol */
				printf(" filename \"%s\"", aux->x_file.x_fname);
				break;
				
			case C_UNTAG:
			case C_ENTAG:
			case C_STRTAG: {
				if (DTYPE(sp->n_type) == DT_NON
				    && (BTYPE(sp->n_type) == T_NULL
					|| BTYPE(sp->n_type) == T_STRUCT
					|| BTYPE(sp->n_type) == T_UNION
					|| BTYPE(sp->n_type) == T_ENUM)) {
					printf(" size %d,", aux->x_sym.x_misc.x_lnsz.x_size);
					printf(" endndx %ld", aux->x_sym.x_fcnary.x_fcn.x_endndx);
				} else {
 					printf(" (don't know why this tag has an auxent)");
					abort();
				} /* if I understand */
				
				break;
			} /* tags */
				
			case C_EOS: {
				if (BTYPE(sp->n_type) == DT_NON && BTYPE(sp->n_type) == T_NULL) {
					printf(" tagndx %ld,", aux->x_sym.x_tagndx);
					printf(" size %d,", aux->x_sym.x_misc.x_lnsz.x_size);
				} else {
					printf(" (don't know why this eos has an auxent)");
					abort();
				} /* if I understand */
				break;
			} /* eos */
				
			case C_FCN:
			case C_BLOCK: {
				if (BTYPE(sp->n_type) == DT_NON && BTYPE(sp->n_type) == T_NULL) {
					if (!strcmp(sp->n_name, ".bb") || !strcmp(sp->n_name, ".bf")) {
						printf(" lnno %d", aux->x_sym.x_misc.x_lnsz.x_lnno);
						printf(" endndx %ld", aux->x_sym.x_fcnary.x_fcn.x_endndx);
						break;
						
					} else if (!strcmp(sp->n_name, ".eb") || !strcmp(sp->n_name, ".ef")) {
						printf(" lnno %d", aux->x_sym.x_misc.x_lnsz.x_lnno);
						break;
						
					} /* beginning or ending */
				} /* if I understand */
				
				printf(" (don't know why this fcn or block has an auxent)");
				abort();
				break;
			} /* begin/end blocks */
				
			case C_LEAFEXT:
			case C_LEAFSTAT:
			case C_SCALL:
			case C_EXT: {
				assert(BTYPE(sp->n_type) != T_MOE);

				if (ISFCN(sp->n_type)
				    || BTYPE(sp->n_type) == T_NULL) {
					dump_aux_fcn(aux);
					
					if (sp->n_sclass == C_SCALL) {
						printf(" stindx %ld", aux2->x_sc.x_stindx);
						auxcountshouldbe = 2;
					} else if (sp->n_sclass == C_LEAFEXT
						   || sp->n_sclass == C_LEAFSTAT) {
						printf(" balentry 0x%lx", aux2->x_bal.x_balntry);
						auxcountshouldbe = 2;
					} /* special functions */
				} else if (ISARY(sp->n_type)) {
					dump_aux_array(aux);
				} else if (BTYPE(sp->n_type) == T_STRUCT) {
					printf(" tagndx %ld,", aux->x_sym.x_tagndx);
					printf(" size %d,", aux->x_sym.x_misc.x_lnsz.x_size);
				} else {
					assert(0);
				} /* on type */
				
				break;
			} /* function */
				
			case C_STAT: {
				switch (DTYPE(sp->n_type)) {
				case DT_NON:
					switch (BTYPE(sp->n_type)) {
					case T_NULL: /* section symbol */
						printf(" length 0x%lx, relocs %d, lnnos %d",
						       (unsigned long) aux->x_scn.x_scnlen,
						       aux->x_scn.x_nreloc,
						       aux->x_scn.x_nlinno);
						break;
					case T_STRUCT:
					case T_UNION:
					case T_ENUM:
						dump_aux_tagmember(aux);
						break;
					default:
						printf(" (confused).");
						abort();
					} /* switch on btype */
					break;
					
				case DT_FCN: /* function */
					if (BTYPE(sp->n_type) == T_MOE) {
						printf(" (confused).");
						abort();
					} else {
						dump_aux_fcn(aux);
					} /* if I understand */
					break;
					
				case DT_ARY:
					assert(BTYPE(sp->n_type) != T_MOE);
					dump_aux_array(aux);
					/* intentional fall through */
				case DT_PTR:
					assert(BTYPE(sp->n_type) == T_STRUCT
					       || BTYPE(sp->n_type) == T_UNION
					       || BTYPE(sp->n_type) == T_ENUM);
					dump_aux_tagmember(aux);
					break;
					
				default:
					printf(" (confused.)");
					abort();
				} /* switch on derived type */
				
				break;
			} /* STAT */
				
			case C_AUTO:
			case C_MOS:
			case C_MOU:
			case C_TPDEF:
				if (DTYPE(sp->n_type) == DT_ARY) {
					assert(BTYPE(sp->n_type) != T_MOE);
					dump_aux_array(aux);
				} else {
					dump_aux_tagmember(aux);
				} /* if an array */
				break;
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
			if (sp->n_other == N_CALLNAME) {
				printf(" N_CALLNAME");
			} else if (sp->n_other == N_BALNAME) {
				printf(" N_BALNAME");
			} else if (1 <= sp->n_other && sp->n_other <= 32) {
				printf(" \"trap\"");
			} else {
				printf(" !!!invalid \"other\" field");
			} /* what is it */
#ifndef OBJ_BOUT
#else
			case C_FIELD:
				printf(" tagndx %ld,", aux->x_sym.x_tagndx);
				printf(" size %d,", aux->x_sym.x_misc.x_lnsz.x_size);
				break;
				
			default:
				printf(" (don't know why this symbol has aux entries.)");
				abort();
				break;
			} /* switch on class */
#endif /* OBJ_COFF */
			
#ifndef OBJ_COFF
#else

#endif /* OBJ_BOUT */
			printf(" ]");
		} /* is defined */
#ifndef OBJ_BOUT
#endif /* B_OUT */
#else
			if (sp->n_numaux != auxcountshouldbe) {
				printf(" (expecting %d auxents here)", auxcountshouldbe);
				abort();
			} /* on miscount */
		} /* do aux entries */
		
		i += sp->n_numaux;
		sp += sp->n_numaux;
#endif /* OBJ_COFF */
		
		printf("\n");
	} /* for each symbol */
#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#else
	printf("\n");
#endif /* OBJ_COFF */
	return;
} /* dump_sym() */
#ifndef OBJ_COFF
#else
		printf("\n");
	}
}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#else
#ifdef comment
#endif /* OBJ_COFF */
static void dump_reloc (execp, f)
#ifndef OBJ_COFF
#else
dump_reloc (execp, f)
#endif /* OBJ_BOUT */
struct exec *execp;
#else
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
{
#ifndef OBJ_COFF
	read_symbols (execp, f);
#else
	read_symbols(execp, f);
#endif /* OBJ_COFF */
	if (execp->a_trsize) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		printf("text reloc\n");
#ifndef OBJ_COFF
#else
		printf ("text reloc\n");
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		dump_reloc1 (execp, f, N_TRELOFF (*execp), execp->a_trsize);
	}
	if (execp->a_drsize) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		printf("data reloc\n");
#ifndef OBJ_COFF
#else
		printf ("data reloc\n");
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		dump_reloc1 (execp, f, N_DRELOFF (*execp), execp->a_drsize);
	}
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */

	return;
} /* dump_reloc() */
#ifndef OBJ_COFF
#else
}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
static void dump_reloc1 (execp, f, off, size)
#ifndef OBJ_COFF
#else
dump_reloc1 (execp, f, off, size)
#endif /* OBJ_BOUT */
struct exec *execp;
#else
fileheader *execp;
#endif /* OBJ_COFF */
FILE *f;
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
int off;
int size;
#ifndef OBJ_COFF
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
{
	int nreloc;
	struct relocation_info reloc;
	int i;

	nreloc = size / sizeof (struct relocation_info);

#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
	if (!omit_reloc_numbers_flag) {
		printf("%3s: ", "#");
	} /* if printing numbers */

#ifndef sparc
	printf("%3s ", "len");
#endif /* sparc */

	printf("%8s %4s\n", "adr", "sym");


	fseek(f, off, 0);
#ifndef OBJ_COFF
#else
	printf ("%3s: %3s %8s %4s\n", "#", "len", "adr", "sym");
	fseek (f, off, 0);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
	for (i = 0; i < nreloc; i++) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		if (fread((char *)&reloc, sizeof reloc, 1, f) != 1) {
			fprintf(stderr, "%s: error reading reloc\n",
#ifndef OBJ_COFF
#else
		if (fread ((char *)&reloc, sizeof reloc, 1, f) != 1) {
			fprintf (stderr, "%s: error reading reloc\n",
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
				 program_name);
			return;
		}
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */

		if (!omit_reloc_numbers_flag) {
			printf("%3d: ", i);
		} /* if printing numbers */

#ifndef sparc		
		printf("%3d ", 1 << reloc.r_length);
#endif /* sparc */

		printf("%8lx ", (long unsigned) reloc.r_address);

#ifndef B_OUT
#ifndef OBJ_COFF
#else
		printf ("%3d: %3d %8x ", i, 1 << reloc.r_length,
			reloc.r_address);
		
#ifdef NOT
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		if (reloc.r_extern) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
			if (!omit_sym_numbers_flag) {
				(void) printf("%4d ", reloc.r_symbolnum);
			} else {
				(void) printf("     ");
			} /* if printing sym numbers */

#ifndef OBJ_COFF
#else
			printf ("%4d ", reloc.r_symbolnum);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
			if (reloc.r_symbolnum < nsyms)
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
				printf("%s ", symtbl[reloc.r_symbolnum].n_un.n_name);
#ifndef OBJ_COFF
#else
				printf ("%s ",
					symtbl[reloc.r_symbolnum].n_un.n_name);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
		} else {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
			printf("     ");
#ifndef OBJ_COFF
#else
			printf ("     ");
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
			switch (reloc.r_symbolnum & ~N_EXT) {
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
			case N_TEXT: printf(".text "); break;
			case N_DATA: printf(".data "); break;
			case N_BSS: printf(".bss "); break;
			case N_ABS: printf(".abs "); break;
			default: printf("base %x ", (unsigned) reloc.r_symbolnum); break;
#ifndef OBJ_COFF
#else
			case N_TEXT: printf (".text "); break;
			case N_DATA: printf (".data "); break;
			case N_BSS: printf (".bss "); break;
			case N_ABS: printf (".abs "); break;
			default: printf ("base %x ", reloc.r_symbolnum); break;
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
			}
		}
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
#endif /* not B_OUT */

#ifdef SPARC
		if (reloc.r_addend) printf("+0x%x ", (unsigned) reloc.r_addend);

		switch (reloc.r_type) {
		case RELOC_8: printf("R8 "); break;
		case RELOC_16: printf("R16 "); break;
		case RELOC_32: printf("R32 "); break;
		case RELOC_DISP8: printf("DISP8 "); break;
		case RELOC_DISP16: printf("DISP16 "); break;
		case RELOC_DISP32: printf("DISP32 "); break;
		case RELOC_WDISP30: printf("WDISP30 "); break;
		case RELOC_WDISP22: printf("WDISP22 "); break;
		case RELOC_HI22: printf("HI22 "); break;
		case RELOC_22: printf("R22 "); break;
		case RELOC_13: printf("R13 "); break;
		case RELOC_LO10: printf("LO10 "); break;
		case RELOC_SFA_BASE: printf("SFA_BASE "); break;
		case RELOC_SFA_OFF13: printf("SFA_OFF13 "); break;
		case RELOC_BASE10: printf("BASE10 "); break;
		case RELOC_BASE13: printf("BASE13 "); break;
		case RELOC_BASE22: printf("BASE22 "); break;
		case RELOC_PC10: printf("PC10 "); break;
		case RELOC_PC22: printf("PC22 "); break;
		case RELOC_JMP_TBL: printf("JMP_TBL "); break;
		case RELOC_SEGOFF16: printf("SEGOFF16 "); break;
		case RELOC_GLOB_DAT: printf("GLOB_DAT "); break;
		case RELOC_JMP_SLOT: printf("JMP_SLOT "); break;
		case RELOC_RELATIVE: printf("RELATIVE "); break;
		} /* switch on reloc type */
#else /* SPARC */
		if (reloc.r_pcrel) printf("PCREL ");
#endif /* SPARC */

#ifdef B_OUT
		if (reloc.r_bsr) printf("BSR ");
		if (reloc.r_disp) printf("DISP ");
		if (reloc.r_callj) printf("CALLJ ");
		if (reloc.nuthin) printf("NUTHIN ");
#endif /* B_OUT */

#ifdef SPARC
		{
			struct reloc_info_sparc spare;

			bzero(&spare, sizeof(spare));

			reloc.r_address = 0;
			reloc.r_index = 0;
			reloc.r_extern = 0;
			reloc.r_type = 0;
			reloc.r_addend = 0;

			if (bcmp(&reloc, &spare, sizeof(spare))) {
				printf("(garbage in spare bits) ");
			} /* if garbage in spare bits */
		} /* sparc */
#endif /* SPARC */

#ifndef OBJ_COFF
#else
#endif /* NOT */
		if (reloc.r_pcrel) printf ("PCREL ");
		if (reloc.r_bsr) printf ("BSR ");
		if (reloc.r_disp) printf ("DISP ");
		if (reloc.r_callj) printf ("CALLJ ");
		if (reloc.nuthin) printf ("NUTHIN ");
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
#if 0
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		if (reloc.r_pad) printf("PAD %x ", reloc.r_pad);
#ifndef OBJ_COFF
#else
		if (reloc.r_pad) printf ("PAD %x ", reloc.r_pad);
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
#endif
#ifndef OBJ_COFF
#ifndef OBJ_BOUT
#endif /* OBJ_COFF */
		printf("\n");
	} /* for each reloc record */

	return;
} /* dump_reloc1() */
#ifndef OBJ_COFF
#else
		printf ("\n");
	}
}
#endif /* OBJ_BOUT */
#else
#endif /* comment */
#endif /* OBJ_COFF */

/* Allocate `n' bytes of memory dynamically, with error checking.  */

#ifndef OBJ_COFF
char *
xmalloc (n)
     unsigned n;
{
  char *p;

  p = malloc (n);
  if (p == 0)
    {
#ifndef OBJ_BOUT
      fprintf(stderr, "%s: virtual memory exhausted\n", program_name);
#else
      fprintf (stderr, "%s: virtual memory exhausted\n", program_name);
#endif /* OBJ_BOUT */
      exit (1);
    }
#ifndef OBJ_BOUT
  bzero(p, n);
#endif /* OBJ_BOUT */
  return p;
#ifndef OBJ_BOUT
#else
static char *xmalloc (n)
unsigned n;
{
	char *p;
	
	p = malloc (n);
	if (p == NULL)
	    {
		    fprintf(stderr, "%s: virtual memory exhausted\n", program_name);
		    exit (1);
	    }
	bzero(p, n);
	return p;
#endif /* OBJ_COFF */
} /* xmalloc() */

#ifdef OBJ_COFF
static char *xrealloc(p, size)
char *p;
unsigned size;
{
	p = realloc(p, size);

	if (p == NULL) {
		fprintf(stderr, "%s: virtual memory exhausted\n", program_name);
		exit (1);
	} /* on malloc failure */

	bzero(p, size);
	return(p);
} /* xrealloc() */

#endif /* OBJ_COFF */
/*
 * Local Variables:
 * comment-column: 0
 * fill-column: 131
 * End:
 */

/* end of objdump.c */
#ifndef OBJ_COFF
#else
}
#endif /* OBJ_BOUT */
#endif /* OBJ_COFF */
