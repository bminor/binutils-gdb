/* magic.h - Interface to the General Magic debugger extras */

extern void init_magic();
extern CORE_ADDR deal_with_dispatch(CORE_ADDR pc);
extern struct type *type_of_object(CORE_ADDR object);
extern CORE_ADDR baseptr_of_object(CORE_ADDR object);
