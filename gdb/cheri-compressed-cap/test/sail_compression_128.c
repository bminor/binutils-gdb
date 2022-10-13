#include "sail.h"
#include "rts.h"
#include "elf.h"

// enum write_kind
enum zwrite_kind { zWrite_plain, zWrite_conditional, zWrite_release, zWrite_exclusive, zWrite_exclusive_release, zWrite_RISCV_release, zWrite_RISCV_strong_release, zWrite_RISCV_conditional, zWrite_RISCV_conditional_release, zWrite_RISCV_conditional_strong_release, zWrite_X86_locked };

static bool eq_zwrite_kind(enum zwrite_kind op1, enum zwrite_kind op2) { return op1 == op2; }

static enum zwrite_kind UNDEFINED(zwrite_kind)(unit u) { return zWrite_plain; }

// enum trans_kind
enum ztrans_kind { zTransaction_start, zTransaction_commit, zTransaction_abort };

static bool eq_ztrans_kind(enum ztrans_kind op1, enum ztrans_kind op2) { return op1 == op2; }

static enum ztrans_kind UNDEFINED(ztrans_kind)(unit u) { return zTransaction_start; }

// struct tuple_(%string, %i)
struct ztuple_z8z5stringzCz0z5iz9 {
  sail_string ztup0;
  sail_int ztup1;
};

static void COPY(ztuple_z8z5stringzCz0z5iz9)(struct ztuple_z8z5stringzCz0z5iz9 *rop, const struct ztuple_z8z5stringzCz0z5iz9 op) {
  COPY(sail_string)(&rop->ztup0, op.ztup0);
  COPY(sail_int)(&rop->ztup1, op.ztup1);
}

static void CREATE(ztuple_z8z5stringzCz0z5iz9)(struct ztuple_z8z5stringzCz0z5iz9 *op) {
  CREATE(sail_string)(&op->ztup0);
  CREATE(sail_int)(&op->ztup1);
}

static void RECREATE(ztuple_z8z5stringzCz0z5iz9)(struct ztuple_z8z5stringzCz0z5iz9 *op) {
  RECREATE(sail_string)(&op->ztup0);
  RECREATE(sail_int)(&op->ztup1);
}

static void KILL(ztuple_z8z5stringzCz0z5iz9)(struct ztuple_z8z5stringzCz0z5iz9 *op) {
  KILL(sail_string)(&op->ztup0);
  KILL(sail_int)(&op->ztup1);
}

static bool EQUAL(ztuple_z8z5stringzCz0z5iz9)(struct ztuple_z8z5stringzCz0z5iz9 op1, struct ztuple_z8z5stringzCz0z5iz9 op2) {
  return EQUAL(sail_string)(op1.ztup0, op2.ztup0) && EQUAL(sail_int)(op1.ztup1, op2.ztup1);
}


// struct tuple_(%string, %i, %i)
struct ztuple_z8z5stringzCz0z5izCz0z5iz9 {
  sail_string ztup0;
  sail_int ztup1;
  sail_int ztup2;
};

static void COPY(ztuple_z8z5stringzCz0z5izCz0z5iz9)(struct ztuple_z8z5stringzCz0z5izCz0z5iz9 *rop, const struct ztuple_z8z5stringzCz0z5izCz0z5iz9 op) {
  COPY(sail_string)(&rop->ztup0, op.ztup0);
  COPY(sail_int)(&rop->ztup1, op.ztup1);
  COPY(sail_int)(&rop->ztup2, op.ztup2);
}

static void CREATE(ztuple_z8z5stringzCz0z5izCz0z5iz9)(struct ztuple_z8z5stringzCz0z5izCz0z5iz9 *op) {
  CREATE(sail_string)(&op->ztup0);
  CREATE(sail_int)(&op->ztup1);
  CREATE(sail_int)(&op->ztup2);
}

static void RECREATE(ztuple_z8z5stringzCz0z5izCz0z5iz9)(struct ztuple_z8z5stringzCz0z5izCz0z5iz9 *op) {
  RECREATE(sail_string)(&op->ztup0);
  RECREATE(sail_int)(&op->ztup1);
  RECREATE(sail_int)(&op->ztup2);
}

static void KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(struct ztuple_z8z5stringzCz0z5izCz0z5iz9 *op) {
  KILL(sail_string)(&op->ztup0);
  KILL(sail_int)(&op->ztup1);
  KILL(sail_int)(&op->ztup2);
}

static bool EQUAL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(struct ztuple_z8z5stringzCz0z5izCz0z5iz9 op1, struct ztuple_z8z5stringzCz0z5izCz0z5iz9 op2) {
  return EQUAL(sail_string)(op1.ztup0, op2.ztup0) && EQUAL(sail_int)(op1.ztup1, op2.ztup1) && EQUAL(sail_int)(op1.ztup2, op2.ztup2);
}


// struct tuple_(%string, %string)
struct ztuple_z8z5stringzCz0z5stringz9 {
  sail_string ztup0;
  sail_string ztup1;
};

static void COPY(ztuple_z8z5stringzCz0z5stringz9)(struct ztuple_z8z5stringzCz0z5stringz9 *rop, const struct ztuple_z8z5stringzCz0z5stringz9 op) {
  COPY(sail_string)(&rop->ztup0, op.ztup0);
  COPY(sail_string)(&rop->ztup1, op.ztup1);
}

static void CREATE(ztuple_z8z5stringzCz0z5stringz9)(struct ztuple_z8z5stringzCz0z5stringz9 *op) {
  CREATE(sail_string)(&op->ztup0);
  CREATE(sail_string)(&op->ztup1);
}

static void RECREATE(ztuple_z8z5stringzCz0z5stringz9)(struct ztuple_z8z5stringzCz0z5stringz9 *op) {
  RECREATE(sail_string)(&op->ztup0);
  RECREATE(sail_string)(&op->ztup1);
}

static void KILL(ztuple_z8z5stringzCz0z5stringz9)(struct ztuple_z8z5stringzCz0z5stringz9 *op) {
  KILL(sail_string)(&op->ztup0);
  KILL(sail_string)(&op->ztup1);
}

static bool EQUAL(ztuple_z8z5stringzCz0z5stringz9)(struct ztuple_z8z5stringzCz0z5stringz9 op1, struct ztuple_z8z5stringzCz0z5stringz9 op2) {
  return EQUAL(sail_string)(op1.ztup0, op2.ztup0) && EQUAL(sail_string)(op1.ztup1, op2.ztup1);
}

// union regfp
enum kind_zregfp { Kind_zRField, Kind_zRFull, Kind_zRSlice, Kind_zRSliceBit };

struct zregfp {
  enum kind_zregfp kind;
  union {
    struct { struct ztuple_z8z5stringzCz0z5stringz9 zRField; };
    struct { sail_string zRFull; };
    struct { struct ztuple_z8z5stringzCz0z5izCz0z5iz9 zRSlice; };
    struct { struct ztuple_z8z5stringzCz0z5iz9 zRSliceBit; };
  };
};

static void CREATE(zregfp)(struct zregfp *op)
{
  op->kind = Kind_zRField;
  CREATE(ztuple_z8z5stringzCz0z5stringz9)(&op->zRField);
}

static void RECREATE(zregfp)(struct zregfp *op) {}

static void KILL(zregfp)(struct zregfp *op)
{
  if (op->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&op->zRField);
  } else if (op->kind == Kind_zRFull) {
    KILL(sail_string)(&op->zRFull);
  } else if (op->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&op->zRSlice);
  } else if (op->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&op->zRSliceBit);
  };
}

static void COPY(zregfp)(struct zregfp *rop, struct zregfp op)
{
  if (rop->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  } else if (rop->kind == Kind_zRFull) {
    KILL(sail_string)(&rop->zRFull);
  } else if (rop->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  } else if (rop->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zRField) {
  
  CREATE(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField); COPY(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField, op.zRField);
  } else if (op.kind == Kind_zRFull) {
    CREATE(sail_string)(&rop->zRFull); COPY(sail_string)(&rop->zRFull, op.zRFull);
  } else if (op.kind == Kind_zRSlice) {
  
  CREATE(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice); COPY(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice, op.zRSlice);
  } else if (op.kind == Kind_zRSliceBit){
  
  CREATE(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit); COPY(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit, op.zRSliceBit);
  }
}

static bool EQUAL(zregfp)(struct zregfp op1, struct zregfp op2) {
  if (op1.kind == Kind_zRField && op2.kind == Kind_zRField) {
    return EQUAL(ztuple_z8z5stringzCz0z5stringz9)(op1.zRField, op2.zRField);
  } else if (op1.kind == Kind_zRFull && op2.kind == Kind_zRFull) {
    return EQUAL(sail_string)(op1.zRFull, op2.zRFull);
  } else if (op1.kind == Kind_zRSlice && op2.kind == Kind_zRSlice) {
    return EQUAL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(op1.zRSlice, op2.zRSlice);
  } else if (op1.kind == Kind_zRSliceBit && op2.kind == Kind_zRSliceBit) {
    return EQUAL(ztuple_z8z5stringzCz0z5iz9)(op1.zRSliceBit, op2.zRSliceBit);
  } else return false;
}

static void sailgen_RField(struct zregfp *rop, struct ztuple_z8z5stringzCz0z5stringz9 op)
{
  if (rop->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  } else if (rop->kind == Kind_zRFull) {
    KILL(sail_string)(&rop->zRFull);
  } else if (rop->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  } else if (rop->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  }
  rop->kind = Kind_zRField;
  CREATE(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  COPY(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField, op);
  
}

static void sailgen_RFull(struct zregfp *rop, sail_string op)
{
  if (rop->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  } else if (rop->kind == Kind_zRFull) {
    KILL(sail_string)(&rop->zRFull);
  } else if (rop->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  } else if (rop->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  }
  rop->kind = Kind_zRFull;
  CREATE(sail_string)(&rop->zRFull);
  COPY(sail_string)(&rop->zRFull, op);
  
}

static void sailgen_RSlice(struct zregfp *rop, struct ztuple_z8z5stringzCz0z5izCz0z5iz9 op)
{
  if (rop->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  } else if (rop->kind == Kind_zRFull) {
    KILL(sail_string)(&rop->zRFull);
  } else if (rop->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  } else if (rop->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  }
  rop->kind = Kind_zRSlice;
  CREATE(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  COPY(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice, op);
  
}

static void sailgen_RSliceBit(struct zregfp *rop, struct ztuple_z8z5stringzCz0z5iz9 op)
{
  if (rop->kind == Kind_zRField) {
    KILL(ztuple_z8z5stringzCz0z5stringz9)(&rop->zRField);
  } else if (rop->kind == Kind_zRFull) {
    KILL(sail_string)(&rop->zRFull);
  } else if (rop->kind == Kind_zRSlice) {
    KILL(ztuple_z8z5stringzCz0z5izCz0z5iz9)(&rop->zRSlice);
  } else if (rop->kind == Kind_zRSliceBit){
    KILL(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  }
  rop->kind = Kind_zRSliceBit;
  CREATE(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit);
  COPY(ztuple_z8z5stringzCz0z5iz9)(&rop->zRSliceBit, op);
  
}

// enum read_kind
enum zread_kind { zRead_plain, zRead_reserve, zRead_acquire, zRead_exclusive, zRead_exclusive_acquire, zRead_stream, zRead_RISCV_acquire, zRead_RISCV_strong_acquire, zRead_RISCV_reserved, zRead_RISCV_reserved_acquire, zRead_RISCV_reserved_strong_acquire, zRead_X86_locked };

static bool eq_zread_kind(enum zread_kind op1, enum zread_kind op2) { return op1 == op2; }

static enum zread_kind UNDEFINED(zread_kind)(unit u) { return zRead_plain; }

// union option
enum kind_zoption { Kind_zNone };

struct zoption {
  enum kind_zoption kind;
  union {struct { unit zNone; };};
};

static void CREATE(zoption)(struct zoption *op)
{
  op->kind = Kind_zNone;
  
}

static void RECREATE(zoption)(struct zoption *op) {}

static void KILL(zoption)(struct zoption *op)
{
  if (op->kind == Kind_zNone){
    /* do nothing */
  };
}

static void COPY(zoption)(struct zoption *rop, struct zoption op)
{
  if (rop->kind == Kind_zNone){
    /* do nothing */
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zNone){
    rop->zNone = op.zNone;
  }
}

static bool EQUAL(zoption)(struct zoption op1, struct zoption op2) {
  if (op1.kind == Kind_zNone && op2.kind == Kind_zNone) {
    return EQUAL(unit)(op1.zNone, op2.zNone);
  } else return false;
}

static void sailgen_None(struct zoption *rop, unit op)
{
  if (rop->kind == Kind_zNone){
    /* do nothing */
  }
  rop->kind = Kind_zNone;
  rop->zNone = op;
}

// union niafp
enum kind_zniafp { Kind_zNIAFP_concrete_address, Kind_zNIAFP_indirect_address, Kind_zNIAFP_successor };

struct zniafp {
  enum kind_zniafp kind;
  union {
    struct { uint64_t zNIAFP_concrete_address; };
    struct { unit zNIAFP_indirect_address; };
    struct { unit zNIAFP_successor; };
  };
};

static void CREATE(zniafp)(struct zniafp *op)
{
  op->kind = Kind_zNIAFP_concrete_address;
  
}

static void RECREATE(zniafp)(struct zniafp *op) {}

static void KILL(zniafp)(struct zniafp *op)
{
  if (op->kind == Kind_zNIAFP_concrete_address) {
    /* do nothing */
  } else if (op->kind == Kind_zNIAFP_indirect_address) {
    /* do nothing */
  } else if (op->kind == Kind_zNIAFP_successor){
    /* do nothing */
  };
}

static void COPY(zniafp)(struct zniafp *rop, struct zniafp op)
{
  if (rop->kind == Kind_zNIAFP_concrete_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_indirect_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_successor){
    /* do nothing */
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zNIAFP_concrete_address) {
    rop->zNIAFP_concrete_address = op.zNIAFP_concrete_address;
  } else if (op.kind == Kind_zNIAFP_indirect_address) {
    rop->zNIAFP_indirect_address = op.zNIAFP_indirect_address;
  } else if (op.kind == Kind_zNIAFP_successor){
    rop->zNIAFP_successor = op.zNIAFP_successor;
  }
}

static bool EQUAL(zniafp)(struct zniafp op1, struct zniafp op2) {
  if (op1.kind == Kind_zNIAFP_concrete_address && op2.kind == Kind_zNIAFP_concrete_address) {
    return EQUAL(fbits)(op1.zNIAFP_concrete_address, op2.zNIAFP_concrete_address);
  } else if (op1.kind == Kind_zNIAFP_indirect_address && op2.kind == Kind_zNIAFP_indirect_address) {
    return EQUAL(unit)(op1.zNIAFP_indirect_address, op2.zNIAFP_indirect_address);
  } else if (op1.kind == Kind_zNIAFP_successor && op2.kind == Kind_zNIAFP_successor) {
    return EQUAL(unit)(op1.zNIAFP_successor, op2.zNIAFP_successor);
  } else return false;
}

static void sailgen_NIAFP_concrete_address(struct zniafp *rop, uint64_t op)
{
  if (rop->kind == Kind_zNIAFP_concrete_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_indirect_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_successor){
    /* do nothing */
  }
  rop->kind = Kind_zNIAFP_concrete_address;
  rop->zNIAFP_concrete_address = op;
}

static void sailgen_NIAFP_indirect_address(struct zniafp *rop, unit op)
{
  if (rop->kind == Kind_zNIAFP_concrete_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_indirect_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_successor){
    /* do nothing */
  }
  rop->kind = Kind_zNIAFP_indirect_address;
  rop->zNIAFP_indirect_address = op;
}

static void sailgen_NIAFP_successor(struct zniafp *rop, unit op)
{
  if (rop->kind == Kind_zNIAFP_concrete_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_indirect_address) {
    /* do nothing */
  } else if (rop->kind == Kind_zNIAFP_successor){
    /* do nothing */
  }
  rop->kind = Kind_zNIAFP_successor;
  rop->zNIAFP_successor = op;
}



// union diafp
enum kind_zdiafp { Kind_zDIAFP_concrete, Kind_zDIAFP_none, Kind_zDIAFP_reg };

struct zdiafp {
  enum kind_zdiafp kind;
  union {
    struct { uint64_t zDIAFP_concrete; };
    struct { unit zDIAFP_none; };
    struct { struct zregfp zDIAFP_reg; };
  };
};

static void CREATE(zdiafp)(struct zdiafp *op)
{
  op->kind = Kind_zDIAFP_concrete;
  
}

static void RECREATE(zdiafp)(struct zdiafp *op) {}

static void KILL(zdiafp)(struct zdiafp *op)
{
  if (op->kind == Kind_zDIAFP_concrete) {
    /* do nothing */
  } else if (op->kind == Kind_zDIAFP_none) {
    /* do nothing */
  } else if (op->kind == Kind_zDIAFP_reg){
    KILL(zregfp)(&op->zDIAFP_reg);
  };
}

static void COPY(zdiafp)(struct zdiafp *rop, struct zdiafp op)
{
  if (rop->kind == Kind_zDIAFP_concrete) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_none) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_reg){
    KILL(zregfp)(&rop->zDIAFP_reg);
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zDIAFP_concrete) {
    rop->zDIAFP_concrete = op.zDIAFP_concrete;
  } else if (op.kind == Kind_zDIAFP_none) {
    rop->zDIAFP_none = op.zDIAFP_none;
  } else if (op.kind == Kind_zDIAFP_reg){
    CREATE(zregfp)(&rop->zDIAFP_reg); COPY(zregfp)(&rop->zDIAFP_reg, op.zDIAFP_reg);
  }
}

static bool EQUAL(zdiafp)(struct zdiafp op1, struct zdiafp op2) {
  if (op1.kind == Kind_zDIAFP_concrete && op2.kind == Kind_zDIAFP_concrete) {
    return EQUAL(fbits)(op1.zDIAFP_concrete, op2.zDIAFP_concrete);
  } else if (op1.kind == Kind_zDIAFP_none && op2.kind == Kind_zDIAFP_none) {
    return EQUAL(unit)(op1.zDIAFP_none, op2.zDIAFP_none);
  } else if (op1.kind == Kind_zDIAFP_reg && op2.kind == Kind_zDIAFP_reg) {
    return EQUAL(zregfp)(op1.zDIAFP_reg, op2.zDIAFP_reg);
  } else return false;
}

static void sailgen_DIAFP_concrete(struct zdiafp *rop, uint64_t op)
{
  if (rop->kind == Kind_zDIAFP_concrete) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_none) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_reg){
    KILL(zregfp)(&rop->zDIAFP_reg);
  }
  rop->kind = Kind_zDIAFP_concrete;
  rop->zDIAFP_concrete = op;
}

static void sailgen_DIAFP_none(struct zdiafp *rop, unit op)
{
  if (rop->kind == Kind_zDIAFP_concrete) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_none) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_reg){
    KILL(zregfp)(&rop->zDIAFP_reg);
  }
  rop->kind = Kind_zDIAFP_none;
  rop->zDIAFP_none = op;
}

static void sailgen_DIAFP_reg(struct zdiafp *rop, struct zregfp op)
{
  if (rop->kind == Kind_zDIAFP_concrete) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_none) {
    /* do nothing */
  } else if (rop->kind == Kind_zDIAFP_reg){
    KILL(zregfp)(&rop->zDIAFP_reg);
  }
  rop->kind = Kind_zDIAFP_reg;
  CREATE(zregfp)(&rop->zDIAFP_reg);
  COPY(zregfp)(&rop->zDIAFP_reg, op);
  
}

// enum cache_op_kind
enum zcache_op_kind { zCache_op_D_IVAC, zCache_op_D_ISW, zCache_op_D_CSW, zCache_op_D_CISW, zCache_op_D_ZVA, zCache_op_D_CVAC, zCache_op_D_CVAU, zCache_op_D_CIVAC, zCache_op_I_IALLUIS, zCache_op_I_IALLU, zCache_op_I_IVAU };

static bool eq_zcache_op_kind(enum zcache_op_kind op1, enum zcache_op_kind op2) { return op1 == op2; }

static enum zcache_op_kind UNDEFINED(zcache_op_kind)(unit u) { return zCache_op_D_IVAC; }

// enum a64_barrier_type
enum za64_barrier_type { zA64_barrier_all, zA64_barrier_LD, zA64_barrier_ST };

static bool eq_za64_barrier_type(enum za64_barrier_type op1, enum za64_barrier_type op2) { return op1 == op2; }

static enum za64_barrier_type UNDEFINED(za64_barrier_type)(unit u) { return zA64_barrier_all; }

// enum a64_barrier_domain
enum za64_barrier_domain { zA64_FullShare, zA64_InnerShare, zA64_OuterShare, zA64_NonShare };

static bool eq_za64_barrier_domain(enum za64_barrier_domain op1, enum za64_barrier_domain op2) { return op1 == op2; }

static enum za64_barrier_domain UNDEFINED(za64_barrier_domain)(unit u) { return zA64_FullShare; }

// struct tuple_(%enum za64_barrier_domain, %enum za64_barrier_type)
struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 {
  enum za64_barrier_domain ztup0;
  enum za64_barrier_type ztup1;
};

static void COPY(ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9)(struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 *rop, const struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 op) {
  rop->ztup0 = op.ztup0;
  rop->ztup1 = op.ztup1;
}

static bool EQUAL(ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9)(struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 op1, struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 op2) {
  return EQUAL(za64_barrier_domain)(op1.ztup0, op2.ztup0) && EQUAL(za64_barrier_type)(op1.ztup1, op2.ztup1);
}

// union barrier_kind
enum kind_zbarrier_kind { Kind_zBarrier_DMB, Kind_zBarrier_DSB, Kind_zBarrier_Eieio, Kind_zBarrier_ISB, Kind_zBarrier_Isync, Kind_zBarrier_LwSync, Kind_zBarrier_MIPS_SYNC, Kind_zBarrier_RISCV_i, Kind_zBarrier_RISCV_r_r, Kind_zBarrier_RISCV_r_rw, Kind_zBarrier_RISCV_r_w, Kind_zBarrier_RISCV_rw_r, Kind_zBarrier_RISCV_rw_rw, Kind_zBarrier_RISCV_rw_w, Kind_zBarrier_RISCV_tso, Kind_zBarrier_RISCV_w_r, Kind_zBarrier_RISCV_w_rw, Kind_zBarrier_RISCV_w_w, Kind_zBarrier_Sync, Kind_zBarrier_x86_MFENCE };

struct zbarrier_kind {
  enum kind_zbarrier_kind kind;
  union {
    struct { struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 zBarrier_DMB; };
    struct { struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 zBarrier_DSB; };
    struct { unit zBarrier_Eieio; };
    struct { unit zBarrier_ISB; };
    struct { unit zBarrier_Isync; };
    struct { unit zBarrier_LwSync; };
    struct { unit zBarrier_MIPS_SYNC; };
    struct { unit zBarrier_RISCV_i; };
    struct { unit zBarrier_RISCV_r_r; };
    struct { unit zBarrier_RISCV_r_rw; };
    struct { unit zBarrier_RISCV_r_w; };
    struct { unit zBarrier_RISCV_rw_r; };
    struct { unit zBarrier_RISCV_rw_rw; };
    struct { unit zBarrier_RISCV_rw_w; };
    struct { unit zBarrier_RISCV_tso; };
    struct { unit zBarrier_RISCV_w_r; };
    struct { unit zBarrier_RISCV_w_rw; };
    struct { unit zBarrier_RISCV_w_w; };
    struct { unit zBarrier_Sync; };
    struct { unit zBarrier_x86_MFENCE; };
  };
};

static void CREATE(zbarrier_kind)(struct zbarrier_kind *op)
{
  op->kind = Kind_zBarrier_DMB;
  
}

static void RECREATE(zbarrier_kind)(struct zbarrier_kind *op) {}

static void KILL(zbarrier_kind)(struct zbarrier_kind *op)
{
  if (op->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (op->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  };
}

static void COPY(zbarrier_kind)(struct zbarrier_kind *rop, struct zbarrier_kind op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zBarrier_DMB) {
    rop->zBarrier_DMB = op.zBarrier_DMB;
  } else if (op.kind == Kind_zBarrier_DSB) {
    rop->zBarrier_DSB = op.zBarrier_DSB;
  } else if (op.kind == Kind_zBarrier_Eieio) {
    rop->zBarrier_Eieio = op.zBarrier_Eieio;
  } else if (op.kind == Kind_zBarrier_ISB) {
    rop->zBarrier_ISB = op.zBarrier_ISB;
  } else if (op.kind == Kind_zBarrier_Isync) {
    rop->zBarrier_Isync = op.zBarrier_Isync;
  } else if (op.kind == Kind_zBarrier_LwSync) {
    rop->zBarrier_LwSync = op.zBarrier_LwSync;
  } else if (op.kind == Kind_zBarrier_MIPS_SYNC) {
    rop->zBarrier_MIPS_SYNC = op.zBarrier_MIPS_SYNC;
  } else if (op.kind == Kind_zBarrier_RISCV_i) {
    rop->zBarrier_RISCV_i = op.zBarrier_RISCV_i;
  } else if (op.kind == Kind_zBarrier_RISCV_r_r) {
    rop->zBarrier_RISCV_r_r = op.zBarrier_RISCV_r_r;
  } else if (op.kind == Kind_zBarrier_RISCV_r_rw) {
    rop->zBarrier_RISCV_r_rw = op.zBarrier_RISCV_r_rw;
  } else if (op.kind == Kind_zBarrier_RISCV_r_w) {
    rop->zBarrier_RISCV_r_w = op.zBarrier_RISCV_r_w;
  } else if (op.kind == Kind_zBarrier_RISCV_rw_r) {
    rop->zBarrier_RISCV_rw_r = op.zBarrier_RISCV_rw_r;
  } else if (op.kind == Kind_zBarrier_RISCV_rw_rw) {
    rop->zBarrier_RISCV_rw_rw = op.zBarrier_RISCV_rw_rw;
  } else if (op.kind == Kind_zBarrier_RISCV_rw_w) {
    rop->zBarrier_RISCV_rw_w = op.zBarrier_RISCV_rw_w;
  } else if (op.kind == Kind_zBarrier_RISCV_tso) {
    rop->zBarrier_RISCV_tso = op.zBarrier_RISCV_tso;
  } else if (op.kind == Kind_zBarrier_RISCV_w_r) {
    rop->zBarrier_RISCV_w_r = op.zBarrier_RISCV_w_r;
  } else if (op.kind == Kind_zBarrier_RISCV_w_rw) {
    rop->zBarrier_RISCV_w_rw = op.zBarrier_RISCV_w_rw;
  } else if (op.kind == Kind_zBarrier_RISCV_w_w) {
    rop->zBarrier_RISCV_w_w = op.zBarrier_RISCV_w_w;
  } else if (op.kind == Kind_zBarrier_Sync) {
    rop->zBarrier_Sync = op.zBarrier_Sync;
  } else if (op.kind == Kind_zBarrier_x86_MFENCE){
    rop->zBarrier_x86_MFENCE = op.zBarrier_x86_MFENCE;
  }
}

static bool EQUAL(zbarrier_kind)(struct zbarrier_kind op1, struct zbarrier_kind op2) {
  if (op1.kind == Kind_zBarrier_DMB && op2.kind == Kind_zBarrier_DMB) {
  
  return EQUAL(ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9)(op1.zBarrier_DMB, op2.zBarrier_DMB);
  } else if (op1.kind == Kind_zBarrier_DSB && op2.kind == Kind_zBarrier_DSB) {
  
  return EQUAL(ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9)(op1.zBarrier_DSB, op2.zBarrier_DSB);
  } else if (op1.kind == Kind_zBarrier_Eieio && op2.kind == Kind_zBarrier_Eieio) {
    return EQUAL(unit)(op1.zBarrier_Eieio, op2.zBarrier_Eieio);
  } else if (op1.kind == Kind_zBarrier_ISB && op2.kind == Kind_zBarrier_ISB) {
    return EQUAL(unit)(op1.zBarrier_ISB, op2.zBarrier_ISB);
  } else if (op1.kind == Kind_zBarrier_Isync && op2.kind == Kind_zBarrier_Isync) {
    return EQUAL(unit)(op1.zBarrier_Isync, op2.zBarrier_Isync);
  } else if (op1.kind == Kind_zBarrier_LwSync && op2.kind == Kind_zBarrier_LwSync) {
    return EQUAL(unit)(op1.zBarrier_LwSync, op2.zBarrier_LwSync);
  } else if (op1.kind == Kind_zBarrier_MIPS_SYNC && op2.kind == Kind_zBarrier_MIPS_SYNC) {
    return EQUAL(unit)(op1.zBarrier_MIPS_SYNC, op2.zBarrier_MIPS_SYNC);
  } else if (op1.kind == Kind_zBarrier_RISCV_i && op2.kind == Kind_zBarrier_RISCV_i) {
    return EQUAL(unit)(op1.zBarrier_RISCV_i, op2.zBarrier_RISCV_i);
  } else if (op1.kind == Kind_zBarrier_RISCV_r_r && op2.kind == Kind_zBarrier_RISCV_r_r) {
    return EQUAL(unit)(op1.zBarrier_RISCV_r_r, op2.zBarrier_RISCV_r_r);
  } else if (op1.kind == Kind_zBarrier_RISCV_r_rw && op2.kind == Kind_zBarrier_RISCV_r_rw) {
    return EQUAL(unit)(op1.zBarrier_RISCV_r_rw, op2.zBarrier_RISCV_r_rw);
  } else if (op1.kind == Kind_zBarrier_RISCV_r_w && op2.kind == Kind_zBarrier_RISCV_r_w) {
    return EQUAL(unit)(op1.zBarrier_RISCV_r_w, op2.zBarrier_RISCV_r_w);
  } else if (op1.kind == Kind_zBarrier_RISCV_rw_r && op2.kind == Kind_zBarrier_RISCV_rw_r) {
    return EQUAL(unit)(op1.zBarrier_RISCV_rw_r, op2.zBarrier_RISCV_rw_r);
  } else if (op1.kind == Kind_zBarrier_RISCV_rw_rw && op2.kind == Kind_zBarrier_RISCV_rw_rw) {
    return EQUAL(unit)(op1.zBarrier_RISCV_rw_rw, op2.zBarrier_RISCV_rw_rw);
  } else if (op1.kind == Kind_zBarrier_RISCV_rw_w && op2.kind == Kind_zBarrier_RISCV_rw_w) {
    return EQUAL(unit)(op1.zBarrier_RISCV_rw_w, op2.zBarrier_RISCV_rw_w);
  } else if (op1.kind == Kind_zBarrier_RISCV_tso && op2.kind == Kind_zBarrier_RISCV_tso) {
    return EQUAL(unit)(op1.zBarrier_RISCV_tso, op2.zBarrier_RISCV_tso);
  } else if (op1.kind == Kind_zBarrier_RISCV_w_r && op2.kind == Kind_zBarrier_RISCV_w_r) {
    return EQUAL(unit)(op1.zBarrier_RISCV_w_r, op2.zBarrier_RISCV_w_r);
  } else if (op1.kind == Kind_zBarrier_RISCV_w_rw && op2.kind == Kind_zBarrier_RISCV_w_rw) {
    return EQUAL(unit)(op1.zBarrier_RISCV_w_rw, op2.zBarrier_RISCV_w_rw);
  } else if (op1.kind == Kind_zBarrier_RISCV_w_w && op2.kind == Kind_zBarrier_RISCV_w_w) {
    return EQUAL(unit)(op1.zBarrier_RISCV_w_w, op2.zBarrier_RISCV_w_w);
  } else if (op1.kind == Kind_zBarrier_Sync && op2.kind == Kind_zBarrier_Sync) {
    return EQUAL(unit)(op1.zBarrier_Sync, op2.zBarrier_Sync);
  } else if (op1.kind == Kind_zBarrier_x86_MFENCE && op2.kind == Kind_zBarrier_x86_MFENCE) {
    return EQUAL(unit)(op1.zBarrier_x86_MFENCE, op2.zBarrier_x86_MFENCE);
  } else return false;
}

static void sailgen_Barrier_DMB(struct zbarrier_kind *rop, struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_DMB;
  rop->zBarrier_DMB = op;
}

static void sailgen_Barrier_DSB(struct zbarrier_kind *rop, struct ztuple_z8z5enumz0zza64_barrier_domainzCz0z5enumz0zza64_barrier_typez9 op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_DSB;
  rop->zBarrier_DSB = op;
}

static void sailgen_Barrier_Eieio(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_Eieio;
  rop->zBarrier_Eieio = op;
}

static void sailgen_Barrier_ISB(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_ISB;
  rop->zBarrier_ISB = op;
}

static void sailgen_Barrier_Isync(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_Isync;
  rop->zBarrier_Isync = op;
}

static void sailgen_Barrier_LwSync(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_LwSync;
  rop->zBarrier_LwSync = op;
}

static void sailgen_Barrier_MIPS_SYNC(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_MIPS_SYNC;
  rop->zBarrier_MIPS_SYNC = op;
}

static void sailgen_Barrier_RISCV_i(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_i;
  rop->zBarrier_RISCV_i = op;
}

static void sailgen_Barrier_RISCV_r_r(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_r_r;
  rop->zBarrier_RISCV_r_r = op;
}

static void sailgen_Barrier_RISCV_r_rw(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_r_rw;
  rop->zBarrier_RISCV_r_rw = op;
}

static void sailgen_Barrier_RISCV_r_w(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_r_w;
  rop->zBarrier_RISCV_r_w = op;
}

static void sailgen_Barrier_RISCV_rw_r(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_rw_r;
  rop->zBarrier_RISCV_rw_r = op;
}

static void sailgen_Barrier_RISCV_rw_rw(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_rw_rw;
  rop->zBarrier_RISCV_rw_rw = op;
}

static void sailgen_Barrier_RISCV_rw_w(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_rw_w;
  rop->zBarrier_RISCV_rw_w = op;
}

static void sailgen_Barrier_RISCV_tso(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_tso;
  rop->zBarrier_RISCV_tso = op;
}

static void sailgen_Barrier_RISCV_w_r(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_w_r;
  rop->zBarrier_RISCV_w_r = op;
}

static void sailgen_Barrier_RISCV_w_rw(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_w_rw;
  rop->zBarrier_RISCV_w_rw = op;
}

static void sailgen_Barrier_RISCV_w_w(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_RISCV_w_w;
  rop->zBarrier_RISCV_w_w = op;
}

static void sailgen_Barrier_Sync(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_Sync;
  rop->zBarrier_Sync = op;
}

static void sailgen_Barrier_x86_MFENCE(struct zbarrier_kind *rop, unit op)
{
  if (rop->kind == Kind_zBarrier_DMB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_DSB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Eieio) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_ISB) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Isync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_LwSync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_MIPS_SYNC) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_i) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_r_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_rw_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_tso) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_r) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_rw) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_RISCV_w_w) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_Sync) {
    /* do nothing */
  } else if (rop->kind == Kind_zBarrier_x86_MFENCE){
    /* do nothing */
  }
  rop->kind = Kind_zBarrier_x86_MFENCE;
  rop->zBarrier_x86_MFENCE = op;
}

// struct tuple_(%enum zread_kind, %enum zwrite_kind)
struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 {
  enum zread_kind ztup0;
  enum zwrite_kind ztup1;
};

static void COPY(ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9)(struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 *rop, const struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 op) {
  rop->ztup0 = op.ztup0;
  rop->ztup1 = op.ztup1;
}

static bool EQUAL(ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9)(struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 op1, struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 op2) {
  return EQUAL(zread_kind)(op1.ztup0, op2.ztup0) && EQUAL(zwrite_kind)(op1.ztup1, op2.ztup1);
}



// union instruction_kind
enum kind_zinstruction_kind { Kind_zIK_barrier, Kind_zIK_branch, Kind_zIK_cache_op, Kind_zIK_mem_read, Kind_zIK_mem_rmw, Kind_zIK_mem_write, Kind_zIK_simple, Kind_zIK_trans };

struct zinstruction_kind {
  enum kind_zinstruction_kind kind;
  union {
    struct { struct zbarrier_kind zIK_barrier; };
    struct { unit zIK_branch; };
    struct { enum zcache_op_kind zIK_cache_op; };
    struct { enum zread_kind zIK_mem_read; };
    struct { struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 zIK_mem_rmw; };
    struct { enum zwrite_kind zIK_mem_write; };
    struct { unit zIK_simple; };
    struct { enum ztrans_kind zIK_trans; };
  };
};

static void CREATE(zinstruction_kind)(struct zinstruction_kind *op)
{
  op->kind = Kind_zIK_barrier;
  CREATE(zbarrier_kind)(&op->zIK_barrier);
}

static void RECREATE(zinstruction_kind)(struct zinstruction_kind *op) {}

static void KILL(zinstruction_kind)(struct zinstruction_kind *op)
{
  if (op->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&op->zIK_barrier);
  } else if (op->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (op->kind == Kind_zIK_trans){
    /* do nothing */
  };
}

static void COPY(zinstruction_kind)(struct zinstruction_kind *rop, struct zinstruction_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  };
  rop->kind = op.kind;
  if (op.kind == Kind_zIK_barrier) {
    CREATE(zbarrier_kind)(&rop->zIK_barrier); COPY(zbarrier_kind)(&rop->zIK_barrier, op.zIK_barrier);
  } else if (op.kind == Kind_zIK_branch) {
    rop->zIK_branch = op.zIK_branch;
  } else if (op.kind == Kind_zIK_cache_op) {
    rop->zIK_cache_op = op.zIK_cache_op;
  } else if (op.kind == Kind_zIK_mem_read) {
    rop->zIK_mem_read = op.zIK_mem_read;
  } else if (op.kind == Kind_zIK_mem_rmw) {
    rop->zIK_mem_rmw = op.zIK_mem_rmw;
  } else if (op.kind == Kind_zIK_mem_write) {
    rop->zIK_mem_write = op.zIK_mem_write;
  } else if (op.kind == Kind_zIK_simple) {
    rop->zIK_simple = op.zIK_simple;
  } else if (op.kind == Kind_zIK_trans){
    rop->zIK_trans = op.zIK_trans;
  }
}

