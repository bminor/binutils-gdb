/*	@(#)fcntl.h 1.3 86/07/16 SMI; from UCB 5.1 85/05/30	*/
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef __FCNTL_HEADER__
#define __FCNTL_HEADER__

/*
 * Flag values accessible to open(2) and fcntl(2)
 *  (The first three can only be set by open)
 */
#define	O_RDONLY	0
#define	O_WRONLY	1
#define	O_RDWR		2
#define	O_NDELAY	FNDELAY	/* Non-blocking I/O */
#define	O_APPEND	FAPPEND	/* append (writes guaranteed at the end) */
#define	O_CREAT		FCREAT	/* open with file create */
#define	O_TRUNC		FTRUNC	/* open with truncation */
#define	O_EXCL		FEXCL	/* error on create if file exists */

/* flags for F_GETFL, F_SETFL-- needed by <sys/file.h> */
#define	FNDELAY		00004		/* non-blocking reads */
#define	FAPPEND		00010		/* append on each write */
#define	FASYNC		00100		/* signal pgrp when data ready */
#define	FCREAT		01000		/* create if nonexistant */
#define	FTRUNC		02000		/* truncate to zero length */
#define	FEXCL		04000		/* error if already created */

/* fcntl(2) requests */
#define	F_DUPFD	0	/* Duplicate fildes */
#define	F_GETFD	1	/* Get fildes flags */
#define	F_SETFD	2	/* Set fildes flags */
#define	F_GETFL	3	/* Get file flags */
#define	F_SETFL	4	/* Set file flags */
#define	F_GETOWN 5	/* Get owner */
#define F_SETOWN 6	/* Set owner */
#define F_GETLK  7      /* Get record-locking information */
#define F_SETLK  8      /* Set or Clear a record-lock (Non-Blocking) */
#define F_SETLKW 9      /* Set or Clear a record-lock (Blocking) */

/* access(2) requests */
#define	F_OK		0	/* does file exist */
#define	X_OK		1	/* is it executable by caller */
#define	W_OK		2	/* writable by caller */
#define	R_OK		4	/* readable by caller */

/* System-V record-locking options */
/* lockf(2) requests */
#define F_ULOCK 0       /* Unlock a previously locked region */
#define F_LOCK  1       /* Lock a region for exclusive use */ 
#define F_TLOCK 2       /* Test and lock a region for exclusive use */
#define F_TEST  3       /* Test a region for other processes locks */

/* fcntl(2) flags (l_type field of flock structure) */
#define F_RDLCK 1       /* read lock */
#define F_WRLCK 2       /* write lock */
#define F_UNLCK 3       /* remove lock(s) */


/* file segment locking set data type - information passed to system by user */
struct flock {
        short   l_type;		/* F_RDLCK, F_WRLCK, or F_UNLCK */
        short   l_whence;	/* flag to choose starting offset */
        long    l_start;	/* relative offset, in bytes */
        long    l_len;          /* length, in bytes; 0 means lock to EOF */
        short   l_pid;		/* returned with F_GETLK */
        short   l_xxx;		/* reserved for future use */
};

#endif !__FCNTL_HEADER__
