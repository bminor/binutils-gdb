#define BFD 1


#include <bfd.h>

typedef struct 
{
asymbol sy;
int seg;
} obj_symbol_type;

#define S_GET_NAME(s) (((s)->sy_symbol.sy.name))

typedef struct {
int x;
}
object_headers;

#define DEFAULT_MAGIC_NUMBER_FOR_OBJECT_FILE 1


int lineno_rootP;


#define IEEE_STYLE

/* end of obj-ieee.h */