static bool EQUAL(zinstruction_kind)(struct zinstruction_kind op1, struct zinstruction_kind op2) {
  if (op1.kind == Kind_zIK_barrier && op2.kind == Kind_zIK_barrier) {
    return EQUAL(zbarrier_kind)(op1.zIK_barrier, op2.zIK_barrier);
  } else if (op1.kind == Kind_zIK_branch && op2.kind == Kind_zIK_branch) {
    return EQUAL(unit)(op1.zIK_branch, op2.zIK_branch);
  } else if (op1.kind == Kind_zIK_cache_op && op2.kind == Kind_zIK_cache_op) {
    return EQUAL(zcache_op_kind)(op1.zIK_cache_op, op2.zIK_cache_op);
  } else if (op1.kind == Kind_zIK_mem_read && op2.kind == Kind_zIK_mem_read) {
    return EQUAL(zread_kind)(op1.zIK_mem_read, op2.zIK_mem_read);
  } else if (op1.kind == Kind_zIK_mem_rmw && op2.kind == Kind_zIK_mem_rmw) {
    return EQUAL(ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9)(op1.zIK_mem_rmw, op2.zIK_mem_rmw);
  } else if (op1.kind == Kind_zIK_mem_write && op2.kind == Kind_zIK_mem_write) {
    return EQUAL(zwrite_kind)(op1.zIK_mem_write, op2.zIK_mem_write);
  } else if (op1.kind == Kind_zIK_simple && op2.kind == Kind_zIK_simple) {
    return EQUAL(unit)(op1.zIK_simple, op2.zIK_simple);
  } else if (op1.kind == Kind_zIK_trans && op2.kind == Kind_zIK_trans) {
    return EQUAL(ztrans_kind)(op1.zIK_trans, op2.zIK_trans);
  } else return false;
}

static void sailgen_IK_barrier(struct zinstruction_kind *rop, struct zbarrier_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_barrier;
  CREATE(zbarrier_kind)(&rop->zIK_barrier);
  COPY(zbarrier_kind)(&rop->zIK_barrier, op);
  
}

static void sailgen_IK_branch(struct zinstruction_kind *rop, unit op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_branch;
  rop->zIK_branch = op;
}

static void sailgen_IK_cache_op(struct zinstruction_kind *rop, enum zcache_op_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_cache_op;
  rop->zIK_cache_op = op;
}

static void sailgen_IK_mem_read(struct zinstruction_kind *rop, enum zread_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_mem_read;
  rop->zIK_mem_read = op;
}

static void sailgen_IK_mem_rmw(struct zinstruction_kind *rop, struct ztuple_z8z5enumz0zzread_kindzCz0z5enumz0zzwrite_kindz9 op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_mem_rmw;
  rop->zIK_mem_rmw = op;
}

static void sailgen_IK_mem_write(struct zinstruction_kind *rop, enum zwrite_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_mem_write;
  rop->zIK_mem_write = op;
}

static void sailgen_IK_simple(struct zinstruction_kind *rop, unit op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_simple;
  rop->zIK_simple = op;
}

static void sailgen_IK_trans(struct zinstruction_kind *rop, enum ztrans_kind op)
{
  if (rop->kind == Kind_zIK_barrier) {
    KILL(zbarrier_kind)(&rop->zIK_barrier);
  } else if (rop->kind == Kind_zIK_branch) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_cache_op) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_read) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_rmw) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_mem_write) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_simple) {
    /* do nothing */
  } else if (rop->kind == Kind_zIK_trans){
    /* do nothing */
  }
  rop->kind = Kind_zIK_trans;
  rop->zIK_trans = op;
}

// struct EncCapability
struct zEncCapability {
  uint64_t zB;
  uint64_t zT;
  uint64_t zaddress;
  uint64_t zflags;
  fbits zinternal_E;
  uint64_t zotype;
  uint64_t zperms;
  uint64_t zreserved;
};

static void COPY(zEncCapability)(struct zEncCapability *rop, const struct zEncCapability op) {
  rop->zB = op.zB;
  rop->zT = op.zT;
  rop->zaddress = op.zaddress;
  rop->zflags = op.zflags;
  rop->zinternal_E = op.zinternal_E;
  rop->zotype = op.zotype;
  rop->zperms = op.zperms;
  rop->zreserved = op.zreserved;
}

static bool EQUAL(zEncCapability)(struct zEncCapability op1, struct zEncCapability op2) {
  return EQUAL(fbits)(op1.zB, op2.zB) && EQUAL(fbits)(op1.zT, op2.zT) && EQUAL(fbits)(op1.zaddress, op2.zaddress) && EQUAL(fbits)(op1.zflags, op2.zflags) && EQUAL(fbits)(op1.zinternal_E, op2.zinternal_E) && EQUAL(fbits)(op1.zotype, op2.zotype) && EQUAL(fbits)(op1.zperms, op2.zperms) && EQUAL(fbits)(op1.zreserved, op2.zreserved);
}

// enum ClearRegSet
enum zClearRegSet { zGPRegs, zFPRegs };

static bool eq_zClearRegSet(enum zClearRegSet op1, enum zClearRegSet op2) { return op1 == op2; }

static enum zClearRegSet UNDEFINED(zClearRegSet)(unit u) { return zGPRegs; }

// struct Capability
struct zCapability {
  uint64_t zB;
  uint64_t zE;
  uint64_t zT;
  bool zaccess_system_regs;
  uint64_t zaddress;
  bool zflag_cap_mode;
  bool zglobal;
  bool zinternal_E;
  uint64_t zotype;
  bool zpermit_cinvoke;
  bool zpermit_execute;
  bool zpermit_load;
  bool zpermit_load_cap;
  bool zpermit_seal;
  bool zpermit_set_CID;
  bool zpermit_store;
  bool zpermit_store_cap;
  bool zpermit_store_local_cap;
  bool zpermit_unseal;
  uint64_t zreserved;
  bool ztag;
  uint64_t zuperms;
};

static void COPY(zCapability)(struct zCapability *rop, const struct zCapability op) {
  rop->zB = op.zB;
  rop->zE = op.zE;
  rop->zT = op.zT;
  rop->zaccess_system_regs = op.zaccess_system_regs;
  rop->zaddress = op.zaddress;
  rop->zflag_cap_mode = op.zflag_cap_mode;
  rop->zglobal = op.zglobal;
  rop->zinternal_E = op.zinternal_E;
  rop->zotype = op.zotype;
  rop->zpermit_cinvoke = op.zpermit_cinvoke;
  rop->zpermit_execute = op.zpermit_execute;
  rop->zpermit_load = op.zpermit_load;
  rop->zpermit_load_cap = op.zpermit_load_cap;
  rop->zpermit_seal = op.zpermit_seal;
  rop->zpermit_set_CID = op.zpermit_set_CID;
  rop->zpermit_store = op.zpermit_store;
  rop->zpermit_store_cap = op.zpermit_store_cap;
  rop->zpermit_store_local_cap = op.zpermit_store_local_cap;
  rop->zpermit_unseal = op.zpermit_unseal;
  rop->zreserved = op.zreserved;
  rop->ztag = op.ztag;
  rop->zuperms = op.zuperms;
}

static bool EQUAL(zCapability)(struct zCapability op1, struct zCapability op2) {
  return EQUAL(fbits)(op1.zB, op2.zB) && EQUAL(fbits)(op1.zE, op2.zE) && EQUAL(fbits)(op1.zT, op2.zT) && EQUAL(bool)(op1.zaccess_system_regs, op2.zaccess_system_regs) && EQUAL(fbits)(op1.zaddress, op2.zaddress) && EQUAL(bool)(op1.zflag_cap_mode, op2.zflag_cap_mode) && EQUAL(bool)(op1.zglobal, op2.zglobal) && EQUAL(bool)(op1.zinternal_E, op2.zinternal_E) && EQUAL(fbits)(op1.zotype, op2.zotype) && EQUAL(bool)(op1.zpermit_cinvoke, op2.zpermit_cinvoke) && EQUAL(bool)(op1.zpermit_execute, op2.zpermit_execute) && EQUAL(bool)(op1.zpermit_load, op2.zpermit_load) && EQUAL(bool)(op1.zpermit_load_cap, op2.zpermit_load_cap) && EQUAL(bool)(op1.zpermit_seal, op2.zpermit_seal) && EQUAL(bool)(op1.zpermit_set_CID, op2.zpermit_set_CID) && EQUAL(bool)(op1.zpermit_store, op2.zpermit_store) && EQUAL(bool)(op1.zpermit_store_cap, op2.zpermit_store_cap) && EQUAL(bool)(op1.zpermit_store_local_cap, op2.zpermit_store_local_cap) && EQUAL(bool)(op1.zpermit_unseal, op2.zpermit_unseal) && EQUAL(fbits)(op1.zreserved, op2.zreserved) && EQUAL(bool)(op1.ztag, op2.ztag) && EQUAL(fbits)(op1.zuperms, op2.zuperms);
}

// enum CapEx
enum zCapEx { zCapEx_None, zCapEx_LengthViolation, zCapEx_TagViolation, zCapEx_SealViolation, zCapEx_TypeViolation, zCapEx_CallTrap, zCapEx_ReturnTrap, zCapEx_TSSUnderFlow, zCapEx_UserDefViolation, zCapEx_InexactBounds, zCapEx_UnalignedBase, zCapEx_GlobalViolation, zCapEx_PermitExecuteViolation, zCapEx_PermitLoadViolation, zCapEx_PermitStoreViolation, zCapEx_PermitLoadCapViolation, zCapEx_PermitStoreCapViolation, zCapEx_PermitStoreLocalCapViolation, zCapEx_PermitSealViolation, zCapEx_AccessSystemRegsViolation, zCapEx_PermitCInvokeViolation, zCapEx_AccessCInvokeIDCViolation, zCapEx_PermitUnsealViolation, zCapEx_PermitSetCIDViolation };

static bool eq_zCapEx(enum zCapEx op1, enum zCapEx op2) { return op1 == op2; }

static enum zCapEx UNDEFINED(zCapEx)(unit u) { return zCapEx_None; }

// enum CPtrCmpOp
enum zCPtrCmpOp { zCEQ, zCNE, zCLT, zCLE, zCLTU, zCLEU, zCEXEQ, zCNEXEQ };

static bool eq_zCPtrCmpOp(enum zCPtrCmpOp op1, enum zCPtrCmpOp op2) { return op1 == op2; }

static enum zCPtrCmpOp UNDEFINED(zCPtrCmpOp)(unit u) { return zCEQ; }

















static bool sailgen_zzz3zzzz7mzzJzzK0z3neq_int(sail_int, int64_t);

static bool sailgen_zzz3zzzz7mzzJzzKzzD1z3neq_int(sail_int, int64_t);

static bool sailgen_zzz3zzzz7mzzJzzK0z3neq_int(sail_int zx, int64_t zy)
{
  __label__ end_function_1, end_block_exception_2;

  bool zcbz30;
  bool zgaz30;
  {
    sail_int zgsz30;
    CREATE(sail_int)(&zgsz30);
    CONVERT_OF(sail_int, mach_int)(&zgsz30, zy);
    zgaz30 = eq_int(zx, zgsz30);
    KILL(sail_int)(&zgsz30);
  }
  zcbz30 = not(zgaz30);

end_function_1: ;
  return zcbz30;
end_block_exception_2: ;

  return false;
}

static bool sailgen_zzz3zzzz7mzzJzzKzzD1z3neq_int(sail_int zx, int64_t zy)
{
  __label__ end_function_4, end_block_exception_5;

  bool zcbz31;
  bool zgaz31;
  {
    sail_int zgsz31;
    CREATE(sail_int)(&zgsz31);
    CONVERT_OF(sail_int, mach_int)(&zgsz31, zy);
    zgaz31 = eq_int(zx, zgsz31);
    KILL(sail_int)(&zgsz31);
  }
  zcbz31 = not(zgaz31);

end_function_4: ;
  return zcbz31;
end_block_exception_5: ;

  return false;
}





















static bool sailgen_neq_bits(lbits zx, lbits zy)
{
  __label__ end_function_7, end_block_exception_8;

  bool zcbz32;
  bool zgaz32;
  zgaz32 = eq_bits(zx, zy);
  zcbz32 = not(zgaz32);

end_function_7: ;
  return zcbz32;
end_block_exception_8: ;

  return false;
}

































static uint64_t sailgen_zzzz7nzzJzzK4z3sail_ones(int64_t);

