/* xdr_ptrace.h - xdr header for remote ptrace structures */

/*
modification history
--------------------
01a,05jun90,llk  extracted from xdr_ptrace.h.
*/


#include "xdr_regs.h"
#include "reg.h"

/*
 *  Counted byte structure used by READ/WRITE TEXT/DATA
 */
struct c_bytes {
	u_int	len;
	caddr_t	bytes;
};
typedef struct c_bytes C_bytes;

/*
 * enum for discriminated union ptrace_info
 */
enum ptype {
	NOINFO = 0,		/* no additional infomation	*/
	REGS = 1,		/* regs 	(SETREGS)  	*/
	FPREGS = 2,		/* fp_status 	(SETFPREGS)	*/
	FPAREGS = 3,		/* fpa_regs	(SETFPAREGS)	*/
	DATA = 4,		/* c_bytes	(WRITETEXT/DATA)*/
};
typedef enum ptype ptype;

/*
 * discrimnated union for passing additional data to be 
 * written to the debugged process. With the exception of
 * c_bytes, the structures are defined in <machine/reg.h>
 */
struct ptrace_info {
	ptype	ttype;
	caddr_t	more_data;	
};
typedef struct ptrace_info Ptrace_info;

/*
 * structure passed to server on all remote ptrace calls
 */
struct rptrace {
	int 	pid;
	int 	data;
	int 	addr;	/* FIX! this really should be caddr_t or something */
	Ptrace_info	info;
};
typedef struct rptrace Rptrace;
/*
 * structure returned by server on all remote ptrace calls
 */
struct ptrace_return {
	int status;
	int errno;
	Ptrace_info	info;
};
typedef struct ptrace_return Ptrace_return;

bool_t xdr_c_bytes();
bool_t xdr_ptrace_info();
bool_t xdr_rptrace();
bool_t xdr_ptrace_return();
