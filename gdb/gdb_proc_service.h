typedef enum {
  PS_OK,          /* generic "call succeeded" */
  PS_ERR,         /* generic. */
  PS_BADPID,      /* bad process handle */
  PS_BADLID,      /* bad lwp identifier */
  PS_BADADDR,     /* bad address */
  PS_NOSYM,       /* p_lookup() could not find given symbol */
        PS_NOFREGS
  /*
   * FPU register set not available for given
   * lwp
   */
}       ps_err_e;

typedef unsigned int  lwpid_t;
typedef unsigned long paddr_t;
typedef unsigned long psaddr_t;


typedef gregset_t  prgregset_t;		/* BOGUS BOGUS BOGUS */
typedef fpregset_t prfpregset_t;	/* BOGUS BOGUS BOGUS */


struct ps_prochandle;		/* user defined. */