static void sailgen_zzz3zzz3sail_ones(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK4z3sail_ones(int64_t zn)
{
  __label__ end_function_10, end_block_exception_11;

  uint64_t zcbz33;
  uint64_t zgaz33;
  {
    sail_int zgsz32;
    CREATE(sail_int)(&zgsz32);
    CONVERT_OF(sail_int, mach_int)(&zgsz32, zn);
    lbits zgsz33;
    CREATE(lbits)(&zgsz33);
    zeros(&zgsz33, zgsz32);
    zgaz33 = CONVERT_OF(fbits, lbits)(zgsz33, true);
    KILL(lbits)(&zgsz33);
    KILL(sail_int)(&zgsz32);
  }
  {
    lbits zgsz34;
    CREATE(lbits)(&zgsz34);
    CONVERT_OF(lbits, fbits)(&zgsz34, zgaz33, UINT64_C(4) , true);
    lbits zgsz35;
    CREATE(lbits)(&zgsz35);
    not_bits(&zgsz35, zgsz34);
    zcbz33 = CONVERT_OF(fbits, lbits)(zgsz35, true);
    KILL(lbits)(&zgsz35);
    KILL(lbits)(&zgsz34);
  }

end_function_10: ;
  return zcbz33;
end_block_exception_11: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzz3zzz3sail_ones(lbits *zcbz34, sail_int zn)
{
  __label__ end_function_13, end_block_exception_14, end_function_247;

  lbits zgaz34;
  CREATE(lbits)(&zgaz34);
  zeros(&zgaz34, zn);
  not_bits((*(&zcbz34)), zgaz34);
  KILL(lbits)(&zgaz34);
end_function_13: ;
  goto end_function_247;
end_block_exception_14: ;
  goto end_function_247;
end_function_247: ;
}











static fbits sailgen_not_bit(fbits);

static fbits sailgen_not_bit(fbits zb)
{
  __label__ end_function_16, end_block_exception_17;

  fbits zcbz35;
  bool zgaz35;
  zgaz35 = eq_bit(zb, UINT64_C(1));
  if (zgaz35) {  zcbz35 = UINT64_C(0);  } else {  zcbz35 = UINT64_C(1);  }

end_function_16: ;
  return zcbz35;
end_block_exception_17: ;

  return UINT64_C(0);
}













static uint64_t sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTS(int64_t, uint64_t);

static void sailgen_zzz3zzz3EXTZ(lbits *rop, sail_int, lbits);

static uint64_t sailgen_zzzz7mzzJzzK15zzCzz0zz7nzzJzzK12z3EXTZ(int64_t, uint64_t);

static uint64_t sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTZ(int64_t, uint64_t);

static uint64_t sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTS(int64_t zm, uint64_t zv)
{
  __label__ end_function_19, end_block_exception_20;

  uint64_t zcbz36;
  {
    lbits zgsz36;
    CREATE(lbits)(&zgsz36);
    CONVERT_OF(lbits, fbits)(&zgsz36, zv, UINT64_C(18) , true);
    sail_int zgsz37;
    CREATE(sail_int)(&zgsz37);
    CONVERT_OF(sail_int, mach_int)(&zgsz37, zm);
    lbits zgsz38;
    CREATE(lbits)(&zgsz38);
    sign_extend(&zgsz38, zgsz36, zgsz37);
    zcbz36 = CONVERT_OF(fbits, lbits)(zgsz38, true);
    KILL(lbits)(&zgsz38);
    KILL(sail_int)(&zgsz37);
    KILL(lbits)(&zgsz36);
  }
end_function_19: ;
  return zcbz36;
end_block_exception_20: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzz3zzz3EXTZ(lbits *zcbz37, sail_int zm, lbits zv)
{
  __label__ end_function_22, end_block_exception_23, end_function_246;

  zero_extend((*(&zcbz37)), zv, zm);
end_function_22: ;
  goto end_function_246;
end_block_exception_23: ;
  goto end_function_246;
end_function_246: ;
}

static uint64_t sailgen_zzzz7mzzJzzK15zzCzz0zz7nzzJzzK12z3EXTZ(int64_t zm, uint64_t zv)
{
  __label__ end_function_25, end_block_exception_26;

  uint64_t zcbz38;
  {
    lbits zgsz39;
    CREATE(lbits)(&zgsz39);
    CONVERT_OF(lbits, fbits)(&zgsz39, zv, UINT64_C(12) , true);
    sail_int zgsz310;
    CREATE(sail_int)(&zgsz310);
    CONVERT_OF(sail_int, mach_int)(&zgsz310, zm);
    lbits zgsz311;
    CREATE(lbits)(&zgsz311);
    zero_extend(&zgsz311, zgsz39, zgsz310);
    zcbz38 = CONVERT_OF(fbits, lbits)(zgsz311, true);
    KILL(lbits)(&zgsz311);
    KILL(sail_int)(&zgsz310);
    KILL(lbits)(&zgsz39);
  }
end_function_25: ;
  return zcbz38;
end_block_exception_26: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTZ(int64_t zm, uint64_t zv)
{
  __label__ end_function_28, end_block_exception_29;

  uint64_t zcbz39;
  {
    lbits zgsz312;
    CREATE(lbits)(&zgsz312);
    CONVERT_OF(lbits, fbits)(&zgsz312, zv, UINT64_C(18) , true);
    sail_int zgsz313;
    CREATE(sail_int)(&zgsz313);
    CONVERT_OF(sail_int, mach_int)(&zgsz313, zm);
    lbits zgsz314;
    CREATE(lbits)(&zgsz314);
    zero_extend(&zgsz314, zgsz312, zgsz313);
    zcbz39 = CONVERT_OF(fbits, lbits)(zgsz314, true);
    KILL(lbits)(&zgsz314);
    KILL(sail_int)(&zgsz313);
    KILL(lbits)(&zgsz312);
  }
end_function_28: ;
  return zcbz39;
end_block_exception_29: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK12z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK64z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK14z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK2z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK4z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK6z3zzeros_implicit(int64_t);

static void sailgen_zzz3zzz3zzeros_implicit(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK3z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK11z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK12z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_31, end_block_exception_32;

  uint64_t zcbz310;
  {
    sail_int zgsz315;
    CREATE(sail_int)(&zgsz315);
    CONVERT_OF(sail_int, mach_int)(&zgsz315, zn);
    lbits zgsz316;
    CREATE(lbits)(&zgsz316);
    zeros(&zgsz316, zgsz315);
    zcbz310 = CONVERT_OF(fbits, lbits)(zgsz316, true);
    KILL(lbits)(&zgsz316);
    KILL(sail_int)(&zgsz315);
  }
end_function_31: ;
  return zcbz310;
end_block_exception_32: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK64z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_34, end_block_exception_35;

  uint64_t zcbz311;
  {
    sail_int zgsz317;
    CREATE(sail_int)(&zgsz317);
    CONVERT_OF(sail_int, mach_int)(&zgsz317, zn);
    lbits zgsz318;
    CREATE(lbits)(&zgsz318);
    zeros(&zgsz318, zgsz317);
    zcbz311 = CONVERT_OF(fbits, lbits)(zgsz318, true);
    KILL(lbits)(&zgsz318);
    KILL(sail_int)(&zgsz317);
  }
end_function_34: ;
  return zcbz311;
end_block_exception_35: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK14z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_37, end_block_exception_38;

  uint64_t zcbz312;
  {
    sail_int zgsz319;
    CREATE(sail_int)(&zgsz319);
    CONVERT_OF(sail_int, mach_int)(&zgsz319, zn);
    lbits zgsz320;
    CREATE(lbits)(&zgsz320);
    zeros(&zgsz320, zgsz319);
    zcbz312 = CONVERT_OF(fbits, lbits)(zgsz320, true);
    KILL(lbits)(&zgsz320);
    KILL(sail_int)(&zgsz319);
  }
end_function_37: ;
  return zcbz312;
end_block_exception_38: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK2z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_40, end_block_exception_41;

  uint64_t zcbz313;
  {
    sail_int zgsz321;
    CREATE(sail_int)(&zgsz321);
    CONVERT_OF(sail_int, mach_int)(&zgsz321, zn);
    lbits zgsz322;
    CREATE(lbits)(&zgsz322);
    zeros(&zgsz322, zgsz321);
    zcbz313 = CONVERT_OF(fbits, lbits)(zgsz322, true);
    KILL(lbits)(&zgsz322);
    KILL(sail_int)(&zgsz321);
  }
end_function_40: ;
  return zcbz313;
end_block_exception_41: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK4z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_43, end_block_exception_44;

  uint64_t zcbz314;
  {
    sail_int zgsz323;
    CREATE(sail_int)(&zgsz323);
    CONVERT_OF(sail_int, mach_int)(&zgsz323, zn);
    lbits zgsz324;
    CREATE(lbits)(&zgsz324);
    zeros(&zgsz324, zgsz323);
    zcbz314 = CONVERT_OF(fbits, lbits)(zgsz324, true);
    KILL(lbits)(&zgsz324);
    KILL(sail_int)(&zgsz323);
  }
end_function_43: ;
  return zcbz314;
end_block_exception_44: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK6z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_46, end_block_exception_47;

  uint64_t zcbz315;
  {
    sail_int zgsz325;
    CREATE(sail_int)(&zgsz325);
    CONVERT_OF(sail_int, mach_int)(&zgsz325, zn);
    lbits zgsz326;
    CREATE(lbits)(&zgsz326);
    zeros(&zgsz326, zgsz325);
    zcbz315 = CONVERT_OF(fbits, lbits)(zgsz326, true);
    KILL(lbits)(&zgsz326);
    KILL(sail_int)(&zgsz325);
  }
end_function_46: ;
  return zcbz315;
end_block_exception_47: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzz3zzz3zzeros_implicit(lbits *zcbz316, sail_int zn)
{
  __label__ end_function_49, end_block_exception_50, end_function_245;

  zeros((*(&zcbz316)), zn);
end_function_49: ;
  goto end_function_245;
end_block_exception_50: ;
  goto end_function_245;
end_function_245: ;
}

static uint64_t sailgen_zzzz7nzzJzzK3z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_52, end_block_exception_53;

  uint64_t zcbz317;
  {
    sail_int zgsz327;
    CREATE(sail_int)(&zgsz327);
    CONVERT_OF(sail_int, mach_int)(&zgsz327, zn);
    lbits zgsz328;
    CREATE(lbits)(&zgsz328);
    zeros(&zgsz328, zgsz327);
    zcbz317 = CONVERT_OF(fbits, lbits)(zgsz328, true);
    KILL(lbits)(&zgsz328);
    KILL(sail_int)(&zgsz327);
  }
end_function_52: ;
  return zcbz317;
end_block_exception_53: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK11z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_55, end_block_exception_56;

  uint64_t zcbz318;
  {
    sail_int zgsz329;
    CREATE(sail_int)(&zgsz329);
    CONVERT_OF(sail_int, mach_int)(&zgsz329, zn);
    lbits zgsz330;
    CREATE(lbits)(&zgsz330);
    zeros(&zgsz330, zgsz329);
    zcbz318 = CONVERT_OF(fbits, lbits)(zgsz330, true);
    KILL(lbits)(&zgsz330);
    KILL(sail_int)(&zgsz329);
  }
end_function_55: ;
  return zcbz318;
end_block_exception_56: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK4z3ones(int64_t);

static void sailgen_zzz3zzz3ones(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK4z3ones(int64_t zn)
{
  __label__ end_function_58, end_block_exception_59;

  uint64_t zcbz319;
  zcbz319 = sailgen_zzzz7nzzJzzK4z3sail_ones(zn);
end_function_58: ;
  return zcbz319;
end_block_exception_59: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzz3zzz3ones(lbits *zcbz320, sail_int zn)
{
  __label__ end_function_61, end_block_exception_62, end_function_244;

  sailgen_zzz3zzz3sail_ones((*(&zcbz320)), zn);
end_function_61: ;
  goto end_function_244;
end_block_exception_62: ;
  goto end_function_244;
end_function_244: ;
}

static uint64_t sailgen_bool_to_bits(bool);

static uint64_t sailgen_bool_to_bits(bool zx)
{
  __label__ end_function_64, end_block_exception_65;

  uint64_t zcbz321;
  if (zx) {  zcbz321 = UINT64_C(0b1);  } else {  zcbz321 = UINT64_C(0b0);  }
end_function_64: ;
  return zcbz321;
end_block_exception_65: ;

  return UINT64_C(0xdeadc0de);
}

static bool sailgen_bit_to_bool(fbits);

static bool sailgen_bit_to_bool(fbits zb)
{
  __label__ case_68, case_69, finish_match_67, end_function_70, end_block_exception_71;

  bool zcbz322;
  bool zgsz331;
  {
    fbits zp0z3;
    zp0z3 = zb;
    bool zgsz332;
    zgsz332 = eq_bit(zp0z3, UINT64_C(1));
    if (!(zgsz332)) {
  
    goto case_68;
    }
    zgsz331 = true;
  
    goto finish_match_67;
  }
case_68: ;
  {
    fbits zp0z3;
    zp0z3 = zb;
    bool zgsz333;
    zgsz333 = eq_bit(zp0z3, UINT64_C(0));
    if (!(zgsz333)) {
  
    goto case_69;
    }
    zgsz331 = false;
  
    goto finish_match_67;
  }
case_69: ;
  sail_match_failure("bit_to_bool");
finish_match_67: ;
  zcbz322 = zgsz331;

end_function_70: ;
  return zcbz322;
end_block_exception_71: ;

  return false;
}

static void sailgen_to_bits(lbits *rop, sail_int, sail_int);

static uint64_t sailgen_zzzz7lzzJzzK6z3to_bits(int64_t, sail_int);

static void sailgen_zzzz7lzzJzzK68z3to_bits(lbits *rop, int64_t, sail_int);

static void sailgen_to_bits(lbits *zcbz323, sail_int zl, sail_int zn)
{
  __label__ end_function_73, end_block_exception_74, end_function_243;

  {
    sail_int zgsz334;
    CREATE(sail_int)(&zgsz334);
    CONVERT_OF(sail_int, mach_int)(&zgsz334, INT64_C(0));
    get_slice_int((*(&zcbz323)), zl, zn, zgsz334);
    KILL(sail_int)(&zgsz334);
  }
end_function_73: ;
  goto end_function_243;
end_block_exception_74: ;
  goto end_function_243;
end_function_243: ;
}

static uint64_t sailgen_zzzz7lzzJzzK6z3to_bits(int64_t zl, sail_int zn)
{
  __label__ end_function_76, end_block_exception_77;

  uint64_t zcbz324;
  {
    sail_int zgsz335;
    CREATE(sail_int)(&zgsz335);
    CONVERT_OF(sail_int, mach_int)(&zgsz335, zl);
    sail_int zgsz336;
    CREATE(sail_int)(&zgsz336);
    CONVERT_OF(sail_int, mach_int)(&zgsz336, INT64_C(0));
    lbits zgsz337;
    CREATE(lbits)(&zgsz337);
    get_slice_int(&zgsz337, zgsz335, zn, zgsz336);
    zcbz324 = CONVERT_OF(fbits, lbits)(zgsz337, true);
    KILL(lbits)(&zgsz337);
    KILL(sail_int)(&zgsz336);
    KILL(sail_int)(&zgsz335);
  }
end_function_76: ;
  return zcbz324;
end_block_exception_77: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzzz7lzzJzzK68z3to_bits(lbits *zcbz325, int64_t zl, sail_int zn)
{
  __label__ end_function_79, end_block_exception_80, end_function_242;

  {
    sail_int zgsz339;
    CREATE(sail_int)(&zgsz339);
    CONVERT_OF(sail_int, mach_int)(&zgsz339, INT64_C(0));
    sail_int zgsz338;
    CREATE(sail_int)(&zgsz338);
    CONVERT_OF(sail_int, mach_int)(&zgsz338, zl);
    get_slice_int((*(&zcbz325)), zgsz338, zn, zgsz339);
    KILL(sail_int)(&zgsz339);
    KILL(sail_int)(&zgsz338);
  }
end_function_79: ;
  goto end_function_242;
end_block_exception_80: ;
  goto end_function_242;
end_function_242: ;
}

static bool sailgen_z8operatorz0zzz3zzz3zI_uz9(lbits, lbits);

static bool sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzzz7nzzJzzK14z3zI_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzzz7nzzJzzK14z3zKzJ_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzz3zzz3zI_uz9(lbits zx, lbits zy)
{
  __label__ end_function_82, end_block_exception_83;

  bool zcbz326;
  sail_int zgaz36;
  CREATE(sail_int)(&zgaz36);
  sail_unsigned(&zgaz36, zx);
  sail_int zgaz37;
  CREATE(sail_int)(&zgaz37);
  sail_unsigned(&zgaz37, zy);
  zcbz326 = lt(zgaz36, zgaz37);
  KILL(sail_int)(&zgaz37);
  KILL(sail_int)(&zgaz36);
end_function_82: ;
  return zcbz326;
end_block_exception_83: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_85, end_block_exception_86;

  bool zcbz327;
  int64_t zgaz38;
  zgaz38 = ((mach_int) zx);
  int64_t zgaz39;
  zgaz39 = ((mach_int) zy);
  {
    sail_int zgsz341;
    CREATE(sail_int)(&zgsz341);
    CONVERT_OF(sail_int, mach_int)(&zgsz341, zgaz39);
    sail_int zgsz340;
    CREATE(sail_int)(&zgsz340);
    CONVERT_OF(sail_int, mach_int)(&zgsz340, zgaz38);
    zcbz327 = lt(zgsz340, zgsz341);
    KILL(sail_int)(&zgsz341);
    KILL(sail_int)(&zgsz340);
  }


end_function_85: ;
  return zcbz327;
end_block_exception_86: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK14z3zI_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_88, end_block_exception_89;

  bool zcbz328;
  int64_t zgaz310;
  zgaz310 = ((mach_int) zx);
  int64_t zgaz311;
  zgaz311 = ((mach_int) zy);
  {
    sail_int zgsz343;
    CREATE(sail_int)(&zgsz343);
    CONVERT_OF(sail_int, mach_int)(&zgsz343, zgaz311);
    sail_int zgsz342;
    CREATE(sail_int)(&zgsz342);
    CONVERT_OF(sail_int, mach_int)(&zgsz342, zgaz310);
    zcbz328 = lt(zgsz342, zgsz343);
    KILL(sail_int)(&zgsz343);
    KILL(sail_int)(&zgsz342);
  }


end_function_88: ;
  return zcbz328;
end_block_exception_89: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK14z3zKzJ_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_91, end_block_exception_92;

  bool zcbz329;
  int64_t zgaz312;
  zgaz312 = ((mach_int) zx);
  int64_t zgaz313;
  zgaz313 = ((mach_int) zy);
  {
    sail_int zgsz345;
    CREATE(sail_int)(&zgsz345);
    CONVERT_OF(sail_int, mach_int)(&zgsz345, zgaz313);
    sail_int zgsz344;
    CREATE(sail_int)(&zgsz344);
    CONVERT_OF(sail_int, mach_int)(&zgsz344, zgaz312);
    zcbz329 = gteq(zgsz344, zgsz345);
    KILL(sail_int)(&zgsz345);
    KILL(sail_int)(&zgsz344);
  }


end_function_91: ;
  return zcbz329;
end_block_exception_92: ;

  return false;
}



static void sailgen_n_leading_spaces(sail_int *rop, sail_string);

static void sailgen_n_leading_spaces(sail_int *zcbz330, sail_string zs)
{
  __label__ case_95, case_96, finish_match_94, end_function_100, end_block_exception_101, end_function_241;

  sail_int zgsz346;
  CREATE(sail_int)(&zgsz346);
  {
    sail_string zp0z3;
    CREATE(sail_string)(&zp0z3);
    COPY(sail_string)(&zp0z3, zs);
    bool zgsz347;
    zgsz347 = eq_string(zp0z3, "");
    if (!(zgsz347)) {
    KILL(sail_string)(&zp0z3);
    goto case_95;
    }
    CONVERT_OF(sail_int, mach_int)(&zgsz346, INT64_C(0));
    KILL(sail_string)(&zp0z3);
    goto finish_match_94;
  }
case_95: ;
  {
    __label__ case_98, case_99, finish_match_97;

    sail_string zgaz314;
    CREATE(sail_string)(&zgaz314);
    {
      sail_int zgsz348;
      CREATE(sail_int)(&zgsz348);
      CONVERT_OF(sail_int, mach_int)(&zgsz348, INT64_C(1));
      string_take(&zgaz314, zs, zgsz348);
      KILL(sail_int)(&zgsz348);
    }
    sail_int zgsz349;
    CREATE(sail_int)(&zgsz349);
    {
      sail_string zp0z3;
      CREATE(sail_string)(&zp0z3);
      COPY(sail_string)(&zp0z3, zgaz314);
      bool zgsz351;
      zgsz351 = eq_string(zp0z3, " ");
      if (!(zgsz351)) {
      KILL(sail_string)(&zp0z3);
      goto case_98;
      }
      sail_int zgaz316;
      CREATE(sail_int)(&zgaz316);
      {
        sail_string zgaz315;
        CREATE(sail_string)(&zgaz315);
        {
          sail_int zgsz350;
          CREATE(sail_int)(&zgsz350);
          CONVERT_OF(sail_int, mach_int)(&zgsz350, INT64_C(1));
          string_drop(&zgaz315, zs, zgsz350);
          KILL(sail_int)(&zgsz350);
        }
        sailgen_n_leading_spaces(&zgaz316, zgaz315);
        KILL(sail_string)(&zgaz315);
      }
      {
        sail_int zgsz352;
        CREATE(sail_int)(&zgsz352);
        CONVERT_OF(sail_int, mach_int)(&zgsz352, INT64_C(1));
        add_int(&zgsz349, zgsz352, zgaz316);
        KILL(sail_int)(&zgsz352);
      }
      KILL(sail_int)(&zgaz316);
      KILL(sail_string)(&zp0z3);
      goto finish_match_97;
    }
  case_98: ;
    {
      CONVERT_OF(sail_int, mach_int)(&zgsz349, INT64_C(0));
      goto finish_match_97;
    }
  case_99: ;
    sail_match_failure("n_leading_spaces");
  finish_match_97: ;
    COPY(sail_int)(&zgsz346, zgsz349);
    KILL(sail_int)(&zgsz349);
    KILL(sail_string)(&zgaz314);
    goto finish_match_94;
  }
case_96: ;
  sail_match_failure("n_leading_spaces");
finish_match_94: ;
  COPY(sail_int)((*(&zcbz330)), zgsz346);
  KILL(sail_int)(&zgsz346);
end_function_100: ;
  goto end_function_241;
end_block_exception_101: ;
  goto end_function_241;
end_function_241: ;
}

static void sailgen_zzzz7nzzJzzK64z3MAX(sail_int *rop, int64_t);

static int64_t sailgen_zzzz7nzzJzzK18z3MAX(int64_t);

static void sailgen_zzzz7nzzJzzK64z3MAX(sail_int *zcbz331, int64_t zn)
{
  __label__ end_function_103, end_block_exception_104, end_function_240;

  sail_int zgaz317;
  CREATE(sail_int)(&zgaz317);
  {
    sail_int zgsz355;
    CREATE(sail_int)(&zgsz355);
    CONVERT_OF(sail_int, mach_int)(&zgsz355, zn);
    pow2(&zgaz317, zgsz355);
    KILL(sail_int)(&zgsz355);
  }
  {
    sail_int zgsz356;
    CREATE(sail_int)(&zgsz356);
    CONVERT_OF(sail_int, mach_int)(&zgsz356, INT64_C(1));
    sub_int((*(&zcbz331)), zgaz317, zgsz356);
    KILL(sail_int)(&zgsz356);
  }
  KILL(sail_int)(&zgaz317);
end_function_103: ;
  goto end_function_240;
end_block_exception_104: ;
  goto end_function_240;
end_function_240: ;
}

static int64_t sailgen_zzzz7nzzJzzK18z3MAX(int64_t zn)
{
  __label__ end_function_106, end_block_exception_107;

  int64_t zcbz332;
  int64_t zgaz318;
  {
    sail_int zgsz357;
    CREATE(sail_int)(&zgsz357);
    CONVERT_OF(sail_int, mach_int)(&zgsz357, zn);
    sail_int zgsz358;
    CREATE(sail_int)(&zgsz358);
    pow2(&zgsz358, zgsz357);
    zgaz318 = CONVERT_OF(mach_int, sail_int)(zgsz358);
    KILL(sail_int)(&zgsz358);
    KILL(sail_int)(&zgsz357);
  }
  {
    sail_int zgsz359;
    CREATE(sail_int)(&zgsz359);
    CONVERT_OF(sail_int, mach_int)(&zgsz359, zgaz318);
    sail_int zgsz360;
    CREATE(sail_int)(&zgsz360);
    CONVERT_OF(sail_int, mach_int)(&zgsz360, INT64_C(1));
    sail_int zgsz361;
    CREATE(sail_int)(&zgsz361);
    sub_int(&zgsz361, zgsz359, zgsz360);
    zcbz332 = CONVERT_OF(mach_int, sail_int)(zgsz361);
    KILL(sail_int)(&zgsz361);
    KILL(sail_int)(&zgsz360);
    KILL(sail_int)(&zgsz359);
  }

end_function_106: ;
  return zcbz332;
end_block_exception_107: ;

  return INT64_C(0xdeadc0de);
}



static int64_t zreserved_otypes;
static void create_letbind_0(void) {  


  int64_t zgsz362;
  zgsz362 = INT64_C(4);
  zreserved_otypes = zgsz362;

let_end_108: ;
}
static void kill_letbind_0(void) {  
}

static int64_t zotype_unsealed;
static void create_letbind_1(void) {  


  int64_t zgsz363;
  zgsz363 = INT64_C(-1);
  zotype_unsealed = zgsz363;

let_end_109: ;
}
static void kill_letbind_1(void) {  
}

static int64_t zotype_sentry;
static void create_letbind_2(void) {  


  int64_t zgsz364;
  zgsz364 = INT64_C(-2);
  zotype_sentry = zgsz364;

let_end_110: ;
}
static void kill_letbind_2(void) {  
}

static uint64_t zPCC_IDX;
static void create_letbind_3(void) {  


  uint64_t zgsz365;
  zgsz365 = UINT64_C(0b100000);
  zPCC_IDX = zgsz365;

let_end_111: ;
}
static void kill_letbind_3(void) {  
}

static uint64_t zDDC_IDX;
static void create_letbind_4(void) {  


  uint64_t zgsz366;
  zgsz366 = UINT64_C(0b100001);
  zDDC_IDX = zgsz366;

let_end_112: ;
}
static void kill_letbind_4(void) {  
}

static int64_t zcap_sizze;
static void create_letbind_5(void) {  


  int64_t zgsz367;
  zgsz367 = INT64_C(16);
  zcap_sizze = zgsz367;

let_end_113: ;
}
static void kill_letbind_5(void) {  
}

static int64_t zlog2_cap_sizze;
static void create_letbind_6(void) {  


  int64_t zgsz368;
  zgsz368 = INT64_C(4);
  zlog2_cap_sizze = zgsz368;

let_end_114: ;
}
static void kill_letbind_6(void) {  
}

static int64_t zcap_hperms_width;
static void create_letbind_7(void) {  


  int64_t zgsz369;
  zgsz369 = INT64_C(12);
  zcap_hperms_width = zgsz369;

let_end_115: ;
}
static void kill_letbind_7(void) {  
}

static int64_t zcap_uperms_width;
static void create_letbind_8(void) {  


  int64_t zgsz370;
  zgsz370 = INT64_C(4);
  zcap_uperms_width = zgsz370;

let_end_116: ;
}
static void kill_letbind_8(void) {  
}

static int64_t zcap_otype_width;
static void create_letbind_9(void) {  


  int64_t zgsz371;
  zgsz371 = INT64_C(18);
  zcap_otype_width = zgsz371;

let_end_117: ;
}
static void kill_letbind_9(void) {  
}

static int64_t zcap_reserved_width;
static void create_letbind_10(void) {  


  int64_t zgsz372;
  zgsz372 = INT64_C(2);
  zcap_reserved_width = zgsz372;

let_end_118: ;
}
static void kill_letbind_10(void) {  
}

static int64_t zcap_flags_width;
static void create_letbind_11(void) {  


  int64_t zgsz373;
  zgsz373 = INT64_C(1);
  zcap_flags_width = zgsz373;

let_end_119: ;
}
static void kill_letbind_11(void) {  
}

static int64_t zcap_mantissa_width;
static void create_letbind_12(void) {  


  int64_t zgsz374;
  zgsz374 = INT64_C(14);
  zcap_mantissa_width = zgsz374;

let_end_120: ;
}
static void kill_letbind_12(void) {  
}

static int64_t zcap_E_width;
static void create_letbind_13(void) {  


  int64_t zgsz375;
  zgsz375 = INT64_C(6);
  zcap_E_width = zgsz375;

let_end_121: ;
}
static void kill_letbind_13(void) {  
}

static int64_t zcap_addr_width;
static void create_letbind_14(void) {  


  int64_t zgsz376;
  zgsz376 = INT64_C(64);
  zcap_addr_width = zgsz376;

let_end_122: ;
}
static void kill_letbind_14(void) {  
}

static int64_t zcap_len_width;
static void create_letbind_15(void) {  


  int64_t zgsz377;
  {
    sail_int zgsz378;
    CREATE(sail_int)(&zgsz378);
    CONVERT_OF(sail_int, mach_int)(&zgsz378, INT64_C(64));
    sail_int zgsz379;
    CREATE(sail_int)(&zgsz379);
    CONVERT_OF(sail_int, mach_int)(&zgsz379, INT64_C(1));
    sail_int zgsz380;
    CREATE(sail_int)(&zgsz380);
    add_int(&zgsz380, zgsz378, zgsz379);
    zgsz377 = CONVERT_OF(mach_int, sail_int)(zgsz380);
    KILL(sail_int)(&zgsz380);
    KILL(sail_int)(&zgsz379);
    KILL(sail_int)(&zgsz378);
  }
  zcap_len_width = zgsz377;

let_end_123: ;
}
static void kill_letbind_15(void) {  
}

static int64_t zcaps_per_cache_line;
static void create_letbind_16(void) {  


  int64_t zgsz381;
  zgsz381 = INT64_C(4);
  zcaps_per_cache_line = zgsz381;

let_end_124: ;
}
static void kill_letbind_16(void) {  
}

static int64_t zinternal_E_take_bits;
static void create_letbind_17(void) {  


  int64_t zgsz382;
  zgsz382 = INT64_C(3);
  zinternal_E_take_bits = zgsz382;

let_end_125: ;
}
static void kill_letbind_17(void) {  
}

static struct zEncCapability sailgen_capBitsToEncCapability(lbits);

static struct zEncCapability sailgen_capBitsToEncCapability(lbits zc)
{
  __label__ end_function_127, end_block_exception_128;

  struct zEncCapability zcbz333;
  uint64_t zgaz319;
  {
    sail_int zgsz383;
    CREATE(sail_int)(&zgsz383);
    CONVERT_OF(sail_int, mach_int)(&zgsz383, INT64_C(127));
    sail_int zgsz384;
    CREATE(sail_int)(&zgsz384);
    CONVERT_OF(sail_int, mach_int)(&zgsz384, INT64_C(112));
    lbits zgsz385;
    CREATE(lbits)(&zgsz385);
    vector_subrange_lbits(&zgsz385, zc, zgsz383, zgsz384);
    zgaz319 = CONVERT_OF(fbits, lbits)(zgsz385, true);
    KILL(lbits)(&zgsz385);
    KILL(sail_int)(&zgsz384);
    KILL(sail_int)(&zgsz383);
  }
  uint64_t zgaz320;
  {
    sail_int zgsz386;
    CREATE(sail_int)(&zgsz386);
    CONVERT_OF(sail_int, mach_int)(&zgsz386, INT64_C(111));
    sail_int zgsz387;
    CREATE(sail_int)(&zgsz387);
    CONVERT_OF(sail_int, mach_int)(&zgsz387, INT64_C(110));
    lbits zgsz388;
    CREATE(lbits)(&zgsz388);
    vector_subrange_lbits(&zgsz388, zc, zgsz386, zgsz387);
    zgaz320 = CONVERT_OF(fbits, lbits)(zgsz388, true);
    KILL(lbits)(&zgsz388);
    KILL(sail_int)(&zgsz387);
    KILL(sail_int)(&zgsz386);
  }
  uint64_t zgaz321;
  {
    sail_int zgsz389;
    CREATE(sail_int)(&zgsz389);
    CONVERT_OF(sail_int, mach_int)(&zgsz389, INT64_C(109));
    sail_int zgsz390;
    CREATE(sail_int)(&zgsz390);
    CONVERT_OF(sail_int, mach_int)(&zgsz390, INT64_C(109));
    lbits zgsz391;
    CREATE(lbits)(&zgsz391);
    vector_subrange_lbits(&zgsz391, zc, zgsz389, zgsz390);
    zgaz321 = CONVERT_OF(fbits, lbits)(zgsz391, true);
    KILL(lbits)(&zgsz391);
    KILL(sail_int)(&zgsz390);
    KILL(sail_int)(&zgsz389);
  }
  uint64_t zgaz322;
  {
    sail_int zgsz392;
    CREATE(sail_int)(&zgsz392);
    CONVERT_OF(sail_int, mach_int)(&zgsz392, INT64_C(108));
    sail_int zgsz393;
    CREATE(sail_int)(&zgsz393);
    CONVERT_OF(sail_int, mach_int)(&zgsz393, INT64_C(91));
    lbits zgsz394;
    CREATE(lbits)(&zgsz394);
    vector_subrange_lbits(&zgsz394, zc, zgsz392, zgsz393);
    zgaz322 = CONVERT_OF(fbits, lbits)(zgsz394, true);
    KILL(lbits)(&zgsz394);
    KILL(sail_int)(&zgsz393);
    KILL(sail_int)(&zgsz392);
  }
  fbits zgaz323;
  {
    sail_int zgsz395;
    CREATE(sail_int)(&zgsz395);
    CONVERT_OF(sail_int, mach_int)(&zgsz395, INT64_C(90));
    zgaz323 = bitvector_access(zc, zgsz395);
    KILL(sail_int)(&zgsz395);
  }
  uint64_t zgaz324;
  {
    sail_int zgsz396;
    CREATE(sail_int)(&zgsz396);
    CONVERT_OF(sail_int, mach_int)(&zgsz396, INT64_C(89));
    sail_int zgsz397;
    CREATE(sail_int)(&zgsz397);
    CONVERT_OF(sail_int, mach_int)(&zgsz397, INT64_C(78));
    lbits zgsz398;
    CREATE(lbits)(&zgsz398);
    vector_subrange_lbits(&zgsz398, zc, zgsz396, zgsz397);
    zgaz324 = CONVERT_OF(fbits, lbits)(zgsz398, true);
    KILL(lbits)(&zgsz398);
    KILL(sail_int)(&zgsz397);
    KILL(sail_int)(&zgsz396);
  }
  uint64_t zgaz325;
  {
    sail_int zgsz399;
    CREATE(sail_int)(&zgsz399);
    CONVERT_OF(sail_int, mach_int)(&zgsz399, INT64_C(77));
    sail_int zgsz3100;
    CREATE(sail_int)(&zgsz3100);
    CONVERT_OF(sail_int, mach_int)(&zgsz3100, INT64_C(64));
    lbits zgsz3101;
    CREATE(lbits)(&zgsz3101);
    vector_subrange_lbits(&zgsz3101, zc, zgsz399, zgsz3100);
    zgaz325 = CONVERT_OF(fbits, lbits)(zgsz3101, true);
    KILL(lbits)(&zgsz3101);
    KILL(sail_int)(&zgsz3100);
    KILL(sail_int)(&zgsz399);
  }
  uint64_t zgaz326;
  {
    sail_int zgsz3102;
    CREATE(sail_int)(&zgsz3102);
    CONVERT_OF(sail_int, mach_int)(&zgsz3102, INT64_C(63));
    sail_int zgsz3103;
    CREATE(sail_int)(&zgsz3103);
    CONVERT_OF(sail_int, mach_int)(&zgsz3103, INT64_C(0));
    lbits zgsz3104;
    CREATE(lbits)(&zgsz3104);
    vector_subrange_lbits(&zgsz3104, zc, zgsz3102, zgsz3103);
    zgaz326 = CONVERT_OF(fbits, lbits)(zgsz3104, true);
    KILL(lbits)(&zgsz3104);
    KILL(sail_int)(&zgsz3103);
    KILL(sail_int)(&zgsz3102);
  }
  struct zEncCapability zgsz3105;
  zgsz3105.zB = zgaz325;
  zgsz3105.zT = zgaz324;
  zgsz3105.zaddress = zgaz326;
  zgsz3105.zflags = zgaz321;
  zgsz3105.zinternal_E = zgaz323;
  zgsz3105.zotype = zgaz322;
  zgsz3105.zperms = zgaz319;
  zgsz3105.zreserved = zgaz320;
  zcbz333 = zgsz3105;









end_function_127: ;
  return zcbz333;
end_block_exception_128: ;
  struct zEncCapability zcbz360 = { .zreserved = UINT64_C(0xdeadc0de), .zperms = UINT64_C(0xdeadc0de), .zotype = UINT64_C(0xdeadc0de), .zinternal_E = UINT64_C(0), .zflags = UINT64_C(0xdeadc0de), .zaddress = UINT64_C(0xdeadc0de), .zT = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz360;
}

static void sailgen_encCapToBits(lbits *rop, struct zEncCapability);

static void sailgen_encCapToBits(lbits *zcbz334, struct zEncCapability zcap)
{
  __label__ end_function_130, end_block_exception_131, end_function_239;

  uint64_t zgaz340;
  zgaz340 = zcap.zperms;
  lbits zgaz341;
  CREATE(lbits)(&zgaz341);
  {
    uint64_t zgaz338;
    zgaz338 = zcap.zreserved;
    lbits zgaz339;
    CREATE(lbits)(&zgaz339);
    {
      uint64_t zgaz336;
      zgaz336 = zcap.zflags;
      lbits zgaz337;
      CREATE(lbits)(&zgaz337);
      {
        uint64_t zgaz334;
        zgaz334 = zcap.zotype;
        lbits zgaz335;
        CREATE(lbits)(&zgaz335);
        {
          uint64_t zgaz332;
          {
            fbits zgaz327;
            zgaz327 = zcap.zinternal_E;
            uint64_t zgsz3106;
            zgsz3106 = UINT64_C(0b0);
            zgsz3106 = update_fbits(zgsz3106, INT64_C(0), zgaz327);
            zgaz332 = zgsz3106;
          
          }
          lbits zgaz333;
          CREATE(lbits)(&zgaz333);
          {
            uint64_t zgaz330;
            zgaz330 = zcap.zT;
            lbits zgaz331;
            CREATE(lbits)(&zgaz331);
            {
              uint64_t zgaz328;
              zgaz328 = zcap.zB;
              uint64_t zgaz329;
              zgaz329 = zcap.zaddress;
              {
                lbits zgsz3108;
                CREATE(lbits)(&zgsz3108);
                CONVERT_OF(lbits, fbits)(&zgsz3108, zgaz329, UINT64_C(64) , true);
                lbits zgsz3107;
                CREATE(lbits)(&zgsz3107);
                CONVERT_OF(lbits, fbits)(&zgsz3107, zgaz328, UINT64_C(14) , true);
                append(&zgaz331, zgsz3107, zgsz3108);
                KILL(lbits)(&zgsz3108);
                KILL(lbits)(&zgsz3107);
              }
            
            
            }
            {
              lbits zgsz3109;
              CREATE(lbits)(&zgsz3109);
              CONVERT_OF(lbits, fbits)(&zgsz3109, zgaz330, UINT64_C(12) , true);
              append(&zgaz333, zgsz3109, zgaz331);
              KILL(lbits)(&zgsz3109);
            }
            KILL(lbits)(&zgaz331);
          
          }
          {
            lbits zgsz3110;
            CREATE(lbits)(&zgsz3110);
            CONVERT_OF(lbits, fbits)(&zgsz3110, zgaz332, UINT64_C(1) , true);
            append(&zgaz335, zgsz3110, zgaz333);
            KILL(lbits)(&zgsz3110);
          }
          KILL(lbits)(&zgaz333);
        
        }
        {
          lbits zgsz3111;
          CREATE(lbits)(&zgsz3111);
          CONVERT_OF(lbits, fbits)(&zgsz3111, zgaz334, UINT64_C(18) , true);
          append(&zgaz337, zgsz3111, zgaz335);
          KILL(lbits)(&zgsz3111);
        }
        KILL(lbits)(&zgaz335);
      
      }
      {
        lbits zgsz3112;
        CREATE(lbits)(&zgsz3112);
        CONVERT_OF(lbits, fbits)(&zgsz3112, zgaz336, UINT64_C(1) , true);
        append(&zgaz339, zgsz3112, zgaz337);
        KILL(lbits)(&zgsz3112);
      }
      KILL(lbits)(&zgaz337);
    
    }
    {
      lbits zgsz3113;
      CREATE(lbits)(&zgsz3113);
      CONVERT_OF(lbits, fbits)(&zgsz3113, zgaz338, UINT64_C(2) , true);
      append(&zgaz341, zgsz3113, zgaz339);
      KILL(lbits)(&zgsz3113);
    }
    KILL(lbits)(&zgaz339);
  
  }
  {
    lbits zgsz3114;
    CREATE(lbits)(&zgsz3114);
    CONVERT_OF(lbits, fbits)(&zgsz3114, zgaz340, UINT64_C(16) , true);
    append((*(&zcbz334)), zgsz3114, zgaz341);
    KILL(lbits)(&zgsz3114);
  }
  KILL(lbits)(&zgaz341);

end_function_130: ;
  goto end_function_239;
end_block_exception_131: ;
  goto end_function_239;
end_function_239: ;
}

static sail_int zcap_max_addr;
static void create_letbind_18(void) {    CREATE(sail_int)(&zcap_max_addr);


  sail_int zgsz3115;
  CREATE(sail_int)(&zgsz3115);
  sailgen_zzzz7nzzJzzK64z3MAX(&zgsz3115, zcap_addr_width);
  COPY(sail_int)(&zcap_max_addr, zgsz3115);
  KILL(sail_int)(&zgsz3115);
let_end_132: ;
}
static void kill_letbind_18(void) {    KILL(sail_int)(&zcap_max_addr);
}

static int64_t zcap_max_otype;
static void create_letbind_19(void) {  


  int64_t zgsz3116;
  int64_t zgaz342;
  zgaz342 = sailgen_zzzz7nzzJzzK18z3MAX(zcap_otype_width);
  {
    sail_int zgsz3117;
    CREATE(sail_int)(&zgsz3117);
    CONVERT_OF(sail_int, mach_int)(&zgsz3117, zgaz342);
    sail_int zgsz3118;
    CREATE(sail_int)(&zgsz3118);
    CONVERT_OF(sail_int, mach_int)(&zgsz3118, zreserved_otypes);
    sail_int zgsz3119;
    CREATE(sail_int)(&zgsz3119);
    sub_int(&zgsz3119, zgsz3117, zgsz3118);
    zgsz3116 = CONVERT_OF(mach_int, sail_int)(zgsz3119);
    KILL(sail_int)(&zgsz3119);
    KILL(sail_int)(&zgsz3118);
    KILL(sail_int)(&zgsz3117);
  }

  zcap_max_otype = zgsz3116;

let_end_133: ;
}
static void kill_letbind_19(void) {  
}

static int64_t zcap_uperms_shift;
static void create_letbind_20(void) {  


  int64_t zgsz3120;
  zgsz3120 = INT64_C(15);
  zcap_uperms_shift = zgsz3120;

let_end_134: ;
}
static void kill_letbind_20(void) {  
}

static int64_t zcap_perms_width;
static void create_letbind_21(void) {  


  int64_t zgsz3121;
  {
    sail_int zgsz3122;
    CREATE(sail_int)(&zgsz3122);
    CONVERT_OF(sail_int, mach_int)(&zgsz3122, INT64_C(15));
    sail_int zgsz3123;
    CREATE(sail_int)(&zgsz3123);
    CONVERT_OF(sail_int, mach_int)(&zgsz3123, INT64_C(4));
    sail_int zgsz3124;
    CREATE(sail_int)(&zgsz3124);
    add_int(&zgsz3124, zgsz3122, zgsz3123);
    zgsz3121 = CONVERT_OF(mach_int, sail_int)(zgsz3124);
    KILL(sail_int)(&zgsz3124);
    KILL(sail_int)(&zgsz3123);
    KILL(sail_int)(&zgsz3122);
  }
  zcap_perms_width = zgsz3121;

let_end_135: ;
}
static void kill_letbind_21(void) {  
}

static int64_t zcap_max_E;
static void create_letbind_22(void) {  


  int64_t zgsz3128;
  int64_t zgaz343;
  {
    sail_int zgsz3125;
    CREATE(sail_int)(&zgsz3125);
    CONVERT_OF(sail_int, mach_int)(&zgsz3125, zcap_len_width);
    sail_int zgsz3126;
    CREATE(sail_int)(&zgsz3126);
    CONVERT_OF(sail_int, mach_int)(&zgsz3126, zcap_mantissa_width);
    sail_int zgsz3127;
    CREATE(sail_int)(&zgsz3127);
    sub_int(&zgsz3127, zgsz3125, zgsz3126);
    zgaz343 = CONVERT_OF(mach_int, sail_int)(zgsz3127);
    KILL(sail_int)(&zgsz3127);
    KILL(sail_int)(&zgsz3126);
    KILL(sail_int)(&zgsz3125);
  }
  {
    sail_int zgsz3129;
    CREATE(sail_int)(&zgsz3129);
    CONVERT_OF(sail_int, mach_int)(&zgsz3129, zgaz343);
    sail_int zgsz3130;
    CREATE(sail_int)(&zgsz3130);
    CONVERT_OF(sail_int, mach_int)(&zgsz3130, INT64_C(1));
    sail_int zgsz3131;
    CREATE(sail_int)(&zgsz3131);
    add_int(&zgsz3131, zgsz3129, zgsz3130);
    zgsz3128 = CONVERT_OF(mach_int, sail_int)(zgsz3131);
    KILL(sail_int)(&zgsz3131);
    KILL(sail_int)(&zgsz3130);
    KILL(sail_int)(&zgsz3129);
  }

  zcap_max_E = zgsz3128;

let_end_136: ;
}
static void kill_letbind_22(void) {  
}

static uint64_t zcap_reset_E;
static void create_letbind_23(void) {  


  uint64_t zgsz3132;
  {
    sail_int zgsz3133;
    CREATE(sail_int)(&zgsz3133);
    CONVERT_OF(sail_int, mach_int)(&zgsz3133, zcap_E_width);
    sail_int zgsz3134;
    CREATE(sail_int)(&zgsz3134);
    CONVERT_OF(sail_int, mach_int)(&zgsz3134, zcap_max_E);
    lbits zgsz3135;
    CREATE(lbits)(&zgsz3135);
    sailgen_to_bits(&zgsz3135, zgsz3133, zgsz3134);
    zgsz3132 = CONVERT_OF(fbits, lbits)(zgsz3135, true);
    KILL(lbits)(&zgsz3135);
    KILL(sail_int)(&zgsz3134);
    KILL(sail_int)(&zgsz3133);
  }
  zcap_reset_E = zgsz3132;

let_end_137: ;
}
static void kill_letbind_23(void) {  
}

static uint64_t zcap_reset_T;
static void create_letbind_24(void) {  


  uint64_t zgsz3139;
  uint64_t zgaz345;
  {
    int64_t zgaz344;
    {
      sail_int zgsz3136;
      CREATE(sail_int)(&zgsz3136);
      CONVERT_OF(sail_int, mach_int)(&zgsz3136, zcap_mantissa_width);
      sail_int zgsz3137;
      CREATE(sail_int)(&zgsz3137);
      CONVERT_OF(sail_int, mach_int)(&zgsz3137, INT64_C(2));
      sail_int zgsz3138;
      CREATE(sail_int)(&zgsz3138);
      sub_int(&zgsz3138, zgsz3136, zgsz3137);
      zgaz344 = CONVERT_OF(mach_int, sail_int)(zgsz3138);
      KILL(sail_int)(&zgsz3138);
      KILL(sail_int)(&zgsz3137);
      KILL(sail_int)(&zgsz3136);
    }
    zgaz345 = sailgen_zzzz7nzzJzzK12z3zzeros_implicit(zgaz344);
  
  }
  {
    lbits zgsz3140;
    CREATE(lbits)(&zgsz3140);
    CONVERT_OF(lbits, fbits)(&zgsz3140, UINT64_C(0b01), UINT64_C(2) , true);
    lbits zgsz3141;
    CREATE(lbits)(&zgsz3141);
    CONVERT_OF(lbits, fbits)(&zgsz3141, zgaz345, UINT64_C(12) , true);
    lbits zgsz3142;
    CREATE(lbits)(&zgsz3142);
    append(&zgsz3142, zgsz3140, zgsz3141);
    zgsz3139 = CONVERT_OF(fbits, lbits)(zgsz3142, true);
    KILL(lbits)(&zgsz3142);
    KILL(lbits)(&zgsz3141);
    KILL(lbits)(&zgsz3140);
  }

  zcap_reset_T = zgsz3139;

let_end_138: ;
}
static void kill_letbind_24(void) {  
}

static struct zCapability sailgen_undefined_Capability(unit);

static struct zCapability sailgen_undefined_Capability(unit zgsz3143)
{
  __label__ end_function_140, end_block_exception_141;

  struct zCapability zcbz335;
  bool zgaz346;
  zgaz346 = undefined_bool(UNIT);
  uint64_t zgaz347;
  {
    sail_int zgsz3144;
    CREATE(sail_int)(&zgsz3144);
    CONVERT_OF(sail_int, mach_int)(&zgsz3144, INT64_C(4));
    lbits zgsz3145;
    CREATE(lbits)(&zgsz3145);
    UNDEFINED(lbits)(&zgsz3145, zgsz3144);
    zgaz347 = CONVERT_OF(fbits, lbits)(zgsz3145, true);
    KILL(lbits)(&zgsz3145);
    KILL(sail_int)(&zgsz3144);
  }
  bool zgaz348;
  zgaz348 = undefined_bool(UNIT);
  bool zgaz349;
  zgaz349 = undefined_bool(UNIT);
  bool zgaz350;
  zgaz350 = undefined_bool(UNIT);
  bool zgaz351;
  zgaz351 = undefined_bool(UNIT);
  bool zgaz352;
  zgaz352 = undefined_bool(UNIT);
  bool zgaz353;
  zgaz353 = undefined_bool(UNIT);
  bool zgaz354;
  zgaz354 = undefined_bool(UNIT);
  bool zgaz355;
  zgaz355 = undefined_bool(UNIT);
  bool zgaz356;
  zgaz356 = undefined_bool(UNIT);
  bool zgaz357;
  zgaz357 = undefined_bool(UNIT);
  bool zgaz358;
  zgaz358 = undefined_bool(UNIT);
  bool zgaz359;
  zgaz359 = undefined_bool(UNIT);
  uint64_t zgaz360;
  {
    sail_int zgsz3146;
    CREATE(sail_int)(&zgsz3146);
    CONVERT_OF(sail_int, mach_int)(&zgsz3146, INT64_C(2));
    lbits zgsz3147;
    CREATE(lbits)(&zgsz3147);
    UNDEFINED(lbits)(&zgsz3147, zgsz3146);
    zgaz360 = CONVERT_OF(fbits, lbits)(zgsz3147, true);
    KILL(lbits)(&zgsz3147);
    KILL(sail_int)(&zgsz3146);
  }
  bool zgaz361;
  zgaz361 = undefined_bool(UNIT);
  bool zgaz362;
  zgaz362 = undefined_bool(UNIT);
  uint64_t zgaz363;
  {
    sail_int zgsz3148;
    CREATE(sail_int)(&zgsz3148);
    CONVERT_OF(sail_int, mach_int)(&zgsz3148, INT64_C(6));
    lbits zgsz3149;
    CREATE(lbits)(&zgsz3149);
    UNDEFINED(lbits)(&zgsz3149, zgsz3148);
    zgaz363 = CONVERT_OF(fbits, lbits)(zgsz3149, true);
    KILL(lbits)(&zgsz3149);
    KILL(sail_int)(&zgsz3148);
  }
  uint64_t zgaz364;
  {
    sail_int zgsz3150;
    CREATE(sail_int)(&zgsz3150);
    CONVERT_OF(sail_int, mach_int)(&zgsz3150, INT64_C(14));
    lbits zgsz3151;
    CREATE(lbits)(&zgsz3151);
    UNDEFINED(lbits)(&zgsz3151, zgsz3150);
    zgaz364 = CONVERT_OF(fbits, lbits)(zgsz3151, true);
    KILL(lbits)(&zgsz3151);
    KILL(sail_int)(&zgsz3150);
  }
  uint64_t zgaz365;
  {
    sail_int zgsz3152;
    CREATE(sail_int)(&zgsz3152);
    CONVERT_OF(sail_int, mach_int)(&zgsz3152, INT64_C(14));
    lbits zgsz3153;
    CREATE(lbits)(&zgsz3153);
    UNDEFINED(lbits)(&zgsz3153, zgsz3152);
    zgaz365 = CONVERT_OF(fbits, lbits)(zgsz3153, true);
    KILL(lbits)(&zgsz3153);
    KILL(sail_int)(&zgsz3152);
  }
  uint64_t zgaz366;
  {
    sail_int zgsz3154;
    CREATE(sail_int)(&zgsz3154);
    CONVERT_OF(sail_int, mach_int)(&zgsz3154, INT64_C(18));
    lbits zgsz3155;
    CREATE(lbits)(&zgsz3155);
    UNDEFINED(lbits)(&zgsz3155, zgsz3154);
    zgaz366 = CONVERT_OF(fbits, lbits)(zgsz3155, true);
    KILL(lbits)(&zgsz3155);
    KILL(sail_int)(&zgsz3154);
  }
  uint64_t zgaz367;
  {
    sail_int zgsz3156;
    CREATE(sail_int)(&zgsz3156);
    CONVERT_OF(sail_int, mach_int)(&zgsz3156, INT64_C(64));
    lbits zgsz3157;
    CREATE(lbits)(&zgsz3157);
    UNDEFINED(lbits)(&zgsz3157, zgsz3156);
    zgaz367 = CONVERT_OF(fbits, lbits)(zgsz3157, true);
    KILL(lbits)(&zgsz3157);
    KILL(sail_int)(&zgsz3156);
  }
  struct zCapability zgsz3158;
  zgsz3158.zB = zgaz364;
  zgsz3158.zE = zgaz363;
  zgsz3158.zT = zgaz365;
  zgsz3158.zaccess_system_regs = zgaz349;
  zgsz3158.zaddress = zgaz367;
  zgsz3158.zflag_cap_mode = zgaz361;
  zgsz3158.zglobal = zgaz359;
  zgsz3158.zinternal_E = zgaz362;
  zgsz3158.zotype = zgaz366;
  zgsz3158.zpermit_cinvoke = zgaz351;
  zgsz3158.zpermit_execute = zgaz358;
  zgsz3158.zpermit_load = zgaz357;
  zgsz3158.zpermit_load_cap = zgaz355;
  zgsz3158.zpermit_seal = zgaz352;
  zgsz3158.zpermit_set_CID = zgaz348;
  zgsz3158.zpermit_store = zgaz356;
  zgsz3158.zpermit_store_cap = zgaz354;
  zgsz3158.zpermit_store_local_cap = zgaz353;
  zgsz3158.zpermit_unseal = zgaz350;
  zgsz3158.zreserved = zgaz360;
  zgsz3158.ztag = zgaz346;
  zgsz3158.zuperms = zgaz347;
  zcbz335 = zgsz3158;























end_function_140: ;
  return zcbz335;
end_block_exception_141: ;
  struct zCapability zcbz361 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz361;
}

static struct zCapability znull_cap;
static void create_letbind_25(void) {  


  struct zCapability zgsz3163;
  uint64_t zgaz368;
  zgaz368 = sailgen_zzzz7nzzJzzK4z3zzeros_implicit(INT64_C(4));
  uint64_t zgaz369;
  zgaz369 = sailgen_zzzz7nzzJzzK2z3zzeros_implicit(INT64_C(2));
  uint64_t zgaz370;
  zgaz370 = sailgen_zzzz7nzzJzzK14z3zzeros_implicit(INT64_C(14));
  uint64_t zgaz371;
  {
    sail_int zgsz3159;
    CREATE(sail_int)(&zgsz3159);
    CONVERT_OF(sail_int, mach_int)(&zgsz3159, zcap_otype_width);
    sail_int zgsz3160;
    CREATE(sail_int)(&zgsz3160);
    CONVERT_OF(sail_int, mach_int)(&zgsz3160, zotype_unsealed);
    lbits zgsz3161;
    CREATE(lbits)(&zgsz3161);
    sailgen_to_bits(&zgsz3161, zgsz3159, zgsz3160);
    zgaz371 = CONVERT_OF(fbits, lbits)(zgsz3161, true);
    KILL(lbits)(&zgsz3161);
    KILL(sail_int)(&zgsz3160);
    KILL(sail_int)(&zgsz3159);
  }
  uint64_t zgaz372;
  zgaz372 = sailgen_zzzz7nzzJzzK64z3zzeros_implicit(INT64_C(64));
  struct zCapability zgsz3162;
  zgsz3162.zB = zgaz370;
  zgsz3162.zE = zcap_reset_E;
  zgsz3162.zT = zcap_reset_T;
  zgsz3162.zaccess_system_regs = false;
  zgsz3162.zaddress = zgaz372;
  zgsz3162.zflag_cap_mode = false;
  zgsz3162.zglobal = false;
  zgsz3162.zinternal_E = true;
  zgsz3162.zotype = zgaz371;
  zgsz3162.zpermit_cinvoke = false;
  zgsz3162.zpermit_execute = false;
  zgsz3162.zpermit_load = false;
  zgsz3162.zpermit_load_cap = false;
  zgsz3162.zpermit_seal = false;
  zgsz3162.zpermit_set_CID = false;
  zgsz3162.zpermit_store = false;
  zgsz3162.zpermit_store_cap = false;
  zgsz3162.zpermit_store_local_cap = false;
  zgsz3162.zpermit_unseal = false;
  zgsz3162.zreserved = zgaz369;
  zgsz3162.ztag = false;
  zgsz3162.zuperms = zgaz368;
  zgsz3163 = zgsz3162;






  znull_cap = zgsz3163;

let_end_142: ;
}
static void kill_letbind_25(void) {  
}

static struct zCapability zdefault_cap;
static void create_letbind_26(void) {  


  struct zCapability zgsz3168;
  uint64_t zgaz373;
  zgaz373 = sailgen_zzzz7nzzJzzK4z3ones(INT64_C(4));
  uint64_t zgaz374;
  zgaz374 = sailgen_zzzz7nzzJzzK2z3zzeros_implicit(INT64_C(2));
  uint64_t zgaz375;
  zgaz375 = sailgen_zzzz7nzzJzzK14z3zzeros_implicit(INT64_C(14));
  uint64_t zgaz376;
  {
    sail_int zgsz3164;
    CREATE(sail_int)(&zgsz3164);
    CONVERT_OF(sail_int, mach_int)(&zgsz3164, zcap_otype_width);
    sail_int zgsz3165;
    CREATE(sail_int)(&zgsz3165);
    CONVERT_OF(sail_int, mach_int)(&zgsz3165, zotype_unsealed);
    lbits zgsz3166;
    CREATE(lbits)(&zgsz3166);
    sailgen_to_bits(&zgsz3166, zgsz3164, zgsz3165);
    zgaz376 = CONVERT_OF(fbits, lbits)(zgsz3166, true);
    KILL(lbits)(&zgsz3166);
    KILL(sail_int)(&zgsz3165);
    KILL(sail_int)(&zgsz3164);
  }
  uint64_t zgaz377;
  zgaz377 = sailgen_zzzz7nzzJzzK64z3zzeros_implicit(INT64_C(64));
  struct zCapability zgsz3167;
  zgsz3167.zB = zgaz375;
  zgsz3167.zE = zcap_reset_E;
  zgsz3167.zT = zcap_reset_T;
  zgsz3167.zaccess_system_regs = true;
  zgsz3167.zaddress = zgaz377;
  zgsz3167.zflag_cap_mode = false;
  zgsz3167.zglobal = true;
  zgsz3167.zinternal_E = true;
  zgsz3167.zotype = zgaz376;
  zgsz3167.zpermit_cinvoke = true;
  zgsz3167.zpermit_execute = true;
  zgsz3167.zpermit_load = true;
  zgsz3167.zpermit_load_cap = true;
  zgsz3167.zpermit_seal = true;
  zgsz3167.zpermit_set_CID = true;
  zgsz3167.zpermit_store = true;
  zgsz3167.zpermit_store_cap = true;
  zgsz3167.zpermit_store_local_cap = true;
  zgsz3167.zpermit_unseal = true;
  zgsz3167.zreserved = zgaz374;
  zgsz3167.ztag = true;
  zgsz3167.zuperms = zgaz373;
  zgsz3168 = zgsz3167;






  zdefault_cap = zgsz3168;

let_end_143: ;
}
static void kill_letbind_26(void) {  
}

static uint64_t sailgen_getCapHardPerms(struct zCapability);

static uint64_t sailgen_getCapHardPerms(struct zCapability zcap)
{
  __label__ end_function_145, end_block_exception_146;

  uint64_t zcbz336;
  uint64_t zgaz3110;
  {
    bool zgaz378;
    zgaz378 = zcap.zpermit_set_CID;
    zgaz3110 = sailgen_bool_to_bits(zgaz378);
  
  }
  uint64_t zgaz3111;
  {
    uint64_t zgaz3108;
    {
      bool zgaz379;
      zgaz379 = zcap.zaccess_system_regs;
      zgaz3108 = sailgen_bool_to_bits(zgaz379);
    
    }
    uint64_t zgaz3109;
    {
      uint64_t zgaz3106;
      {
        bool zgaz380;
        zgaz380 = zcap.zpermit_unseal;
        zgaz3106 = sailgen_bool_to_bits(zgaz380);
      
      }
      uint64_t zgaz3107;
      {
        uint64_t zgaz3104;
        {
          bool zgaz381;
          zgaz381 = zcap.zpermit_cinvoke;
          zgaz3104 = sailgen_bool_to_bits(zgaz381);
        
        }
        uint64_t zgaz3105;
        {
          uint64_t zgaz3102;
          {
            bool zgaz382;
            zgaz382 = zcap.zpermit_seal;
            zgaz3102 = sailgen_bool_to_bits(zgaz382);
          
          }
          uint64_t zgaz3103;
          {
            uint64_t zgaz3100;
            {
              bool zgaz383;
              zgaz383 = zcap.zpermit_store_local_cap;
              zgaz3100 = sailgen_bool_to_bits(zgaz383);
            
            }
            uint64_t zgaz3101;
            {
              uint64_t zgaz398;
              {
                bool zgaz384;
                zgaz384 = zcap.zpermit_store_cap;
                zgaz398 = sailgen_bool_to_bits(zgaz384);
              
              }
              uint64_t zgaz399;
              {
                uint64_t zgaz396;
                {
                  bool zgaz385;
                  zgaz385 = zcap.zpermit_load_cap;
                  zgaz396 = sailgen_bool_to_bits(zgaz385);
                
                }
                uint64_t zgaz397;
                {
                  uint64_t zgaz394;
                  {
                    bool zgaz386;
                    zgaz386 = zcap.zpermit_store;
                    zgaz394 = sailgen_bool_to_bits(zgaz386);
                  
                  }
                  uint64_t zgaz395;
                  {
                    uint64_t zgaz392;
                    {
                      bool zgaz387;
                      zgaz387 = zcap.zpermit_load;
                      zgaz392 = sailgen_bool_to_bits(zgaz387);
                    
                    }
                    uint64_t zgaz393;
                    {
                      uint64_t zgaz390;
                      {
                        bool zgaz388;
                        zgaz388 = zcap.zpermit_execute;
                        zgaz390 = sailgen_bool_to_bits(zgaz388);
                      
                      }
                      uint64_t zgaz391;
                      {
                        bool zgaz389;
                        zgaz389 = zcap.zglobal;
                        zgaz391 = sailgen_bool_to_bits(zgaz389);
                      
                      }
                      {
                        lbits zgsz3169;
                        CREATE(lbits)(&zgsz3169);
                        CONVERT_OF(lbits, fbits)(&zgsz3169, zgaz390, UINT64_C(1) , true);
                        lbits zgsz3170;
                        CREATE(lbits)(&zgsz3170);
                        CONVERT_OF(lbits, fbits)(&zgsz3170, zgaz391, UINT64_C(1) , true);
                        lbits zgsz3171;
                        CREATE(lbits)(&zgsz3171);
                        append(&zgsz3171, zgsz3169, zgsz3170);
                        zgaz393 = CONVERT_OF(fbits, lbits)(zgsz3171, true);
                        KILL(lbits)(&zgsz3171);
                        KILL(lbits)(&zgsz3170);
                        KILL(lbits)(&zgsz3169);
                      }
                    
                    
                    }
                    {
                      lbits zgsz3172;
                      CREATE(lbits)(&zgsz3172);
                      CONVERT_OF(lbits, fbits)(&zgsz3172, zgaz392, UINT64_C(1) , true);
                      lbits zgsz3173;
                      CREATE(lbits)(&zgsz3173);
                      CONVERT_OF(lbits, fbits)(&zgsz3173, zgaz393, UINT64_C(2) , true);
                      lbits zgsz3174;
                      CREATE(lbits)(&zgsz3174);
                      append(&zgsz3174, zgsz3172, zgsz3173);
                      zgaz395 = CONVERT_OF(fbits, lbits)(zgsz3174, true);
                      KILL(lbits)(&zgsz3174);
                      KILL(lbits)(&zgsz3173);
                      KILL(lbits)(&zgsz3172);
                    }
                  
                  
                  }
                  {
                    lbits zgsz3175;
                    CREATE(lbits)(&zgsz3175);
                    CONVERT_OF(lbits, fbits)(&zgsz3175, zgaz394, UINT64_C(1) , true);
                    lbits zgsz3176;
                    CREATE(lbits)(&zgsz3176);
                    CONVERT_OF(lbits, fbits)(&zgsz3176, zgaz395, UINT64_C(3) , true);
                    lbits zgsz3177;
                    CREATE(lbits)(&zgsz3177);
                    append(&zgsz3177, zgsz3175, zgsz3176);
                    zgaz397 = CONVERT_OF(fbits, lbits)(zgsz3177, true);
                    KILL(lbits)(&zgsz3177);
                    KILL(lbits)(&zgsz3176);
                    KILL(lbits)(&zgsz3175);
                  }
                
                
                }
                {
                  lbits zgsz3178;
                  CREATE(lbits)(&zgsz3178);
                  CONVERT_OF(lbits, fbits)(&zgsz3178, zgaz396, UINT64_C(1) , true);
                  lbits zgsz3179;
                  CREATE(lbits)(&zgsz3179);
                  CONVERT_OF(lbits, fbits)(&zgsz3179, zgaz397, UINT64_C(4) , true);
                  lbits zgsz3180;
                  CREATE(lbits)(&zgsz3180);
                  append(&zgsz3180, zgsz3178, zgsz3179);
                  zgaz399 = CONVERT_OF(fbits, lbits)(zgsz3180, true);
                  KILL(lbits)(&zgsz3180);
                  KILL(lbits)(&zgsz3179);
                  KILL(lbits)(&zgsz3178);
                }
              
              
              }
              {
                lbits zgsz3181;
                CREATE(lbits)(&zgsz3181);
                CONVERT_OF(lbits, fbits)(&zgsz3181, zgaz398, UINT64_C(1) , true);
                lbits zgsz3182;
                CREATE(lbits)(&zgsz3182);
                CONVERT_OF(lbits, fbits)(&zgsz3182, zgaz399, UINT64_C(5) , true);
                lbits zgsz3183;
                CREATE(lbits)(&zgsz3183);
                append(&zgsz3183, zgsz3181, zgsz3182);
                zgaz3101 = CONVERT_OF(fbits, lbits)(zgsz3183, true);
                KILL(lbits)(&zgsz3183);
                KILL(lbits)(&zgsz3182);
                KILL(lbits)(&zgsz3181);
              }
            
            
            }
            {
              lbits zgsz3184;
              CREATE(lbits)(&zgsz3184);
              CONVERT_OF(lbits, fbits)(&zgsz3184, zgaz3100, UINT64_C(1) , true);
              lbits zgsz3185;
              CREATE(lbits)(&zgsz3185);
              CONVERT_OF(lbits, fbits)(&zgsz3185, zgaz3101, UINT64_C(6) , true);
              lbits zgsz3186;
              CREATE(lbits)(&zgsz3186);
              append(&zgsz3186, zgsz3184, zgsz3185);
              zgaz3103 = CONVERT_OF(fbits, lbits)(zgsz3186, true);
              KILL(lbits)(&zgsz3186);
              KILL(lbits)(&zgsz3185);
              KILL(lbits)(&zgsz3184);
            }
          
          
          }
          {
            lbits zgsz3187;
            CREATE(lbits)(&zgsz3187);
            CONVERT_OF(lbits, fbits)(&zgsz3187, zgaz3102, UINT64_C(1) , true);
            lbits zgsz3188;
            CREATE(lbits)(&zgsz3188);
            CONVERT_OF(lbits, fbits)(&zgsz3188, zgaz3103, UINT64_C(7) , true);
            lbits zgsz3189;
            CREATE(lbits)(&zgsz3189);
            append(&zgsz3189, zgsz3187, zgsz3188);
            zgaz3105 = CONVERT_OF(fbits, lbits)(zgsz3189, true);
            KILL(lbits)(&zgsz3189);
            KILL(lbits)(&zgsz3188);
            KILL(lbits)(&zgsz3187);
          }
        
        
        }
        {
          lbits zgsz3190;
          CREATE(lbits)(&zgsz3190);
          CONVERT_OF(lbits, fbits)(&zgsz3190, zgaz3104, UINT64_C(1) , true);
          lbits zgsz3191;
          CREATE(lbits)(&zgsz3191);
          CONVERT_OF(lbits, fbits)(&zgsz3191, zgaz3105, UINT64_C(8) , true);
          lbits zgsz3192;
          CREATE(lbits)(&zgsz3192);
          append(&zgsz3192, zgsz3190, zgsz3191);
          zgaz3107 = CONVERT_OF(fbits, lbits)(zgsz3192, true);
          KILL(lbits)(&zgsz3192);
          KILL(lbits)(&zgsz3191);
          KILL(lbits)(&zgsz3190);
        }
      
      
      }
      {
        lbits zgsz3193;
        CREATE(lbits)(&zgsz3193);
        CONVERT_OF(lbits, fbits)(&zgsz3193, zgaz3106, UINT64_C(1) , true);
        lbits zgsz3194;
        CREATE(lbits)(&zgsz3194);
        CONVERT_OF(lbits, fbits)(&zgsz3194, zgaz3107, UINT64_C(9) , true);
        lbits zgsz3195;
        CREATE(lbits)(&zgsz3195);
        append(&zgsz3195, zgsz3193, zgsz3194);
        zgaz3109 = CONVERT_OF(fbits, lbits)(zgsz3195, true);
        KILL(lbits)(&zgsz3195);
        KILL(lbits)(&zgsz3194);
        KILL(lbits)(&zgsz3193);
      }
    
    
    }
    {
      lbits zgsz3196;
      CREATE(lbits)(&zgsz3196);
      CONVERT_OF(lbits, fbits)(&zgsz3196, zgaz3108, UINT64_C(1) , true);
      lbits zgsz3197;
      CREATE(lbits)(&zgsz3197);
      CONVERT_OF(lbits, fbits)(&zgsz3197, zgaz3109, UINT64_C(10) , true);
      lbits zgsz3198;
      CREATE(lbits)(&zgsz3198);
      append(&zgsz3198, zgsz3196, zgsz3197);
      zgaz3111 = CONVERT_OF(fbits, lbits)(zgsz3198, true);
      KILL(lbits)(&zgsz3198);
      KILL(lbits)(&zgsz3197);
      KILL(lbits)(&zgsz3196);
    }
  
  
  }
  {
    lbits zgsz3199;
    CREATE(lbits)(&zgsz3199);
    CONVERT_OF(lbits, fbits)(&zgsz3199, zgaz3110, UINT64_C(1) , true);
    lbits zgsz3200;
    CREATE(lbits)(&zgsz3200);
    CONVERT_OF(lbits, fbits)(&zgsz3200, zgaz3111, UINT64_C(11) , true);
    lbits zgsz3201;
    CREATE(lbits)(&zgsz3201);
    append(&zgsz3201, zgsz3199, zgsz3200);
    zcbz336 = CONVERT_OF(fbits, lbits)(zgsz3201, true);
    KILL(lbits)(&zgsz3201);
    KILL(lbits)(&zgsz3200);
    KILL(lbits)(&zgsz3199);
  }


end_function_145: ;
  return zcbz336;
end_block_exception_146: ;

  return UINT64_C(0xdeadc0de);
}

static struct zCapability sailgen_encCapabilityToCapability(bool, struct zEncCapability);

static struct zCapability sailgen_encCapabilityToCapability(bool zt, struct zEncCapability zc)
{
  __label__ cleanup_149, end_cleanup_150, end_function_148, end_block_exception_151;

  struct zCapability zcbz337;
  bool zinternal_E;
  {
    fbits zgaz3183;
    zgaz3183 = zc.zinternal_E;
    zinternal_E = sailgen_bit_to_bool(zgaz3183);
  
  }
  uint64_t zE;
  zE = sailgen_zzzz7nzzJzzK6z3zzeros_implicit(INT64_C(6));
  uint64_t zBs;
  zBs = sailgen_zzzz7nzzJzzK14z3zzeros_implicit(INT64_C(14));
  uint64_t zT;
  {
    int64_t zgaz3182;
    {
      sail_int zgsz3202;
      CREATE(sail_int)(&zgsz3202);
      CONVERT_OF(sail_int, mach_int)(&zgsz3202, INT64_C(14));
      sail_int zgsz3203;
      CREATE(sail_int)(&zgsz3203);
      CONVERT_OF(sail_int, mach_int)(&zgsz3203, INT64_C(2));
      sail_int zgsz3204;
      CREATE(sail_int)(&zgsz3204);
      sub_int(&zgsz3204, zgsz3202, zgsz3203);
      zgaz3182 = CONVERT_OF(mach_int, sail_int)(zgsz3204);
      KILL(sail_int)(&zgsz3204);
      KILL(sail_int)(&zgsz3203);
      KILL(sail_int)(&zgsz3202);
    }
    {
      sail_int zgsz3205;
      CREATE(sail_int)(&zgsz3205);
      CONVERT_OF(sail_int, mach_int)(&zgsz3205, zgaz3182);
      lbits zgsz3206;
      CREATE(lbits)(&zgsz3206);
      sailgen_zzz3zzz3zzeros_implicit(&zgsz3206, zgsz3205);
      zT = CONVERT_OF(fbits, lbits)(zgsz3206, true);
      KILL(lbits)(&zgsz3206);
      KILL(sail_int)(&zgsz3205);
    }
  
  }
  uint64_t zlenMSBs;
  zlenMSBs = sailgen_zzzz7nzzJzzK2z3zzeros_implicit(INT64_C(2));
  {
    unit zgsz3207;
    if (zinternal_E) {
    {
      uint64_t zgaz3116;
      {
        uint64_t zgaz3112;
        zgaz3112 = zc.zT;
        int64_t zgaz3113;
        {
          sail_int zgsz3210;
          CREATE(sail_int)(&zgsz3210);
          CONVERT_OF(sail_int, mach_int)(&zgsz3210, zinternal_E_take_bits);
          sail_int zgsz3211;
          CREATE(sail_int)(&zgsz3211);
          CONVERT_OF(sail_int, mach_int)(&zgsz3211, INT64_C(1));
          sail_int zgsz3212;
          CREATE(sail_int)(&zgsz3212);
          sub_int(&zgsz3212, zgsz3210, zgsz3211);
          zgaz3113 = CONVERT_OF(mach_int, sail_int)(zgsz3212);
          KILL(sail_int)(&zgsz3212);
          KILL(sail_int)(&zgsz3211);
          KILL(sail_int)(&zgsz3210);
        }
        {
          lbits zgsz3213;
          CREATE(lbits)(&zgsz3213);
          CONVERT_OF(lbits, fbits)(&zgsz3213, zgaz3112, UINT64_C(12) , true);
          sail_int zgsz3214;
          CREATE(sail_int)(&zgsz3214);
          CONVERT_OF(sail_int, mach_int)(&zgsz3214, zgaz3113);
          sail_int zgsz3215;
          CREATE(sail_int)(&zgsz3215);
          CONVERT_OF(sail_int, mach_int)(&zgsz3215, INT64_C(0));
          lbits zgsz3216;
          CREATE(lbits)(&zgsz3216);
          vector_subrange_lbits(&zgsz3216, zgsz3213, zgsz3214, zgsz3215);
          zgaz3116 = CONVERT_OF(fbits, lbits)(zgsz3216, true);
          KILL(lbits)(&zgsz3216);
          KILL(sail_int)(&zgsz3215);
          KILL(sail_int)(&zgsz3214);
          KILL(lbits)(&zgsz3213);
        }
      
      
      }
      uint64_t zgaz3117;
      {
        uint64_t zgaz3114;
        zgaz3114 = zc.zB;
        int64_t zgaz3115;
        {
          sail_int zgsz3217;
          CREATE(sail_int)(&zgsz3217);
          CONVERT_OF(sail_int, mach_int)(&zgsz3217, zinternal_E_take_bits);
          sail_int zgsz3218;
          CREATE(sail_int)(&zgsz3218);
          CONVERT_OF(sail_int, mach_int)(&zgsz3218, INT64_C(1));
          sail_int zgsz3219;
          CREATE(sail_int)(&zgsz3219);
          sub_int(&zgsz3219, zgsz3217, zgsz3218);
          zgaz3115 = CONVERT_OF(mach_int, sail_int)(zgsz3219);
          KILL(sail_int)(&zgsz3219);
          KILL(sail_int)(&zgsz3218);
          KILL(sail_int)(&zgsz3217);
        }
        {
          lbits zgsz3220;
          CREATE(lbits)(&zgsz3220);
          CONVERT_OF(lbits, fbits)(&zgsz3220, zgaz3114, UINT64_C(14) , true);
          sail_int zgsz3221;
          CREATE(sail_int)(&zgsz3221);
          CONVERT_OF(sail_int, mach_int)(&zgsz3221, zgaz3115);
          sail_int zgsz3222;
          CREATE(sail_int)(&zgsz3222);
          CONVERT_OF(sail_int, mach_int)(&zgsz3222, INT64_C(0));
          lbits zgsz3223;
          CREATE(lbits)(&zgsz3223);
          vector_subrange_lbits(&zgsz3223, zgsz3220, zgsz3221, zgsz3222);
          zgaz3117 = CONVERT_OF(fbits, lbits)(zgsz3223, true);
          KILL(lbits)(&zgsz3223);
          KILL(sail_int)(&zgsz3222);
          KILL(sail_int)(&zgsz3221);
          KILL(lbits)(&zgsz3220);
        }
      
      
      }
      {
        lbits zgsz3224;
        CREATE(lbits)(&zgsz3224);
        CONVERT_OF(lbits, fbits)(&zgsz3224, zgaz3116, UINT64_C(3) , true);
        lbits zgsz3225;
        CREATE(lbits)(&zgsz3225);
        CONVERT_OF(lbits, fbits)(&zgsz3225, zgaz3117, UINT64_C(3) , true);
        lbits zgsz3226;
        CREATE(lbits)(&zgsz3226);
        append(&zgsz3226, zgsz3224, zgsz3225);
        zE = CONVERT_OF(fbits, lbits)(zgsz3226, true);
        KILL(lbits)(&zgsz3226);
        KILL(lbits)(&zgsz3225);
        KILL(lbits)(&zgsz3224);
      }
      unit zgsz3239;
      zgsz3239 = UNIT;
    
    
    }
    {
      zlenMSBs = UINT64_C(0b01);
      unit zgsz3238;
      zgsz3238 = UNIT;
    }
    {
      uint64_t zgaz3120;
      {
        uint64_t zgaz3118;
        zgaz3118 = zc.zT;
        int64_t zgaz3119;
        {
          sail_int zgsz3227;
          CREATE(sail_int)(&zgsz3227);
          CONVERT_OF(sail_int, mach_int)(&zgsz3227, zcap_mantissa_width);
          sail_int zgsz3228;
          CREATE(sail_int)(&zgsz3228);
          CONVERT_OF(sail_int, mach_int)(&zgsz3228, INT64_C(3));
          sail_int zgsz3229;
          CREATE(sail_int)(&zgsz3229);
          sub_int(&zgsz3229, zgsz3227, zgsz3228);
          zgaz3119 = CONVERT_OF(mach_int, sail_int)(zgsz3229);
          KILL(sail_int)(&zgsz3229);
          KILL(sail_int)(&zgsz3228);
          KILL(sail_int)(&zgsz3227);
        }
        {
          lbits zgsz3230;
          CREATE(lbits)(&zgsz3230);
          CONVERT_OF(lbits, fbits)(&zgsz3230, zgaz3118, UINT64_C(12) , true);
          sail_int zgsz3231;
          CREATE(sail_int)(&zgsz3231);
          CONVERT_OF(sail_int, mach_int)(&zgsz3231, zgaz3119);
          sail_int zgsz3232;
          CREATE(sail_int)(&zgsz3232);
          CONVERT_OF(sail_int, mach_int)(&zgsz3232, INT64_C(3));
          lbits zgsz3233;
          CREATE(lbits)(&zgsz3233);
          vector_subrange_lbits(&zgsz3233, zgsz3230, zgsz3231, zgsz3232);
          zgaz3120 = CONVERT_OF(fbits, lbits)(zgsz3233, true);
          KILL(lbits)(&zgsz3233);
          KILL(sail_int)(&zgsz3232);
          KILL(sail_int)(&zgsz3231);
          KILL(lbits)(&zgsz3230);
        }
      
      
      }
      uint64_t zgaz3121;
      zgaz3121 = sailgen_zzzz7nzzJzzK3z3zzeros_implicit(zinternal_E_take_bits);
      {
        lbits zgsz3234;
        CREATE(lbits)(&zgsz3234);
        CONVERT_OF(lbits, fbits)(&zgsz3234, zgaz3120, UINT64_C(9) , true);
        lbits zgsz3235;
        CREATE(lbits)(&zgsz3235);
        CONVERT_OF(lbits, fbits)(&zgsz3235, zgaz3121, UINT64_C(3) , true);
        lbits zgsz3236;
        CREATE(lbits)(&zgsz3236);
        append(&zgsz3236, zgsz3234, zgsz3235);
        zT = CONVERT_OF(fbits, lbits)(zgsz3236, true);
        KILL(lbits)(&zgsz3236);
        KILL(lbits)(&zgsz3235);
        KILL(lbits)(&zgsz3234);
      }
      unit zgsz3237;
      zgsz3237 = UNIT;
    
    
    }
    uint64_t zgaz3124;
    {
      uint64_t zgaz3122;
      zgaz3122 = zc.zB;
      int64_t zgaz3123;
      {
        sail_int zgsz3240;
        CREATE(sail_int)(&zgsz3240);
        CONVERT_OF(sail_int, mach_int)(&zgsz3240, zcap_mantissa_width);
        sail_int zgsz3241;
        CREATE(sail_int)(&zgsz3241);
        CONVERT_OF(sail_int, mach_int)(&zgsz3241, INT64_C(1));
        sail_int zgsz3242;
        CREATE(sail_int)(&zgsz3242);
        sub_int(&zgsz3242, zgsz3240, zgsz3241);
        zgaz3123 = CONVERT_OF(mach_int, sail_int)(zgsz3242);
        KILL(sail_int)(&zgsz3242);
        KILL(sail_int)(&zgsz3241);
        KILL(sail_int)(&zgsz3240);
      }
      {
        lbits zgsz3243;
        CREATE(lbits)(&zgsz3243);
        CONVERT_OF(lbits, fbits)(&zgsz3243, zgaz3122, UINT64_C(14) , true);
        sail_int zgsz3244;
        CREATE(sail_int)(&zgsz3244);
        CONVERT_OF(sail_int, mach_int)(&zgsz3244, zgaz3123);
        sail_int zgsz3245;
        CREATE(sail_int)(&zgsz3245);
        CONVERT_OF(sail_int, mach_int)(&zgsz3245, INT64_C(3));
        lbits zgsz3246;
        CREATE(lbits)(&zgsz3246);
        vector_subrange_lbits(&zgsz3246, zgsz3243, zgsz3244, zgsz3245);
        zgaz3124 = CONVERT_OF(fbits, lbits)(zgsz3246, true);
        KILL(lbits)(&zgsz3246);
        KILL(sail_int)(&zgsz3245);
        KILL(sail_int)(&zgsz3244);
        KILL(lbits)(&zgsz3243);
      }
    
    
    }
    uint64_t zgaz3125;
    zgaz3125 = sailgen_zzzz7nzzJzzK3z3zzeros_implicit(zinternal_E_take_bits);
    {
      lbits zgsz3247;
      CREATE(lbits)(&zgsz3247);
      CONVERT_OF(lbits, fbits)(&zgsz3247, zgaz3124, UINT64_C(11) , true);
      lbits zgsz3248;
      CREATE(lbits)(&zgsz3248);
      CONVERT_OF(lbits, fbits)(&zgsz3248, zgaz3125, UINT64_C(3) , true);
      lbits zgsz3249;
      CREATE(lbits)(&zgsz3249);
      append(&zgsz3249, zgsz3247, zgsz3248);
      zBs = CONVERT_OF(fbits, lbits)(zgsz3249, true);
      KILL(lbits)(&zgsz3249);
      KILL(lbits)(&zgsz3248);
      KILL(lbits)(&zgsz3247);
    }
    zgsz3207 = UNIT;
  
  
    } else {
    {
      zlenMSBs = UINT64_C(0b00);
      unit zgsz3209;
      zgsz3209 = UNIT;
    }
    {
      zT = zc.zT;
      unit zgsz3208;
      zgsz3208 = UNIT;
    }
    zBs = zc.zB;
    zgsz3207 = UNIT;
    }
  }
  uint64_t zcarry_out;
  {
    bool zgaz3181;
    {
      uint64_t zgaz3180;
      {
        int64_t zgaz3179;
        {
          sail_int zgsz3250;
          CREATE(sail_int)(&zgsz3250);
          CONVERT_OF(sail_int, mach_int)(&zgsz3250, zcap_mantissa_width);
          sail_int zgsz3251;
          CREATE(sail_int)(&zgsz3251);
          CONVERT_OF(sail_int, mach_int)(&zgsz3251, INT64_C(3));
          sail_int zgsz3252;
          CREATE(sail_int)(&zgsz3252);
          sub_int(&zgsz3252, zgsz3250, zgsz3251);
          zgaz3179 = CONVERT_OF(mach_int, sail_int)(zgsz3252);
          KILL(sail_int)(&zgsz3252);
          KILL(sail_int)(&zgsz3251);
          KILL(sail_int)(&zgsz3250);
        }
        {
          lbits zgsz3253;
          CREATE(lbits)(&zgsz3253);
          CONVERT_OF(lbits, fbits)(&zgsz3253, zBs, UINT64_C(14) , true);
          sail_int zgsz3254;
          CREATE(sail_int)(&zgsz3254);
          CONVERT_OF(sail_int, mach_int)(&zgsz3254, zgaz3179);
          sail_int zgsz3255;
          CREATE(sail_int)(&zgsz3255);
          CONVERT_OF(sail_int, mach_int)(&zgsz3255, INT64_C(0));
          lbits zgsz3256;
          CREATE(lbits)(&zgsz3256);
          vector_subrange_lbits(&zgsz3256, zgsz3253, zgsz3254, zgsz3255);
          zgaz3180 = CONVERT_OF(fbits, lbits)(zgsz3256, true);
          KILL(lbits)(&zgsz3256);
          KILL(sail_int)(&zgsz3255);
          KILL(sail_int)(&zgsz3254);
          KILL(lbits)(&zgsz3253);
        }
      
      }
      {
        lbits zgsz3258;
        CREATE(lbits)(&zgsz3258);
        CONVERT_OF(lbits, fbits)(&zgsz3258, zgaz3180, UINT64_C(12) , true);
        lbits zgsz3257;
        CREATE(lbits)(&zgsz3257);
        CONVERT_OF(lbits, fbits)(&zgsz3257, zT, UINT64_C(12) , true);
        zgaz3181 = sailgen_z8operatorz0zzz3zzz3zI_uz9(zgsz3257, zgsz3258);
        KILL(lbits)(&zgsz3258);
        KILL(lbits)(&zgsz3257);
      }
    
    }
    if (zgaz3181) {  zcarry_out = UINT64_C(0b01);  } else {  zcarry_out = UINT64_C(0b00);  }
  
  }
  uint64_t zTtop2;
  {
    uint64_t zgaz3178;
    {
      uint64_t zgaz3177;
      {
        int64_t zgaz3175;
        {
          sail_int zgsz3259;
          CREATE(sail_int)(&zgsz3259);
          CONVERT_OF(sail_int, mach_int)(&zgsz3259, zcap_mantissa_width);
          sail_int zgsz3260;
          CREATE(sail_int)(&zgsz3260);
          CONVERT_OF(sail_int, mach_int)(&zgsz3260, INT64_C(1));
          sail_int zgsz3261;
          CREATE(sail_int)(&zgsz3261);
          sub_int(&zgsz3261, zgsz3259, zgsz3260);
          zgaz3175 = CONVERT_OF(mach_int, sail_int)(zgsz3261);
          KILL(sail_int)(&zgsz3261);
          KILL(sail_int)(&zgsz3260);
          KILL(sail_int)(&zgsz3259);
        }
        int64_t zgaz3176;
        {
          sail_int zgsz3262;
          CREATE(sail_int)(&zgsz3262);
          CONVERT_OF(sail_int, mach_int)(&zgsz3262, zcap_mantissa_width);
          sail_int zgsz3263;
          CREATE(sail_int)(&zgsz3263);
          CONVERT_OF(sail_int, mach_int)(&zgsz3263, INT64_C(2));
          sail_int zgsz3264;
          CREATE(sail_int)(&zgsz3264);
          sub_int(&zgsz3264, zgsz3262, zgsz3263);
          zgaz3176 = CONVERT_OF(mach_int, sail_int)(zgsz3264);
          KILL(sail_int)(&zgsz3264);
          KILL(sail_int)(&zgsz3263);
          KILL(sail_int)(&zgsz3262);
        }
        {
          lbits zgsz3265;
          CREATE(lbits)(&zgsz3265);
          CONVERT_OF(lbits, fbits)(&zgsz3265, zBs, UINT64_C(14) , true);
          sail_int zgsz3266;
          CREATE(sail_int)(&zgsz3266);
          CONVERT_OF(sail_int, mach_int)(&zgsz3266, zgaz3175);
          sail_int zgsz3267;
          CREATE(sail_int)(&zgsz3267);
          CONVERT_OF(sail_int, mach_int)(&zgsz3267, zgaz3176);
          lbits zgsz3268;
          CREATE(lbits)(&zgsz3268);
          vector_subrange_lbits(&zgsz3268, zgsz3265, zgsz3266, zgsz3267);
          zgaz3177 = CONVERT_OF(fbits, lbits)(zgsz3268, true);
          KILL(lbits)(&zgsz3268);
          KILL(sail_int)(&zgsz3267);
          KILL(sail_int)(&zgsz3266);
          KILL(lbits)(&zgsz3265);
        }
      
      
      }
      {
        lbits zgsz3269;
        CREATE(lbits)(&zgsz3269);
        CONVERT_OF(lbits, fbits)(&zgsz3269, zgaz3177, UINT64_C(2) , true);
        lbits zgsz3270;
        CREATE(lbits)(&zgsz3270);
        CONVERT_OF(lbits, fbits)(&zgsz3270, zlenMSBs, UINT64_C(2) , true);
        lbits zgsz3271;
        CREATE(lbits)(&zgsz3271);
        add_bits(&zgsz3271, zgsz3269, zgsz3270);
        zgaz3178 = CONVERT_OF(fbits, lbits)(zgsz3271, true);
        KILL(lbits)(&zgsz3271);
        KILL(lbits)(&zgsz3270);
        KILL(lbits)(&zgsz3269);
      }
    
    }
    {
      lbits zgsz3272;
      CREATE(lbits)(&zgsz3272);
      CONVERT_OF(lbits, fbits)(&zgsz3272, zgaz3178, UINT64_C(2) , true);
      lbits zgsz3273;
      CREATE(lbits)(&zgsz3273);
      CONVERT_OF(lbits, fbits)(&zgsz3273, zcarry_out, UINT64_C(2) , true);
      lbits zgsz3274;
      CREATE(lbits)(&zgsz3274);
      add_bits(&zgsz3274, zgsz3272, zgsz3273);
      zTtop2 = CONVERT_OF(fbits, lbits)(zgsz3274, true);
      KILL(lbits)(&zgsz3274);
      KILL(lbits)(&zgsz3273);
      KILL(lbits)(&zgsz3272);
    }
  
  }
  struct zCapability zgaz3174;
  {
    uint64_t zgaz3130;
    {
      bool zgaz3126;
      {
        sail_int zgsz3276;
        CREATE(sail_int)(&zgsz3276);
        CONVERT_OF(sail_int, mach_int)(&zgsz3276, INT64_C(0));
        sail_int zgsz3275;
        CREATE(sail_int)(&zgsz3275);
        CONVERT_OF(sail_int, mach_int)(&zgsz3275, zcap_uperms_width);
        zgaz3126 = gt(zgsz3275, zgsz3276);
        KILL(sail_int)(&zgsz3276);
        KILL(sail_int)(&zgsz3275);
      }
      uint64_t zgaz3128;
      zgaz3128 = zc.zperms;
      int64_t zgaz3129;
      {
        int64_t zgaz3127;
        {
          sail_int zgsz3277;
          CREATE(sail_int)(&zgsz3277);
          CONVERT_OF(sail_int, mach_int)(&zgsz3277, zcap_hperms_width);
          sail_int zgsz3278;
          CREATE(sail_int)(&zgsz3278);
          CONVERT_OF(sail_int, mach_int)(&zgsz3278, zcap_uperms_width);
          sail_int zgsz3279;
          CREATE(sail_int)(&zgsz3279);
          add_int(&zgsz3279, zgsz3277, zgsz3278);
          zgaz3127 = CONVERT_OF(mach_int, sail_int)(zgsz3279);
          KILL(sail_int)(&zgsz3279);
          KILL(sail_int)(&zgsz3278);
          KILL(sail_int)(&zgsz3277);
        }
        {
          sail_int zgsz3280;
          CREATE(sail_int)(&zgsz3280);
          CONVERT_OF(sail_int, mach_int)(&zgsz3280, zgaz3127);
          sail_int zgsz3281;
          CREATE(sail_int)(&zgsz3281);
          CONVERT_OF(sail_int, mach_int)(&zgsz3281, INT64_C(1));
          sail_int zgsz3282;
          CREATE(sail_int)(&zgsz3282);
          sub_int(&zgsz3282, zgsz3280, zgsz3281);
          zgaz3129 = CONVERT_OF(mach_int, sail_int)(zgsz3282);
          KILL(sail_int)(&zgsz3282);
          KILL(sail_int)(&zgsz3281);
          KILL(sail_int)(&zgsz3280);
        }
      
      }
      {
        lbits zgsz3283;
        CREATE(lbits)(&zgsz3283);
        CONVERT_OF(lbits, fbits)(&zgsz3283, zgaz3128, UINT64_C(16) , true);
        sail_int zgsz3284;
        CREATE(sail_int)(&zgsz3284);
        CONVERT_OF(sail_int, mach_int)(&zgsz3284, zgaz3129);
        sail_int zgsz3285;
        CREATE(sail_int)(&zgsz3285);
        CONVERT_OF(sail_int, mach_int)(&zgsz3285, zcap_hperms_width);
        lbits zgsz3286;
        CREATE(lbits)(&zgsz3286);
        vector_subrange_lbits(&zgsz3286, zgsz3283, zgsz3284, zgsz3285);
        zgaz3130 = CONVERT_OF(fbits, lbits)(zgsz3286, true);
        KILL(lbits)(&zgsz3286);
        KILL(sail_int)(&zgsz3285);
        KILL(sail_int)(&zgsz3284);
        KILL(lbits)(&zgsz3283);
      }
    
    
    
    }
    bool zgaz3133;
    {
      fbits zgaz3132;
      {
        uint64_t zgaz3131;
        zgaz3131 = zc.zperms;
        {
          sail_int zgsz3288;
          CREATE(sail_int)(&zgsz3288);
          CONVERT_OF(sail_int, mach_int)(&zgsz3288, INT64_C(11));
          lbits zgsz3287;
          CREATE(lbits)(&zgsz3287);
          CONVERT_OF(lbits, fbits)(&zgsz3287, zgaz3131, UINT64_C(16) , true);
          zgaz3132 = bitvector_access(zgsz3287, zgsz3288);
          KILL(sail_int)(&zgsz3288);
          KILL(lbits)(&zgsz3287);
        }
      
      }
      zgaz3133 = sailgen_bit_to_bool(zgaz3132);
    
    }
    bool zgaz3136;
    {
      fbits zgaz3135;
      {
        uint64_t zgaz3134;
        zgaz3134 = zc.zperms;
        {
          sail_int zgsz3290;
          CREATE(sail_int)(&zgsz3290);
          CONVERT_OF(sail_int, mach_int)(&zgsz3290, INT64_C(10));
          lbits zgsz3289;
          CREATE(lbits)(&zgsz3289);
          CONVERT_OF(lbits, fbits)(&zgsz3289, zgaz3134, UINT64_C(16) , true);
          zgaz3135 = bitvector_access(zgsz3289, zgsz3290);
          KILL(sail_int)(&zgsz3290);
          KILL(lbits)(&zgsz3289);
        }
      
      }
      zgaz3136 = sailgen_bit_to_bool(zgaz3135);
    
    }
    bool zgaz3139;
    {
      fbits zgaz3138;
      {
        uint64_t zgaz3137;
        zgaz3137 = zc.zperms;
        {
          sail_int zgsz3292;
          CREATE(sail_int)(&zgsz3292);
          CONVERT_OF(sail_int, mach_int)(&zgsz3292, INT64_C(9));
          lbits zgsz3291;
          CREATE(lbits)(&zgsz3291);
          CONVERT_OF(lbits, fbits)(&zgsz3291, zgaz3137, UINT64_C(16) , true);
          zgaz3138 = bitvector_access(zgsz3291, zgsz3292);
          KILL(sail_int)(&zgsz3292);
          KILL(lbits)(&zgsz3291);
        }
      
      }
      zgaz3139 = sailgen_bit_to_bool(zgaz3138);
    
    }
    bool zgaz3142;
    {
      fbits zgaz3141;
      {
        uint64_t zgaz3140;
        zgaz3140 = zc.zperms;
        {
          sail_int zgsz3294;
          CREATE(sail_int)(&zgsz3294);
          CONVERT_OF(sail_int, mach_int)(&zgsz3294, INT64_C(8));
          lbits zgsz3293;
          CREATE(lbits)(&zgsz3293);
          CONVERT_OF(lbits, fbits)(&zgsz3293, zgaz3140, UINT64_C(16) , true);
          zgaz3141 = bitvector_access(zgsz3293, zgsz3294);
          KILL(sail_int)(&zgsz3294);
          KILL(lbits)(&zgsz3293);
        }
      
      }
      zgaz3142 = sailgen_bit_to_bool(zgaz3141);
    
    }
    bool zgaz3145;
    {
      fbits zgaz3144;
      {
        uint64_t zgaz3143;
        zgaz3143 = zc.zperms;
        {
          sail_int zgsz3296;
          CREATE(sail_int)(&zgsz3296);
          CONVERT_OF(sail_int, mach_int)(&zgsz3296, INT64_C(7));
          lbits zgsz3295;
          CREATE(lbits)(&zgsz3295);
          CONVERT_OF(lbits, fbits)(&zgsz3295, zgaz3143, UINT64_C(16) , true);
          zgaz3144 = bitvector_access(zgsz3295, zgsz3296);
          KILL(sail_int)(&zgsz3296);
          KILL(lbits)(&zgsz3295);
        }
      
      }
      zgaz3145 = sailgen_bit_to_bool(zgaz3144);
    
    }
    bool zgaz3148;
    {
      fbits zgaz3147;
      {
        uint64_t zgaz3146;
        zgaz3146 = zc.zperms;
        {
          sail_int zgsz3298;
          CREATE(sail_int)(&zgsz3298);
          CONVERT_OF(sail_int, mach_int)(&zgsz3298, INT64_C(6));
          lbits zgsz3297;
          CREATE(lbits)(&zgsz3297);
          CONVERT_OF(lbits, fbits)(&zgsz3297, zgaz3146, UINT64_C(16) , true);
          zgaz3147 = bitvector_access(zgsz3297, zgsz3298);
          KILL(sail_int)(&zgsz3298);
          KILL(lbits)(&zgsz3297);
        }
      
      }
      zgaz3148 = sailgen_bit_to_bool(zgaz3147);
    
    }
    bool zgaz3151;
    {
      fbits zgaz3150;
      {
        uint64_t zgaz3149;
        zgaz3149 = zc.zperms;
        {
          sail_int zgsz3300;
          CREATE(sail_int)(&zgsz3300);
          CONVERT_OF(sail_int, mach_int)(&zgsz3300, INT64_C(5));
          lbits zgsz3299;
          CREATE(lbits)(&zgsz3299);
          CONVERT_OF(lbits, fbits)(&zgsz3299, zgaz3149, UINT64_C(16) , true);
          zgaz3150 = bitvector_access(zgsz3299, zgsz3300);
          KILL(sail_int)(&zgsz3300);
          KILL(lbits)(&zgsz3299);
        }
      
      }
      zgaz3151 = sailgen_bit_to_bool(zgaz3150);
    
    }
    bool zgaz3154;
    {
      fbits zgaz3153;
      {
        uint64_t zgaz3152;
        zgaz3152 = zc.zperms;
        {
          sail_int zgsz3302;
          CREATE(sail_int)(&zgsz3302);
          CONVERT_OF(sail_int, mach_int)(&zgsz3302, INT64_C(4));
          lbits zgsz3301;
          CREATE(lbits)(&zgsz3301);
          CONVERT_OF(lbits, fbits)(&zgsz3301, zgaz3152, UINT64_C(16) , true);
          zgaz3153 = bitvector_access(zgsz3301, zgsz3302);
          KILL(sail_int)(&zgsz3302);
          KILL(lbits)(&zgsz3301);
        }
      
      }
      zgaz3154 = sailgen_bit_to_bool(zgaz3153);
    
    }
    bool zgaz3157;
    {
      fbits zgaz3156;
      {
        uint64_t zgaz3155;
        zgaz3155 = zc.zperms;
        {
          sail_int zgsz3304;
          CREATE(sail_int)(&zgsz3304);
          CONVERT_OF(sail_int, mach_int)(&zgsz3304, INT64_C(3));
          lbits zgsz3303;
          CREATE(lbits)(&zgsz3303);
          CONVERT_OF(lbits, fbits)(&zgsz3303, zgaz3155, UINT64_C(16) , true);
          zgaz3156 = bitvector_access(zgsz3303, zgsz3304);
          KILL(sail_int)(&zgsz3304);
          KILL(lbits)(&zgsz3303);
        }
      
      }
      zgaz3157 = sailgen_bit_to_bool(zgaz3156);
    
    }
    bool zgaz3160;
    {
      fbits zgaz3159;
      {
        uint64_t zgaz3158;
        zgaz3158 = zc.zperms;
        {
          sail_int zgsz3306;
          CREATE(sail_int)(&zgsz3306);
          CONVERT_OF(sail_int, mach_int)(&zgsz3306, INT64_C(2));
          lbits zgsz3305;
          CREATE(lbits)(&zgsz3305);
          CONVERT_OF(lbits, fbits)(&zgsz3305, zgaz3158, UINT64_C(16) , true);
          zgaz3159 = bitvector_access(zgsz3305, zgsz3306);
          KILL(sail_int)(&zgsz3306);
          KILL(lbits)(&zgsz3305);
        }
      
      }
      zgaz3160 = sailgen_bit_to_bool(zgaz3159);
    
    }
    bool zgaz3163;
    {
      fbits zgaz3162;
      {
        uint64_t zgaz3161;
        zgaz3161 = zc.zperms;
        {
          sail_int zgsz3308;
          CREATE(sail_int)(&zgsz3308);
          CONVERT_OF(sail_int, mach_int)(&zgsz3308, INT64_C(1));
          lbits zgsz3307;
          CREATE(lbits)(&zgsz3307);
          CONVERT_OF(lbits, fbits)(&zgsz3307, zgaz3161, UINT64_C(16) , true);
          zgaz3162 = bitvector_access(zgsz3307, zgsz3308);
          KILL(sail_int)(&zgsz3308);
          KILL(lbits)(&zgsz3307);
        }
      
      }
      zgaz3163 = sailgen_bit_to_bool(zgaz3162);
    
    }
    bool zgaz3166;
    {
      fbits zgaz3165;
      {
        uint64_t zgaz3164;
        zgaz3164 = zc.zperms;
        {
          sail_int zgsz3310;
          CREATE(sail_int)(&zgsz3310);
          CONVERT_OF(sail_int, mach_int)(&zgsz3310, INT64_C(0));
          lbits zgsz3309;
          CREATE(lbits)(&zgsz3309);
          CONVERT_OF(lbits, fbits)(&zgsz3309, zgaz3164, UINT64_C(16) , true);
          zgaz3165 = bitvector_access(zgsz3309, zgsz3310);
          KILL(sail_int)(&zgsz3310);
          KILL(lbits)(&zgsz3309);
        }
      
      }
      zgaz3166 = sailgen_bit_to_bool(zgaz3165);
    
    }
    uint64_t zgaz3167;
    zgaz3167 = zc.zreserved;
    bool zgaz3170;
    {
      fbits zgaz3169;
      {
        uint64_t zgaz3168;
        zgaz3168 = zc.zflags;
        {
          sail_int zgsz3312;
          CREATE(sail_int)(&zgsz3312);
          CONVERT_OF(sail_int, mach_int)(&zgsz3312, INT64_C(0));
          lbits zgsz3311;
          CREATE(lbits)(&zgsz3311);
          CONVERT_OF(lbits, fbits)(&zgsz3311, zgaz3168, UINT64_C(1) , true);
          zgaz3169 = bitvector_access(zgsz3311, zgsz3312);
          KILL(sail_int)(&zgsz3312);
          KILL(lbits)(&zgsz3311);
        }
      
      }
      zgaz3170 = sailgen_bit_to_bool(zgaz3169);
    
    }
    uint64_t zgaz3171;
    {
      lbits zgsz3313;
      CREATE(lbits)(&zgsz3313);
      CONVERT_OF(lbits, fbits)(&zgsz3313, zTtop2, UINT64_C(2) , true);
      lbits zgsz3314;
      CREATE(lbits)(&zgsz3314);
      CONVERT_OF(lbits, fbits)(&zgsz3314, zT, UINT64_C(12) , true);
      lbits zgsz3315;
      CREATE(lbits)(&zgsz3315);
      append(&zgsz3315, zgsz3313, zgsz3314);
      zgaz3171 = CONVERT_OF(fbits, lbits)(zgsz3315, true);
      KILL(lbits)(&zgsz3315);
      KILL(lbits)(&zgsz3314);
      KILL(lbits)(&zgsz3313);
    }
    uint64_t zgaz3172;
    zgaz3172 = zc.zotype;
    uint64_t zgaz3173;
    zgaz3173 = zc.zaddress;
    struct zCapability zgsz3316;
    zgsz3316.zB = zBs;
    zgsz3316.zE = zE;
    zgsz3316.zT = zgaz3171;
    zgsz3316.zaccess_system_regs = zgaz3136;
    zgsz3316.zaddress = zgaz3173;
    zgsz3316.zflag_cap_mode = zgaz3170;
    zgsz3316.zglobal = zgaz3166;
    zgsz3316.zinternal_E = zinternal_E;
    zgsz3316.zotype = zgaz3172;
    zgsz3316.zpermit_cinvoke = zgaz3142;
    zgsz3316.zpermit_execute = zgaz3163;
    zgsz3316.zpermit_load = zgaz3160;
    zgsz3316.zpermit_load_cap = zgaz3154;
    zgsz3316.zpermit_seal = zgaz3145;
    zgsz3316.zpermit_set_CID = zgaz3133;
    zgsz3316.zpermit_store = zgaz3157;
    zgsz3316.zpermit_store_cap = zgaz3151;
    zgsz3316.zpermit_store_local_cap = zgaz3148;
    zgsz3316.zpermit_unseal = zgaz3139;
    zgsz3316.zreserved = zgaz3167;
    zgsz3316.ztag = zt;
    zgsz3316.zuperms = zgaz3130;
    zgaz3174 = zgsz3316;
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  }
  zcbz337 = zgaz3174;
  goto cleanup_149;
  /* unreachable after return */








  goto end_cleanup_150;
cleanup_149: ;








  goto end_function_148;
end_cleanup_150: ;
end_function_148: ;
  return zcbz337;
end_block_exception_151: ;
  struct zCapability zcbz362 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz362;
}

static struct zCapability sailgen_capBitsToCapability(bool, lbits);

static struct zCapability sailgen_capBitsToCapability(bool zt, lbits zc)
{
  __label__ end_function_153, end_block_exception_154;

  struct zCapability zcbz338;
  struct zEncCapability zgaz3184;
  zgaz3184 = sailgen_capBitsToEncCapability(zc);
  zcbz338 = sailgen_encCapabilityToCapability(zt, zgaz3184);

end_function_153: ;
  return zcbz338;
end_block_exception_154: ;
  struct zCapability zcbz363 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz363;
}

static struct zEncCapability sailgen_capToEncCap(struct zCapability);

static struct zEncCapability sailgen_capToEncCap(struct zCapability zcap)
{
  __label__ cleanup_157, end_cleanup_158, end_function_156, end_block_exception_159;

  struct zEncCapability zcbz339;
  uint64_t zt_hi;
  {
    uint64_t zgaz3211;
    zgaz3211 = zcap.zT;
    int64_t zgaz3212;
    {
      sail_int zgsz3317;
      CREATE(sail_int)(&zgsz3317);
      CONVERT_OF(sail_int, mach_int)(&zgsz3317, zcap_mantissa_width);
      sail_int zgsz3318;
      CREATE(sail_int)(&zgsz3318);
      CONVERT_OF(sail_int, mach_int)(&zgsz3318, INT64_C(3));
      sail_int zgsz3319;
      CREATE(sail_int)(&zgsz3319);
      sub_int(&zgsz3319, zgsz3317, zgsz3318);
      zgaz3212 = CONVERT_OF(mach_int, sail_int)(zgsz3319);
      KILL(sail_int)(&zgsz3319);
      KILL(sail_int)(&zgsz3318);
      KILL(sail_int)(&zgsz3317);
    }
    {
      lbits zgsz3320;
      CREATE(lbits)(&zgsz3320);
      CONVERT_OF(lbits, fbits)(&zgsz3320, zgaz3211, UINT64_C(14) , true);
      sail_int zgsz3321;
      CREATE(sail_int)(&zgsz3321);
      CONVERT_OF(sail_int, mach_int)(&zgsz3321, zgaz3212);
      sail_int zgsz3322;
      CREATE(sail_int)(&zgsz3322);
      CONVERT_OF(sail_int, mach_int)(&zgsz3322, zinternal_E_take_bits);
      lbits zgsz3323;
      CREATE(lbits)(&zgsz3323);
      vector_subrange_lbits(&zgsz3323, zgsz3320, zgsz3321, zgsz3322);
      zt_hi = CONVERT_OF(fbits, lbits)(zgsz3323, true);
      KILL(lbits)(&zgsz3323);
      KILL(sail_int)(&zgsz3322);
      KILL(sail_int)(&zgsz3321);
      KILL(lbits)(&zgsz3320);
    }
  
  
  }
  uint64_t zt_lo;
  {
    uint64_t zgaz3209;
    zgaz3209 = zcap.zT;
    int64_t zgaz3210;
    {
      sail_int zgsz3324;
      CREATE(sail_int)(&zgsz3324);
      CONVERT_OF(sail_int, mach_int)(&zgsz3324, zinternal_E_take_bits);
      sail_int zgsz3325;
      CREATE(sail_int)(&zgsz3325);
      CONVERT_OF(sail_int, mach_int)(&zgsz3325, INT64_C(1));
      sail_int zgsz3326;
      CREATE(sail_int)(&zgsz3326);
      sub_int(&zgsz3326, zgsz3324, zgsz3325);
      zgaz3210 = CONVERT_OF(mach_int, sail_int)(zgsz3326);
      KILL(sail_int)(&zgsz3326);
      KILL(sail_int)(&zgsz3325);
      KILL(sail_int)(&zgsz3324);
    }
    {
      lbits zgsz3327;
      CREATE(lbits)(&zgsz3327);
      CONVERT_OF(lbits, fbits)(&zgsz3327, zgaz3209, UINT64_C(14) , true);
      sail_int zgsz3328;
      CREATE(sail_int)(&zgsz3328);
      CONVERT_OF(sail_int, mach_int)(&zgsz3328, zgaz3210);
      sail_int zgsz3329;
      CREATE(sail_int)(&zgsz3329);
      CONVERT_OF(sail_int, mach_int)(&zgsz3329, INT64_C(0));
      lbits zgsz3330;
      CREATE(lbits)(&zgsz3330);
      vector_subrange_lbits(&zgsz3330, zgsz3327, zgsz3328, zgsz3329);
      zt_lo = CONVERT_OF(fbits, lbits)(zgsz3330, true);
      KILL(lbits)(&zgsz3330);
      KILL(sail_int)(&zgsz3329);
      KILL(sail_int)(&zgsz3328);
      KILL(lbits)(&zgsz3327);
    }
  
  
  }
  uint64_t zb_hi;
  {
    uint64_t zgaz3207;
    zgaz3207 = zcap.zB;
    int64_t zgaz3208;
    {
      sail_int zgsz3331;
      CREATE(sail_int)(&zgsz3331);
      CONVERT_OF(sail_int, mach_int)(&zgsz3331, zcap_mantissa_width);
      sail_int zgsz3332;
      CREATE(sail_int)(&zgsz3332);
      CONVERT_OF(sail_int, mach_int)(&zgsz3332, INT64_C(1));
      sail_int zgsz3333;
      CREATE(sail_int)(&zgsz3333);
      sub_int(&zgsz3333, zgsz3331, zgsz3332);
      zgaz3208 = CONVERT_OF(mach_int, sail_int)(zgsz3333);
      KILL(sail_int)(&zgsz3333);
      KILL(sail_int)(&zgsz3332);
      KILL(sail_int)(&zgsz3331);
    }
    {
      lbits zgsz3334;
      CREATE(lbits)(&zgsz3334);
      CONVERT_OF(lbits, fbits)(&zgsz3334, zgaz3207, UINT64_C(14) , true);
      sail_int zgsz3335;
      CREATE(sail_int)(&zgsz3335);
      CONVERT_OF(sail_int, mach_int)(&zgsz3335, zgaz3208);
      sail_int zgsz3336;
      CREATE(sail_int)(&zgsz3336);
      CONVERT_OF(sail_int, mach_int)(&zgsz3336, zinternal_E_take_bits);
      lbits zgsz3337;
      CREATE(lbits)(&zgsz3337);
      vector_subrange_lbits(&zgsz3337, zgsz3334, zgsz3335, zgsz3336);
      zb_hi = CONVERT_OF(fbits, lbits)(zgsz3337, true);
      KILL(lbits)(&zgsz3337);
      KILL(sail_int)(&zgsz3336);
      KILL(sail_int)(&zgsz3335);
      KILL(lbits)(&zgsz3334);
    }
  
  
  }
  uint64_t zb_lo;
  {
    uint64_t zgaz3205;
    zgaz3205 = zcap.zB;
    int64_t zgaz3206;
    {
      sail_int zgsz3338;
      CREATE(sail_int)(&zgsz3338);
      CONVERT_OF(sail_int, mach_int)(&zgsz3338, zinternal_E_take_bits);
      sail_int zgsz3339;
      CREATE(sail_int)(&zgsz3339);
      CONVERT_OF(sail_int, mach_int)(&zgsz3339, INT64_C(1));
      sail_int zgsz3340;
      CREATE(sail_int)(&zgsz3340);
      sub_int(&zgsz3340, zgsz3338, zgsz3339);
      zgaz3206 = CONVERT_OF(mach_int, sail_int)(zgsz3340);
      KILL(sail_int)(&zgsz3340);
      KILL(sail_int)(&zgsz3339);
      KILL(sail_int)(&zgsz3338);
    }
    {
      lbits zgsz3341;
      CREATE(lbits)(&zgsz3341);
      CONVERT_OF(lbits, fbits)(&zgsz3341, zgaz3205, UINT64_C(14) , true);
      sail_int zgsz3342;
      CREATE(sail_int)(&zgsz3342);
      CONVERT_OF(sail_int, mach_int)(&zgsz3342, zgaz3206);
      sail_int zgsz3343;
      CREATE(sail_int)(&zgsz3343);
      CONVERT_OF(sail_int, mach_int)(&zgsz3343, INT64_C(0));
      lbits zgsz3344;
      CREATE(lbits)(&zgsz3344);
      vector_subrange_lbits(&zgsz3344, zgsz3341, zgsz3342, zgsz3343);
      zb_lo = CONVERT_OF(fbits, lbits)(zgsz3344, true);
      KILL(lbits)(&zgsz3344);
      KILL(sail_int)(&zgsz3343);
      KILL(sail_int)(&zgsz3342);
      KILL(lbits)(&zgsz3341);
    }
  
  
  }
  {
    bool zgaz3185;
    zgaz3185 = zcap.zinternal_E;
    unit zgsz3345;
    if (zgaz3185) {
    {
      uint64_t zgaz3187;
      zgaz3187 = zcap.zE;
      int64_t zgaz3188;
      {
        int64_t zgaz3186;
        {
          sail_int zgsz3346;
          CREATE(sail_int)(&zgsz3346);
          CONVERT_OF(sail_int, mach_int)(&zgsz3346, INT64_C(2));
          sail_int zgsz3347;
          CREATE(sail_int)(&zgsz3347);
          CONVERT_OF(sail_int, mach_int)(&zgsz3347, zinternal_E_take_bits);
          sail_int zgsz3348;
          CREATE(sail_int)(&zgsz3348);
          mult_int(&zgsz3348, zgsz3346, zgsz3347);
          zgaz3186 = CONVERT_OF(mach_int, sail_int)(zgsz3348);
          KILL(sail_int)(&zgsz3348);
          KILL(sail_int)(&zgsz3347);
          KILL(sail_int)(&zgsz3346);
        }
        {
          sail_int zgsz3349;
          CREATE(sail_int)(&zgsz3349);
          CONVERT_OF(sail_int, mach_int)(&zgsz3349, zgaz3186);
          sail_int zgsz3350;
          CREATE(sail_int)(&zgsz3350);
          CONVERT_OF(sail_int, mach_int)(&zgsz3350, INT64_C(1));
          sail_int zgsz3351;
          CREATE(sail_int)(&zgsz3351);
          sub_int(&zgsz3351, zgsz3349, zgsz3350);
          zgaz3188 = CONVERT_OF(mach_int, sail_int)(zgsz3351);
          KILL(sail_int)(&zgsz3351);
          KILL(sail_int)(&zgsz3350);
          KILL(sail_int)(&zgsz3349);
        }
      
      }
      {
        lbits zgsz3352;
        CREATE(lbits)(&zgsz3352);
        CONVERT_OF(lbits, fbits)(&zgsz3352, zgaz3187, UINT64_C(6) , true);
        sail_int zgsz3353;
        CREATE(sail_int)(&zgsz3353);
        CONVERT_OF(sail_int, mach_int)(&zgsz3353, zgaz3188);
        sail_int zgsz3354;
        CREATE(sail_int)(&zgsz3354);
        CONVERT_OF(sail_int, mach_int)(&zgsz3354, zinternal_E_take_bits);
        lbits zgsz3355;
        CREATE(lbits)(&zgsz3355);
        vector_subrange_lbits(&zgsz3355, zgsz3352, zgsz3353, zgsz3354);
        zt_lo = CONVERT_OF(fbits, lbits)(zgsz3355, true);
        KILL(lbits)(&zgsz3355);
        KILL(sail_int)(&zgsz3354);
        KILL(sail_int)(&zgsz3353);
        KILL(lbits)(&zgsz3352);
      }
      unit zgsz3356;
      zgsz3356 = UNIT;
    
    
    }
    uint64_t zgaz3189;
    zgaz3189 = zcap.zE;
    int64_t zgaz3190;
    {
      sail_int zgsz3357;
      CREATE(sail_int)(&zgsz3357);
      CONVERT_OF(sail_int, mach_int)(&zgsz3357, zinternal_E_take_bits);
      sail_int zgsz3358;
      CREATE(sail_int)(&zgsz3358);
      CONVERT_OF(sail_int, mach_int)(&zgsz3358, INT64_C(1));
      sail_int zgsz3359;
      CREATE(sail_int)(&zgsz3359);
      sub_int(&zgsz3359, zgsz3357, zgsz3358);
      zgaz3190 = CONVERT_OF(mach_int, sail_int)(zgsz3359);
      KILL(sail_int)(&zgsz3359);
      KILL(sail_int)(&zgsz3358);
      KILL(sail_int)(&zgsz3357);
    }
    {
      lbits zgsz3360;
      CREATE(lbits)(&zgsz3360);
      CONVERT_OF(lbits, fbits)(&zgsz3360, zgaz3189, UINT64_C(6) , true);
      sail_int zgsz3361;
      CREATE(sail_int)(&zgsz3361);
      CONVERT_OF(sail_int, mach_int)(&zgsz3361, zgaz3190);
      sail_int zgsz3362;
      CREATE(sail_int)(&zgsz3362);
      CONVERT_OF(sail_int, mach_int)(&zgsz3362, INT64_C(0));
      lbits zgsz3363;
      CREATE(lbits)(&zgsz3363);
      vector_subrange_lbits(&zgsz3363, zgsz3360, zgsz3361, zgsz3362);
      zb_lo = CONVERT_OF(fbits, lbits)(zgsz3363, true);
      KILL(lbits)(&zgsz3363);
      KILL(sail_int)(&zgsz3362);
      KILL(sail_int)(&zgsz3361);
      KILL(lbits)(&zgsz3360);
    }
    zgsz3345 = UNIT;
  
  
    } else {  zgsz3345 = UNIT;  }
  
  }
  struct zEncCapability zgaz3204;
  {
    uint64_t zgaz3193;
    {
      uint64_t zgaz3191;
      zgaz3191 = zcap.zuperms;
      uint64_t zgaz3192;
      zgaz3192 = sailgen_getCapHardPerms(zcap);
      {
        lbits zgsz3364;
        CREATE(lbits)(&zgsz3364);
        CONVERT_OF(lbits, fbits)(&zgsz3364, zgaz3191, UINT64_C(4) , true);
        lbits zgsz3365;
        CREATE(lbits)(&zgsz3365);
        CONVERT_OF(lbits, fbits)(&zgsz3365, zgaz3192, UINT64_C(12) , true);
        lbits zgsz3366;
        CREATE(lbits)(&zgsz3366);
        append(&zgsz3366, zgsz3364, zgsz3365);
        zgaz3193 = CONVERT_OF(fbits, lbits)(zgsz3366, true);
        KILL(lbits)(&zgsz3366);
        KILL(lbits)(&zgsz3365);
        KILL(lbits)(&zgsz3364);
      }
    
    
    }
    uint64_t zgaz3194;
    zgaz3194 = zcap.zreserved;
    uint64_t zgaz3196;
    {
      bool zgaz3195;
      zgaz3195 = zcap.zflag_cap_mode;
      zgaz3196 = sailgen_bool_to_bits(zgaz3195);
    
    }
    uint64_t zgaz3197;
    zgaz3197 = zcap.zotype;
    fbits zgaz3200;
    {
      uint64_t zgaz3199;
      {
        bool zgaz3198;
        zgaz3198 = zcap.zinternal_E;
        zgaz3199 = sailgen_bool_to_bits(zgaz3198);
      
      }
      {
        sail_int zgsz3368;
        CREATE(sail_int)(&zgsz3368);
        CONVERT_OF(sail_int, mach_int)(&zgsz3368, INT64_C(0));
        lbits zgsz3367;
        CREATE(lbits)(&zgsz3367);
        CONVERT_OF(lbits, fbits)(&zgsz3367, zgaz3199, UINT64_C(1) , true);
        zgaz3200 = bitvector_access(zgsz3367, zgsz3368);
        KILL(sail_int)(&zgsz3368);
        KILL(lbits)(&zgsz3367);
      }
    
    }
    uint64_t zgaz3201;
    {
      lbits zgsz3369;
      CREATE(lbits)(&zgsz3369);
      CONVERT_OF(lbits, fbits)(&zgsz3369, zt_hi, UINT64_C(9) , true);
      lbits zgsz3370;
      CREATE(lbits)(&zgsz3370);
      CONVERT_OF(lbits, fbits)(&zgsz3370, zt_lo, UINT64_C(3) , true);
      lbits zgsz3371;
      CREATE(lbits)(&zgsz3371);
      append(&zgsz3371, zgsz3369, zgsz3370);
      zgaz3201 = CONVERT_OF(fbits, lbits)(zgsz3371, true);
      KILL(lbits)(&zgsz3371);
      KILL(lbits)(&zgsz3370);
      KILL(lbits)(&zgsz3369);
    }
    uint64_t zgaz3202;
    {
      lbits zgsz3372;
      CREATE(lbits)(&zgsz3372);
      CONVERT_OF(lbits, fbits)(&zgsz3372, zb_hi, UINT64_C(11) , true);
      lbits zgsz3373;
      CREATE(lbits)(&zgsz3373);
      CONVERT_OF(lbits, fbits)(&zgsz3373, zb_lo, UINT64_C(3) , true);
      lbits zgsz3374;
      CREATE(lbits)(&zgsz3374);
      append(&zgsz3374, zgsz3372, zgsz3373);
      zgaz3202 = CONVERT_OF(fbits, lbits)(zgsz3374, true);
      KILL(lbits)(&zgsz3374);
      KILL(lbits)(&zgsz3373);
      KILL(lbits)(&zgsz3372);
    }
    uint64_t zgaz3203;
    zgaz3203 = zcap.zaddress;
    struct zEncCapability zgsz3375;
    zgsz3375.zB = zgaz3202;
    zgsz3375.zT = zgaz3201;
    zgsz3375.zaddress = zgaz3203;
    zgsz3375.zflags = zgaz3196;
    zgsz3375.zinternal_E = zgaz3200;
    zgsz3375.zotype = zgaz3197;
    zgsz3375.zperms = zgaz3193;
    zgsz3375.zreserved = zgaz3194;
    zgaz3204 = zgsz3375;
  
  
  
  
  
  
  
  
  
  }
  zcbz339 = zgaz3204;
  goto cleanup_157;
  /* unreachable after return */





  goto end_cleanup_158;
cleanup_157: ;





  goto end_function_156;
end_cleanup_158: ;
end_function_156: ;
  return zcbz339;
end_block_exception_159: ;
  struct zEncCapability zcbz364 = { .zreserved = UINT64_C(0xdeadc0de), .zperms = UINT64_C(0xdeadc0de), .zotype = UINT64_C(0xdeadc0de), .zinternal_E = UINT64_C(0), .zflags = UINT64_C(0xdeadc0de), .zaddress = UINT64_C(0xdeadc0de), .zT = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz364;
}

static void sailgen_capToBits(lbits *rop, struct zCapability);

static void sailgen_capToBits(lbits *zcbz340, struct zCapability zcap)
{
  __label__ end_function_161, end_block_exception_162, end_function_238;

  struct zEncCapability zgaz3213;
  zgaz3213 = sailgen_capToEncCap(zcap);
  sailgen_encCapToBits((*(&zcbz340)), zgaz3213);

end_function_161: ;
  goto end_function_238;
end_block_exception_162: ;
  goto end_function_238;
end_function_238: ;
}

static lbits znull_cap_bits;
static void create_letbind_27(void) {    CREATE(lbits)(&znull_cap_bits);


  lbits zgsz3376;
  CREATE(lbits)(&zgsz3376);
  sailgen_capToBits(&zgsz3376, znull_cap);
  COPY(lbits)(&znull_cap_bits, zgsz3376);
  KILL(lbits)(&zgsz3376);
let_end_163: ;
}
static void kill_letbind_27(void) {    KILL(lbits)(&znull_cap_bits);
}

static void sailgen_capToMemBits(lbits *rop, struct zCapability);

static void sailgen_capToMemBits(lbits *zcbz341, struct zCapability zcap)
{
  __label__ end_function_165, end_block_exception_166, end_function_237;

  lbits zgaz3214;
  CREATE(lbits)(&zgaz3214);
  sailgen_capToBits(&zgaz3214, zcap);
  xor_bits((*(&zcbz341)), zgaz3214, znull_cap_bits);
  KILL(lbits)(&zgaz3214);
end_function_165: ;
  goto end_function_237;
end_block_exception_166: ;
  goto end_function_237;
end_function_237: ;
}

static struct zCapability sailgen_memBitsToCapability(bool, lbits);

static struct zCapability sailgen_memBitsToCapability(bool ztag, lbits zb)
{
  __label__ end_function_168, end_block_exception_169;

  struct zCapability zcbz342;
  lbits zgaz3215;
  CREATE(lbits)(&zgaz3215);
  xor_bits(&zgaz3215, zb, znull_cap_bits);
  zcbz342 = sailgen_capBitsToCapability(ztag, zgaz3215);
  KILL(lbits)(&zgaz3215);
end_function_168: ;
  return zcbz342;
end_block_exception_169: ;
  struct zCapability zcbz365 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz365;
}

// struct tuple_(%bv64, %bv)
struct ztuple_z8z5bv64zCz0z5bvz9 {
  uint64_t ztup0;
  lbits ztup1;
};

static void COPY(ztuple_z8z5bv64zCz0z5bvz9)(struct ztuple_z8z5bv64zCz0z5bvz9 *rop, const struct ztuple_z8z5bv64zCz0z5bvz9 op) {
  rop->ztup0 = op.ztup0;
  COPY(lbits)(&rop->ztup1, op.ztup1);
}

static void CREATE(ztuple_z8z5bv64zCz0z5bvz9)(struct ztuple_z8z5bv64zCz0z5bvz9 *op) {CREATE(lbits)(&op->ztup1);}

static void RECREATE(ztuple_z8z5bv64zCz0z5bvz9)(struct ztuple_z8z5bv64zCz0z5bvz9 *op) {RECREATE(lbits)(&op->ztup1);}

static void KILL(ztuple_z8z5bv64zCz0z5bvz9)(struct ztuple_z8z5bv64zCz0z5bvz9 *op) {KILL(lbits)(&op->ztup1);}

static bool EQUAL(ztuple_z8z5bv64zCz0z5bvz9)(struct ztuple_z8z5bv64zCz0z5bvz9 op1, struct ztuple_z8z5bv64zCz0z5bvz9 op2) {
  return EQUAL(fbits)(op1.ztup0, op2.ztup0) && EQUAL(lbits)(op1.ztup1, op2.ztup1);
}

static void sailgen_getCapBoundsBits(struct ztuple_z8z5bv64zCz0z5bvz9 *rop, struct zCapability);

static void sailgen_getCapBoundsBits(struct ztuple_z8z5bv64zCz0z5bvz9 *zcbz343, struct zCapability zc)
{
  __label__ end_function_171, end_block_exception_172, end_function_236;

  int64_t zE;
  {
    int64_t zgaz3249;
    {
      uint64_t zgaz3248;
      zgaz3248 = zc.zE;
      zgaz3249 = ((mach_int) zgaz3248);
    
    }
    {
      sail_int zgsz3377;
      CREATE(sail_int)(&zgsz3377);
      CONVERT_OF(sail_int, mach_int)(&zgsz3377, zcap_max_E);
      sail_int zgsz3378;
      CREATE(sail_int)(&zgsz3378);
      CONVERT_OF(sail_int, mach_int)(&zgsz3378, zgaz3249);
      sail_int zgsz3379;
      CREATE(sail_int)(&zgsz3379);
      min_int(&zgsz3379, zgsz3377, zgsz3378);
      zE = CONVERT_OF(mach_int, sail_int)(zgsz3379);
      KILL(sail_int)(&zgsz3379);
      KILL(sail_int)(&zgsz3378);
      KILL(sail_int)(&zgsz3377);
    }
  
  }
  uint64_t za;
  za = zc.zaddress;
  uint64_t za3;
  {
    uint64_t zgaz3247;
    {
      int64_t zgaz3246;
      {
        int64_t zgaz3245;
        {
          sail_int zgsz3380;
          CREATE(sail_int)(&zgsz3380);
          CONVERT_OF(sail_int, mach_int)(&zgsz3380, zE);
          sail_int zgsz3381;
          CREATE(sail_int)(&zgsz3381);
          CONVERT_OF(sail_int, mach_int)(&zgsz3381, zcap_mantissa_width);
          sail_int zgsz3382;
          CREATE(sail_int)(&zgsz3382);
          add_int(&zgsz3382, zgsz3380, zgsz3381);
          zgaz3245 = CONVERT_OF(mach_int, sail_int)(zgsz3382);
          KILL(sail_int)(&zgsz3382);
          KILL(sail_int)(&zgsz3381);
          KILL(sail_int)(&zgsz3380);
        }
        {
          sail_int zgsz3383;
          CREATE(sail_int)(&zgsz3383);
          CONVERT_OF(sail_int, mach_int)(&zgsz3383, zgaz3245);
          sail_int zgsz3384;
          CREATE(sail_int)(&zgsz3384);
          CONVERT_OF(sail_int, mach_int)(&zgsz3384, INT64_C(3));
          sail_int zgsz3385;
          CREATE(sail_int)(&zgsz3385);
          sub_int(&zgsz3385, zgsz3383, zgsz3384);
          zgaz3246 = CONVERT_OF(mach_int, sail_int)(zgsz3385);
          KILL(sail_int)(&zgsz3385);
          KILL(sail_int)(&zgsz3384);
          KILL(sail_int)(&zgsz3383);
        }
      
      }
      {
        lbits zgsz3386;
        CREATE(lbits)(&zgsz3386);
        CONVERT_OF(lbits, fbits)(&zgsz3386, za, UINT64_C(64) , true);
        sail_int zgsz3387;
        CREATE(sail_int)(&zgsz3387);
        CONVERT_OF(sail_int, mach_int)(&zgsz3387, zgaz3246);
        lbits zgsz3388;
        CREATE(lbits)(&zgsz3388);
        shiftr(&zgsz3388, zgsz3386, zgsz3387);
        zgaz3247 = CONVERT_OF(fbits, lbits)(zgsz3388, true);
        KILL(lbits)(&zgsz3388);
        KILL(sail_int)(&zgsz3387);
        KILL(lbits)(&zgsz3386);
      }
    
    }
    {
      lbits zgsz3389;
      CREATE(lbits)(&zgsz3389);
      CONVERT_OF(lbits, fbits)(&zgsz3389, zgaz3247, UINT64_C(64) , true);
      sail_int zgsz3390;
      CREATE(sail_int)(&zgsz3390);
      CONVERT_OF(sail_int, mach_int)(&zgsz3390, INT64_C(3));
      lbits zgsz3391;
      CREATE(lbits)(&zgsz3391);
      sail_truncate(&zgsz3391, zgsz3389, zgsz3390);
      za3 = CONVERT_OF(fbits, lbits)(zgsz3391, true);
      KILL(lbits)(&zgsz3391);
      KILL(sail_int)(&zgsz3390);
      KILL(lbits)(&zgsz3389);
    }
  
  }
  uint64_t zB3;
  {
    uint64_t zgaz3244;
    zgaz3244 = zc.zB;
    {
      lbits zgsz3392;
      CREATE(lbits)(&zgsz3392);
      CONVERT_OF(lbits, fbits)(&zgsz3392, zgaz3244, UINT64_C(14) , true);
      sail_int zgsz3393;
      CREATE(sail_int)(&zgsz3393);
      CONVERT_OF(sail_int, mach_int)(&zgsz3393, INT64_C(3));
      lbits zgsz3394;
      CREATE(lbits)(&zgsz3394);
      sail_truncateLSB(&zgsz3394, zgsz3392, zgsz3393);
      zB3 = CONVERT_OF(fbits, lbits)(zgsz3394, true);
      KILL(lbits)(&zgsz3394);
      KILL(sail_int)(&zgsz3393);
      KILL(lbits)(&zgsz3392);
    }
  
  }
  uint64_t zT3;
  {
    uint64_t zgaz3243;
    zgaz3243 = zc.zT;
    {
      lbits zgsz3395;
      CREATE(lbits)(&zgsz3395);
      CONVERT_OF(lbits, fbits)(&zgsz3395, zgaz3243, UINT64_C(14) , true);
      sail_int zgsz3396;
      CREATE(sail_int)(&zgsz3396);
      CONVERT_OF(sail_int, mach_int)(&zgsz3396, INT64_C(3));
      lbits zgsz3397;
      CREATE(lbits)(&zgsz3397);
      sail_truncateLSB(&zgsz3397, zgsz3395, zgsz3396);
      zT3 = CONVERT_OF(fbits, lbits)(zgsz3397, true);
      KILL(lbits)(&zgsz3397);
      KILL(sail_int)(&zgsz3396);
      KILL(lbits)(&zgsz3395);
    }
  
  }
  uint64_t zR3;
  {
    lbits zgsz3398;
    CREATE(lbits)(&zgsz3398);
    CONVERT_OF(lbits, fbits)(&zgsz3398, zB3, UINT64_C(3) , true);
    lbits zgsz3399;
    CREATE(lbits)(&zgsz3399);
    CONVERT_OF(lbits, fbits)(&zgsz3399, UINT64_C(0b001), UINT64_C(3) , true);
    lbits zgsz3400;
    CREATE(lbits)(&zgsz3400);
    sub_bits(&zgsz3400, zgsz3398, zgsz3399);
    zR3 = CONVERT_OF(fbits, lbits)(zgsz3400, true);
    KILL(lbits)(&zgsz3400);
    KILL(lbits)(&zgsz3399);
    KILL(lbits)(&zgsz3398);
  }
  int64_t zaHi;
  {
    bool zgaz3242;
    zgaz3242 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(za3, zR3);
    if (zgaz3242) {  zaHi = INT64_C(1);  } else {  zaHi = INT64_C(0);  }
  
  }
  int64_t zbHi;
  {
    bool zgaz3241;
    zgaz3241 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(zB3, zR3);
    if (zgaz3241) {  zbHi = INT64_C(1);  } else {  zbHi = INT64_C(0);  }
  
  }
  int64_t ztHi;
  {
    bool zgaz3240;
    zgaz3240 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(zT3, zR3);
    if (zgaz3240) {  ztHi = INT64_C(1);  } else {  ztHi = INT64_C(0);  }
  
  }
  int64_t zcorrection_base;
  {
    sail_int zgsz3401;
    CREATE(sail_int)(&zgsz3401);
    CONVERT_OF(sail_int, mach_int)(&zgsz3401, zbHi);
    sail_int zgsz3402;
    CREATE(sail_int)(&zgsz3402);
    CONVERT_OF(sail_int, mach_int)(&zgsz3402, zaHi);
    sail_int zgsz3403;
    CREATE(sail_int)(&zgsz3403);
    sub_int(&zgsz3403, zgsz3401, zgsz3402);
    zcorrection_base = CONVERT_OF(mach_int, sail_int)(zgsz3403);
    KILL(sail_int)(&zgsz3403);
    KILL(sail_int)(&zgsz3402);
    KILL(sail_int)(&zgsz3401);
  }
  int64_t zcorrection_top;
  {
    sail_int zgsz3404;
    CREATE(sail_int)(&zgsz3404);
    CONVERT_OF(sail_int, mach_int)(&zgsz3404, ztHi);
    sail_int zgsz3405;
    CREATE(sail_int)(&zgsz3405);
    CONVERT_OF(sail_int, mach_int)(&zgsz3405, zaHi);
    sail_int zgsz3406;
    CREATE(sail_int)(&zgsz3406);
    sub_int(&zgsz3406, zgsz3404, zgsz3405);
    zcorrection_top = CONVERT_OF(mach_int, sail_int)(zgsz3406);
    KILL(sail_int)(&zgsz3406);
    KILL(sail_int)(&zgsz3405);
    KILL(sail_int)(&zgsz3404);
  }
  uint64_t za_top;
  {
    int64_t zgaz3239;
    {
      sail_int zgsz3407;
      CREATE(sail_int)(&zgsz3407);
      CONVERT_OF(sail_int, mach_int)(&zgsz3407, zE);
      sail_int zgsz3408;
      CREATE(sail_int)(&zgsz3408);
      CONVERT_OF(sail_int, mach_int)(&zgsz3408, zcap_mantissa_width);
      sail_int zgsz3409;
      CREATE(sail_int)(&zgsz3409);
      add_int(&zgsz3409, zgsz3407, zgsz3408);
      zgaz3239 = CONVERT_OF(mach_int, sail_int)(zgsz3409);
      KILL(sail_int)(&zgsz3409);
      KILL(sail_int)(&zgsz3408);
      KILL(sail_int)(&zgsz3407);
    }
    {
      lbits zgsz3410;
      CREATE(lbits)(&zgsz3410);
      CONVERT_OF(lbits, fbits)(&zgsz3410, za, UINT64_C(64) , true);
      sail_int zgsz3411;
      CREATE(sail_int)(&zgsz3411);
      CONVERT_OF(sail_int, mach_int)(&zgsz3411, zgaz3239);
      lbits zgsz3412;
      CREATE(lbits)(&zgsz3412);
      shiftr(&zgsz3412, zgsz3410, zgsz3411);
      za_top = CONVERT_OF(fbits, lbits)(zgsz3412, true);
      KILL(lbits)(&zgsz3412);
      KILL(sail_int)(&zgsz3411);
      KILL(lbits)(&zgsz3410);
    }
  
  }
  lbits zbase;
  CREATE(lbits)(&zbase);
  {
    lbits zgaz3238;
    CREATE(lbits)(&zgaz3238);
    {
      uint64_t zgaz3236;
      {
        lbits zgsz3413;
        CREATE(lbits)(&zgsz3413);
        CONVERT_OF(lbits, fbits)(&zgsz3413, za_top, UINT64_C(64) , true);
        sail_int zgsz3414;
        CREATE(sail_int)(&zgsz3414);
        CONVERT_OF(sail_int, mach_int)(&zgsz3414, zcorrection_base);
        lbits zgsz3415;
        CREATE(lbits)(&zgsz3415);
        add_bits_int(&zgsz3415, zgsz3413, zgsz3414);
        zgaz3236 = CONVERT_OF(fbits, lbits)(zgsz3415, true);
        KILL(lbits)(&zgsz3415);
        KILL(sail_int)(&zgsz3414);
        KILL(lbits)(&zgsz3413);
      }
      lbits zgaz3237;
      CREATE(lbits)(&zgaz3237);
      {
        uint64_t zgaz3234;
        zgaz3234 = zc.zB;
        sbits zgaz3235;
        {
          sail_int zgsz3416;
          CREATE(sail_int)(&zgsz3416);
          CONVERT_OF(sail_int, mach_int)(&zgsz3416, zE);
          lbits zgsz3417;
          CREATE(lbits)(&zgsz3417);
          sailgen_zzz3zzz3zzeros_implicit(&zgsz3417, zgsz3416);
          zgaz3235 = CONVERT_OF(sbits, lbits)(zgsz3417, true);
          KILL(lbits)(&zgsz3417);
          KILL(sail_int)(&zgsz3416);
        }
        {
          lbits zgsz3419;
          CREATE(lbits)(&zgsz3419);
          CONVERT_OF(lbits, sbits)(&zgsz3419, zgaz3235, true);
          lbits zgsz3418;
          CREATE(lbits)(&zgsz3418);
          CONVERT_OF(lbits, fbits)(&zgsz3418, zgaz3234, UINT64_C(14) , true);
          append(&zgaz3237, zgsz3418, zgsz3419);
          KILL(lbits)(&zgsz3419);
          KILL(lbits)(&zgsz3418);
        }
      
      
      }
      {
        lbits zgsz3420;
        CREATE(lbits)(&zgsz3420);
        CONVERT_OF(lbits, fbits)(&zgsz3420, zgaz3236, UINT64_C(64) , true);
        append(&zgaz3238, zgsz3420, zgaz3237);
        KILL(lbits)(&zgsz3420);
      }
      KILL(lbits)(&zgaz3237);
    
    }
    {
      sail_int zgsz3421;
      CREATE(sail_int)(&zgsz3421);
      CONVERT_OF(sail_int, mach_int)(&zgsz3421, zcap_len_width);
      sail_truncate(&zbase, zgaz3238, zgsz3421);
      KILL(sail_int)(&zgsz3421);
    }
    KILL(lbits)(&zgaz3238);
  }
  lbits ztop;
  CREATE(lbits)(&ztop);
  {
    lbits zgaz3233;
    CREATE(lbits)(&zgaz3233);
    {
      uint64_t zgaz3231;
      {
        lbits zgsz3422;
        CREATE(lbits)(&zgsz3422);
        CONVERT_OF(lbits, fbits)(&zgsz3422, za_top, UINT64_C(64) , true);
        sail_int zgsz3423;
        CREATE(sail_int)(&zgsz3423);
        CONVERT_OF(sail_int, mach_int)(&zgsz3423, zcorrection_top);
        lbits zgsz3424;
        CREATE(lbits)(&zgsz3424);
        add_bits_int(&zgsz3424, zgsz3422, zgsz3423);
        zgaz3231 = CONVERT_OF(fbits, lbits)(zgsz3424, true);
        KILL(lbits)(&zgsz3424);
        KILL(sail_int)(&zgsz3423);
        KILL(lbits)(&zgsz3422);
      }
      lbits zgaz3232;
      CREATE(lbits)(&zgaz3232);
      {
        uint64_t zgaz3229;
        zgaz3229 = zc.zT;
        sbits zgaz3230;
        {
          sail_int zgsz3425;
          CREATE(sail_int)(&zgsz3425);
          CONVERT_OF(sail_int, mach_int)(&zgsz3425, zE);
          lbits zgsz3426;
          CREATE(lbits)(&zgsz3426);
          sailgen_zzz3zzz3zzeros_implicit(&zgsz3426, zgsz3425);
          zgaz3230 = CONVERT_OF(sbits, lbits)(zgsz3426, true);
          KILL(lbits)(&zgsz3426);
          KILL(sail_int)(&zgsz3425);
        }
        {
          lbits zgsz3428;
          CREATE(lbits)(&zgsz3428);
          CONVERT_OF(lbits, sbits)(&zgsz3428, zgaz3230, true);
          lbits zgsz3427;
          CREATE(lbits)(&zgsz3427);
          CONVERT_OF(lbits, fbits)(&zgsz3427, zgaz3229, UINT64_C(14) , true);
          append(&zgaz3232, zgsz3427, zgsz3428);
          KILL(lbits)(&zgsz3428);
          KILL(lbits)(&zgsz3427);
        }
      
      
      }
      {
        lbits zgsz3429;
        CREATE(lbits)(&zgsz3429);
        CONVERT_OF(lbits, fbits)(&zgsz3429, zgaz3231, UINT64_C(64) , true);
        append(&zgaz3233, zgsz3429, zgaz3232);
        KILL(lbits)(&zgsz3429);
      }
      KILL(lbits)(&zgaz3232);
    
    }
    {
      sail_int zgsz3430;
      CREATE(sail_int)(&zgsz3430);
      CONVERT_OF(sail_int, mach_int)(&zgsz3430, zcap_len_width);
      sail_truncate(&ztop, zgaz3233, zgsz3430);
      KILL(sail_int)(&zgsz3430);
    }
    KILL(lbits)(&zgaz3233);
  }
  uint64_t zbase2;
  {
    uint64_t zgaz3228;
    {
      fbits zgaz3227;
      {
        int64_t zgaz3226;
        {
          sail_int zgsz3431;
          CREATE(sail_int)(&zgsz3431);
          CONVERT_OF(sail_int, mach_int)(&zgsz3431, zcap_addr_width);
          sail_int zgsz3432;
          CREATE(sail_int)(&zgsz3432);
          CONVERT_OF(sail_int, mach_int)(&zgsz3432, INT64_C(1));
          sail_int zgsz3433;
          CREATE(sail_int)(&zgsz3433);
          sub_int(&zgsz3433, zgsz3431, zgsz3432);
          zgaz3226 = CONVERT_OF(mach_int, sail_int)(zgsz3433);
          KILL(sail_int)(&zgsz3433);
          KILL(sail_int)(&zgsz3432);
          KILL(sail_int)(&zgsz3431);
        }
        {
          sail_int zgsz3434;
          CREATE(sail_int)(&zgsz3434);
          CONVERT_OF(sail_int, mach_int)(&zgsz3434, zgaz3226);
          zgaz3227 = bitvector_access(zbase, zgsz3434);
          KILL(sail_int)(&zgsz3434);
        }
      
      }
      uint64_t zgsz3435;
      zgsz3435 = UINT64_C(0b0);
      zgsz3435 = update_fbits(zgsz3435, INT64_C(0), zgaz3227);
      zgaz3228 = zgsz3435;
    
    }
    {
      lbits zgsz3436;
      CREATE(lbits)(&zgsz3436);
      CONVERT_OF(lbits, fbits)(&zgsz3436, UINT64_C(0b0), UINT64_C(1) , true);
      lbits zgsz3437;
      CREATE(lbits)(&zgsz3437);
      CONVERT_OF(lbits, fbits)(&zgsz3437, zgaz3228, UINT64_C(1) , true);
      lbits zgsz3438;
      CREATE(lbits)(&zgsz3438);
      append(&zgsz3438, zgsz3436, zgsz3437);
      zbase2 = CONVERT_OF(fbits, lbits)(zgsz3438, true);
      KILL(lbits)(&zgsz3438);
      KILL(lbits)(&zgsz3437);
      KILL(lbits)(&zgsz3436);
    }
  
  }
  uint64_t ztop2;
  {
    int64_t zgaz3225;
    {
      sail_int zgsz3439;
      CREATE(sail_int)(&zgsz3439);
      CONVERT_OF(sail_int, mach_int)(&zgsz3439, zcap_addr_width);
      sail_int zgsz3440;
      CREATE(sail_int)(&zgsz3440);
      CONVERT_OF(sail_int, mach_int)(&zgsz3440, INT64_C(1));
      sail_int zgsz3441;
      CREATE(sail_int)(&zgsz3441);
      sub_int(&zgsz3441, zgsz3439, zgsz3440);
      zgaz3225 = CONVERT_OF(mach_int, sail_int)(zgsz3441);
      KILL(sail_int)(&zgsz3441);
      KILL(sail_int)(&zgsz3440);
      KILL(sail_int)(&zgsz3439);
    }
    {
      sail_int zgsz3442;
      CREATE(sail_int)(&zgsz3442);
      CONVERT_OF(sail_int, mach_int)(&zgsz3442, zcap_addr_width);
      sail_int zgsz3443;
      CREATE(sail_int)(&zgsz3443);
      CONVERT_OF(sail_int, mach_int)(&zgsz3443, zgaz3225);
      lbits zgsz3444;
      CREATE(lbits)(&zgsz3444);
      vector_subrange_lbits(&zgsz3444, ztop, zgsz3442, zgsz3443);
      ztop2 = CONVERT_OF(fbits, lbits)(zgsz3444, true);
      KILL(lbits)(&zgsz3444);
      KILL(sail_int)(&zgsz3443);
      KILL(sail_int)(&zgsz3442);
    }
  
  }
  {
    bool zgaz3220;
    {
      bool zgaz3219;
      {
        int64_t zgaz3216;
        {
          sail_int zgsz3445;
          CREATE(sail_int)(&zgsz3445);
          CONVERT_OF(sail_int, mach_int)(&zgsz3445, zcap_max_E);
          sail_int zgsz3446;
          CREATE(sail_int)(&zgsz3446);
          CONVERT_OF(sail_int, mach_int)(&zgsz3446, INT64_C(1));
          sail_int zgsz3447;
          CREATE(sail_int)(&zgsz3447);
          sub_int(&zgsz3447, zgsz3445, zgsz3446);
          zgaz3216 = CONVERT_OF(mach_int, sail_int)(zgsz3447);
          KILL(sail_int)(&zgsz3447);
          KILL(sail_int)(&zgsz3446);
          KILL(sail_int)(&zgsz3445);
        }
        {
          sail_int zgsz3449;
          CREATE(sail_int)(&zgsz3449);
          CONVERT_OF(sail_int, mach_int)(&zgsz3449, zgaz3216);
          sail_int zgsz3448;
          CREATE(sail_int)(&zgsz3448);
          CONVERT_OF(sail_int, mach_int)(&zgsz3448, zE);
          zgaz3219 = lt(zgsz3448, zgsz3449);
          KILL(sail_int)(&zgsz3449);
          KILL(sail_int)(&zgsz3448);
        }
      
      }
      bool zgsz3453;
      if (zgaz3219) {
      int64_t zgaz3218;
      {
        uint64_t zgaz3217;
        {
          lbits zgsz3450;
          CREATE(lbits)(&zgsz3450);
          CONVERT_OF(lbits, fbits)(&zgsz3450, ztop2, UINT64_C(2) , true);
          lbits zgsz3451;
          CREATE(lbits)(&zgsz3451);
          CONVERT_OF(lbits, fbits)(&zgsz3451, zbase2, UINT64_C(2) , true);
          lbits zgsz3452;
          CREATE(lbits)(&zgsz3452);
          sub_bits(&zgsz3452, zgsz3450, zgsz3451);
          zgaz3217 = CONVERT_OF(fbits, lbits)(zgsz3452, true);
          KILL(lbits)(&zgsz3452);
          KILL(lbits)(&zgsz3451);
          KILL(lbits)(&zgsz3450);
        }
        zgaz3218 = ((mach_int) zgaz3217);
      
      }
      {
        sail_int zgsz3455;
        CREATE(sail_int)(&zgsz3455);
        CONVERT_OF(sail_int, mach_int)(&zgsz3455, INT64_C(1));
        sail_int zgsz3454;
        CREATE(sail_int)(&zgsz3454);
        CONVERT_OF(sail_int, mach_int)(&zgsz3454, zgaz3218);
        zgsz3453 = gt(zgsz3454, zgsz3455);
        KILL(sail_int)(&zgsz3455);
        KILL(sail_int)(&zgsz3454);
      }
    
      } else {  zgsz3453 = false;  }
      zgaz3220 = zgsz3453;
    
    }
    unit zgsz3456;
    if (zgaz3220) {
    fbits zgaz3222;
    {
      fbits zgaz3221;
      {
        sail_int zgsz3457;
        CREATE(sail_int)(&zgsz3457);
        CONVERT_OF(sail_int, mach_int)(&zgsz3457, zcap_addr_width);
        zgaz3221 = bitvector_access(ztop, zgsz3457);
        KILL(sail_int)(&zgsz3457);
      }
      zgaz3222 = sailgen_not_bit(zgaz3221);
    
    }
    {
      sail_int zgsz3458;
      CREATE(sail_int)(&zgsz3458);
      CONVERT_OF(sail_int, mach_int)(&zgsz3458, zcap_addr_width);
      update_lbits(&ztop, ztop, zgsz3458, zgaz3222);
      KILL(sail_int)(&zgsz3458);
    }
    zgsz3456 = UNIT;
  
    } else {  zgsz3456 = UNIT;  }
  
  }
  uint64_t zgaz3224;
  {
    int64_t zgaz3223;
    {
      sail_int zgsz3459;
      CREATE(sail_int)(&zgsz3459);
      CONVERT_OF(sail_int, mach_int)(&zgsz3459, zcap_addr_width);
      sail_int zgsz3460;
      CREATE(sail_int)(&zgsz3460);
      CONVERT_OF(sail_int, mach_int)(&zgsz3460, INT64_C(1));
      sail_int zgsz3461;
      CREATE(sail_int)(&zgsz3461);
      sub_int(&zgsz3461, zgsz3459, zgsz3460);
      zgaz3223 = CONVERT_OF(mach_int, sail_int)(zgsz3461);
      KILL(sail_int)(&zgsz3461);
      KILL(sail_int)(&zgsz3460);
      KILL(sail_int)(&zgsz3459);
    }
    {
      sail_int zgsz3462;
      CREATE(sail_int)(&zgsz3462);
      CONVERT_OF(sail_int, mach_int)(&zgsz3462, zgaz3223);
      sail_int zgsz3463;
      CREATE(sail_int)(&zgsz3463);
      CONVERT_OF(sail_int, mach_int)(&zgsz3463, INT64_C(0));
      lbits zgsz3464;
      CREATE(lbits)(&zgsz3464);
      vector_subrange_lbits(&zgsz3464, zbase, zgsz3462, zgsz3463);
      zgaz3224 = CONVERT_OF(fbits, lbits)(zgsz3464, true);
      KILL(lbits)(&zgsz3464);
      KILL(sail_int)(&zgsz3463);
      KILL(sail_int)(&zgsz3462);
    }
  
  }
  struct ztuple_z8z5bv64zCz0z5bvz9 zgsz3465;
  CREATE(ztuple_z8z5bv64zCz0z5bvz9)(&zgsz3465);
  zgsz3465.ztup0 = zgaz3224;
  COPY(lbits)(&((&zgsz3465)->ztup1), ztop);
  COPY(ztuple_z8z5bv64zCz0z5bvz9)((*(&zcbz343)), zgsz3465);
  KILL(ztuple_z8z5bv64zCz0z5bvz9)(&zgsz3465);



  KILL(lbits)(&ztop);
  KILL(lbits)(&zbase);












end_function_171: ;
  goto end_function_236;
end_block_exception_172: ;
  goto end_function_236;
end_function_236: ;
}

// struct tuple_(%i, %i)
struct ztuple_z8z5izCz0z5iz9 {
  sail_int ztup0;
  sail_int ztup1;
};

static void COPY(ztuple_z8z5izCz0z5iz9)(struct ztuple_z8z5izCz0z5iz9 *rop, const struct ztuple_z8z5izCz0z5iz9 op) {
  COPY(sail_int)(&rop->ztup0, op.ztup0);
  COPY(sail_int)(&rop->ztup1, op.ztup1);
}

static void CREATE(ztuple_z8z5izCz0z5iz9)(struct ztuple_z8z5izCz0z5iz9 *op) {
  CREATE(sail_int)(&op->ztup0);
  CREATE(sail_int)(&op->ztup1);
}

static void RECREATE(ztuple_z8z5izCz0z5iz9)(struct ztuple_z8z5izCz0z5iz9 *op) {
  RECREATE(sail_int)(&op->ztup0);
  RECREATE(sail_int)(&op->ztup1);
}

static void KILL(ztuple_z8z5izCz0z5iz9)(struct ztuple_z8z5izCz0z5iz9 *op) {
  KILL(sail_int)(&op->ztup0);
  KILL(sail_int)(&op->ztup1);
}

static bool EQUAL(ztuple_z8z5izCz0z5iz9)(struct ztuple_z8z5izCz0z5iz9 op1, struct ztuple_z8z5izCz0z5iz9 op2) {
  return EQUAL(sail_int)(op1.ztup0, op2.ztup0) && EQUAL(sail_int)(op1.ztup1, op2.ztup1);
}

static void sailgen_getCapBounds(struct ztuple_z8z5izCz0z5iz9 *rop, struct zCapability);


static void sailgen_getCapBounds(struct ztuple_z8z5izCz0z5iz9 *zcbz344, struct zCapability zcap)
{
  __label__ case_175, finish_match_174, end_function_176, end_block_exception_177, end_function_235;

  struct ztuple_z8z5bv64zCz0z5bvz9 zgaz3250;
  CREATE(ztuple_z8z5bv64zCz0z5bvz9)(&zgaz3250);
  sailgen_getCapBoundsBits(&zgaz3250, zcap);
  struct ztuple_z8z5izCz0z5iz9 zgsz3466;
  CREATE(ztuple_z8z5izCz0z5iz9)(&zgsz3466);
  {
    uint64_t zbase;
    zbase = zgaz3250.ztup0;
    lbits ztop;
    CREATE(lbits)(&ztop);
    COPY(lbits)(&ztop, zgaz3250.ztup1);
    sail_int zgaz3251;
    CREATE(sail_int)(&zgaz3251);
    {
      lbits zgsz3467;
      CREATE(lbits)(&zgsz3467);
      CONVERT_OF(lbits, fbits)(&zgsz3467, zbase, UINT64_C(64) , true);
      sail_unsigned(&zgaz3251, zgsz3467);
      KILL(lbits)(&zgsz3467);
    }
    sail_int zgaz3252;
    CREATE(sail_int)(&zgaz3252);
    sail_unsigned(&zgaz3252, ztop);
    struct ztuple_z8z5izCz0z5iz9 zgsz3468;
    CREATE(ztuple_z8z5izCz0z5iz9)(&zgsz3468);
    COPY(sail_int)(&((&zgsz3468)->ztup0), zgaz3251);
    COPY(sail_int)(&((&zgsz3468)->ztup1), zgaz3252);
    COPY(ztuple_z8z5izCz0z5iz9)(&zgsz3466, zgsz3468);
    KILL(ztuple_z8z5izCz0z5iz9)(&zgsz3468);
    KILL(sail_int)(&zgaz3252);
    KILL(sail_int)(&zgaz3251);
    KILL(lbits)(&ztop);
  
    goto finish_match_174;
  }
case_175: ;
  sail_match_failure("getCapBounds");
finish_match_174: ;
  COPY(ztuple_z8z5izCz0z5iz9)((*(&zcbz344)), zgsz3466);
  KILL(ztuple_z8z5izCz0z5iz9)(&zgsz3466);
  KILL(ztuple_z8z5bv64zCz0z5bvz9)(&zgaz3250);
end_function_176: ;
  goto end_function_235;
end_block_exception_177: ;
  goto end_function_235;
end_function_235: ;
}

// struct tuple_(%bool, %struct zCapability)
struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 {
  bool ztup0;
  struct zCapability ztup1;
};

static void COPY(ztuple_z8z5boolzCz0z5structz0zzCapabilityz9)(struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 *rop, const struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 op) {
  rop->ztup0 = op.ztup0;
  rop->ztup1 = op.ztup1;
}

static bool EQUAL(ztuple_z8z5boolzCz0z5structz0zzCapabilityz9)(struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 op1, struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 op2) {
  return EQUAL(bool)(op1.ztup0, op2.ztup0) && EQUAL(zCapability)(op1.ztup1, op2.ztup1);
}

static struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 sailgen_setCapBounds(struct zCapability, uint64_t, lbits);

static struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 sailgen_setCapBounds(struct zCapability zcap, uint64_t zbase, lbits ztop)
{
  __label__ end_function_179, end_block_exception_180;

  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zcbz345;
  lbits zext_base;
  CREATE(lbits)(&zext_base);
  {
    lbits zgsz3471;
    CREATE(lbits)(&zgsz3471);
    CONVERT_OF(lbits, fbits)(&zgsz3471, zbase, UINT64_C(64) , true);
    lbits zgsz3470;
    CREATE(lbits)(&zgsz3470);
    CONVERT_OF(lbits, fbits)(&zgsz3470, UINT64_C(0b0), UINT64_C(1) , true);
    append(&zext_base, zgsz3470, zgsz3471);
    KILL(lbits)(&zgsz3471);
    KILL(lbits)(&zgsz3470);
  }
  lbits zlength;
  CREATE(lbits)(&zlength);
  sub_bits(&zlength, ztop, zext_base);
  int64_t ze;
  {
    int64_t zgaz3286;
    {
      uint64_t zgaz3285;
      {
        int64_t zgaz3284;
        {
          sail_int zgsz3472;
          CREATE(sail_int)(&zgsz3472);
          CONVERT_OF(sail_int, mach_int)(&zgsz3472, zcap_mantissa_width);
          sail_int zgsz3473;
          CREATE(sail_int)(&zgsz3473);
          CONVERT_OF(sail_int, mach_int)(&zgsz3473, INT64_C(1));
          sail_int zgsz3474;
          CREATE(sail_int)(&zgsz3474);
          sub_int(&zgsz3474, zgsz3472, zgsz3473);
          zgaz3284 = CONVERT_OF(mach_int, sail_int)(zgsz3474);
          KILL(sail_int)(&zgsz3474);
          KILL(sail_int)(&zgsz3473);
          KILL(sail_int)(&zgsz3472);
        }
        {
          sail_int zgsz3475;
          CREATE(sail_int)(&zgsz3475);
          CONVERT_OF(sail_int, mach_int)(&zgsz3475, zcap_addr_width);
          sail_int zgsz3476;
          CREATE(sail_int)(&zgsz3476);
          CONVERT_OF(sail_int, mach_int)(&zgsz3476, zgaz3284);
          lbits zgsz3477;
          CREATE(lbits)(&zgsz3477);
          vector_subrange_lbits(&zgsz3477, zlength, zgsz3475, zgsz3476);
          zgaz3285 = CONVERT_OF(fbits, lbits)(zgsz3477, true);
          KILL(lbits)(&zgsz3477);
          KILL(sail_int)(&zgsz3476);
          KILL(sail_int)(&zgsz3475);
        }
      
      }
      {
        lbits zgsz3478;
        CREATE(lbits)(&zgsz3478);
        CONVERT_OF(lbits, fbits)(&zgsz3478, zgaz3285, UINT64_C(52) , true);
        sail_int zgsz3479;
        CREATE(sail_int)(&zgsz3479);
        count_leading_zeros(&zgsz3479, zgsz3478);
        zgaz3286 = CONVERT_OF(mach_int, sail_int)(zgsz3479);
        KILL(sail_int)(&zgsz3479);
        KILL(lbits)(&zgsz3478);
      }
    
    }
    {
      sail_int zgsz3480;
      CREATE(sail_int)(&zgsz3480);
      CONVERT_OF(sail_int, mach_int)(&zgsz3480, zcap_max_E);
      sail_int zgsz3481;
      CREATE(sail_int)(&zgsz3481);
      CONVERT_OF(sail_int, mach_int)(&zgsz3481, zgaz3286);
      sail_int zgsz3482;
      CREATE(sail_int)(&zgsz3482);
      sub_int(&zgsz3482, zgsz3480, zgsz3481);
      ze = CONVERT_OF(mach_int, sail_int)(zgsz3482);
      KILL(sail_int)(&zgsz3482);
      KILL(sail_int)(&zgsz3481);
      KILL(sail_int)(&zgsz3480);
    }
  
  }
  bool zie;
  {
    bool zgaz3283;
    {
      sail_int zgsz3483;
      CREATE(sail_int)(&zgsz3483);
      CONVERT_OF(sail_int, mach_int)(&zgsz3483, ze);
      zgaz3283 = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgsz3483, INT64_C(0));
      KILL(sail_int)(&zgsz3483);
    }
    bool zgsz3488;
    if (zgaz3283) {  zgsz3488 = true;  } else {
    fbits zgaz3282;
    {
      int64_t zgaz3281;
      {
        sail_int zgsz3484;
        CREATE(sail_int)(&zgsz3484);
        CONVERT_OF(sail_int, mach_int)(&zgsz3484, zcap_mantissa_width);
        sail_int zgsz3485;
        CREATE(sail_int)(&zgsz3485);
        CONVERT_OF(sail_int, mach_int)(&zgsz3485, INT64_C(2));
        sail_int zgsz3486;
        CREATE(sail_int)(&zgsz3486);
        sub_int(&zgsz3486, zgsz3484, zgsz3485);
        zgaz3281 = CONVERT_OF(mach_int, sail_int)(zgsz3486);
        KILL(sail_int)(&zgsz3486);
        KILL(sail_int)(&zgsz3485);
        KILL(sail_int)(&zgsz3484);
      }
      {
        sail_int zgsz3487;
        CREATE(sail_int)(&zgsz3487);
        CONVERT_OF(sail_int, mach_int)(&zgsz3487, zgaz3281);
        zgaz3282 = bitvector_access(zlength, zgsz3487);
        KILL(sail_int)(&zgsz3487);
      }
    
    }
    zgsz3488 = eq_bit(zgaz3282, UINT64_C(1));
  
    }
    zie = zgsz3488;
  
  }
  uint64_t zBbits;
  {
    lbits zgsz3489;
    CREATE(lbits)(&zgsz3489);
    CONVERT_OF(lbits, fbits)(&zgsz3489, zbase, UINT64_C(64) , true);
    sail_int zgsz3490;
    CREATE(sail_int)(&zgsz3490);
    CONVERT_OF(sail_int, mach_int)(&zgsz3490, zcap_mantissa_width);
    lbits zgsz3491;
    CREATE(lbits)(&zgsz3491);
    sail_truncate(&zgsz3491, zgsz3489, zgsz3490);
    zBbits = CONVERT_OF(fbits, lbits)(zgsz3491, true);
    KILL(lbits)(&zgsz3491);
    KILL(sail_int)(&zgsz3490);
    KILL(lbits)(&zgsz3489);
  }
  uint64_t zTbits;
  {
    sail_int zgsz3492;
    CREATE(sail_int)(&zgsz3492);
    CONVERT_OF(sail_int, mach_int)(&zgsz3492, zcap_mantissa_width);
    lbits zgsz3493;
    CREATE(lbits)(&zgsz3493);
    sail_truncate(&zgsz3493, ztop, zgsz3492);
    zTbits = CONVERT_OF(fbits, lbits)(zgsz3493, true);
    KILL(lbits)(&zgsz3493);
    KILL(sail_int)(&zgsz3492);
  }
  bool zlostSignificantTop;
  zlostSignificantTop = false;
  bool zlostSignificantBase;
  zlostSignificantBase = false;
  bool zincE;
  zincE = false;
  {
    unit zgsz3494;
    if (zie) {
    uint64_t zB_ie;
    {
      uint64_t zgaz3276;
      {
        int64_t zgaz3275;
        {
          sail_int zgsz3495;
          CREATE(sail_int)(&zgsz3495);
          CONVERT_OF(sail_int, mach_int)(&zgsz3495, ze);
          sail_int zgsz3496;
          CREATE(sail_int)(&zgsz3496);
          CONVERT_OF(sail_int, mach_int)(&zgsz3496, INT64_C(3));
          sail_int zgsz3497;
          CREATE(sail_int)(&zgsz3497);
          add_int(&zgsz3497, zgsz3495, zgsz3496);
          zgaz3275 = CONVERT_OF(mach_int, sail_int)(zgsz3497);
          KILL(sail_int)(&zgsz3497);
          KILL(sail_int)(&zgsz3496);
          KILL(sail_int)(&zgsz3495);
        }
        {
          lbits zgsz3498;
          CREATE(lbits)(&zgsz3498);
          CONVERT_OF(lbits, fbits)(&zgsz3498, zbase, UINT64_C(64) , true);
          sail_int zgsz3499;
          CREATE(sail_int)(&zgsz3499);
          CONVERT_OF(sail_int, mach_int)(&zgsz3499, zgaz3275);
          lbits zgsz3500;
          CREATE(lbits)(&zgsz3500);
          shiftr(&zgsz3500, zgsz3498, zgsz3499);
          zgaz3276 = CONVERT_OF(fbits, lbits)(zgsz3500, true);
          KILL(lbits)(&zgsz3500);
          KILL(sail_int)(&zgsz3499);
          KILL(lbits)(&zgsz3498);
        }
      
      }
      int64_t zgaz3277;
      {
        sail_int zgsz3501;
        CREATE(sail_int)(&zgsz3501);
        CONVERT_OF(sail_int, mach_int)(&zgsz3501, zcap_mantissa_width);
        sail_int zgsz3502;
        CREATE(sail_int)(&zgsz3502);
        CONVERT_OF(sail_int, mach_int)(&zgsz3502, INT64_C(3));
        sail_int zgsz3503;
        CREATE(sail_int)(&zgsz3503);
        sub_int(&zgsz3503, zgsz3501, zgsz3502);
        zgaz3277 = CONVERT_OF(mach_int, sail_int)(zgsz3503);
        KILL(sail_int)(&zgsz3503);
        KILL(sail_int)(&zgsz3502);
        KILL(sail_int)(&zgsz3501);
      }
      {
        lbits zgsz3504;
        CREATE(lbits)(&zgsz3504);
        CONVERT_OF(lbits, fbits)(&zgsz3504, zgaz3276, UINT64_C(64) , true);
        sail_int zgsz3505;
        CREATE(sail_int)(&zgsz3505);
        CONVERT_OF(sail_int, mach_int)(&zgsz3505, zgaz3277);
        lbits zgsz3506;
        CREATE(lbits)(&zgsz3506);
        sail_truncate(&zgsz3506, zgsz3504, zgsz3505);
        zB_ie = CONVERT_OF(fbits, lbits)(zgsz3506, true);
        KILL(lbits)(&zgsz3506);
        KILL(sail_int)(&zgsz3505);
        KILL(lbits)(&zgsz3504);
      }
    
    
    }
    uint64_t zT_ie;
    {
      lbits zgaz3273;
      CREATE(lbits)(&zgaz3273);
      {
        int64_t zgaz3272;
        {
          sail_int zgsz3507;
          CREATE(sail_int)(&zgsz3507);
          CONVERT_OF(sail_int, mach_int)(&zgsz3507, ze);
          sail_int zgsz3508;
          CREATE(sail_int)(&zgsz3508);
          CONVERT_OF(sail_int, mach_int)(&zgsz3508, INT64_C(3));
          sail_int zgsz3509;
          CREATE(sail_int)(&zgsz3509);
          add_int(&zgsz3509, zgsz3507, zgsz3508);
          zgaz3272 = CONVERT_OF(mach_int, sail_int)(zgsz3509);
          KILL(sail_int)(&zgsz3509);
          KILL(sail_int)(&zgsz3508);
          KILL(sail_int)(&zgsz3507);
        }
        {
          sail_int zgsz3510;
          CREATE(sail_int)(&zgsz3510);
          CONVERT_OF(sail_int, mach_int)(&zgsz3510, zgaz3272);
          shiftr(&zgaz3273, ztop, zgsz3510);
          KILL(sail_int)(&zgsz3510);
        }
      
      }
      int64_t zgaz3274;
      {
        sail_int zgsz3511;
        CREATE(sail_int)(&zgsz3511);
        CONVERT_OF(sail_int, mach_int)(&zgsz3511, zcap_mantissa_width);
        sail_int zgsz3512;
        CREATE(sail_int)(&zgsz3512);
        CONVERT_OF(sail_int, mach_int)(&zgsz3512, INT64_C(3));
        sail_int zgsz3513;
        CREATE(sail_int)(&zgsz3513);
        sub_int(&zgsz3513, zgsz3511, zgsz3512);
        zgaz3274 = CONVERT_OF(mach_int, sail_int)(zgsz3513);
        KILL(sail_int)(&zgsz3513);
        KILL(sail_int)(&zgsz3512);
        KILL(sail_int)(&zgsz3511);
      }
      {
        sail_int zgsz3514;
        CREATE(sail_int)(&zgsz3514);
        CONVERT_OF(sail_int, mach_int)(&zgsz3514, zgaz3274);
        lbits zgsz3515;
        CREATE(lbits)(&zgsz3515);
        sail_truncate(&zgsz3515, zgaz3273, zgsz3514);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3515, true);
        KILL(lbits)(&zgsz3515);
        KILL(sail_int)(&zgsz3514);
      }
    
      KILL(lbits)(&zgaz3273);
    }
    lbits zmaskLo;
    CREATE(lbits)(&zmaskLo);
    {
      int64_t zgaz3270;
      {
        sail_int zgsz3516;
        CREATE(sail_int)(&zgsz3516);
        CONVERT_OF(sail_int, mach_int)(&zgsz3516, INT64_C(64));
        sail_int zgsz3517;
        CREATE(sail_int)(&zgsz3517);
        CONVERT_OF(sail_int, mach_int)(&zgsz3517, INT64_C(1));
        sail_int zgsz3518;
        CREATE(sail_int)(&zgsz3518);
        add_int(&zgsz3518, zgsz3516, zgsz3517);
        zgaz3270 = CONVERT_OF(mach_int, sail_int)(zgsz3518);
        KILL(sail_int)(&zgsz3518);
        KILL(sail_int)(&zgsz3517);
        KILL(sail_int)(&zgsz3516);
      }
      sbits zgaz3271;
      {
        int64_t zgaz3269;
        {
          sail_int zgsz3519;
          CREATE(sail_int)(&zgsz3519);
          CONVERT_OF(sail_int, mach_int)(&zgsz3519, ze);
          sail_int zgsz3520;
          CREATE(sail_int)(&zgsz3520);
          CONVERT_OF(sail_int, mach_int)(&zgsz3520, INT64_C(3));
          sail_int zgsz3521;
          CREATE(sail_int)(&zgsz3521);
          add_int(&zgsz3521, zgsz3519, zgsz3520);
          zgaz3269 = CONVERT_OF(mach_int, sail_int)(zgsz3521);
          KILL(sail_int)(&zgsz3521);
          KILL(sail_int)(&zgsz3520);
          KILL(sail_int)(&zgsz3519);
        }
        {
          sail_int zgsz3522;
          CREATE(sail_int)(&zgsz3522);
          CONVERT_OF(sail_int, mach_int)(&zgsz3522, zgaz3269);
          lbits zgsz3523;
          CREATE(lbits)(&zgsz3523);
          sailgen_zzz3zzz3ones(&zgsz3523, zgsz3522);
          zgaz3271 = CONVERT_OF(sbits, lbits)(zgsz3523, true);
          KILL(lbits)(&zgsz3523);
          KILL(sail_int)(&zgsz3522);
        }
      
      }
      {
        lbits zgsz3525;
        CREATE(lbits)(&zgsz3525);
        CONVERT_OF(lbits, sbits)(&zgsz3525, zgaz3271, true);
        sail_int zgsz3524;
        CREATE(sail_int)(&zgsz3524);
        CONVERT_OF(sail_int, mach_int)(&zgsz3524, zgaz3270);
        sailgen_zzz3zzz3EXTZ(&zmaskLo, zgsz3524, zgsz3525);
        KILL(lbits)(&zgsz3525);
        KILL(sail_int)(&zgsz3524);
      }
    
    
    }
    {
      sail_int zgaz3254;
      CREATE(sail_int)(&zgaz3254);
      {
        lbits zgaz3253;
        CREATE(lbits)(&zgaz3253);
        and_bits(&zgaz3253, zext_base, zmaskLo);
        sail_unsigned(&zgaz3254, zgaz3253);
        KILL(lbits)(&zgaz3253);
      }
      zlostSignificantBase = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgaz3254, INT64_C(0));
      unit zgsz3531;
      zgsz3531 = UNIT;
      KILL(sail_int)(&zgaz3254);
    }
    {
      sail_int zgaz3256;
      CREATE(sail_int)(&zgaz3256);
      {
        lbits zgaz3255;
        CREATE(lbits)(&zgaz3255);
        and_bits(&zgaz3255, ztop, zmaskLo);
        sail_unsigned(&zgaz3256, zgaz3255);
        KILL(lbits)(&zgaz3255);
      }
      zlostSignificantTop = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgaz3256, INT64_C(0));
      unit zgsz3530;
      zgsz3530 = UNIT;
      KILL(sail_int)(&zgaz3256);
    }
    {
      unit zgsz3526;
      if (zlostSignificantTop) {
      {
        lbits zgsz3527;
        CREATE(lbits)(&zgsz3527);
        CONVERT_OF(lbits, fbits)(&zgsz3527, zT_ie, UINT64_C(11) , true);
        sail_int zgsz3528;
        CREATE(sail_int)(&zgsz3528);
        CONVERT_OF(sail_int, mach_int)(&zgsz3528, INT64_C(1));
        lbits zgsz3529;
        CREATE(lbits)(&zgsz3529);
        add_bits_int(&zgsz3529, zgsz3527, zgsz3528);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3529, true);
        KILL(lbits)(&zgsz3529);
        KILL(sail_int)(&zgsz3528);
        KILL(lbits)(&zgsz3527);
      }
      zgsz3526 = UNIT;
      } else {  zgsz3526 = UNIT;  }
    }
    uint64_t zlen_ie;
    {
      lbits zgsz3532;
      CREATE(lbits)(&zgsz3532);
      CONVERT_OF(lbits, fbits)(&zgsz3532, zT_ie, UINT64_C(11) , true);
      lbits zgsz3533;
      CREATE(lbits)(&zgsz3533);
      CONVERT_OF(lbits, fbits)(&zgsz3533, zB_ie, UINT64_C(11) , true);
      lbits zgsz3534;
      CREATE(lbits)(&zgsz3534);
      sub_bits(&zgsz3534, zgsz3532, zgsz3533);
      zlen_ie = CONVERT_OF(fbits, lbits)(zgsz3534, true);
      KILL(lbits)(&zgsz3534);
      KILL(lbits)(&zgsz3533);
      KILL(lbits)(&zgsz3532);
    }
    {
      bool zgaz3259;
      {
        fbits zgaz3258;
        {
          int64_t zgaz3257;
          {
            sail_int zgsz3535;
            CREATE(sail_int)(&zgsz3535);
            CONVERT_OF(sail_int, mach_int)(&zgsz3535, zcap_mantissa_width);
            sail_int zgsz3536;
            CREATE(sail_int)(&zgsz3536);
            CONVERT_OF(sail_int, mach_int)(&zgsz3536, INT64_C(4));
            sail_int zgsz3537;
            CREATE(sail_int)(&zgsz3537);
            sub_int(&zgsz3537, zgsz3535, zgsz3536);
            zgaz3257 = CONVERT_OF(mach_int, sail_int)(zgsz3537);
            KILL(sail_int)(&zgsz3537);
            KILL(sail_int)(&zgsz3536);
            KILL(sail_int)(&zgsz3535);
          }
          {
            sail_int zgsz3539;
            CREATE(sail_int)(&zgsz3539);
            CONVERT_OF(sail_int, mach_int)(&zgsz3539, zgaz3257);
            lbits zgsz3538;
            CREATE(lbits)(&zgsz3538);
            CONVERT_OF(lbits, fbits)(&zgsz3538, zlen_ie, UINT64_C(11) , true);
            zgaz3258 = bitvector_access(zgsz3538, zgsz3539);
            KILL(sail_int)(&zgsz3539);
            KILL(lbits)(&zgsz3538);
          }
        
        }
        zgaz3259 = eq_bit(zgaz3258, UINT64_C(1));
      
      }
      unit zgsz3544;
      if (zgaz3259) {
      {
        zincE = true;
        unit zgsz3566;
        zgsz3566 = UNIT;
      }
      {
        bool zgsz3547;
        if (zlostSignificantBase) {  zgsz3547 = true;  } else {
        fbits zgaz3260;
        {
          sail_int zgsz3546;
          CREATE(sail_int)(&zgsz3546);
          CONVERT_OF(sail_int, mach_int)(&zgsz3546, INT64_C(0));
          lbits zgsz3545;
          CREATE(lbits)(&zgsz3545);
          CONVERT_OF(lbits, fbits)(&zgsz3545, zB_ie, UINT64_C(11) , true);
          zgaz3260 = bitvector_access(zgsz3545, zgsz3546);
          KILL(sail_int)(&zgsz3546);
          KILL(lbits)(&zgsz3545);
        }
        zgsz3547 = eq_bit(zgaz3260, UINT64_C(1));
      
        }
        zlostSignificantBase = zgsz3547;
        unit zgsz3565;
        zgsz3565 = UNIT;
      }
      {
        bool zgsz3550;
        if (zlostSignificantTop) {  zgsz3550 = true;  } else {
        fbits zgaz3261;
        {
          sail_int zgsz3549;
          CREATE(sail_int)(&zgsz3549);
          CONVERT_OF(sail_int, mach_int)(&zgsz3549, INT64_C(0));
          lbits zgsz3548;
          CREATE(lbits)(&zgsz3548);
          CONVERT_OF(lbits, fbits)(&zgsz3548, zT_ie, UINT64_C(11) , true);
          zgaz3261 = bitvector_access(zgsz3548, zgsz3549);
          KILL(sail_int)(&zgsz3549);
          KILL(lbits)(&zgsz3548);
        }
        zgsz3550 = eq_bit(zgaz3261, UINT64_C(1));
      
        }
        zlostSignificantTop = zgsz3550;
        unit zgsz3564;
        zgsz3564 = UNIT;
      }
      {
        uint64_t zgaz3263;
        {
          int64_t zgaz3262;
          {
            sail_int zgsz3551;
            CREATE(sail_int)(&zgsz3551);
            CONVERT_OF(sail_int, mach_int)(&zgsz3551, ze);
            sail_int zgsz3552;
            CREATE(sail_int)(&zgsz3552);
            CONVERT_OF(sail_int, mach_int)(&zgsz3552, INT64_C(4));
            sail_int zgsz3553;
            CREATE(sail_int)(&zgsz3553);
            add_int(&zgsz3553, zgsz3551, zgsz3552);
            zgaz3262 = CONVERT_OF(mach_int, sail_int)(zgsz3553);
            KILL(sail_int)(&zgsz3553);
            KILL(sail_int)(&zgsz3552);
            KILL(sail_int)(&zgsz3551);
          }
          {
            lbits zgsz3554;
            CREATE(lbits)(&zgsz3554);
            CONVERT_OF(lbits, fbits)(&zgsz3554, zbase, UINT64_C(64) , true);
            sail_int zgsz3555;
            CREATE(sail_int)(&zgsz3555);
            CONVERT_OF(sail_int, mach_int)(&zgsz3555, zgaz3262);
            lbits zgsz3556;
            CREATE(lbits)(&zgsz3556);
            shiftr(&zgsz3556, zgsz3554, zgsz3555);
            zgaz3263 = CONVERT_OF(fbits, lbits)(zgsz3556, true);
            KILL(lbits)(&zgsz3556);
            KILL(sail_int)(&zgsz3555);
            KILL(lbits)(&zgsz3554);
          }
        
        }
        int64_t zgaz3264;
        {
          sail_int zgsz3557;
          CREATE(sail_int)(&zgsz3557);
          CONVERT_OF(sail_int, mach_int)(&zgsz3557, zcap_mantissa_width);
          sail_int zgsz3558;
          CREATE(sail_int)(&zgsz3558);
          CONVERT_OF(sail_int, mach_int)(&zgsz3558, INT64_C(3));
          sail_int zgsz3559;
          CREATE(sail_int)(&zgsz3559);
          sub_int(&zgsz3559, zgsz3557, zgsz3558);
          zgaz3264 = CONVERT_OF(mach_int, sail_int)(zgsz3559);
          KILL(sail_int)(&zgsz3559);
          KILL(sail_int)(&zgsz3558);
          KILL(sail_int)(&zgsz3557);
        }
        {
          lbits zgsz3560;
          CREATE(lbits)(&zgsz3560);
          CONVERT_OF(lbits, fbits)(&zgsz3560, zgaz3263, UINT64_C(64) , true);
          sail_int zgsz3561;
          CREATE(sail_int)(&zgsz3561);
          CONVERT_OF(sail_int, mach_int)(&zgsz3561, zgaz3264);
          lbits zgsz3562;
          CREATE(lbits)(&zgsz3562);
          sail_truncate(&zgsz3562, zgsz3560, zgsz3561);
          zB_ie = CONVERT_OF(fbits, lbits)(zgsz3562, true);
          KILL(lbits)(&zgsz3562);
          KILL(sail_int)(&zgsz3561);
          KILL(lbits)(&zgsz3560);
        }
        unit zgsz3563;
        zgsz3563 = UNIT;
      
      
      }
      int64_t zincT;
      if (zlostSignificantTop) {  zincT = INT64_C(1);  } else {  zincT = INT64_C(0);  }
      uint64_t zgaz3268;
      {
        lbits zgaz3266;
        CREATE(lbits)(&zgaz3266);
        {
          int64_t zgaz3265;
          {
            sail_int zgsz3567;
            CREATE(sail_int)(&zgsz3567);
            CONVERT_OF(sail_int, mach_int)(&zgsz3567, ze);
            sail_int zgsz3568;
            CREATE(sail_int)(&zgsz3568);
            CONVERT_OF(sail_int, mach_int)(&zgsz3568, INT64_C(4));
            sail_int zgsz3569;
            CREATE(sail_int)(&zgsz3569);
            add_int(&zgsz3569, zgsz3567, zgsz3568);
            zgaz3265 = CONVERT_OF(mach_int, sail_int)(zgsz3569);
            KILL(sail_int)(&zgsz3569);
            KILL(sail_int)(&zgsz3568);
            KILL(sail_int)(&zgsz3567);
          }
          {
            sail_int zgsz3570;
            CREATE(sail_int)(&zgsz3570);
            CONVERT_OF(sail_int, mach_int)(&zgsz3570, zgaz3265);
            shiftr(&zgaz3266, ztop, zgsz3570);
            KILL(sail_int)(&zgsz3570);
          }
        
        }
        int64_t zgaz3267;
        {
          sail_int zgsz3571;
          CREATE(sail_int)(&zgsz3571);
          CONVERT_OF(sail_int, mach_int)(&zgsz3571, zcap_mantissa_width);
          sail_int zgsz3572;
          CREATE(sail_int)(&zgsz3572);
          CONVERT_OF(sail_int, mach_int)(&zgsz3572, INT64_C(3));
          sail_int zgsz3573;
          CREATE(sail_int)(&zgsz3573);
          sub_int(&zgsz3573, zgsz3571, zgsz3572);
          zgaz3267 = CONVERT_OF(mach_int, sail_int)(zgsz3573);
          KILL(sail_int)(&zgsz3573);
          KILL(sail_int)(&zgsz3572);
          KILL(sail_int)(&zgsz3571);
        }
        {
          sail_int zgsz3574;
          CREATE(sail_int)(&zgsz3574);
          CONVERT_OF(sail_int, mach_int)(&zgsz3574, zgaz3267);
          lbits zgsz3575;
          CREATE(lbits)(&zgsz3575);
          sail_truncate(&zgsz3575, zgaz3266, zgsz3574);
          zgaz3268 = CONVERT_OF(fbits, lbits)(zgsz3575, true);
          KILL(lbits)(&zgsz3575);
          KILL(sail_int)(&zgsz3574);
        }
      
        KILL(lbits)(&zgaz3266);
      }
      {
        lbits zgsz3576;
        CREATE(lbits)(&zgsz3576);
        CONVERT_OF(lbits, fbits)(&zgsz3576, zgaz3268, UINT64_C(11) , true);
        sail_int zgsz3577;
        CREATE(sail_int)(&zgsz3577);
        CONVERT_OF(sail_int, mach_int)(&zgsz3577, zincT);
        lbits zgsz3578;
        CREATE(lbits)(&zgsz3578);
        add_bits_int(&zgsz3578, zgsz3576, zgsz3577);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3578, true);
        KILL(lbits)(&zgsz3578);
        KILL(sail_int)(&zgsz3577);
        KILL(lbits)(&zgsz3576);
      }
      zgsz3544 = UNIT;
    
    
      } else {  zgsz3544 = UNIT;  }
    
    }
    {
      {
        lbits zgsz3540;
        CREATE(lbits)(&zgsz3540);
        CONVERT_OF(lbits, fbits)(&zgsz3540, zB_ie, UINT64_C(11) , true);
        lbits zgsz3541;
        CREATE(lbits)(&zgsz3541);
        CONVERT_OF(lbits, fbits)(&zgsz3541, UINT64_C(0b000), UINT64_C(3) , true);
        lbits zgsz3542;
        CREATE(lbits)(&zgsz3542);
        append(&zgsz3542, zgsz3540, zgsz3541);
        zBbits = CONVERT_OF(fbits, lbits)(zgsz3542, true);
        KILL(lbits)(&zgsz3542);
        KILL(lbits)(&zgsz3541);
        KILL(lbits)(&zgsz3540);
      }
      unit zgsz3543;
      zgsz3543 = UNIT;
    }
    {
      lbits zgsz3579;
      CREATE(lbits)(&zgsz3579);
      CONVERT_OF(lbits, fbits)(&zgsz3579, zT_ie, UINT64_C(11) , true);
      lbits zgsz3580;
      CREATE(lbits)(&zgsz3580);
      CONVERT_OF(lbits, fbits)(&zgsz3580, UINT64_C(0b000), UINT64_C(3) , true);
      lbits zgsz3581;
      CREATE(lbits)(&zgsz3581);
      append(&zgsz3581, zgsz3579, zgsz3580);
      zTbits = CONVERT_OF(fbits, lbits)(zgsz3581, true);
      KILL(lbits)(&zgsz3581);
      KILL(lbits)(&zgsz3580);
      KILL(lbits)(&zgsz3579);
    }
    zgsz3494 = UNIT;
  
    KILL(lbits)(&zmaskLo);
  
  
    } else {  zgsz3494 = UNIT;  }
  }
  struct zCapability znewCap;
  {
    uint64_t zgaz3280;
    {
      int64_t zgaz3279;
      if (zincE) {
      {
        sail_int zgsz3582;
        CREATE(sail_int)(&zgsz3582);
        CONVERT_OF(sail_int, mach_int)(&zgsz3582, ze);
        sail_int zgsz3583;
        CREATE(sail_int)(&zgsz3583);
        CONVERT_OF(sail_int, mach_int)(&zgsz3583, INT64_C(1));
        sail_int zgsz3584;
        CREATE(sail_int)(&zgsz3584);
        add_int(&zgsz3584, zgsz3582, zgsz3583);
        zgaz3279 = CONVERT_OF(mach_int, sail_int)(zgsz3584);
        KILL(sail_int)(&zgsz3584);
        KILL(sail_int)(&zgsz3583);
        KILL(sail_int)(&zgsz3582);
      }
      } else {  zgaz3279 = ze;  }
      {
        sail_int zgsz3585;
        CREATE(sail_int)(&zgsz3585);
        CONVERT_OF(sail_int, mach_int)(&zgsz3585, zgaz3279);
        zgaz3280 = sailgen_zzzz7lzzJzzK6z3to_bits(INT64_C(6), zgsz3585);
        KILL(sail_int)(&zgsz3585);
      }
    
    }
    struct zCapability zgsz3586;
    zgsz3586 = zcap;
    zgsz3586.zB = zBbits;
    zgsz3586.zE = zgaz3280;
    zgsz3586.zT = zTbits;
    zgsz3586.zaddress = zbase;
    zgsz3586.zinternal_E = zie;
    znewCap = zgsz3586;
  
  
  }
  bool zexact;
  {
    bool zgaz3278;
    {
      bool zgsz3587;
      if (zlostSignificantBase) {  zgsz3587 = true;  } else {  zgsz3587 = zlostSignificantTop;  }
      zgaz3278 = zgsz3587;
    }
    zexact = not(zgaz3278);
  
  }
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgsz3588;
  zgsz3588.ztup0 = zexact;
  zgsz3588.ztup1 = znewCap;
  zcbz345 = zgsz3588;










  KILL(lbits)(&zlength);
  KILL(lbits)(&zext_base);
end_function_179: ;
  return zcbz345;
end_block_exception_180: ;
  struct zCapability zcbz367 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zcbz366 = { .ztup1 = zcbz367, .ztup0 = false };
  return zcbz366;
}

static uint64_t sailgen_getCapPerms(struct zCapability);

static uint64_t sailgen_getCapPerms(struct zCapability zcap)
{
  __label__ end_function_182, end_block_exception_183;

  uint64_t zcbz346;
  uint64_t zperms;
  {
    uint64_t zgaz3288;
    zgaz3288 = sailgen_getCapHardPerms(zcap);
    zperms = sailgen_zzzz7mzzJzzK15zzCzz0zz7nzzJzzK12z3EXTZ(INT64_C(15), zgaz3288);
  
  }
  uint64_t zgaz3287;
  zgaz3287 = zcap.zuperms;
  {
    lbits zgsz3589;
    CREATE(lbits)(&zgsz3589);
    CONVERT_OF(lbits, fbits)(&zgsz3589, zgaz3287, UINT64_C(4) , true);
    lbits zgsz3590;
    CREATE(lbits)(&zgsz3590);
    CONVERT_OF(lbits, fbits)(&zgsz3590, zperms, UINT64_C(15) , true);
    lbits zgsz3591;
    CREATE(lbits)(&zgsz3591);
    append(&zgsz3591, zgsz3589, zgsz3590);
    zcbz346 = CONVERT_OF(fbits, lbits)(zgsz3591, true);
    KILL(lbits)(&zgsz3591);
    KILL(lbits)(&zgsz3590);
    KILL(lbits)(&zgsz3589);
  }


end_function_182: ;
  return zcbz346;
end_block_exception_183: ;

  return UINT64_C(0xdeadc0de);
}

static struct zCapability sailgen_setCapPerms(struct zCapability, uint64_t);

static struct zCapability sailgen_setCapPerms(struct zCapability zcap, uint64_t zperms)
{
  __label__ end_function_185, end_block_exception_186;

  struct zCapability zcbz347;
  uint64_t zgaz3290;
  {
    uint64_t zgaz3289;
    {
      lbits zgsz3592;
      CREATE(lbits)(&zgsz3592);
      CONVERT_OF(lbits, fbits)(&zgsz3592, zperms, UINT64_C(19) , true);
      sail_int zgsz3593;
      CREATE(sail_int)(&zgsz3593);
      CONVERT_OF(sail_int, mach_int)(&zgsz3593, zcap_uperms_shift);
      lbits zgsz3594;
      CREATE(lbits)(&zgsz3594);
      shiftr(&zgsz3594, zgsz3592, zgsz3593);
      zgaz3289 = CONVERT_OF(fbits, lbits)(zgsz3594, true);
      KILL(lbits)(&zgsz3594);
      KILL(sail_int)(&zgsz3593);
      KILL(lbits)(&zgsz3592);
    }
    {
      lbits zgsz3595;
      CREATE(lbits)(&zgsz3595);
      CONVERT_OF(lbits, fbits)(&zgsz3595, zgaz3289, UINT64_C(19) , true);
      sail_int zgsz3596;
      CREATE(sail_int)(&zgsz3596);
      CONVERT_OF(sail_int, mach_int)(&zgsz3596, zcap_uperms_width);
      lbits zgsz3597;
      CREATE(lbits)(&zgsz3597);
      sail_truncate(&zgsz3597, zgsz3595, zgsz3596);
      zgaz3290 = CONVERT_OF(fbits, lbits)(zgsz3597, true);
      KILL(lbits)(&zgsz3597);
      KILL(sail_int)(&zgsz3596);
      KILL(lbits)(&zgsz3595);
    }
  
  }
  bool zgaz3292;
  {
    fbits zgaz3291;
    {
      sail_int zgsz3599;
      CREATE(sail_int)(&zgsz3599);
      CONVERT_OF(sail_int, mach_int)(&zgsz3599, INT64_C(11));
      lbits zgsz3598;
      CREATE(lbits)(&zgsz3598);
      CONVERT_OF(lbits, fbits)(&zgsz3598, zperms, UINT64_C(19) , true);
      zgaz3291 = bitvector_access(zgsz3598, zgsz3599);
      KILL(sail_int)(&zgsz3599);
      KILL(lbits)(&zgsz3598);
    }
    zgaz3292 = sailgen_bit_to_bool(zgaz3291);
  
  }
  bool zgaz3294;
  {
    fbits zgaz3293;
    {
      sail_int zgsz3601;
      CREATE(sail_int)(&zgsz3601);
      CONVERT_OF(sail_int, mach_int)(&zgsz3601, INT64_C(10));
      lbits zgsz3600;
      CREATE(lbits)(&zgsz3600);
      CONVERT_OF(lbits, fbits)(&zgsz3600, zperms, UINT64_C(19) , true);
      zgaz3293 = bitvector_access(zgsz3600, zgsz3601);
      KILL(sail_int)(&zgsz3601);
      KILL(lbits)(&zgsz3600);
    }
    zgaz3294 = sailgen_bit_to_bool(zgaz3293);
  
  }
  bool zgaz3296;
  {
    fbits zgaz3295;
    {
      sail_int zgsz3603;
      CREATE(sail_int)(&zgsz3603);
      CONVERT_OF(sail_int, mach_int)(&zgsz3603, INT64_C(9));
      lbits zgsz3602;
      CREATE(lbits)(&zgsz3602);
      CONVERT_OF(lbits, fbits)(&zgsz3602, zperms, UINT64_C(19) , true);
      zgaz3295 = bitvector_access(zgsz3602, zgsz3603);
      KILL(sail_int)(&zgsz3603);
      KILL(lbits)(&zgsz3602);
    }
    zgaz3296 = sailgen_bit_to_bool(zgaz3295);
  
  }
  bool zgaz3298;
  {
    fbits zgaz3297;
    {
      sail_int zgsz3605;
      CREATE(sail_int)(&zgsz3605);
      CONVERT_OF(sail_int, mach_int)(&zgsz3605, INT64_C(8));
      lbits zgsz3604;
      CREATE(lbits)(&zgsz3604);
      CONVERT_OF(lbits, fbits)(&zgsz3604, zperms, UINT64_C(19) , true);
      zgaz3297 = bitvector_access(zgsz3604, zgsz3605);
      KILL(sail_int)(&zgsz3605);
      KILL(lbits)(&zgsz3604);
    }
    zgaz3298 = sailgen_bit_to_bool(zgaz3297);
  
  }
  bool zgaz3300;
  {
    fbits zgaz3299;
    {
      sail_int zgsz3607;
      CREATE(sail_int)(&zgsz3607);
      CONVERT_OF(sail_int, mach_int)(&zgsz3607, INT64_C(7));
      lbits zgsz3606;
      CREATE(lbits)(&zgsz3606);
      CONVERT_OF(lbits, fbits)(&zgsz3606, zperms, UINT64_C(19) , true);
      zgaz3299 = bitvector_access(zgsz3606, zgsz3607);
      KILL(sail_int)(&zgsz3607);
      KILL(lbits)(&zgsz3606);
    }
    zgaz3300 = sailgen_bit_to_bool(zgaz3299);
  
  }
  bool zgaz3302;
  {
    fbits zgaz3301;
    {
      sail_int zgsz3609;
      CREATE(sail_int)(&zgsz3609);
      CONVERT_OF(sail_int, mach_int)(&zgsz3609, INT64_C(6));
      lbits zgsz3608;
      CREATE(lbits)(&zgsz3608);
      CONVERT_OF(lbits, fbits)(&zgsz3608, zperms, UINT64_C(19) , true);
      zgaz3301 = bitvector_access(zgsz3608, zgsz3609);
      KILL(sail_int)(&zgsz3609);
      KILL(lbits)(&zgsz3608);
    }
    zgaz3302 = sailgen_bit_to_bool(zgaz3301);
  
  }
  bool zgaz3304;
  {
    fbits zgaz3303;
    {
      sail_int zgsz3611;
      CREATE(sail_int)(&zgsz3611);
      CONVERT_OF(sail_int, mach_int)(&zgsz3611, INT64_C(5));
      lbits zgsz3610;
      CREATE(lbits)(&zgsz3610);
      CONVERT_OF(lbits, fbits)(&zgsz3610, zperms, UINT64_C(19) , true);
      zgaz3303 = bitvector_access(zgsz3610, zgsz3611);
      KILL(sail_int)(&zgsz3611);
      KILL(lbits)(&zgsz3610);
    }
    zgaz3304 = sailgen_bit_to_bool(zgaz3303);
  
  }
  bool zgaz3306;
  {
    fbits zgaz3305;
    {
      sail_int zgsz3613;
      CREATE(sail_int)(&zgsz3613);
      CONVERT_OF(sail_int, mach_int)(&zgsz3613, INT64_C(4));
      lbits zgsz3612;
      CREATE(lbits)(&zgsz3612);
      CONVERT_OF(lbits, fbits)(&zgsz3612, zperms, UINT64_C(19) , true);
      zgaz3305 = bitvector_access(zgsz3612, zgsz3613);
      KILL(sail_int)(&zgsz3613);
      KILL(lbits)(&zgsz3612);
    }
    zgaz3306 = sailgen_bit_to_bool(zgaz3305);
  
  }
  bool zgaz3308;
  {
    fbits zgaz3307;
    {
      sail_int zgsz3615;
      CREATE(sail_int)(&zgsz3615);
      CONVERT_OF(sail_int, mach_int)(&zgsz3615, INT64_C(3));
      lbits zgsz3614;
      CREATE(lbits)(&zgsz3614);
      CONVERT_OF(lbits, fbits)(&zgsz3614, zperms, UINT64_C(19) , true);
      zgaz3307 = bitvector_access(zgsz3614, zgsz3615);
      KILL(sail_int)(&zgsz3615);
      KILL(lbits)(&zgsz3614);
    }
    zgaz3308 = sailgen_bit_to_bool(zgaz3307);
  
  }
  bool zgaz3310;
  {
    fbits zgaz3309;
    {
      sail_int zgsz3617;
      CREATE(sail_int)(&zgsz3617);
      CONVERT_OF(sail_int, mach_int)(&zgsz3617, INT64_C(2));
      lbits zgsz3616;
      CREATE(lbits)(&zgsz3616);
      CONVERT_OF(lbits, fbits)(&zgsz3616, zperms, UINT64_C(19) , true);
      zgaz3309 = bitvector_access(zgsz3616, zgsz3617);
      KILL(sail_int)(&zgsz3617);
      KILL(lbits)(&zgsz3616);
    }
    zgaz3310 = sailgen_bit_to_bool(zgaz3309);
  
  }
  bool zgaz3312;
  {
    fbits zgaz3311;
    {
      sail_int zgsz3619;
      CREATE(sail_int)(&zgsz3619);
      CONVERT_OF(sail_int, mach_int)(&zgsz3619, INT64_C(1));
      lbits zgsz3618;
      CREATE(lbits)(&zgsz3618);
      CONVERT_OF(lbits, fbits)(&zgsz3618, zperms, UINT64_C(19) , true);
      zgaz3311 = bitvector_access(zgsz3618, zgsz3619);
      KILL(sail_int)(&zgsz3619);
      KILL(lbits)(&zgsz3618);
    }
    zgaz3312 = sailgen_bit_to_bool(zgaz3311);
  
  }
  bool zgaz3314;
  {
    fbits zgaz3313;
    {
      sail_int zgsz3621;
      CREATE(sail_int)(&zgsz3621);
      CONVERT_OF(sail_int, mach_int)(&zgsz3621, INT64_C(0));
      lbits zgsz3620;
      CREATE(lbits)(&zgsz3620);
      CONVERT_OF(lbits, fbits)(&zgsz3620, zperms, UINT64_C(19) , true);
      zgaz3313 = bitvector_access(zgsz3620, zgsz3621);
      KILL(sail_int)(&zgsz3621);
      KILL(lbits)(&zgsz3620);
    }
    zgaz3314 = sailgen_bit_to_bool(zgaz3313);
  
  }
  struct zCapability zgsz3622;
  zgsz3622 = zcap;
  zgsz3622.zaccess_system_regs = zgaz3294;
  zgsz3622.zglobal = zgaz3314;
  zgsz3622.zpermit_cinvoke = zgaz3298;
  zgsz3622.zpermit_execute = zgaz3312;
  zgsz3622.zpermit_load = zgaz3310;
  zgsz3622.zpermit_load_cap = zgaz3306;
  zgsz3622.zpermit_seal = zgaz3300;
  zgsz3622.zpermit_set_CID = zgaz3292;
  zgsz3622.zpermit_store = zgaz3308;
  zgsz3622.zpermit_store_cap = zgaz3304;
  zgsz3622.zpermit_store_local_cap = zgaz3302;
  zgsz3622.zpermit_unseal = zgaz3296;
  zgsz3622.zuperms = zgaz3290;
  zcbz347 = zgsz3622;














end_function_185: ;
  return zcbz347;
end_block_exception_186: ;
  struct zCapability zcbz368 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz368;
}

static uint64_t sailgen_getCapFlags(struct zCapability);

static uint64_t sailgen_getCapFlags(struct zCapability zcap)
{
  __label__ end_function_188, end_block_exception_189;

  uint64_t zcbz348;
  bool zgaz3315;
  zgaz3315 = zcap.zflag_cap_mode;
  zcbz348 = sailgen_bool_to_bits(zgaz3315);

end_function_188: ;
  return zcbz348;
end_block_exception_189: ;

  return UINT64_C(0xdeadc0de);
}

static bool sailgen_isCapSealed(struct zCapability);

static bool sailgen_isCapSealed(struct zCapability zcap)
{
  __label__ end_function_191, end_block_exception_192;

  bool zcbz349;
  int64_t zgaz3317;
  {
    uint64_t zgaz3316;
    zgaz3316 = zcap.zotype;
    zgaz3317 = fast_signed(zgaz3316, 18);
  
  }
  {
    sail_int zgsz3623;
    CREATE(sail_int)(&zgsz3623);
    CONVERT_OF(sail_int, mach_int)(&zgsz3623, zgaz3317);
    zcbz349 = sailgen_zzz3zzzz7mzzJzzKzzD1z3neq_int(zgsz3623, zotype_unsealed);
    KILL(sail_int)(&zgsz3623);
  }

end_function_191: ;
  return zcbz349;
end_block_exception_192: ;

  return false;
}

static bool sailgen_hasReservedOType(struct zCapability);

static bool sailgen_hasReservedOType(struct zCapability zcap)
{
  __label__ end_function_194, end_block_exception_195;

  bool zcbz350;
  int64_t zgaz3319;
  {
    uint64_t zgaz3318;
    zgaz3318 = zcap.zotype;
    zgaz3319 = ((mach_int) zgaz3318);
  
  }
  {
    sail_int zgsz3625;
    CREATE(sail_int)(&zgsz3625);
    CONVERT_OF(sail_int, mach_int)(&zgsz3625, zcap_max_otype);
    sail_int zgsz3624;
    CREATE(sail_int)(&zgsz3624);
    CONVERT_OF(sail_int, mach_int)(&zgsz3624, zgaz3319);
    zcbz350 = gt(zgsz3624, zgsz3625);
    KILL(sail_int)(&zgsz3625);
    KILL(sail_int)(&zgsz3624);
  }

end_function_194: ;
  return zcbz350;
end_block_exception_195: ;

  return false;
}

static uint64_t sailgen_getCapBaseBits(struct zCapability);

static uint64_t sailgen_getCapBaseBits(struct zCapability zc)
{
  __label__ case_198, finish_match_197, end_function_199, end_block_exception_200;

  uint64_t zcbz351;
  struct ztuple_z8z5bv64zCz0z5bvz9 zgaz3320;
  CREATE(ztuple_z8z5bv64zCz0z5bvz9)(&zgaz3320);
  sailgen_getCapBoundsBits(&zgaz3320, zc);
  uint64_t zgsz3626;
  {
    uint64_t zbase;
    zbase = zgaz3320.ztup0;
    zgsz3626 = zbase;
  
    goto finish_match_197;
  }
case_198: ;
  sail_match_failure("getCapBaseBits");
finish_match_197: ;
  zcbz351 = zgsz3626;

  KILL(ztuple_z8z5bv64zCz0z5bvz9)(&zgaz3320);
end_function_199: ;
  return zcbz351;
end_block_exception_200: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_getCapOffsetBits(struct zCapability);

static uint64_t sailgen_getCapOffsetBits(struct zCapability zc)
{
  __label__ end_function_202, end_block_exception_203;

  uint64_t zcbz352;
  uint64_t zbase;
  zbase = sailgen_getCapBaseBits(zc);
  uint64_t zgaz3321;
  zgaz3321 = zc.zaddress;
  {
    lbits zgsz3628;
    CREATE(lbits)(&zgsz3628);
    CONVERT_OF(lbits, fbits)(&zgsz3628, zgaz3321, UINT64_C(64) , true);
    lbits zgsz3629;
    CREATE(lbits)(&zgsz3629);
    CONVERT_OF(lbits, fbits)(&zgsz3629, zbase, UINT64_C(64) , true);
    lbits zgsz3630;
    CREATE(lbits)(&zgsz3630);
    sub_bits(&zgsz3630, zgsz3628, zgsz3629);
    zcbz352 = CONVERT_OF(fbits, lbits)(zgsz3630, true);
    KILL(lbits)(&zgsz3630);
    KILL(lbits)(&zgsz3629);
    KILL(lbits)(&zgsz3628);
  }


end_function_202: ;
  return zcbz352;
end_block_exception_203: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_getCapLength(sail_int *rop, struct zCapability);

static void sailgen_getCapLength(sail_int *zcbz353, struct zCapability zc)
{
  __label__ case_206, finish_match_205, end_function_207, end_block_exception_208, end_function_234;

  struct ztuple_z8z5izCz0z5iz9 zgaz3322;
  CREATE(ztuple_z8z5izCz0z5iz9)(&zgaz3322);
  sailgen_getCapBounds(&zgaz3322, zc);
  sail_int zgsz3631;
  CREATE(sail_int)(&zgsz3631);
  {
    sail_int zbase;
    CREATE(sail_int)(&zbase);
    COPY(sail_int)(&zbase, zgaz3322.ztup0);
    sail_int ztop;
    CREATE(sail_int)(&ztop);
    COPY(sail_int)(&ztop, zgaz3322.ztup1);
    {
      bool zgaz3325;
      {
        bool zgaz3324;
        {
          bool zgaz3323;
          zgaz3323 = zc.ztag;
          zgaz3324 = not(zgaz3323);
        
        }
        bool zgsz3632;
        if (zgaz3324) {  zgsz3632 = true;  } else {  zgsz3632 = gteq(ztop, zbase);  }
        zgaz3325 = zgsz3632;
      
      }
      unit zgsz3633;
      zgsz3633 = sail_assert(zgaz3325, "src/cheri_cap_common.sail 425:40 - 425:41");
    
    }
    sail_int zgaz3326;
    CREATE(sail_int)(&zgaz3326);
    sub_int(&zgaz3326, ztop, zbase);
    sail_int zgaz3327;
    CREATE(sail_int)(&zgaz3327);
    {
      sail_int zgsz3634;
      CREATE(sail_int)(&zgsz3634);
      CONVERT_OF(sail_int, mach_int)(&zgsz3634, zcap_len_width);
      pow2(&zgaz3327, zgsz3634);
      KILL(sail_int)(&zgsz3634);
    }
    emod_int(&zgsz3631, zgaz3326, zgaz3327);
    KILL(sail_int)(&zgaz3327);
    KILL(sail_int)(&zgaz3326);
    KILL(sail_int)(&ztop);
    KILL(sail_int)(&zbase);
    goto finish_match_205;
  }
case_206: ;
  sail_match_failure("getCapLength");
finish_match_205: ;
  COPY(sail_int)((*(&zcbz353)), zgsz3631);
  KILL(sail_int)(&zgsz3631);
  KILL(ztuple_z8z5izCz0z5iz9)(&zgaz3322);
end_function_207: ;
  goto end_function_234;
end_block_exception_208: ;
  goto end_function_234;
end_function_234: ;
}

static bool sailgen_fastRepCheck(struct zCapability, uint64_t);

static bool sailgen_fastRepCheck(struct zCapability zc, uint64_t zi)
{
  __label__ end_function_210, end_block_exception_211;

  bool zcbz354;
  int64_t zE;
  {
    uint64_t zgaz3341;
    zgaz3341 = zc.zE;
    zE = ((mach_int) zgaz3341);
  
  }
  bool zgaz3329;
  {
    int64_t zgaz3328;
    {
      sail_int zgsz3636;
      CREATE(sail_int)(&zgsz3636);
      CONVERT_OF(sail_int, mach_int)(&zgsz3636, zcap_max_E);
      sail_int zgsz3637;
      CREATE(sail_int)(&zgsz3637);
      CONVERT_OF(sail_int, mach_int)(&zgsz3637, INT64_C(2));
      sail_int zgsz3638;
      CREATE(sail_int)(&zgsz3638);
      sub_int(&zgsz3638, zgsz3636, zgsz3637);
      zgaz3328 = CONVERT_OF(mach_int, sail_int)(zgsz3638);
      KILL(sail_int)(&zgsz3638);
      KILL(sail_int)(&zgsz3637);
      KILL(sail_int)(&zgsz3636);
    }
    {
      sail_int zgsz3640;
      CREATE(sail_int)(&zgsz3640);
      CONVERT_OF(sail_int, mach_int)(&zgsz3640, zgaz3328);
      sail_int zgsz3639;
      CREATE(sail_int)(&zgsz3639);
      CONVERT_OF(sail_int, mach_int)(&zgsz3639, zE);
      zgaz3329 = gteq(zgsz3639, zgsz3640);
      KILL(sail_int)(&zgsz3640);
      KILL(sail_int)(&zgsz3639);
    }
  
  }
  if (zgaz3329) {  zcbz354 = true;  } else {
  int64_t zi_top;
  {
    uint64_t zgaz3340;
    {
      int64_t zgaz3339;
      {
        sail_int zgsz3641;
        CREATE(sail_int)(&zgsz3641);
        CONVERT_OF(sail_int, mach_int)(&zgsz3641, zE);
        sail_int zgsz3642;
        CREATE(sail_int)(&zgsz3642);
        CONVERT_OF(sail_int, mach_int)(&zgsz3642, zcap_mantissa_width);
        sail_int zgsz3643;
        CREATE(sail_int)(&zgsz3643);
        add_int(&zgsz3643, zgsz3641, zgsz3642);
        zgaz3339 = CONVERT_OF(mach_int, sail_int)(zgsz3643);
        KILL(sail_int)(&zgsz3643);
        KILL(sail_int)(&zgsz3642);
        KILL(sail_int)(&zgsz3641);
      }
      {
        lbits zgsz3644;
        CREATE(lbits)(&zgsz3644);
        CONVERT_OF(lbits, fbits)(&zgsz3644, zi, UINT64_C(64) , true);
        sail_int zgsz3645;
        CREATE(sail_int)(&zgsz3645);
        CONVERT_OF(sail_int, mach_int)(&zgsz3645, zgaz3339);
        lbits zgsz3646;
        CREATE(lbits)(&zgsz3646);
        arith_shiftr(&zgsz3646, zgsz3644, zgsz3645);
        zgaz3340 = CONVERT_OF(fbits, lbits)(zgsz3646, true);
        KILL(lbits)(&zgsz3646);
        KILL(sail_int)(&zgsz3645);
        KILL(lbits)(&zgsz3644);
      }
    
    }
    zi_top = fast_signed(zgaz3340, 64);
  
  }
  uint64_t zi_mid;
  {
    uint64_t zgaz3338;
    {
      lbits zgsz3647;
      CREATE(lbits)(&zgsz3647);
      CONVERT_OF(lbits, fbits)(&zgsz3647, zi, UINT64_C(64) , true);
      sail_int zgsz3648;
      CREATE(sail_int)(&zgsz3648);
      CONVERT_OF(sail_int, mach_int)(&zgsz3648, zE);
      lbits zgsz3649;
      CREATE(lbits)(&zgsz3649);
      shiftr(&zgsz3649, zgsz3647, zgsz3648);
      zgaz3338 = CONVERT_OF(fbits, lbits)(zgsz3649, true);
      KILL(lbits)(&zgsz3649);
      KILL(sail_int)(&zgsz3648);
      KILL(lbits)(&zgsz3647);
    }
    {
      lbits zgsz3650;
      CREATE(lbits)(&zgsz3650);
      CONVERT_OF(lbits, fbits)(&zgsz3650, zgaz3338, UINT64_C(64) , true);
      sail_int zgsz3651;
      CREATE(sail_int)(&zgsz3651);
      CONVERT_OF(sail_int, mach_int)(&zgsz3651, zcap_mantissa_width);
      lbits zgsz3652;
      CREATE(lbits)(&zgsz3652);
      sail_truncate(&zgsz3652, zgsz3650, zgsz3651);
      zi_mid = CONVERT_OF(fbits, lbits)(zgsz3652, true);
      KILL(lbits)(&zgsz3652);
      KILL(sail_int)(&zgsz3651);
      KILL(lbits)(&zgsz3650);
    }
  
  }
  uint64_t za_mid;
  {
    uint64_t zgaz3337;
    {
      uint64_t zgaz3336;
      zgaz3336 = zc.zaddress;
      {
        lbits zgsz3653;
        CREATE(lbits)(&zgsz3653);
        CONVERT_OF(lbits, fbits)(&zgsz3653, zgaz3336, UINT64_C(64) , true);
        sail_int zgsz3654;
        CREATE(sail_int)(&zgsz3654);
        CONVERT_OF(sail_int, mach_int)(&zgsz3654, zE);
        lbits zgsz3655;
        CREATE(lbits)(&zgsz3655);
        shiftr(&zgsz3655, zgsz3653, zgsz3654);
        zgaz3337 = CONVERT_OF(fbits, lbits)(zgsz3655, true);
        KILL(lbits)(&zgsz3655);
        KILL(sail_int)(&zgsz3654);
        KILL(lbits)(&zgsz3653);
      }
    
    }
    {
      lbits zgsz3656;
      CREATE(lbits)(&zgsz3656);
      CONVERT_OF(lbits, fbits)(&zgsz3656, zgaz3337, UINT64_C(64) , true);
      sail_int zgsz3657;
      CREATE(sail_int)(&zgsz3657);
      CONVERT_OF(sail_int, mach_int)(&zgsz3657, zcap_mantissa_width);
      lbits zgsz3658;
      CREATE(lbits)(&zgsz3658);
      sail_truncate(&zgsz3658, zgsz3656, zgsz3657);
      za_mid = CONVERT_OF(fbits, lbits)(zgsz3658, true);
      KILL(lbits)(&zgsz3658);
      KILL(sail_int)(&zgsz3657);
      KILL(lbits)(&zgsz3656);
    }
  
  }
  uint64_t zB3;
  {
    uint64_t zgaz3335;
    zgaz3335 = zc.zB;
    {
      lbits zgsz3659;
      CREATE(lbits)(&zgsz3659);
      CONVERT_OF(lbits, fbits)(&zgsz3659, zgaz3335, UINT64_C(14) , true);
      sail_int zgsz3660;
      CREATE(sail_int)(&zgsz3660);
      CONVERT_OF(sail_int, mach_int)(&zgsz3660, INT64_C(3));
      lbits zgsz3661;
      CREATE(lbits)(&zgsz3661);
      sail_truncateLSB(&zgsz3661, zgsz3659, zgsz3660);
      zB3 = CONVERT_OF(fbits, lbits)(zgsz3661, true);
      KILL(lbits)(&zgsz3661);
      KILL(sail_int)(&zgsz3660);
      KILL(lbits)(&zgsz3659);
    }
  
  }
  uint64_t zR3;
  {
    lbits zgsz3662;
    CREATE(lbits)(&zgsz3662);
    CONVERT_OF(lbits, fbits)(&zgsz3662, zB3, UINT64_C(3) , true);
    lbits zgsz3663;
    CREATE(lbits)(&zgsz3663);
    CONVERT_OF(lbits, fbits)(&zgsz3663, UINT64_C(0b001), UINT64_C(3) , true);
    lbits zgsz3664;
    CREATE(lbits)(&zgsz3664);
    sub_bits(&zgsz3664, zgsz3662, zgsz3663);
    zR3 = CONVERT_OF(fbits, lbits)(zgsz3664, true);
    KILL(lbits)(&zgsz3664);
    KILL(lbits)(&zgsz3663);
    KILL(lbits)(&zgsz3662);
  }
  uint64_t zR;
  {
    uint64_t zgaz3334;
    {
      int64_t zgaz3333;
      {
        sail_int zgsz3665;
        CREATE(sail_int)(&zgsz3665);
        CONVERT_OF(sail_int, mach_int)(&zgsz3665, zcap_mantissa_width);
        sail_int zgsz3666;
        CREATE(sail_int)(&zgsz3666);
        CONVERT_OF(sail_int, mach_int)(&zgsz3666, INT64_C(3));
        sail_int zgsz3667;
        CREATE(sail_int)(&zgsz3667);
        sub_int(&zgsz3667, zgsz3665, zgsz3666);
        zgaz3333 = CONVERT_OF(mach_int, sail_int)(zgsz3667);
        KILL(sail_int)(&zgsz3667);
        KILL(sail_int)(&zgsz3666);
        KILL(sail_int)(&zgsz3665);
      }
      zgaz3334 = sailgen_zzzz7nzzJzzK11z3zzeros_implicit(zgaz3333);
    
    }
    {
      lbits zgsz3668;
      CREATE(lbits)(&zgsz3668);
      CONVERT_OF(lbits, fbits)(&zgsz3668, zR3, UINT64_C(3) , true);
      lbits zgsz3669;
      CREATE(lbits)(&zgsz3669);
      CONVERT_OF(lbits, fbits)(&zgsz3669, zgaz3334, UINT64_C(11) , true);
      lbits zgsz3670;
      CREATE(lbits)(&zgsz3670);
      append(&zgsz3670, zgsz3668, zgsz3669);
      zR = CONVERT_OF(fbits, lbits)(zgsz3670, true);
      KILL(lbits)(&zgsz3670);
      KILL(lbits)(&zgsz3669);
      KILL(lbits)(&zgsz3668);
    }
  
  }
  uint64_t zdiff;
  {
    lbits zgsz3671;
    CREATE(lbits)(&zgsz3671);
    CONVERT_OF(lbits, fbits)(&zgsz3671, zR, UINT64_C(14) , true);
    lbits zgsz3672;
    CREATE(lbits)(&zgsz3672);
    CONVERT_OF(lbits, fbits)(&zgsz3672, za_mid, UINT64_C(14) , true);
    lbits zgsz3673;
    CREATE(lbits)(&zgsz3673);
    sub_bits(&zgsz3673, zgsz3671, zgsz3672);
    zdiff = CONVERT_OF(fbits, lbits)(zgsz3673, true);
    KILL(lbits)(&zgsz3673);
    KILL(lbits)(&zgsz3672);
    KILL(lbits)(&zgsz3671);
  }
  uint64_t zdiff1;
  {
    lbits zgsz3674;
    CREATE(lbits)(&zgsz3674);
    CONVERT_OF(lbits, fbits)(&zgsz3674, zdiff, UINT64_C(14) , true);
    sail_int zgsz3675;
    CREATE(sail_int)(&zgsz3675);
    CONVERT_OF(sail_int, mach_int)(&zgsz3675, INT64_C(1));
    lbits zgsz3676;
    CREATE(lbits)(&zgsz3676);
    sub_bits_int(&zgsz3676, zgsz3674, zgsz3675);
    zdiff1 = CONVERT_OF(fbits, lbits)(zgsz3676, true);
    KILL(lbits)(&zgsz3676);
    KILL(sail_int)(&zgsz3675);
    KILL(lbits)(&zgsz3674);
  }
  bool zgaz3330;
  {
    sail_int zgsz3678;
    CREATE(sail_int)(&zgsz3678);
    CONVERT_OF(sail_int, mach_int)(&zgsz3678, INT64_C(0));
    sail_int zgsz3677;
    CREATE(sail_int)(&zgsz3677);
    CONVERT_OF(sail_int, mach_int)(&zgsz3677, zi_top);
    zgaz3330 = eq_int(zgsz3677, zgsz3678);
    KILL(sail_int)(&zgsz3678);
    KILL(sail_int)(&zgsz3677);
  }
  if (zgaz3330) {  zcbz354 = sailgen_z8operatorz0zzzz7nzzJzzK14z3zI_uz9(zi_mid, zdiff1);  } else {
  bool zgaz3331;
  {
    sail_int zgsz3680;
    CREATE(sail_int)(&zgsz3680);
    CONVERT_OF(sail_int, mach_int)(&zgsz3680, INT64_C(-1));
    sail_int zgsz3679;
    CREATE(sail_int)(&zgsz3679);
    CONVERT_OF(sail_int, mach_int)(&zgsz3679, zi_top);
    zgaz3331 = eq_int(zgsz3679, zgsz3680);
    KILL(sail_int)(&zgsz3680);
    KILL(sail_int)(&zgsz3679);
  }
  if (zgaz3331) {
  bool zgaz3332;
  zgaz3332 = sailgen_z8operatorz0zzzz7nzzJzzK14z3zKzJ_uz9(zi_mid, zdiff);
  bool zgsz3681;
  if (zgaz3332) {
  {
    lbits zgsz3683;
    CREATE(lbits)(&zgsz3683);
    CONVERT_OF(lbits, fbits)(&zgsz3683, za_mid, UINT64_C(14) , true);
    lbits zgsz3682;
    CREATE(lbits)(&zgsz3682);
    CONVERT_OF(lbits, fbits)(&zgsz3682, zR, UINT64_C(14) , true);
    zgsz3681 = neq_bits(zgsz3682, zgsz3683);
    KILL(lbits)(&zgsz3683);
    KILL(lbits)(&zgsz3682);
  }
  } else {  zgsz3681 = false;  }
  zcbz354 = zgsz3681;

  } else {  zcbz354 = false;  }

  }









  }


end_function_210: ;
  return zcbz354;
end_block_exception_211: ;

  return false;
}

static void sailgen_capToString(sail_string *rop, struct zCapability);

static void sailgen_capToString(sail_string *zcbz355, struct zCapability zcap)
{
  __label__ end_function_213, end_block_exception_214, end_function_233;

  sail_int zlen;
  CREATE(sail_int)(&zlen);
  sailgen_getCapLength(&zlen, zcap);
  if (have_exception) {
  KILL(sail_int)(&zlen);
  goto end_block_exception_214;
  }
  sail_string zlen_str;
  CREATE(sail_string)(&zlen_str);
  {
    lbits zgaz3370;
    CREATE(lbits)(&zgaz3370);
    {
      int64_t zgaz3369;
      {
        sail_int zgsz3684;
        CREATE(sail_int)(&zgsz3684);
        CONVERT_OF(sail_int, mach_int)(&zgsz3684, zcap_len_width);
        sail_int zgsz3685;
        CREATE(sail_int)(&zgsz3685);
        CONVERT_OF(sail_int, mach_int)(&zgsz3685, INT64_C(3));
        sail_int zgsz3686;
        CREATE(sail_int)(&zgsz3686);
        add_int(&zgsz3686, zgsz3684, zgsz3685);
        zgaz3369 = CONVERT_OF(mach_int, sail_int)(zgsz3686);
        KILL(sail_int)(&zgsz3686);
        KILL(sail_int)(&zgsz3685);
        KILL(sail_int)(&zgsz3684);
      }
      sailgen_zzzz7lzzJzzK68z3to_bits(&zgaz3370, zgaz3369, zlen);
    
    }
    string_of_lbits(&zlen_str, zgaz3370);
    KILL(lbits)(&zgaz3370);
  }
  uint64_t zotype64;
  {
    bool zgaz3366;
    zgaz3366 = sailgen_hasReservedOType(zcap);
    if (zgaz3366) {
    uint64_t zgaz3367;
    zgaz3367 = zcap.zotype;
    zotype64 = sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTS(INT64_C(64), zgaz3367);
  
    } else {
    uint64_t zgaz3368;
    zgaz3368 = zcap.zotype;
    zotype64 = sailgen_zzzz7mzzJzzK64zzCzz0zz7nzzJzzK18z3EXTZ(INT64_C(64), zgaz3368);
  
    }
  
  }
  sail_string zgaz3365;
  CREATE(sail_string)(&zgaz3365);
  {
    sail_string zgaz3363;
    CREATE(sail_string)(&zgaz3363);
    {
      bool zgaz3342;
      zgaz3342 = zcap.ztag;
      if (zgaz3342) {  COPY(sail_string)(&zgaz3363, "1");  } else {  COPY(sail_string)(&zgaz3363, "0");  }
    
    }
    sail_string zgaz3364;
    CREATE(sail_string)(&zgaz3364);
    {
      sail_string zgaz3362;
      CREATE(sail_string)(&zgaz3362);
      {
        sail_string zgaz3360;
        CREATE(sail_string)(&zgaz3360);
        {
          bool zgaz3343;
          zgaz3343 = sailgen_isCapSealed(zcap);
          if (zgaz3343) {  COPY(sail_string)(&zgaz3360, "1");  } else {  COPY(sail_string)(&zgaz3360, "0");  }
        
        }
        sail_string zgaz3361;
        CREATE(sail_string)(&zgaz3361);
        {
          sail_string zgaz3359;
          CREATE(sail_string)(&zgaz3359);
          {
            sail_string zgaz3357;
            CREATE(sail_string)(&zgaz3357);
            {
              uint64_t zgaz3345;
              {
                uint64_t zgaz3344;
                zgaz3344 = sailgen_getCapPerms(zcap);
                {
                  lbits zgsz3687;
                  CREATE(lbits)(&zgsz3687);
                  CONVERT_OF(lbits, fbits)(&zgsz3687, UINT64_C(0b0), UINT64_C(1) , true);
                  lbits zgsz3688;
                  CREATE(lbits)(&zgsz3688);
                  CONVERT_OF(lbits, fbits)(&zgsz3688, zgaz3344, UINT64_C(19) , true);
                  lbits zgsz3689;
                  CREATE(lbits)(&zgsz3689);
                  append(&zgsz3689, zgsz3687, zgsz3688);
                  zgaz3345 = CONVERT_OF(fbits, lbits)(zgsz3689, true);
                  KILL(lbits)(&zgsz3689);
                  KILL(lbits)(&zgsz3688);
                  KILL(lbits)(&zgsz3687);
                }
              
              }
              {
                lbits zgsz3690;
                CREATE(lbits)(&zgsz3690);
                CONVERT_OF(lbits, fbits)(&zgsz3690, zgaz3345, UINT64_C(20) , true);
                string_of_lbits(&zgaz3357, zgsz3690);
                KILL(lbits)(&zgsz3690);
              }
            
            }
            sail_string zgaz3358;
            CREATE(sail_string)(&zgaz3358);
            {
              sail_string zgaz3356;
              CREATE(sail_string)(&zgaz3356);
              {
                sail_string zgaz3354;
                CREATE(sail_string)(&zgaz3354);
                {
                  lbits zgsz3691;
                  CREATE(lbits)(&zgsz3691);
                  CONVERT_OF(lbits, fbits)(&zgsz3691, zotype64, UINT64_C(64) , true);
                  string_of_lbits(&zgaz3354, zgsz3691);
                  KILL(lbits)(&zgsz3691);
                }
                sail_string zgaz3355;
                CREATE(sail_string)(&zgaz3355);
                {
                  sail_string zgaz3353;
                  CREATE(sail_string)(&zgaz3353);
                  {
                    sail_string zgaz3351;
                    CREATE(sail_string)(&zgaz3351);
                    {
                      uint64_t zgaz3346;
                      zgaz3346 = sailgen_getCapOffsetBits(zcap);
                      {
                        lbits zgsz3692;
                        CREATE(lbits)(&zgsz3692);
                        CONVERT_OF(lbits, fbits)(&zgsz3692, zgaz3346, UINT64_C(64) , true);
                        string_of_lbits(&zgaz3351, zgsz3692);
                        KILL(lbits)(&zgsz3692);
                      }
                    
                    }
                    sail_string zgaz3352;
                    CREATE(sail_string)(&zgaz3352);
                    {
                      sail_string zgaz3350;
                      CREATE(sail_string)(&zgaz3350);
                      {
                        sail_string zgaz3348;
                        CREATE(sail_string)(&zgaz3348);
                        {
                          uint64_t zgaz3347;
                          zgaz3347 = sailgen_getCapBaseBits(zcap);
                          {
                            lbits zgsz3693;
                            CREATE(lbits)(&zgsz3693);
                            CONVERT_OF(lbits, fbits)(&zgsz3693, zgaz3347, UINT64_C(64) , true);
                            string_of_lbits(&zgaz3348, zgsz3693);
                            KILL(lbits)(&zgsz3693);
                          }
                        
                        }
                        sail_string zgaz3349;
                        CREATE(sail_string)(&zgaz3349);
                        concat_str(&zgaz3349, " length:", zlen_str);
                        concat_str(&zgaz3350, zgaz3348, zgaz3349);
                        KILL(sail_string)(&zgaz3349);
                        KILL(sail_string)(&zgaz3348);
                      }
                      concat_str(&zgaz3352, " base:", zgaz3350);
                      KILL(sail_string)(&zgaz3350);
                    }
                    concat_str(&zgaz3353, zgaz3351, zgaz3352);
                    KILL(sail_string)(&zgaz3352);
                    KILL(sail_string)(&zgaz3351);
                  }
                  concat_str(&zgaz3355, " offset:", zgaz3353);
                  KILL(sail_string)(&zgaz3353);
                }
                concat_str(&zgaz3356, zgaz3354, zgaz3355);
                KILL(sail_string)(&zgaz3355);
                KILL(sail_string)(&zgaz3354);
              }
              concat_str(&zgaz3358, " type:", zgaz3356);
              KILL(sail_string)(&zgaz3356);
            }
            concat_str(&zgaz3359, zgaz3357, zgaz3358);
            KILL(sail_string)(&zgaz3358);
            KILL(sail_string)(&zgaz3357);
          }
          concat_str(&zgaz3361, " perms:", zgaz3359);
          KILL(sail_string)(&zgaz3359);
        }
        concat_str(&zgaz3362, zgaz3360, zgaz3361);
        KILL(sail_string)(&zgaz3361);
        KILL(sail_string)(&zgaz3360);
      }
      concat_str(&zgaz3364, " s:", zgaz3362);
      KILL(sail_string)(&zgaz3362);
    }
    concat_str(&zgaz3365, zgaz3363, zgaz3364);
    KILL(sail_string)(&zgaz3364);
    KILL(sail_string)(&zgaz3363);
  }
  concat_str((*(&zcbz355)), " t:", zgaz3365);
  KILL(sail_string)(&zgaz3365);

  KILL(sail_string)(&zlen_str);
  KILL(sail_int)(&zlen);
end_function_213: ;
  goto end_function_233;
end_block_exception_214: ;
  goto end_function_233;
end_function_233: ;
}

static uint64_t sailgen_getRepresentableAlignmentMask(uint64_t);

static uint64_t sailgen_getRepresentableAlignmentMask(uint64_t zlen)
{
  __label__ case_217, finish_match_216, end_function_218, end_block_exception_219;

  uint64_t zcbz356;
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgaz3373;
  {
    uint64_t zgaz3371;
    {
      sail_int zgsz3694;
      CREATE(sail_int)(&zgsz3694);
      CONVERT_OF(sail_int, mach_int)(&zgsz3694, INT64_C(64));
      sail_int zgsz3695;
      CREATE(sail_int)(&zgsz3695);
      CONVERT_OF(sail_int, mach_int)(&zgsz3695, INT64_C(0));
      lbits zgsz3696;
      CREATE(lbits)(&zgsz3696);
      sailgen_to_bits(&zgsz3696, zgsz3694, zgsz3695);
      zgaz3371 = CONVERT_OF(fbits, lbits)(zgsz3696, true);
      KILL(lbits)(&zgsz3696);
      KILL(sail_int)(&zgsz3695);
      KILL(sail_int)(&zgsz3694);
    }
    lbits zgaz3372;
    CREATE(lbits)(&zgaz3372);
    {
      lbits zgsz3698;
      CREATE(lbits)(&zgsz3698);
      CONVERT_OF(lbits, fbits)(&zgsz3698, zlen, UINT64_C(64) , true);
      lbits zgsz3697;
      CREATE(lbits)(&zgsz3697);
      CONVERT_OF(lbits, fbits)(&zgsz3697, UINT64_C(0b0), UINT64_C(1) , true);
      append(&zgaz3372, zgsz3697, zgsz3698);
      KILL(lbits)(&zgsz3698);
      KILL(lbits)(&zgsz3697);
    }
    zgaz3373 = sailgen_setCapBounds(zdefault_cap, zgaz3371, zgaz3372);
    KILL(lbits)(&zgaz3372);
  
  }
  uint64_t zgsz3699;
  {
    bool zexact;
    zexact = zgaz3373.ztup0;
    struct zCapability zc;
    zc = zgaz3373.ztup1;
    int64_t ze;
    {
      int64_t zgaz3379;
      {
        uint64_t zgaz3378;
        zgaz3378 = zc.zE;
        zgaz3379 = ((mach_int) zgaz3378);
      
      }
      {
        sail_int zgsz3700;
        CREATE(sail_int)(&zgsz3700);
        CONVERT_OF(sail_int, mach_int)(&zgsz3700, zgaz3379);
        sail_int zgsz3701;
        CREATE(sail_int)(&zgsz3701);
        CONVERT_OF(sail_int, mach_int)(&zgsz3701, zcap_max_E);
        sail_int zgsz3702;
        CREATE(sail_int)(&zgsz3702);
        min_int(&zgsz3702, zgsz3700, zgsz3701);
        ze = CONVERT_OF(mach_int, sail_int)(zgsz3702);
        KILL(sail_int)(&zgsz3702);
        KILL(sail_int)(&zgsz3701);
        KILL(sail_int)(&zgsz3700);
      }
    
    }
    int64_t zez7;
    {
      bool zgaz3377;
      zgaz3377 = zc.zinternal_E;
      if (zgaz3377) {
      {
        sail_int zgsz3703;
        CREATE(sail_int)(&zgsz3703);
        CONVERT_OF(sail_int, mach_int)(&zgsz3703, ze);
        sail_int zgsz3704;
        CREATE(sail_int)(&zgsz3704);
        CONVERT_OF(sail_int, mach_int)(&zgsz3704, zinternal_E_take_bits);
        sail_int zgsz3705;
        CREATE(sail_int)(&zgsz3705);
        add_int(&zgsz3705, zgsz3703, zgsz3704);
        zez7 = CONVERT_OF(mach_int, sail_int)(zgsz3705);
        KILL(sail_int)(&zgsz3705);
        KILL(sail_int)(&zgsz3704);
        KILL(sail_int)(&zgsz3703);
      }
      } else {  zez7 = INT64_C(0);  }
    
    }
    sbits zgaz3375;
    {
      int64_t zgaz3374;
      {
        sail_int zgsz3706;
        CREATE(sail_int)(&zgsz3706);
        CONVERT_OF(sail_int, mach_int)(&zgsz3706, INT64_C(64));
        sail_int zgsz3707;
        CREATE(sail_int)(&zgsz3707);
        CONVERT_OF(sail_int, mach_int)(&zgsz3707, zez7);
        sail_int zgsz3708;
        CREATE(sail_int)(&zgsz3708);
        sub_int(&zgsz3708, zgsz3706, zgsz3707);
        zgaz3374 = CONVERT_OF(mach_int, sail_int)(zgsz3708);
        KILL(sail_int)(&zgsz3708);
        KILL(sail_int)(&zgsz3707);
        KILL(sail_int)(&zgsz3706);
      }
      {
        sail_int zgsz3709;
        CREATE(sail_int)(&zgsz3709);
        CONVERT_OF(sail_int, mach_int)(&zgsz3709, zgaz3374);
        lbits zgsz3710;
        CREATE(lbits)(&zgsz3710);
        sailgen_zzz3zzz3ones(&zgsz3710, zgsz3709);
        zgaz3375 = CONVERT_OF(sbits, lbits)(zgsz3710, true);
        KILL(lbits)(&zgsz3710);
        KILL(sail_int)(&zgsz3709);
      }
    
    }
    sbits zgaz3376;
    {
      sail_int zgsz3711;
      CREATE(sail_int)(&zgsz3711);
      CONVERT_OF(sail_int, mach_int)(&zgsz3711, zez7);
      lbits zgsz3712;
      CREATE(lbits)(&zgsz3712);
      sailgen_zzz3zzz3zzeros_implicit(&zgsz3712, zgsz3711);
      zgaz3376 = CONVERT_OF(sbits, lbits)(zgsz3712, true);
      KILL(lbits)(&zgsz3712);
      KILL(sail_int)(&zgsz3711);
    }
    {
      lbits zgsz3714;
      CREATE(lbits)(&zgsz3714);
      CONVERT_OF(lbits, sbits)(&zgsz3714, zgaz3375, true);
      lbits zgsz3715;
      CREATE(lbits)(&zgsz3715);
      CONVERT_OF(lbits, sbits)(&zgsz3715, zgaz3376, true);
      lbits zgsz3716;
      CREATE(lbits)(&zgsz3716);
      append(&zgsz3716, zgsz3714, zgsz3715);
      zgsz3699 = CONVERT_OF(fbits, lbits)(zgsz3716, true);
      KILL(lbits)(&zgsz3716);
      KILL(lbits)(&zgsz3715);
      KILL(lbits)(&zgsz3714);
    }
  
  
  
  
  
  
    goto finish_match_216;
  }
case_217: ;
  sail_match_failure("getRepresentableAlignmentMask");
finish_match_216: ;
  zcbz356 = zgsz3699;


end_function_218: ;
  return zcbz356;
end_block_exception_219: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_getRepresentableLength(uint64_t);

static uint64_t sailgen_getRepresentableLength(uint64_t zlen)
{
  __label__ end_function_221, end_block_exception_222;

  uint64_t zcbz357;
  uint64_t zm;
  zm = sailgen_getRepresentableAlignmentMask(zlen);
  uint64_t zgaz3381;
  {
    uint64_t zgaz3380;
    {
      lbits zgsz3717;
      CREATE(lbits)(&zgsz3717);
      CONVERT_OF(lbits, fbits)(&zgsz3717, zm, UINT64_C(64) , true);
      lbits zgsz3718;
      CREATE(lbits)(&zgsz3718);
      not_bits(&zgsz3718, zgsz3717);
      zgaz3380 = CONVERT_OF(fbits, lbits)(zgsz3718, true);
      KILL(lbits)(&zgsz3718);
      KILL(lbits)(&zgsz3717);
    }
    {
      lbits zgsz3719;
      CREATE(lbits)(&zgsz3719);
      CONVERT_OF(lbits, fbits)(&zgsz3719, zlen, UINT64_C(64) , true);
      lbits zgsz3720;
      CREATE(lbits)(&zgsz3720);
      CONVERT_OF(lbits, fbits)(&zgsz3720, zgaz3380, UINT64_C(64) , true);
      lbits zgsz3721;
      CREATE(lbits)(&zgsz3721);
      add_bits(&zgsz3721, zgsz3719, zgsz3720);
      zgaz3381 = CONVERT_OF(fbits, lbits)(zgsz3721, true);
      KILL(lbits)(&zgsz3721);
      KILL(lbits)(&zgsz3720);
      KILL(lbits)(&zgsz3719);
    }
  
  }
  {
    lbits zgsz3722;
    CREATE(lbits)(&zgsz3722);
    CONVERT_OF(lbits, fbits)(&zgsz3722, zgaz3381, UINT64_C(64) , true);
    lbits zgsz3723;
    CREATE(lbits)(&zgsz3723);
    CONVERT_OF(lbits, fbits)(&zgsz3723, zm, UINT64_C(64) , true);
    lbits zgsz3724;
    CREATE(lbits)(&zgsz3724);
    and_bits(&zgsz3724, zgsz3722, zgsz3723);
    zcbz357 = CONVERT_OF(fbits, lbits)(zgsz3724, true);
    KILL(lbits)(&zgsz3724);
    KILL(lbits)(&zgsz3723);
    KILL(lbits)(&zgsz3722);
  }


end_function_221: ;
  return zcbz357;
end_block_exception_222: ;

  return UINT64_C(0xdeadc0de);
}

// register TestCap
static struct zCapability zTestCap;

// register TestAddr
static uint64_t zTestAddr;

// register TestLen
static lbits zTestLen;

static unit sailgen_main(unit);


static unit sailgen_main(unit zgsz3725)
{
  __label__ end_function_228, end_block_exception_229;

  unit zcbz358;
  {
    zTestCap = znull_cap;
    unit zgsz3726;
    zgsz3726 = UNIT;
  }
  lbits zbits1;
  CREATE(lbits)(&zbits1);
  sailgen_capToBits(&zbits1, zTestCap);
  lbits zbits2;
  CREATE(lbits)(&zbits2);
  sailgen_capToMemBits(&zbits2, zTestCap);
  struct zCapability zcap1;
  {
    bool zgaz3387;
    zgaz3387 = zTestCap.ztag;
    zcap1 = sailgen_capBitsToCapability(zgaz3387, zbits1);
  
  }
  struct zCapability zcap2;
  {
    bool zgaz3386;
    zgaz3386 = zTestCap.ztag;
    zcap2 = sailgen_memBitsToCapability(zgaz3386, zbits2);
  
  }
  bool zrep;
  zrep = sailgen_fastRepCheck(zTestCap, zTestAddr);
  {
    __label__ case_225, finish_match_224;

    struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgaz3382;
    zgaz3382 = sailgen_setCapBounds(zTestCap, zTestAddr, zTestLen);
    unit zgsz3727;
    {
      bool ztup__0;
      ztup__0 = zgaz3382.ztup0;
      struct zCapability ztup__1;
      ztup__1 = zgaz3382.ztup1;
      bool zexact;
      zexact = ztup__0;
      struct zCapability zcap;
      zcap = ztup__1;
      zgsz3727 = UNIT;
    
    
    
    
      goto finish_match_224;
    }
  case_225: ;
    sail_match_failure("main");
  finish_match_224: ;
    unit zgsz3732;
    zgsz3732 = zgsz3727;
  
  
  }
  {
    __label__ case_227, finish_match_226;

    struct ztuple_z8z5izCz0z5iz9 zgaz3383;
    CREATE(ztuple_z8z5izCz0z5iz9)(&zgaz3383);
    sailgen_getCapBounds(&zgaz3383, zTestCap);
    unit zgsz3729;
    {
      sail_int ztup__0;
      CREATE(sail_int)(&ztup__0);
      COPY(sail_int)(&ztup__0, zgaz3383.ztup0);
      sail_int ztup__1;
      CREATE(sail_int)(&ztup__1);
      COPY(sail_int)(&ztup__1, zgaz3383.ztup1);
      sail_int zbase;
      CREATE(sail_int)(&zbase);
      COPY(sail_int)(&zbase, ztup__0);
      sail_int ztop;
      CREATE(sail_int)(&ztop);
      COPY(sail_int)(&ztop, ztup__1);
      zgsz3729 = UNIT;
      KILL(sail_int)(&ztop);
      KILL(sail_int)(&zbase);
      KILL(sail_int)(&ztup__1);
      KILL(sail_int)(&ztup__0);
      goto finish_match_226;
    }
  case_227: ;
    sail_match_failure("main");
  finish_match_226: ;
    unit zgsz3731;
    zgsz3731 = zgsz3729;
  
    KILL(ztuple_z8z5izCz0z5iz9)(&zgaz3383);
  }
  uint64_t zflags;
  zflags = sailgen_getCapFlags(zTestCap);
  struct zCapability znew_cap;
  {
    uint64_t zgaz3385;
    {
      int64_t zgaz3384;
      {
        sail_int zgsz3733;
        CREATE(sail_int)(&zgsz3733);
        CONVERT_OF(sail_int, mach_int)(&zgsz3733, INT64_C(15));
        sail_int zgsz3734;
        CREATE(sail_int)(&zgsz3734);
        CONVERT_OF(sail_int, mach_int)(&zgsz3734, INT64_C(4));
        sail_int zgsz3735;
        CREATE(sail_int)(&zgsz3735);
        add_int(&zgsz3735, zgsz3733, zgsz3734);
        zgaz3384 = CONVERT_OF(mach_int, sail_int)(zgsz3735);
        KILL(sail_int)(&zgsz3735);
        KILL(sail_int)(&zgsz3734);
        KILL(sail_int)(&zgsz3733);
      }
      {
        sail_int zgsz3736;
        CREATE(sail_int)(&zgsz3736);
        CONVERT_OF(sail_int, mach_int)(&zgsz3736, zgaz3384);
        lbits zgsz3737;
        CREATE(lbits)(&zgsz3737);
        sailgen_zzz3zzz3ones(&zgsz3737, zgsz3736);
        zgaz3385 = CONVERT_OF(fbits, lbits)(zgsz3737, true);
        KILL(lbits)(&zgsz3737);
        KILL(sail_int)(&zgsz3736);
      }
    
    }
    znew_cap = sailgen_setCapPerms(zTestCap, zgaz3385);
  
  }
  sail_string zcap_str;
  CREATE(sail_string)(&zcap_str);
  sailgen_capToString(&zcap_str, zTestCap);
  if (have_exception) {
  KILL(lbits)(&zbits1);
  KILL(lbits)(&zbits2);





  KILL(sail_string)(&zcap_str);
  goto end_block_exception_229;
  }
  uint64_t zlen;
  zlen = sailgen_getRepresentableLength(zTestAddr);
  uint64_t zmask;
  zmask = sailgen_getRepresentableAlignmentMask(zTestAddr);
  zcbz358 = UNIT;


  KILL(sail_string)(&zcap_str);





  KILL(lbits)(&zbits2);
  KILL(lbits)(&zbits1);
end_function_228: ;
  return zcbz358;
end_block_exception_229: ;

  return UNIT;
}

static unit sailgen_initializze_registers(unit);

static unit sailgen_initializze_registers(unit zgsz3738)
{
  __label__ end_function_231, end_block_exception_232;

  unit zcbz359;
  {
    zTestCap = sailgen_undefined_Capability(UNIT);
    unit zgsz3742;
    zgsz3742 = UNIT;
  }
  {
    {
      sail_int zgsz3739;
      CREATE(sail_int)(&zgsz3739);
      CONVERT_OF(sail_int, mach_int)(&zgsz3739, INT64_C(64));
      lbits zgsz3740;
      CREATE(lbits)(&zgsz3740);
      UNDEFINED(lbits)(&zgsz3740, zgsz3739);
      zTestAddr = CONVERT_OF(fbits, lbits)(zgsz3740, true);
      KILL(lbits)(&zgsz3740);
      KILL(sail_int)(&zgsz3739);
    }
    unit zgsz3741;
    zgsz3741 = UNIT;
  }
  int64_t zgaz3388;
  {
    sail_int zgsz3743;
    CREATE(sail_int)(&zgsz3743);
    CONVERT_OF(sail_int, mach_int)(&zgsz3743, INT64_C(64));
    sail_int zgsz3744;
    CREATE(sail_int)(&zgsz3744);
    CONVERT_OF(sail_int, mach_int)(&zgsz3744, INT64_C(1));
    sail_int zgsz3745;
    CREATE(sail_int)(&zgsz3745);
    add_int(&zgsz3745, zgsz3743, zgsz3744);
    zgaz3388 = CONVERT_OF(mach_int, sail_int)(zgsz3745);
    KILL(sail_int)(&zgsz3745);
    KILL(sail_int)(&zgsz3744);
    KILL(sail_int)(&zgsz3743);
  }
  {
    sail_int zgsz3746;
    CREATE(sail_int)(&zgsz3746);
    CONVERT_OF(sail_int, mach_int)(&zgsz3746, zgaz3388);
    UNDEFINED(lbits)(&zTestLen, zgsz3746);
    KILL(sail_int)(&zgsz3746);
  }
  zcbz359 = UNIT;

end_function_231: ;
  return zcbz359;
end_block_exception_232: ;

  return UNIT;
}

static void model_init(void)
{
  setup_rts();
  CREATE(lbits)(&zTestLen);
  sailgen_initializze_registers(UNIT);
  create_letbind_0();
  create_letbind_1();
  create_letbind_2();
  create_letbind_3();
  create_letbind_4();
  create_letbind_5();
  create_letbind_6();
  create_letbind_7();
  create_letbind_8();
  create_letbind_9();
  create_letbind_10();
  create_letbind_11();
  create_letbind_12();
  create_letbind_13();
  create_letbind_14();
  create_letbind_15();
  create_letbind_16();
  create_letbind_17();
  create_letbind_18();
  create_letbind_19();
  create_letbind_20();
  create_letbind_21();
  create_letbind_22();
  create_letbind_23();
  create_letbind_24();
  create_letbind_25();
  create_letbind_26();
  create_letbind_27();
}

static void model_fini(void)
{
  kill_letbind_27();
  kill_letbind_26();
  kill_letbind_25();
  kill_letbind_24();
  kill_letbind_23();
  kill_letbind_22();
  kill_letbind_21();
  kill_letbind_20();
  kill_letbind_19();
  kill_letbind_18();
  kill_letbind_17();
  kill_letbind_16();
  kill_letbind_15();
  kill_letbind_14();
  kill_letbind_13();
  kill_letbind_12();
  kill_letbind_11();
  kill_letbind_10();
  kill_letbind_9();
  kill_letbind_8();
  kill_letbind_7();
  kill_letbind_6();
  kill_letbind_5();
  kill_letbind_4();
  kill_letbind_3();
  kill_letbind_2();
  kill_letbind_1();
  kill_letbind_0();
  KILL(lbits)(&zTestLen);
  cleanup_rts();
}

static int model_main(int argc, char *argv[])
{
  model_init();
  if (process_arguments(argc, argv)) exit(EXIT_FAILURE);
  sailgen_main(UNIT);
  model_fini();
  return EXIT_SUCCESS;
}


