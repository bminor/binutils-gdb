/* magic.h - Interface to the General Magic debugger extras */

/* object framework definitions, from FrameworkDefines.asm.h and Generic.h */

#define	kIDBitObject		31	/* set for valid object IDs */
#define	kIDBitUsable		26	/* set if objectID is usable */

/* Return nonzero if <object> is a valid ObjectID */

#define IsObjectID(object)	(((unsigned long)(object) & 0x80000000) != 0)

#define kSizeOfObjectHeader	12	/* size of object header in bytes */

/* public interfaces */

extern void
init_magic PARAMS ((void));

extern void
magic_create_inferior_hook PARAMS ((void));

extern struct type *
type_of_object PARAMS ((CORE_ADDR object));

extern CORE_ADDR
baseptr_of_object PARAMS ((CORE_ADDR object));

extern int
magic_in_dispatcher PARAMS ((CORE_ADDR stop_pc, char *name));

extern CORE_ADDR
magic_skip_dispatcher PARAMS ((CORE_ADDR stop_pc));
