#if !defined(_MODULE_H)
#define _MODULE_H

/* OS-9000 i386 module header definitions */
#define _MPF386

#if 0
/* These have name conflicts with Mach and don't appear to be used.  */
typedef unsigned char u_int16[2];
typedef unsigned char u_int32[4];
typedef unsigned char owner_id[4];
#endif

/* sizeof common header less parity field */
#define N_M_PARITY  (sizeof(mh_com)-sizeof(unisgned short))
#define OLD_M_PARITY 46
#define M_PARITY N_M_PARITY

#ifdef _MPF68K
#define MODSYNC 0x4afd      /* module header sync code for 680x0 processors */
#endif

#ifdef _MPF386
#define MODSYNC 0x4afc      /* module header sync code for 80386 processors */
#endif

#define MODREV	1			/* module format revision 1 */
#define CRCCON	0x800fe3	/* crc polynomial constant */

/* Module access permission values */
#define MP_OWNER_READ	0x0001
#define MP_OWNER_WRITE	0x0002
#define MP_OWNER_EXEC	0x0004
#define MP_GROUP_READ	0x0010
#define MP_GROUP_WRITE	0x0020
#define MP_GROUP_EXEC	0x0040
#define MP_WORLD_READ	0x0100
#define MP_WORLD_WRITE	0x0200
#define MP_WORLD_EXEC	0x0400
#define MP_WORLD_ACCESS	0x0777
#define MP_OWNER_MASK	0x000f
#define MP_GROUP_MASK	0x00f0
#define MP_WORLD_MASK	0x0f00
#define MP_SYSTM_MASK	0xf000

/* Module Type/Language values */
#define MT_ANY		0
#define MT_PROGRAM	0x0001
#define MT_SUBROUT	0x0002
#define MT_MULTI	0x0003
#define MT_DATA		0x0004
#define MT_TRAPLIB	0x000b
#define MT_SYSTEM	0x000c
#define MT_FILEMAN	0x000d
#define MT_DEVDRVR	0x000e 
#define MT_DEVDESC	0x000f
#define MT_MASK		0xff00

#define ML_ANY		0
#define ML_OBJECT	1
#define ML_ICODE	2
#define ML_PCODE	3
#define ML_CCODE	4
#define ML_CBLCODE	5
#define ML_FRTNCODE	6
#define ML_MASK		0x00ff

#define mktypelang(type,lang)	(((type)<<8)|(lang))

/* Module Attribute values */
#define MA_REENT	0x80
#define MA_GHOST	0x40
#define MA_SUPER	0x20
#define MA_MASK		0xff00
#define MR_MASK		0x00ff

#define mkattrevs(attr, revs)	(((attr)<<8)|(revs))

#define m_user m_owner.grp_usr.usr
#define m_group m_owner.grp_usr.grp
#define m_group_user m_owner.group_user

/* macro definitions for accessing module header fields */
#define MODNAME(mod) ((u_char*)((u_char*)mod + ((Mh_com)mod)->m_name))
#define MODSIZE(mod) ((u_int32)((Mh_com)mod)->m_size)
#define MHCOM_BYTES_SIZE 80
#define N_BADMAG(a) (((a).a_info) != MODSYNC)

#if 0
/* These don't appear to be used.  */
typedef struct mh_com {
	u_int16
		m_sync,			/* sync bytes ($4afc)								*/
		m_sysrev;		/* system revision check value						*/
	u_int32
		m_size;			/* module size										*/
	owner_id
		m_owner;		/* group/user id									*/
	u_int32
		m_name;			/* offset to module name							*/
	u_int16
		m_access,		/* access permissions								*/
		m_tylan,		/* type/lang										*/
		m_attrev,		/* rev/attr											*/
		m_edit;			/* edition											*/
	u_int32
		m_needs,		/* module hardware requirements flags. (reserved)	*/
		m_usage,		/* comment string offset							*/
		m_symbol,		/* symbol table offset								*/
		m_exec,			/* offset to execution entry point					*/
		m_excpt,		/* offset to exception entry point					*/
		m_data,			/* data storage requirement							*/
		m_stack,		/* stack size										*/
		m_idata,		/* offset to initialized data						*/
		m_idref,		/* offset to data reference lists					*/
		m_init,			/* initialization routine offset					*/
		m_term;			/* termination routine offset						*/
	u_int16
		m_ident;		/* ident code for ident program						*/
	char
		m_spare[8];	/* reserved bytes									*/
	u_int16
		m_parity; 		/* header parity									*/
} mh_com,*Mh_com;

/* Executable memory module */
typedef mh_com *Mh_exec,mh_exec;

/* Data memory module */
typedef mh_com *Mh_data,mh_data;

/* File manager memory module */
typedef mh_com *Mh_fman,mh_fman;

/* device driver module */
typedef mh_com *Mh_drvr,mh_drvr;

/* trap handler module */
typedef	mh_com mh_trap, *Mh_trap;

/* Device descriptor module */
typedef	mh_com *Mh_dev,mh_dev;

/* Configuration module */
typedef mh_com *Mh_config, mh_config;

#endif

#if 0 

#if !defined(_MODDIR_H)
/* go get _os_fmod (and others) */
#include <moddir.h>
#endif

error_code _os_crc(void *, u_int32, int *);
error_code _os_datmod(char *, u_int32, u_int16 *, u_int16 *, u_int32, void **, mh_data **);
error_code _os_get_moddir(void *, u_int32 *);
error_code _os_initdata(mh_com *, void *);
error_code _os_link(char **, mh_com **, void **, u_int16 *, u_int16 *);
error_code _os_linkm(mh_com *, void **, u_int16 *, u_int16 *);
error_code _os_load(char *, mh_com **, void **, u_int32, u_int16 *, u_int16 *, u_int32);
error_code _os_mkmodule(char *, u_int32, u_int16 *, u_int16 *, u_int32, void **, mh_com **, u_int32);
error_code _os_modaddr(void *, mh_com **);
error_code _os_setcrc(mh_com *);
error_code _os_slink(u_int32, char *, void **, void **, mh_com **);
error_code _os_slinkm(u_int32, mh_com *, void **, void **);
error_code _os_unlink(mh_com *);
error_code _os_unload(char *, u_int32);
error_code _os_tlink(u_int32, char *, void **, mh_trap **, void *, u_int32);
error_code _os_tlinkm(u_int32, mh_com *, void **, void *, u_int32);
error_code _os_iodel(mh_com *);
error_code _os_vmodul(mh_com *, mh_com *, u_int32);
#endif /* 0 */

#endif
