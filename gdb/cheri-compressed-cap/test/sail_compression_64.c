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

































static uint64_t sailgen_zzzz7nzzJzzK0z3sail_ones(int64_t);

static void sailgen_zzz3zzz3sail_ones(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK0z3sail_ones(int64_t zn)
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
    CONVERT_OF(lbits, fbits)(&zgsz34, zgaz33, UINT64_C(0) , true);
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
  __label__ end_function_13, end_block_exception_14, end_function_236;

  lbits zgaz34;
  CREATE(lbits)(&zgaz34);
  zeros(&zgaz34, zn);
  not_bits((*(&zcbz34)), zgaz34);
  KILL(lbits)(&zgaz34);
end_function_13: ;
  goto end_function_236;
end_block_exception_14: ;
  goto end_function_236;
end_function_236: ;
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













static uint64_t sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTS(int64_t, uint64_t);

static void sailgen_zzz3zzz3EXTZ(lbits *rop, sail_int, lbits);

static uint64_t sailgen_zzzz7mzzJzzK15zzCzz0zz7nzzJzzK12z3EXTZ(int64_t, uint64_t);

static uint64_t sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTZ(int64_t, uint64_t);

static uint64_t sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTS(int64_t zm, uint64_t zv)
{
  __label__ end_function_19, end_block_exception_20;

  uint64_t zcbz36;
  {
    lbits zgsz36;
    CREATE(lbits)(&zgsz36);
    CONVERT_OF(lbits, fbits)(&zgsz36, zv, UINT64_C(4) , true);
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
  __label__ end_function_22, end_block_exception_23, end_function_235;

  zero_extend((*(&zcbz37)), zv, zm);
end_function_22: ;
  goto end_function_235;
end_block_exception_23: ;
  goto end_function_235;
end_function_235: ;
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

static uint64_t sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTZ(int64_t zm, uint64_t zv)
{
  __label__ end_function_28, end_block_exception_29;

  uint64_t zcbz39;
  {
    lbits zgsz312;
    CREATE(lbits)(&zgsz312);
    CONVERT_OF(lbits, fbits)(&zgsz312, zv, UINT64_C(4) , true);
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

static uint64_t sailgen_zzzz7nzzJzzK6z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK32z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK8z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK0z3zzeros_implicit(int64_t);

static void sailgen_zzz3zzz3zzeros_implicit(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK2z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK3z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK5z3zzeros_implicit(int64_t);

static uint64_t sailgen_zzzz7nzzJzzK6z3zzeros_implicit(int64_t zn)
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

static uint64_t sailgen_zzzz7nzzJzzK32z3zzeros_implicit(int64_t zn)
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

static uint64_t sailgen_zzzz7nzzJzzK8z3zzeros_implicit(int64_t zn)
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

static uint64_t sailgen_zzzz7nzzJzzK0z3zzeros_implicit(int64_t zn)
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

static void sailgen_zzz3zzz3zzeros_implicit(lbits *zcbz314, sail_int zn)
{
  __label__ end_function_43, end_block_exception_44, end_function_234;

  zeros((*(&zcbz314)), zn);
end_function_43: ;
  goto end_function_234;
end_block_exception_44: ;
  goto end_function_234;
end_function_234: ;
}

static uint64_t sailgen_zzzz7nzzJzzK2z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_46, end_block_exception_47;

  uint64_t zcbz315;
  {
    sail_int zgsz323;
    CREATE(sail_int)(&zgsz323);
    CONVERT_OF(sail_int, mach_int)(&zgsz323, zn);
    lbits zgsz324;
    CREATE(lbits)(&zgsz324);
    zeros(&zgsz324, zgsz323);
    zcbz315 = CONVERT_OF(fbits, lbits)(zgsz324, true);
    KILL(lbits)(&zgsz324);
    KILL(sail_int)(&zgsz323);
  }
end_function_46: ;
  return zcbz315;
end_block_exception_47: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK3z3zzeros_implicit(int64_t zn)
{
  __label__ end_function_49, end_block_exception_50;

  uint64_t zcbz316;
  {
    sail_int zgsz325;
    CREATE(sail_int)(&zgsz325);
    CONVERT_OF(sail_int, mach_int)(&zgsz325, zn);
    lbits zgsz326;
    CREATE(lbits)(&zgsz326);
    zeros(&zgsz326, zgsz325);
    zcbz316 = CONVERT_OF(fbits, lbits)(zgsz326, true);
    KILL(lbits)(&zgsz326);
    KILL(sail_int)(&zgsz325);
  }
end_function_49: ;
  return zcbz316;
end_block_exception_50: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7nzzJzzK5z3zzeros_implicit(int64_t zn)
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

static uint64_t sailgen_zzzz7nzzJzzK0z3ones(int64_t);

static void sailgen_zzz3zzz3ones(lbits *rop, sail_int);

static uint64_t sailgen_zzzz7nzzJzzK0z3ones(int64_t zn)
{
  __label__ end_function_55, end_block_exception_56;

  uint64_t zcbz318;
  zcbz318 = sailgen_zzzz7nzzJzzK0z3sail_ones(zn);
end_function_55: ;
  return zcbz318;
end_block_exception_56: ;

  return UINT64_C(0xdeadc0de);
}

static void sailgen_zzz3zzz3ones(lbits *zcbz319, sail_int zn)
{
  __label__ end_function_58, end_block_exception_59, end_function_233;

  sailgen_zzz3zzz3sail_ones((*(&zcbz319)), zn);
end_function_58: ;
  goto end_function_233;
end_block_exception_59: ;
  goto end_function_233;
end_function_233: ;
}

static uint64_t sailgen_bool_to_bits(bool);

static uint64_t sailgen_bool_to_bits(bool zx)
{
  __label__ end_function_61, end_block_exception_62;

  uint64_t zcbz320;
  if (zx) {  zcbz320 = UINT64_C(0b1);  } else {  zcbz320 = UINT64_C(0b0);  }
end_function_61: ;
  return zcbz320;
end_block_exception_62: ;

  return UINT64_C(0xdeadc0de);
}

static bool sailgen_bit_to_bool(fbits);

static bool sailgen_bit_to_bool(fbits zb)
{
  __label__ case_65, case_66, finish_match_64, end_function_67, end_block_exception_68;

  bool zcbz321;
  bool zgsz329;
  {
    fbits zp0z3;
    zp0z3 = zb;
    bool zgsz330;
    zgsz330 = eq_bit(zp0z3, UINT64_C(1));
    if (!(zgsz330)) {
  
    goto case_65;
    }
    zgsz329 = true;
  
    goto finish_match_64;
  }
case_65: ;
  {
    fbits zp0z3;
    zp0z3 = zb;
    bool zgsz331;
    zgsz331 = eq_bit(zp0z3, UINT64_C(0));
    if (!(zgsz331)) {
  
    goto case_66;
    }
    zgsz329 = false;
  
    goto finish_match_64;
  }
case_66: ;
  sail_match_failure("bit_to_bool");
finish_match_64: ;
  zcbz321 = zgsz329;

end_function_67: ;
  return zcbz321;
end_block_exception_68: ;

  return false;
}

static void sailgen_to_bits(lbits *rop, sail_int, sail_int);

static uint64_t sailgen_zzzz7lzzJzzK6z3to_bits(int64_t, sail_int);

static uint64_t sailgen_zzzz7lzzJzzK36z3to_bits(int64_t, sail_int);

static void sailgen_to_bits(lbits *zcbz322, sail_int zl, sail_int zn)
{
  __label__ end_function_70, end_block_exception_71, end_function_232;

  {
    sail_int zgsz332;
    CREATE(sail_int)(&zgsz332);
    CONVERT_OF(sail_int, mach_int)(&zgsz332, INT64_C(0));
    get_slice_int((*(&zcbz322)), zl, zn, zgsz332);
    KILL(sail_int)(&zgsz332);
  }
end_function_70: ;
  goto end_function_232;
end_block_exception_71: ;
  goto end_function_232;
end_function_232: ;
}

static uint64_t sailgen_zzzz7lzzJzzK6z3to_bits(int64_t zl, sail_int zn)
{
  __label__ end_function_73, end_block_exception_74;

  uint64_t zcbz323;
  {
    sail_int zgsz333;
    CREATE(sail_int)(&zgsz333);
    CONVERT_OF(sail_int, mach_int)(&zgsz333, zl);
    sail_int zgsz334;
    CREATE(sail_int)(&zgsz334);
    CONVERT_OF(sail_int, mach_int)(&zgsz334, INT64_C(0));
    lbits zgsz335;
    CREATE(lbits)(&zgsz335);
    get_slice_int(&zgsz335, zgsz333, zn, zgsz334);
    zcbz323 = CONVERT_OF(fbits, lbits)(zgsz335, true);
    KILL(lbits)(&zgsz335);
    KILL(sail_int)(&zgsz334);
    KILL(sail_int)(&zgsz333);
  }
end_function_73: ;
  return zcbz323;
end_block_exception_74: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_zzzz7lzzJzzK36z3to_bits(int64_t zl, sail_int zn)
{
  __label__ end_function_76, end_block_exception_77;

  uint64_t zcbz324;
  {
    sail_int zgsz336;
    CREATE(sail_int)(&zgsz336);
    CONVERT_OF(sail_int, mach_int)(&zgsz336, zl);
    sail_int zgsz337;
    CREATE(sail_int)(&zgsz337);
    CONVERT_OF(sail_int, mach_int)(&zgsz337, INT64_C(0));
    lbits zgsz338;
    CREATE(lbits)(&zgsz338);
    get_slice_int(&zgsz338, zgsz336, zn, zgsz337);
    zcbz324 = CONVERT_OF(fbits, lbits)(zgsz338, true);
    KILL(lbits)(&zgsz338);
    KILL(sail_int)(&zgsz337);
    KILL(sail_int)(&zgsz336);
  }
end_function_76: ;
  return zcbz324;
end_block_exception_77: ;

  return UINT64_C(0xdeadc0de);
}

static bool sailgen_z8operatorz0zzz3zzz3zI_uz9(lbits, lbits);

static bool sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzzz7nzzJzzK8z3zI_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzzz7nzzJzzK8z3zKzJ_uz9(uint64_t, uint64_t);

static bool sailgen_z8operatorz0zzz3zzz3zI_uz9(lbits zx, lbits zy)
{
  __label__ end_function_79, end_block_exception_80;

  bool zcbz325;
  sail_int zgaz36;
  CREATE(sail_int)(&zgaz36);
  sail_unsigned(&zgaz36, zx);
  sail_int zgaz37;
  CREATE(sail_int)(&zgaz37);
  sail_unsigned(&zgaz37, zy);
  zcbz325 = lt(zgaz36, zgaz37);
  KILL(sail_int)(&zgaz37);
  KILL(sail_int)(&zgaz36);
end_function_79: ;
  return zcbz325;
end_block_exception_80: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_82, end_block_exception_83;

  bool zcbz326;
  int64_t zgaz38;
  zgaz38 = ((mach_int) zx);
  int64_t zgaz39;
  zgaz39 = ((mach_int) zy);
  {
    sail_int zgsz340;
    CREATE(sail_int)(&zgsz340);
    CONVERT_OF(sail_int, mach_int)(&zgsz340, zgaz39);
    sail_int zgsz339;
    CREATE(sail_int)(&zgsz339);
    CONVERT_OF(sail_int, mach_int)(&zgsz339, zgaz38);
    zcbz326 = lt(zgsz339, zgsz340);
    KILL(sail_int)(&zgsz340);
    KILL(sail_int)(&zgsz339);
  }


end_function_82: ;
  return zcbz326;
end_block_exception_83: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK8z3zI_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_85, end_block_exception_86;

  bool zcbz327;
  int64_t zgaz310;
  zgaz310 = ((mach_int) zx);
  int64_t zgaz311;
  zgaz311 = ((mach_int) zy);
  {
    sail_int zgsz342;
    CREATE(sail_int)(&zgsz342);
    CONVERT_OF(sail_int, mach_int)(&zgsz342, zgaz311);
    sail_int zgsz341;
    CREATE(sail_int)(&zgsz341);
    CONVERT_OF(sail_int, mach_int)(&zgsz341, zgaz310);
    zcbz327 = lt(zgsz341, zgsz342);
    KILL(sail_int)(&zgsz342);
    KILL(sail_int)(&zgsz341);
  }


end_function_85: ;
  return zcbz327;
end_block_exception_86: ;

  return false;
}

static bool sailgen_z8operatorz0zzzz7nzzJzzK8z3zKzJ_uz9(uint64_t zx, uint64_t zy)
{
  __label__ end_function_88, end_block_exception_89;

  bool zcbz328;
  int64_t zgaz312;
  zgaz312 = ((mach_int) zx);
  int64_t zgaz313;
  zgaz313 = ((mach_int) zy);
  {
    sail_int zgsz344;
    CREATE(sail_int)(&zgsz344);
    CONVERT_OF(sail_int, mach_int)(&zgsz344, zgaz313);
    sail_int zgsz343;
    CREATE(sail_int)(&zgsz343);
    CONVERT_OF(sail_int, mach_int)(&zgsz343, zgaz312);
    zcbz328 = gteq(zgsz343, zgsz344);
    KILL(sail_int)(&zgsz344);
    KILL(sail_int)(&zgsz343);
  }


end_function_88: ;
  return zcbz328;
end_block_exception_89: ;

  return false;
}



static void sailgen_n_leading_spaces(sail_int *rop, sail_string);

static void sailgen_n_leading_spaces(sail_int *zcbz329, sail_string zs)
{
  __label__ case_92, case_93, finish_match_91, end_function_97, end_block_exception_98, end_function_231;

  sail_int zgsz345;
  CREATE(sail_int)(&zgsz345);
  {
    sail_string zp0z3;
    CREATE(sail_string)(&zp0z3);
    COPY(sail_string)(&zp0z3, zs);
    bool zgsz346;
    zgsz346 = eq_string(zp0z3, "");
    if (!(zgsz346)) {
    KILL(sail_string)(&zp0z3);
    goto case_92;
    }
    CONVERT_OF(sail_int, mach_int)(&zgsz345, INT64_C(0));
    KILL(sail_string)(&zp0z3);
    goto finish_match_91;
  }
case_92: ;
  {
    __label__ case_95, case_96, finish_match_94;

    sail_string zgaz314;
    CREATE(sail_string)(&zgaz314);
    {
      sail_int zgsz347;
      CREATE(sail_int)(&zgsz347);
      CONVERT_OF(sail_int, mach_int)(&zgsz347, INT64_C(1));
      string_take(&zgaz314, zs, zgsz347);
      KILL(sail_int)(&zgsz347);
    }
    sail_int zgsz348;
    CREATE(sail_int)(&zgsz348);
    {
      sail_string zp0z3;
      CREATE(sail_string)(&zp0z3);
      COPY(sail_string)(&zp0z3, zgaz314);
      bool zgsz350;
      zgsz350 = eq_string(zp0z3, " ");
      if (!(zgsz350)) {
      KILL(sail_string)(&zp0z3);
      goto case_95;
      }
      sail_int zgaz316;
      CREATE(sail_int)(&zgaz316);
      {
        sail_string zgaz315;
        CREATE(sail_string)(&zgaz315);
        {
          sail_int zgsz349;
          CREATE(sail_int)(&zgsz349);
          CONVERT_OF(sail_int, mach_int)(&zgsz349, INT64_C(1));
          string_drop(&zgaz315, zs, zgsz349);
          KILL(sail_int)(&zgsz349);
        }
        sailgen_n_leading_spaces(&zgaz316, zgaz315);
        KILL(sail_string)(&zgaz315);
      }
      {
        sail_int zgsz351;
        CREATE(sail_int)(&zgsz351);
        CONVERT_OF(sail_int, mach_int)(&zgsz351, INT64_C(1));
        add_int(&zgsz348, zgsz351, zgaz316);
        KILL(sail_int)(&zgsz351);
      }
      KILL(sail_int)(&zgaz316);
      KILL(sail_string)(&zp0z3);
      goto finish_match_94;
    }
  case_95: ;
    {
      CONVERT_OF(sail_int, mach_int)(&zgsz348, INT64_C(0));
      goto finish_match_94;
    }
  case_96: ;
    sail_match_failure("n_leading_spaces");
  finish_match_94: ;
    COPY(sail_int)(&zgsz345, zgsz348);
    KILL(sail_int)(&zgsz348);
    KILL(sail_string)(&zgaz314);
    goto finish_match_91;
  }
case_93: ;
  sail_match_failure("n_leading_spaces");
finish_match_91: ;
  COPY(sail_int)((*(&zcbz329)), zgsz345);
  KILL(sail_int)(&zgsz345);
end_function_97: ;
  goto end_function_231;
end_block_exception_98: ;
  goto end_function_231;
end_function_231: ;
}

static int64_t sailgen_zzzz7nzzJzzK32z3MAX(int64_t);

static int64_t sailgen_zzzz7nzzJzzK4z3MAX(int64_t);

static int64_t sailgen_zzzz7nzzJzzK32z3MAX(int64_t zn)
{
  __label__ end_function_100, end_block_exception_101;

  int64_t zcbz330;
  int64_t zgaz317;
  {
    sail_int zgsz354;
    CREATE(sail_int)(&zgsz354);
    CONVERT_OF(sail_int, mach_int)(&zgsz354, zn);
    sail_int zgsz355;
    CREATE(sail_int)(&zgsz355);
    pow2(&zgsz355, zgsz354);
    zgaz317 = CONVERT_OF(mach_int, sail_int)(zgsz355);
    KILL(sail_int)(&zgsz355);
    KILL(sail_int)(&zgsz354);
  }
  {
    sail_int zgsz356;
    CREATE(sail_int)(&zgsz356);
    CONVERT_OF(sail_int, mach_int)(&zgsz356, zgaz317);
    sail_int zgsz357;
    CREATE(sail_int)(&zgsz357);
    CONVERT_OF(sail_int, mach_int)(&zgsz357, INT64_C(1));
    sail_int zgsz358;
    CREATE(sail_int)(&zgsz358);
    sub_int(&zgsz358, zgsz356, zgsz357);
    zcbz330 = CONVERT_OF(mach_int, sail_int)(zgsz358);
    KILL(sail_int)(&zgsz358);
    KILL(sail_int)(&zgsz357);
    KILL(sail_int)(&zgsz356);
  }

end_function_100: ;
  return zcbz330;
end_block_exception_101: ;

  return INT64_C(0xdeadc0de);
}

static int64_t sailgen_zzzz7nzzJzzK4z3MAX(int64_t zn)
{
  __label__ end_function_103, end_block_exception_104;

  int64_t zcbz331;
  int64_t zgaz318;
  {
    sail_int zgsz359;
    CREATE(sail_int)(&zgsz359);
    CONVERT_OF(sail_int, mach_int)(&zgsz359, zn);
    sail_int zgsz360;
    CREATE(sail_int)(&zgsz360);
    pow2(&zgsz360, zgsz359);
    zgaz318 = CONVERT_OF(mach_int, sail_int)(zgsz360);
    KILL(sail_int)(&zgsz360);
    KILL(sail_int)(&zgsz359);
  }
  {
    sail_int zgsz361;
    CREATE(sail_int)(&zgsz361);
    CONVERT_OF(sail_int, mach_int)(&zgsz361, zgaz318);
    sail_int zgsz362;
    CREATE(sail_int)(&zgsz362);
    CONVERT_OF(sail_int, mach_int)(&zgsz362, INT64_C(1));
    sail_int zgsz363;
    CREATE(sail_int)(&zgsz363);
    sub_int(&zgsz363, zgsz361, zgsz362);
    zcbz331 = CONVERT_OF(mach_int, sail_int)(zgsz363);
    KILL(sail_int)(&zgsz363);
    KILL(sail_int)(&zgsz362);
    KILL(sail_int)(&zgsz361);
  }

end_function_103: ;
  return zcbz331;
end_block_exception_104: ;

  return INT64_C(0xdeadc0de);
}



static int64_t zreserved_otypes;
static void create_letbind_0(void) {  


  int64_t zgsz364;
  zgsz364 = INT64_C(4);
  zreserved_otypes = zgsz364;

let_end_105: ;
}
static void kill_letbind_0(void) {  
}

static int64_t zotype_unsealed;
static void create_letbind_1(void) {  


  int64_t zgsz365;
  zgsz365 = INT64_C(-1);
  zotype_unsealed = zgsz365;

let_end_106: ;
}
static void kill_letbind_1(void) {  
}

static int64_t zotype_sentry;
static void create_letbind_2(void) {  


  int64_t zgsz366;
  zgsz366 = INT64_C(-2);
  zotype_sentry = zgsz366;

let_end_107: ;
}
static void kill_letbind_2(void) {  
}

static uint64_t zPCC_IDX;
static void create_letbind_3(void) {  


  uint64_t zgsz367;
  zgsz367 = UINT64_C(0b100000);
  zPCC_IDX = zgsz367;

let_end_108: ;
}
static void kill_letbind_3(void) {  
}

static uint64_t zDDC_IDX;
static void create_letbind_4(void) {  


  uint64_t zgsz368;
  zgsz368 = UINT64_C(0b100001);
  zDDC_IDX = zgsz368;

let_end_109: ;
}
static void kill_letbind_4(void) {  
}

static int64_t zcap_sizze;
static void create_letbind_5(void) {  


  int64_t zgsz369;
  zgsz369 = INT64_C(8);
  zcap_sizze = zgsz369;

let_end_110: ;
}
static void kill_letbind_5(void) {  
}

static int64_t zlog2_cap_sizze;
static void create_letbind_6(void) {  


  int64_t zgsz370;
  zgsz370 = INT64_C(3);
  zlog2_cap_sizze = zgsz370;

let_end_111: ;
}
static void kill_letbind_6(void) {  
}

static int64_t zcap_hperms_width;
static void create_letbind_7(void) {  


  int64_t zgsz371;
  zgsz371 = INT64_C(12);
  zcap_hperms_width = zgsz371;

let_end_112: ;
}
static void kill_letbind_7(void) {  
}

static int64_t zcap_uperms_width;
static void create_letbind_8(void) {  


  int64_t zgsz372;
  zgsz372 = INT64_C(0);
  zcap_uperms_width = zgsz372;

let_end_113: ;
}
static void kill_letbind_8(void) {  
}

static int64_t zcap_otype_width;
static void create_letbind_9(void) {  


  int64_t zgsz373;
  zgsz373 = INT64_C(4);
  zcap_otype_width = zgsz373;

let_end_114: ;
}
static void kill_letbind_9(void) {  
}

static int64_t zcap_reserved_width;
static void create_letbind_10(void) {  


  int64_t zgsz374;
  zgsz374 = INT64_C(0);
  zcap_reserved_width = zgsz374;

let_end_115: ;
}
static void kill_letbind_10(void) {  
}

static int64_t zcap_flags_width;
static void create_letbind_11(void) {  


  int64_t zgsz375;
  zgsz375 = INT64_C(1);
  zcap_flags_width = zgsz375;

let_end_116: ;
}
static void kill_letbind_11(void) {  
}

static int64_t zcap_mantissa_width;
static void create_letbind_12(void) {  


  int64_t zgsz376;
  zgsz376 = INT64_C(8);
  zcap_mantissa_width = zgsz376;

let_end_117: ;
}
static void kill_letbind_12(void) {  
}

static int64_t zcap_E_width;
static void create_letbind_13(void) {  


  int64_t zgsz377;
  zgsz377 = INT64_C(6);
  zcap_E_width = zgsz377;

let_end_118: ;
}
static void kill_letbind_13(void) {  
}

static int64_t zcap_addr_width;
static void create_letbind_14(void) {  


  int64_t zgsz378;
  zgsz378 = INT64_C(32);
  zcap_addr_width = zgsz378;

let_end_119: ;
}
static void kill_letbind_14(void) {  
}

static int64_t zcap_len_width;
static void create_letbind_15(void) {  


  int64_t zgsz379;
  {
    sail_int zgsz380;
    CREATE(sail_int)(&zgsz380);
    CONVERT_OF(sail_int, mach_int)(&zgsz380, INT64_C(32));
    sail_int zgsz381;
    CREATE(sail_int)(&zgsz381);
    CONVERT_OF(sail_int, mach_int)(&zgsz381, INT64_C(1));
    sail_int zgsz382;
    CREATE(sail_int)(&zgsz382);
    add_int(&zgsz382, zgsz380, zgsz381);
    zgsz379 = CONVERT_OF(mach_int, sail_int)(zgsz382);
    KILL(sail_int)(&zgsz382);
    KILL(sail_int)(&zgsz381);
    KILL(sail_int)(&zgsz380);
  }
  zcap_len_width = zgsz379;

let_end_120: ;
}
static void kill_letbind_15(void) {  
}

static int64_t zcaps_per_cache_line;
static void create_letbind_16(void) {  


  int64_t zgsz383;
  zgsz383 = INT64_C(8);
  zcaps_per_cache_line = zgsz383;

let_end_121: ;
}
static void kill_letbind_16(void) {  
}

static int64_t zinternal_E_take_bits;
static void create_letbind_17(void) {  


  int64_t zgsz384;
  zgsz384 = INT64_C(3);
  zinternal_E_take_bits = zgsz384;

let_end_122: ;
}
static void kill_letbind_17(void) {  
}

static struct zEncCapability sailgen_capBitsToEncCapability(uint64_t);

static struct zEncCapability sailgen_capBitsToEncCapability(uint64_t zc)
{
  __label__ end_function_124, end_block_exception_125;

  struct zEncCapability zcbz332;
  uint64_t zgaz319;
  {
    lbits zgsz385;
    CREATE(lbits)(&zgsz385);
    CONVERT_OF(lbits, fbits)(&zgsz385, zc, UINT64_C(64) , true);
    sail_int zgsz386;
    CREATE(sail_int)(&zgsz386);
    CONVERT_OF(sail_int, mach_int)(&zgsz386, INT64_C(63));
    sail_int zgsz387;
    CREATE(sail_int)(&zgsz387);
    CONVERT_OF(sail_int, mach_int)(&zgsz387, INT64_C(52));
    lbits zgsz388;
    CREATE(lbits)(&zgsz388);
    vector_subrange_lbits(&zgsz388, zgsz385, zgsz386, zgsz387);
    zgaz319 = CONVERT_OF(fbits, lbits)(zgsz388, true);
    KILL(lbits)(&zgsz388);
    KILL(sail_int)(&zgsz387);
    KILL(sail_int)(&zgsz386);
    KILL(lbits)(&zgsz385);
  }
  uint64_t zgaz320;
  {
    lbits zgsz389;
    CREATE(lbits)(&zgsz389);
    CONVERT_OF(lbits, fbits)(&zgsz389, zc, UINT64_C(64) , true);
    sail_int zgsz390;
    CREATE(sail_int)(&zgsz390);
    CONVERT_OF(sail_int, mach_int)(&zgsz390, INT64_C(51));
    sail_int zgsz391;
    CREATE(sail_int)(&zgsz391);
    CONVERT_OF(sail_int, mach_int)(&zgsz391, INT64_C(51));
    lbits zgsz392;
    CREATE(lbits)(&zgsz392);
    vector_subrange_lbits(&zgsz392, zgsz389, zgsz390, zgsz391);
    zgaz320 = CONVERT_OF(fbits, lbits)(zgsz392, true);
    KILL(lbits)(&zgsz392);
    KILL(sail_int)(&zgsz391);
    KILL(sail_int)(&zgsz390);
    KILL(lbits)(&zgsz389);
  }
  uint64_t zgaz321;
  {
    lbits zgsz393;
    CREATE(lbits)(&zgsz393);
    CONVERT_OF(lbits, fbits)(&zgsz393, zc, UINT64_C(64) , true);
    sail_int zgsz394;
    CREATE(sail_int)(&zgsz394);
    CONVERT_OF(sail_int, mach_int)(&zgsz394, INT64_C(50));
    sail_int zgsz395;
    CREATE(sail_int)(&zgsz395);
    CONVERT_OF(sail_int, mach_int)(&zgsz395, INT64_C(47));
    lbits zgsz396;
    CREATE(lbits)(&zgsz396);
    vector_subrange_lbits(&zgsz396, zgsz393, zgsz394, zgsz395);
    zgaz321 = CONVERT_OF(fbits, lbits)(zgsz396, true);
    KILL(lbits)(&zgsz396);
    KILL(sail_int)(&zgsz395);
    KILL(sail_int)(&zgsz394);
    KILL(lbits)(&zgsz393);
  }
  fbits zgaz322;
  {
    sail_int zgsz398;
    CREATE(sail_int)(&zgsz398);
    CONVERT_OF(sail_int, mach_int)(&zgsz398, INT64_C(46));
    lbits zgsz397;
    CREATE(lbits)(&zgsz397);
    CONVERT_OF(lbits, fbits)(&zgsz397, zc, UINT64_C(64) , true);
    zgaz322 = bitvector_access(zgsz397, zgsz398);
    KILL(sail_int)(&zgsz398);
    KILL(lbits)(&zgsz397);
  }
  uint64_t zgaz323;
  {
    lbits zgsz399;
    CREATE(lbits)(&zgsz399);
    CONVERT_OF(lbits, fbits)(&zgsz399, zc, UINT64_C(64) , true);
    sail_int zgsz3100;
    CREATE(sail_int)(&zgsz3100);
    CONVERT_OF(sail_int, mach_int)(&zgsz3100, INT64_C(45));
    sail_int zgsz3101;
    CREATE(sail_int)(&zgsz3101);
    CONVERT_OF(sail_int, mach_int)(&zgsz3101, INT64_C(40));
    lbits zgsz3102;
    CREATE(lbits)(&zgsz3102);
    vector_subrange_lbits(&zgsz3102, zgsz399, zgsz3100, zgsz3101);
    zgaz323 = CONVERT_OF(fbits, lbits)(zgsz3102, true);
    KILL(lbits)(&zgsz3102);
    KILL(sail_int)(&zgsz3101);
    KILL(sail_int)(&zgsz3100);
    KILL(lbits)(&zgsz399);
  }
  uint64_t zgaz324;
  {
    lbits zgsz3103;
    CREATE(lbits)(&zgsz3103);
    CONVERT_OF(lbits, fbits)(&zgsz3103, zc, UINT64_C(64) , true);
    sail_int zgsz3104;
    CREATE(sail_int)(&zgsz3104);
    CONVERT_OF(sail_int, mach_int)(&zgsz3104, INT64_C(39));
    sail_int zgsz3105;
    CREATE(sail_int)(&zgsz3105);
    CONVERT_OF(sail_int, mach_int)(&zgsz3105, INT64_C(32));
    lbits zgsz3106;
    CREATE(lbits)(&zgsz3106);
    vector_subrange_lbits(&zgsz3106, zgsz3103, zgsz3104, zgsz3105);
    zgaz324 = CONVERT_OF(fbits, lbits)(zgsz3106, true);
    KILL(lbits)(&zgsz3106);
    KILL(sail_int)(&zgsz3105);
    KILL(sail_int)(&zgsz3104);
    KILL(lbits)(&zgsz3103);
  }
  uint64_t zgaz325;
  {
    lbits zgsz3107;
    CREATE(lbits)(&zgsz3107);
    CONVERT_OF(lbits, fbits)(&zgsz3107, zc, UINT64_C(64) , true);
    sail_int zgsz3108;
    CREATE(sail_int)(&zgsz3108);
    CONVERT_OF(sail_int, mach_int)(&zgsz3108, INT64_C(31));
    sail_int zgsz3109;
    CREATE(sail_int)(&zgsz3109);
    CONVERT_OF(sail_int, mach_int)(&zgsz3109, INT64_C(0));
    lbits zgsz3110;
    CREATE(lbits)(&zgsz3110);
    vector_subrange_lbits(&zgsz3110, zgsz3107, zgsz3108, zgsz3109);
    zgaz325 = CONVERT_OF(fbits, lbits)(zgsz3110, true);
    KILL(lbits)(&zgsz3110);
    KILL(sail_int)(&zgsz3109);
    KILL(sail_int)(&zgsz3108);
    KILL(lbits)(&zgsz3107);
  }
  struct zEncCapability zgsz3111;
  zgsz3111.zB = zgaz324;
  zgsz3111.zT = zgaz323;
  zgsz3111.zaddress = zgaz325;
  zgsz3111.zflags = zgaz320;
  zgsz3111.zinternal_E = zgaz322;
  zgsz3111.zotype = zgaz321;
  zgsz3111.zperms = zgaz319;
  zgsz3111.zreserved = UINT64_C(0);
  zcbz332 = zgsz3111;








end_function_124: ;
  return zcbz332;
end_block_exception_125: ;
  struct zEncCapability zcbz359 = { .zreserved = UINT64_C(0xdeadc0de), .zperms = UINT64_C(0xdeadc0de), .zotype = UINT64_C(0xdeadc0de), .zinternal_E = UINT64_C(0), .zflags = UINT64_C(0xdeadc0de), .zaddress = UINT64_C(0xdeadc0de), .zT = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz359;
}

static uint64_t sailgen_encCapToBits(struct zEncCapability);

static uint64_t sailgen_encCapToBits(struct zEncCapability zcap)
{
  __label__ end_function_127, end_block_exception_128;

  uint64_t zcbz333;
  uint64_t zgaz339;
  zgaz339 = zcap.zperms;
  uint64_t zgaz340;
  {
    uint64_t zgaz337;
    zgaz337 = zcap.zreserved;
    uint64_t zgaz338;
    {
      uint64_t zgaz335;
      zgaz335 = zcap.zflags;
      uint64_t zgaz336;
      {
        uint64_t zgaz333;
        zgaz333 = zcap.zotype;
        uint64_t zgaz334;
        {
          uint64_t zgaz331;
          {
            fbits zgaz326;
            zgaz326 = zcap.zinternal_E;
            uint64_t zgsz3112;
            zgsz3112 = UINT64_C(0b0);
            zgsz3112 = update_fbits(zgsz3112, INT64_C(0), zgaz326);
            zgaz331 = zgsz3112;
          
          }
          uint64_t zgaz332;
          {
            uint64_t zgaz329;
            zgaz329 = zcap.zT;
            uint64_t zgaz330;
            {
              uint64_t zgaz327;
              zgaz327 = zcap.zB;
              uint64_t zgaz328;
              zgaz328 = zcap.zaddress;
              {
                lbits zgsz3113;
                CREATE(lbits)(&zgsz3113);
                CONVERT_OF(lbits, fbits)(&zgsz3113, zgaz327, UINT64_C(8) , true);
                lbits zgsz3114;
                CREATE(lbits)(&zgsz3114);
                CONVERT_OF(lbits, fbits)(&zgsz3114, zgaz328, UINT64_C(32) , true);
                lbits zgsz3115;
                CREATE(lbits)(&zgsz3115);
                append(&zgsz3115, zgsz3113, zgsz3114);
                zgaz330 = CONVERT_OF(fbits, lbits)(zgsz3115, true);
                KILL(lbits)(&zgsz3115);
                KILL(lbits)(&zgsz3114);
                KILL(lbits)(&zgsz3113);
              }
            
            
            }
            {
              lbits zgsz3116;
              CREATE(lbits)(&zgsz3116);
              CONVERT_OF(lbits, fbits)(&zgsz3116, zgaz329, UINT64_C(6) , true);
              lbits zgsz3117;
              CREATE(lbits)(&zgsz3117);
              CONVERT_OF(lbits, fbits)(&zgsz3117, zgaz330, UINT64_C(40) , true);
              lbits zgsz3118;
              CREATE(lbits)(&zgsz3118);
              append(&zgsz3118, zgsz3116, zgsz3117);
              zgaz332 = CONVERT_OF(fbits, lbits)(zgsz3118, true);
              KILL(lbits)(&zgsz3118);
              KILL(lbits)(&zgsz3117);
              KILL(lbits)(&zgsz3116);
            }
          
          
          }
          {
            lbits zgsz3119;
            CREATE(lbits)(&zgsz3119);
            CONVERT_OF(lbits, fbits)(&zgsz3119, zgaz331, UINT64_C(1) , true);
            lbits zgsz3120;
            CREATE(lbits)(&zgsz3120);
            CONVERT_OF(lbits, fbits)(&zgsz3120, zgaz332, UINT64_C(46) , true);
            lbits zgsz3121;
            CREATE(lbits)(&zgsz3121);
            append(&zgsz3121, zgsz3119, zgsz3120);
            zgaz334 = CONVERT_OF(fbits, lbits)(zgsz3121, true);
            KILL(lbits)(&zgsz3121);
            KILL(lbits)(&zgsz3120);
            KILL(lbits)(&zgsz3119);
          }
        
        
        }
        {
          lbits zgsz3122;
          CREATE(lbits)(&zgsz3122);
          CONVERT_OF(lbits, fbits)(&zgsz3122, zgaz333, UINT64_C(4) , true);
          lbits zgsz3123;
          CREATE(lbits)(&zgsz3123);
          CONVERT_OF(lbits, fbits)(&zgsz3123, zgaz334, UINT64_C(47) , true);
          lbits zgsz3124;
          CREATE(lbits)(&zgsz3124);
          append(&zgsz3124, zgsz3122, zgsz3123);
          zgaz336 = CONVERT_OF(fbits, lbits)(zgsz3124, true);
          KILL(lbits)(&zgsz3124);
          KILL(lbits)(&zgsz3123);
          KILL(lbits)(&zgsz3122);
        }
      
      
      }
      {
        lbits zgsz3125;
        CREATE(lbits)(&zgsz3125);
        CONVERT_OF(lbits, fbits)(&zgsz3125, zgaz335, UINT64_C(1) , true);
        lbits zgsz3126;
        CREATE(lbits)(&zgsz3126);
        CONVERT_OF(lbits, fbits)(&zgsz3126, zgaz336, UINT64_C(51) , true);
        lbits zgsz3127;
        CREATE(lbits)(&zgsz3127);
        append(&zgsz3127, zgsz3125, zgsz3126);
        zgaz338 = CONVERT_OF(fbits, lbits)(zgsz3127, true);
        KILL(lbits)(&zgsz3127);
        KILL(lbits)(&zgsz3126);
        KILL(lbits)(&zgsz3125);
      }
    
    
    }
    {
      lbits zgsz3128;
      CREATE(lbits)(&zgsz3128);
      CONVERT_OF(lbits, fbits)(&zgsz3128, zgaz337, UINT64_C(0) , true);
      lbits zgsz3129;
      CREATE(lbits)(&zgsz3129);
      CONVERT_OF(lbits, fbits)(&zgsz3129, zgaz338, UINT64_C(52) , true);
      lbits zgsz3130;
      CREATE(lbits)(&zgsz3130);
      append(&zgsz3130, zgsz3128, zgsz3129);
      zgaz340 = CONVERT_OF(fbits, lbits)(zgsz3130, true);
      KILL(lbits)(&zgsz3130);
      KILL(lbits)(&zgsz3129);
      KILL(lbits)(&zgsz3128);
    }
  
  
  }
  {
    lbits zgsz3131;
    CREATE(lbits)(&zgsz3131);
    CONVERT_OF(lbits, fbits)(&zgsz3131, zgaz339, UINT64_C(12) , true);
    lbits zgsz3132;
    CREATE(lbits)(&zgsz3132);
    CONVERT_OF(lbits, fbits)(&zgsz3132, zgaz340, UINT64_C(52) , true);
    lbits zgsz3133;
    CREATE(lbits)(&zgsz3133);
    append(&zgsz3133, zgsz3131, zgsz3132);
    zcbz333 = CONVERT_OF(fbits, lbits)(zgsz3133, true);
    KILL(lbits)(&zgsz3133);
    KILL(lbits)(&zgsz3132);
    KILL(lbits)(&zgsz3131);
  }


end_function_127: ;
  return zcbz333;
end_block_exception_128: ;

  return UINT64_C(0xdeadc0de);
}

static int64_t zcap_max_addr;
static void create_letbind_18(void) {  


  int64_t zgsz3134;
  zgsz3134 = sailgen_zzzz7nzzJzzK32z3MAX(zcap_addr_width);
  zcap_max_addr = zgsz3134;

let_end_129: ;
}
static void kill_letbind_18(void) {  
}

static int64_t zcap_max_otype;
static void create_letbind_19(void) {  


  int64_t zgsz3135;
  int64_t zgaz341;
  zgaz341 = sailgen_zzzz7nzzJzzK4z3MAX(zcap_otype_width);
  {
    sail_int zgsz3136;
    CREATE(sail_int)(&zgsz3136);
    CONVERT_OF(sail_int, mach_int)(&zgsz3136, zgaz341);
    sail_int zgsz3137;
    CREATE(sail_int)(&zgsz3137);
    CONVERT_OF(sail_int, mach_int)(&zgsz3137, zreserved_otypes);
    sail_int zgsz3138;
    CREATE(sail_int)(&zgsz3138);
    sub_int(&zgsz3138, zgsz3136, zgsz3137);
    zgsz3135 = CONVERT_OF(mach_int, sail_int)(zgsz3138);
    KILL(sail_int)(&zgsz3138);
    KILL(sail_int)(&zgsz3137);
    KILL(sail_int)(&zgsz3136);
  }

  zcap_max_otype = zgsz3135;

let_end_130: ;
}
static void kill_letbind_19(void) {  
}

static int64_t zcap_uperms_shift;
static void create_letbind_20(void) {  


  int64_t zgsz3139;
  zgsz3139 = INT64_C(15);
  zcap_uperms_shift = zgsz3139;

let_end_131: ;
}
static void kill_letbind_20(void) {  
}

static int64_t zcap_perms_width;
static void create_letbind_21(void) {  


  int64_t zgsz3140;
  {
    sail_int zgsz3141;
    CREATE(sail_int)(&zgsz3141);
    CONVERT_OF(sail_int, mach_int)(&zgsz3141, INT64_C(15));
    sail_int zgsz3142;
    CREATE(sail_int)(&zgsz3142);
    CONVERT_OF(sail_int, mach_int)(&zgsz3142, INT64_C(0));
    sail_int zgsz3143;
    CREATE(sail_int)(&zgsz3143);
    add_int(&zgsz3143, zgsz3141, zgsz3142);
    zgsz3140 = CONVERT_OF(mach_int, sail_int)(zgsz3143);
    KILL(sail_int)(&zgsz3143);
    KILL(sail_int)(&zgsz3142);
    KILL(sail_int)(&zgsz3141);
  }
  zcap_perms_width = zgsz3140;

let_end_132: ;
}
static void kill_letbind_21(void) {  
}

static int64_t zcap_max_E;
static void create_letbind_22(void) {  


  int64_t zgsz3147;
  int64_t zgaz342;
  {
    sail_int zgsz3144;
    CREATE(sail_int)(&zgsz3144);
    CONVERT_OF(sail_int, mach_int)(&zgsz3144, zcap_len_width);
    sail_int zgsz3145;
    CREATE(sail_int)(&zgsz3145);
    CONVERT_OF(sail_int, mach_int)(&zgsz3145, zcap_mantissa_width);
    sail_int zgsz3146;
    CREATE(sail_int)(&zgsz3146);
    sub_int(&zgsz3146, zgsz3144, zgsz3145);
    zgaz342 = CONVERT_OF(mach_int, sail_int)(zgsz3146);
    KILL(sail_int)(&zgsz3146);
    KILL(sail_int)(&zgsz3145);
    KILL(sail_int)(&zgsz3144);
  }
  {
    sail_int zgsz3148;
    CREATE(sail_int)(&zgsz3148);
    CONVERT_OF(sail_int, mach_int)(&zgsz3148, zgaz342);
    sail_int zgsz3149;
    CREATE(sail_int)(&zgsz3149);
    CONVERT_OF(sail_int, mach_int)(&zgsz3149, INT64_C(1));
    sail_int zgsz3150;
    CREATE(sail_int)(&zgsz3150);
    add_int(&zgsz3150, zgsz3148, zgsz3149);
    zgsz3147 = CONVERT_OF(mach_int, sail_int)(zgsz3150);
    KILL(sail_int)(&zgsz3150);
    KILL(sail_int)(&zgsz3149);
    KILL(sail_int)(&zgsz3148);
  }

  zcap_max_E = zgsz3147;

let_end_133: ;
}
static void kill_letbind_22(void) {  
}

static uint64_t zcap_reset_E;
static void create_letbind_23(void) {  


  uint64_t zgsz3151;
  {
    sail_int zgsz3152;
    CREATE(sail_int)(&zgsz3152);
    CONVERT_OF(sail_int, mach_int)(&zgsz3152, zcap_E_width);
    sail_int zgsz3153;
    CREATE(sail_int)(&zgsz3153);
    CONVERT_OF(sail_int, mach_int)(&zgsz3153, zcap_max_E);
    lbits zgsz3154;
    CREATE(lbits)(&zgsz3154);
    sailgen_to_bits(&zgsz3154, zgsz3152, zgsz3153);
    zgsz3151 = CONVERT_OF(fbits, lbits)(zgsz3154, true);
    KILL(lbits)(&zgsz3154);
    KILL(sail_int)(&zgsz3153);
    KILL(sail_int)(&zgsz3152);
  }
  zcap_reset_E = zgsz3151;

let_end_134: ;
}
static void kill_letbind_23(void) {  
}

static uint64_t zcap_reset_T;
static void create_letbind_24(void) {  


  uint64_t zgsz3158;
  uint64_t zgaz344;
  {
    int64_t zgaz343;
    {
      sail_int zgsz3155;
      CREATE(sail_int)(&zgsz3155);
      CONVERT_OF(sail_int, mach_int)(&zgsz3155, zcap_mantissa_width);
      sail_int zgsz3156;
      CREATE(sail_int)(&zgsz3156);
      CONVERT_OF(sail_int, mach_int)(&zgsz3156, INT64_C(2));
      sail_int zgsz3157;
      CREATE(sail_int)(&zgsz3157);
      sub_int(&zgsz3157, zgsz3155, zgsz3156);
      zgaz343 = CONVERT_OF(mach_int, sail_int)(zgsz3157);
      KILL(sail_int)(&zgsz3157);
      KILL(sail_int)(&zgsz3156);
      KILL(sail_int)(&zgsz3155);
    }
    zgaz344 = sailgen_zzzz7nzzJzzK6z3zzeros_implicit(zgaz343);
  
  }
  {
    lbits zgsz3159;
    CREATE(lbits)(&zgsz3159);
    CONVERT_OF(lbits, fbits)(&zgsz3159, UINT64_C(0b01), UINT64_C(2) , true);
    lbits zgsz3160;
    CREATE(lbits)(&zgsz3160);
    CONVERT_OF(lbits, fbits)(&zgsz3160, zgaz344, UINT64_C(6) , true);
    lbits zgsz3161;
    CREATE(lbits)(&zgsz3161);
    append(&zgsz3161, zgsz3159, zgsz3160);
    zgsz3158 = CONVERT_OF(fbits, lbits)(zgsz3161, true);
    KILL(lbits)(&zgsz3161);
    KILL(lbits)(&zgsz3160);
    KILL(lbits)(&zgsz3159);
  }

  zcap_reset_T = zgsz3158;

let_end_135: ;
}
static void kill_letbind_24(void) {  
}

static struct zCapability sailgen_undefined_Capability(unit);

static struct zCapability sailgen_undefined_Capability(unit zgsz3162)
{
  __label__ end_function_137, end_block_exception_138;

  struct zCapability zcbz334;
  bool zgaz345;
  zgaz345 = undefined_bool(UNIT);
  uint64_t zgaz346;
  {
    sail_int zgsz3163;
    CREATE(sail_int)(&zgsz3163);
    CONVERT_OF(sail_int, mach_int)(&zgsz3163, INT64_C(0));
    lbits zgsz3164;
    CREATE(lbits)(&zgsz3164);
    UNDEFINED(lbits)(&zgsz3164, zgsz3163);
    zgaz346 = CONVERT_OF(fbits, lbits)(zgsz3164, true);
    KILL(lbits)(&zgsz3164);
    KILL(sail_int)(&zgsz3163);
  }
  bool zgaz347;
  zgaz347 = undefined_bool(UNIT);
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
  uint64_t zgaz359;
  {
    sail_int zgsz3165;
    CREATE(sail_int)(&zgsz3165);
    CONVERT_OF(sail_int, mach_int)(&zgsz3165, INT64_C(0));
    lbits zgsz3166;
    CREATE(lbits)(&zgsz3166);
    UNDEFINED(lbits)(&zgsz3166, zgsz3165);
    zgaz359 = CONVERT_OF(fbits, lbits)(zgsz3166, true);
    KILL(lbits)(&zgsz3166);
    KILL(sail_int)(&zgsz3165);
  }
  bool zgaz360;
  zgaz360 = undefined_bool(UNIT);
  bool zgaz361;
  zgaz361 = undefined_bool(UNIT);
  uint64_t zgaz362;
  {
    sail_int zgsz3167;
    CREATE(sail_int)(&zgsz3167);
    CONVERT_OF(sail_int, mach_int)(&zgsz3167, INT64_C(6));
    lbits zgsz3168;
    CREATE(lbits)(&zgsz3168);
    UNDEFINED(lbits)(&zgsz3168, zgsz3167);
    zgaz362 = CONVERT_OF(fbits, lbits)(zgsz3168, true);
    KILL(lbits)(&zgsz3168);
    KILL(sail_int)(&zgsz3167);
  }
  uint64_t zgaz363;
  {
    sail_int zgsz3169;
    CREATE(sail_int)(&zgsz3169);
    CONVERT_OF(sail_int, mach_int)(&zgsz3169, INT64_C(8));
    lbits zgsz3170;
    CREATE(lbits)(&zgsz3170);
    UNDEFINED(lbits)(&zgsz3170, zgsz3169);
    zgaz363 = CONVERT_OF(fbits, lbits)(zgsz3170, true);
    KILL(lbits)(&zgsz3170);
    KILL(sail_int)(&zgsz3169);
  }
  uint64_t zgaz364;
  {
    sail_int zgsz3171;
    CREATE(sail_int)(&zgsz3171);
    CONVERT_OF(sail_int, mach_int)(&zgsz3171, INT64_C(8));
    lbits zgsz3172;
    CREATE(lbits)(&zgsz3172);
    UNDEFINED(lbits)(&zgsz3172, zgsz3171);
    zgaz364 = CONVERT_OF(fbits, lbits)(zgsz3172, true);
    KILL(lbits)(&zgsz3172);
    KILL(sail_int)(&zgsz3171);
  }
  uint64_t zgaz365;
  {
    sail_int zgsz3173;
    CREATE(sail_int)(&zgsz3173);
    CONVERT_OF(sail_int, mach_int)(&zgsz3173, INT64_C(4));
    lbits zgsz3174;
    CREATE(lbits)(&zgsz3174);
    UNDEFINED(lbits)(&zgsz3174, zgsz3173);
    zgaz365 = CONVERT_OF(fbits, lbits)(zgsz3174, true);
    KILL(lbits)(&zgsz3174);
    KILL(sail_int)(&zgsz3173);
  }
  uint64_t zgaz366;
  {
    sail_int zgsz3175;
    CREATE(sail_int)(&zgsz3175);
    CONVERT_OF(sail_int, mach_int)(&zgsz3175, INT64_C(32));
    lbits zgsz3176;
    CREATE(lbits)(&zgsz3176);
    UNDEFINED(lbits)(&zgsz3176, zgsz3175);
    zgaz366 = CONVERT_OF(fbits, lbits)(zgsz3176, true);
    KILL(lbits)(&zgsz3176);
    KILL(sail_int)(&zgsz3175);
  }
  struct zCapability zgsz3177;
  zgsz3177.zB = zgaz363;
  zgsz3177.zE = zgaz362;
  zgsz3177.zT = zgaz364;
  zgsz3177.zaccess_system_regs = zgaz348;
  zgsz3177.zaddress = zgaz366;
  zgsz3177.zflag_cap_mode = zgaz360;
  zgsz3177.zglobal = zgaz358;
  zgsz3177.zinternal_E = zgaz361;
  zgsz3177.zotype = zgaz365;
  zgsz3177.zpermit_cinvoke = zgaz350;
  zgsz3177.zpermit_execute = zgaz357;
  zgsz3177.zpermit_load = zgaz356;
  zgsz3177.zpermit_load_cap = zgaz354;
  zgsz3177.zpermit_seal = zgaz351;
  zgsz3177.zpermit_set_CID = zgaz347;
  zgsz3177.zpermit_store = zgaz355;
  zgsz3177.zpermit_store_cap = zgaz353;
  zgsz3177.zpermit_store_local_cap = zgaz352;
  zgsz3177.zpermit_unseal = zgaz349;
  zgsz3177.zreserved = zgaz359;
  zgsz3177.ztag = zgaz345;
  zgsz3177.zuperms = zgaz346;
  zcbz334 = zgsz3177;























end_function_137: ;
  return zcbz334;
end_block_exception_138: ;
  struct zCapability zcbz360 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz360;
}

static struct zCapability znull_cap;
static void create_letbind_25(void) {  


  struct zCapability zgsz3182;
  uint64_t zgaz367;
  zgaz367 = sailgen_zzzz7nzzJzzK0z3zzeros_implicit(INT64_C(0));
  uint64_t zgaz368;
  zgaz368 = sailgen_zzzz7nzzJzzK0z3zzeros_implicit(INT64_C(0));
  uint64_t zgaz369;
  zgaz369 = sailgen_zzzz7nzzJzzK8z3zzeros_implicit(INT64_C(8));
  uint64_t zgaz370;
  {
    sail_int zgsz3178;
    CREATE(sail_int)(&zgsz3178);
    CONVERT_OF(sail_int, mach_int)(&zgsz3178, zcap_otype_width);
    sail_int zgsz3179;
    CREATE(sail_int)(&zgsz3179);
    CONVERT_OF(sail_int, mach_int)(&zgsz3179, zotype_unsealed);
    lbits zgsz3180;
    CREATE(lbits)(&zgsz3180);
    sailgen_to_bits(&zgsz3180, zgsz3178, zgsz3179);
    zgaz370 = CONVERT_OF(fbits, lbits)(zgsz3180, true);
    KILL(lbits)(&zgsz3180);
    KILL(sail_int)(&zgsz3179);
    KILL(sail_int)(&zgsz3178);
  }
  uint64_t zgaz371;
  zgaz371 = sailgen_zzzz7nzzJzzK32z3zzeros_implicit(INT64_C(32));
  struct zCapability zgsz3181;
  zgsz3181.zB = zgaz369;
  zgsz3181.zE = zcap_reset_E;
  zgsz3181.zT = zcap_reset_T;
  zgsz3181.zaccess_system_regs = false;
  zgsz3181.zaddress = zgaz371;
  zgsz3181.zflag_cap_mode = false;
  zgsz3181.zglobal = false;
  zgsz3181.zinternal_E = true;
  zgsz3181.zotype = zgaz370;
  zgsz3181.zpermit_cinvoke = false;
  zgsz3181.zpermit_execute = false;
  zgsz3181.zpermit_load = false;
  zgsz3181.zpermit_load_cap = false;
  zgsz3181.zpermit_seal = false;
  zgsz3181.zpermit_set_CID = false;
  zgsz3181.zpermit_store = false;
  zgsz3181.zpermit_store_cap = false;
  zgsz3181.zpermit_store_local_cap = false;
  zgsz3181.zpermit_unseal = false;
  zgsz3181.zreserved = zgaz368;
  zgsz3181.ztag = false;
  zgsz3181.zuperms = zgaz367;
  zgsz3182 = zgsz3181;






  znull_cap = zgsz3182;

let_end_139: ;
}
static void kill_letbind_25(void) {  
}

static struct zCapability zdefault_cap;
static void create_letbind_26(void) {  


  struct zCapability zgsz3187;
  uint64_t zgaz372;
  zgaz372 = sailgen_zzzz7nzzJzzK0z3ones(INT64_C(0));
  uint64_t zgaz373;
  zgaz373 = sailgen_zzzz7nzzJzzK0z3zzeros_implicit(INT64_C(0));
  uint64_t zgaz374;
  zgaz374 = sailgen_zzzz7nzzJzzK8z3zzeros_implicit(INT64_C(8));
  uint64_t zgaz375;
  {
    sail_int zgsz3183;
    CREATE(sail_int)(&zgsz3183);
    CONVERT_OF(sail_int, mach_int)(&zgsz3183, zcap_otype_width);
    sail_int zgsz3184;
    CREATE(sail_int)(&zgsz3184);
    CONVERT_OF(sail_int, mach_int)(&zgsz3184, zotype_unsealed);
    lbits zgsz3185;
    CREATE(lbits)(&zgsz3185);
    sailgen_to_bits(&zgsz3185, zgsz3183, zgsz3184);
    zgaz375 = CONVERT_OF(fbits, lbits)(zgsz3185, true);
    KILL(lbits)(&zgsz3185);
    KILL(sail_int)(&zgsz3184);
    KILL(sail_int)(&zgsz3183);
  }
  uint64_t zgaz376;
  zgaz376 = sailgen_zzzz7nzzJzzK32z3zzeros_implicit(INT64_C(32));
  struct zCapability zgsz3186;
  zgsz3186.zB = zgaz374;
  zgsz3186.zE = zcap_reset_E;
  zgsz3186.zT = zcap_reset_T;
  zgsz3186.zaccess_system_regs = true;
  zgsz3186.zaddress = zgaz376;
  zgsz3186.zflag_cap_mode = false;
  zgsz3186.zglobal = true;
  zgsz3186.zinternal_E = true;
  zgsz3186.zotype = zgaz375;
  zgsz3186.zpermit_cinvoke = true;
  zgsz3186.zpermit_execute = true;
  zgsz3186.zpermit_load = true;
  zgsz3186.zpermit_load_cap = true;
  zgsz3186.zpermit_seal = true;
  zgsz3186.zpermit_set_CID = true;
  zgsz3186.zpermit_store = true;
  zgsz3186.zpermit_store_cap = true;
  zgsz3186.zpermit_store_local_cap = true;
  zgsz3186.zpermit_unseal = true;
  zgsz3186.zreserved = zgaz373;
  zgsz3186.ztag = true;
  zgsz3186.zuperms = zgaz372;
  zgsz3187 = zgsz3186;






  zdefault_cap = zgsz3187;

let_end_140: ;
}
static void kill_letbind_26(void) {  
}

static uint64_t sailgen_getCapHardPerms(struct zCapability);

static uint64_t sailgen_getCapHardPerms(struct zCapability zcap)
{
  __label__ end_function_142, end_block_exception_143;

  uint64_t zcbz335;
  uint64_t zgaz3109;
  {
    bool zgaz377;
    zgaz377 = zcap.zpermit_set_CID;
    zgaz3109 = sailgen_bool_to_bits(zgaz377);
  
  }
  uint64_t zgaz3110;
  {
    uint64_t zgaz3107;
    {
      bool zgaz378;
      zgaz378 = zcap.zaccess_system_regs;
      zgaz3107 = sailgen_bool_to_bits(zgaz378);
    
    }
    uint64_t zgaz3108;
    {
      uint64_t zgaz3105;
      {
        bool zgaz379;
        zgaz379 = zcap.zpermit_unseal;
        zgaz3105 = sailgen_bool_to_bits(zgaz379);
      
      }
      uint64_t zgaz3106;
      {
        uint64_t zgaz3103;
        {
          bool zgaz380;
          zgaz380 = zcap.zpermit_cinvoke;
          zgaz3103 = sailgen_bool_to_bits(zgaz380);
        
        }
        uint64_t zgaz3104;
        {
          uint64_t zgaz3101;
          {
            bool zgaz381;
            zgaz381 = zcap.zpermit_seal;
            zgaz3101 = sailgen_bool_to_bits(zgaz381);
          
          }
          uint64_t zgaz3102;
          {
            uint64_t zgaz399;
            {
              bool zgaz382;
              zgaz382 = zcap.zpermit_store_local_cap;
              zgaz399 = sailgen_bool_to_bits(zgaz382);
            
            }
            uint64_t zgaz3100;
            {
              uint64_t zgaz397;
              {
                bool zgaz383;
                zgaz383 = zcap.zpermit_store_cap;
                zgaz397 = sailgen_bool_to_bits(zgaz383);
              
              }
              uint64_t zgaz398;
              {
                uint64_t zgaz395;
                {
                  bool zgaz384;
                  zgaz384 = zcap.zpermit_load_cap;
                  zgaz395 = sailgen_bool_to_bits(zgaz384);
                
                }
                uint64_t zgaz396;
                {
                  uint64_t zgaz393;
                  {
                    bool zgaz385;
                    zgaz385 = zcap.zpermit_store;
                    zgaz393 = sailgen_bool_to_bits(zgaz385);
                  
                  }
                  uint64_t zgaz394;
                  {
                    uint64_t zgaz391;
                    {
                      bool zgaz386;
                      zgaz386 = zcap.zpermit_load;
                      zgaz391 = sailgen_bool_to_bits(zgaz386);
                    
                    }
                    uint64_t zgaz392;
                    {
                      uint64_t zgaz389;
                      {
                        bool zgaz387;
                        zgaz387 = zcap.zpermit_execute;
                        zgaz389 = sailgen_bool_to_bits(zgaz387);
                      
                      }
                      uint64_t zgaz390;
                      {
                        bool zgaz388;
                        zgaz388 = zcap.zglobal;
                        zgaz390 = sailgen_bool_to_bits(zgaz388);
                      
                      }
                      {
                        lbits zgsz3188;
                        CREATE(lbits)(&zgsz3188);
                        CONVERT_OF(lbits, fbits)(&zgsz3188, zgaz389, UINT64_C(1) , true);
                        lbits zgsz3189;
                        CREATE(lbits)(&zgsz3189);
                        CONVERT_OF(lbits, fbits)(&zgsz3189, zgaz390, UINT64_C(1) , true);
                        lbits zgsz3190;
                        CREATE(lbits)(&zgsz3190);
                        append(&zgsz3190, zgsz3188, zgsz3189);
                        zgaz392 = CONVERT_OF(fbits, lbits)(zgsz3190, true);
                        KILL(lbits)(&zgsz3190);
                        KILL(lbits)(&zgsz3189);
                        KILL(lbits)(&zgsz3188);
                      }
                    
                    
                    }
                    {
                      lbits zgsz3191;
                      CREATE(lbits)(&zgsz3191);
                      CONVERT_OF(lbits, fbits)(&zgsz3191, zgaz391, UINT64_C(1) , true);
                      lbits zgsz3192;
                      CREATE(lbits)(&zgsz3192);
                      CONVERT_OF(lbits, fbits)(&zgsz3192, zgaz392, UINT64_C(2) , true);
                      lbits zgsz3193;
                      CREATE(lbits)(&zgsz3193);
                      append(&zgsz3193, zgsz3191, zgsz3192);
                      zgaz394 = CONVERT_OF(fbits, lbits)(zgsz3193, true);
                      KILL(lbits)(&zgsz3193);
                      KILL(lbits)(&zgsz3192);
                      KILL(lbits)(&zgsz3191);
                    }
                  
                  
                  }
                  {
                    lbits zgsz3194;
                    CREATE(lbits)(&zgsz3194);
                    CONVERT_OF(lbits, fbits)(&zgsz3194, zgaz393, UINT64_C(1) , true);
                    lbits zgsz3195;
                    CREATE(lbits)(&zgsz3195);
                    CONVERT_OF(lbits, fbits)(&zgsz3195, zgaz394, UINT64_C(3) , true);
                    lbits zgsz3196;
                    CREATE(lbits)(&zgsz3196);
                    append(&zgsz3196, zgsz3194, zgsz3195);
                    zgaz396 = CONVERT_OF(fbits, lbits)(zgsz3196, true);
                    KILL(lbits)(&zgsz3196);
                    KILL(lbits)(&zgsz3195);
                    KILL(lbits)(&zgsz3194);
                  }
                
                
                }
                {
                  lbits zgsz3197;
                  CREATE(lbits)(&zgsz3197);
                  CONVERT_OF(lbits, fbits)(&zgsz3197, zgaz395, UINT64_C(1) , true);
                  lbits zgsz3198;
                  CREATE(lbits)(&zgsz3198);
                  CONVERT_OF(lbits, fbits)(&zgsz3198, zgaz396, UINT64_C(4) , true);
                  lbits zgsz3199;
                  CREATE(lbits)(&zgsz3199);
                  append(&zgsz3199, zgsz3197, zgsz3198);
                  zgaz398 = CONVERT_OF(fbits, lbits)(zgsz3199, true);
                  KILL(lbits)(&zgsz3199);
                  KILL(lbits)(&zgsz3198);
                  KILL(lbits)(&zgsz3197);
                }
              
              
              }
              {
                lbits zgsz3200;
                CREATE(lbits)(&zgsz3200);
                CONVERT_OF(lbits, fbits)(&zgsz3200, zgaz397, UINT64_C(1) , true);
                lbits zgsz3201;
                CREATE(lbits)(&zgsz3201);
                CONVERT_OF(lbits, fbits)(&zgsz3201, zgaz398, UINT64_C(5) , true);
                lbits zgsz3202;
                CREATE(lbits)(&zgsz3202);
                append(&zgsz3202, zgsz3200, zgsz3201);
                zgaz3100 = CONVERT_OF(fbits, lbits)(zgsz3202, true);
                KILL(lbits)(&zgsz3202);
                KILL(lbits)(&zgsz3201);
                KILL(lbits)(&zgsz3200);
              }
            
            
            }
            {
              lbits zgsz3203;
              CREATE(lbits)(&zgsz3203);
              CONVERT_OF(lbits, fbits)(&zgsz3203, zgaz399, UINT64_C(1) , true);
              lbits zgsz3204;
              CREATE(lbits)(&zgsz3204);
              CONVERT_OF(lbits, fbits)(&zgsz3204, zgaz3100, UINT64_C(6) , true);
              lbits zgsz3205;
              CREATE(lbits)(&zgsz3205);
              append(&zgsz3205, zgsz3203, zgsz3204);
              zgaz3102 = CONVERT_OF(fbits, lbits)(zgsz3205, true);
              KILL(lbits)(&zgsz3205);
              KILL(lbits)(&zgsz3204);
              KILL(lbits)(&zgsz3203);
            }
          
          
          }
          {
            lbits zgsz3206;
            CREATE(lbits)(&zgsz3206);
            CONVERT_OF(lbits, fbits)(&zgsz3206, zgaz3101, UINT64_C(1) , true);
            lbits zgsz3207;
            CREATE(lbits)(&zgsz3207);
            CONVERT_OF(lbits, fbits)(&zgsz3207, zgaz3102, UINT64_C(7) , true);
            lbits zgsz3208;
            CREATE(lbits)(&zgsz3208);
            append(&zgsz3208, zgsz3206, zgsz3207);
            zgaz3104 = CONVERT_OF(fbits, lbits)(zgsz3208, true);
            KILL(lbits)(&zgsz3208);
            KILL(lbits)(&zgsz3207);
            KILL(lbits)(&zgsz3206);
          }
        
        
        }
        {
          lbits zgsz3209;
          CREATE(lbits)(&zgsz3209);
          CONVERT_OF(lbits, fbits)(&zgsz3209, zgaz3103, UINT64_C(1) , true);
          lbits zgsz3210;
          CREATE(lbits)(&zgsz3210);
          CONVERT_OF(lbits, fbits)(&zgsz3210, zgaz3104, UINT64_C(8) , true);
          lbits zgsz3211;
          CREATE(lbits)(&zgsz3211);
          append(&zgsz3211, zgsz3209, zgsz3210);
          zgaz3106 = CONVERT_OF(fbits, lbits)(zgsz3211, true);
          KILL(lbits)(&zgsz3211);
          KILL(lbits)(&zgsz3210);
          KILL(lbits)(&zgsz3209);
        }
      
      
      }
      {
        lbits zgsz3212;
        CREATE(lbits)(&zgsz3212);
        CONVERT_OF(lbits, fbits)(&zgsz3212, zgaz3105, UINT64_C(1) , true);
        lbits zgsz3213;
        CREATE(lbits)(&zgsz3213);
        CONVERT_OF(lbits, fbits)(&zgsz3213, zgaz3106, UINT64_C(9) , true);
        lbits zgsz3214;
        CREATE(lbits)(&zgsz3214);
        append(&zgsz3214, zgsz3212, zgsz3213);
        zgaz3108 = CONVERT_OF(fbits, lbits)(zgsz3214, true);
        KILL(lbits)(&zgsz3214);
        KILL(lbits)(&zgsz3213);
        KILL(lbits)(&zgsz3212);
      }
    
    
    }
    {
      lbits zgsz3215;
      CREATE(lbits)(&zgsz3215);
      CONVERT_OF(lbits, fbits)(&zgsz3215, zgaz3107, UINT64_C(1) , true);
      lbits zgsz3216;
      CREATE(lbits)(&zgsz3216);
      CONVERT_OF(lbits, fbits)(&zgsz3216, zgaz3108, UINT64_C(10) , true);
      lbits zgsz3217;
      CREATE(lbits)(&zgsz3217);
      append(&zgsz3217, zgsz3215, zgsz3216);
      zgaz3110 = CONVERT_OF(fbits, lbits)(zgsz3217, true);
      KILL(lbits)(&zgsz3217);
      KILL(lbits)(&zgsz3216);
      KILL(lbits)(&zgsz3215);
    }
  
  
  }
  {
    lbits zgsz3218;
    CREATE(lbits)(&zgsz3218);
    CONVERT_OF(lbits, fbits)(&zgsz3218, zgaz3109, UINT64_C(1) , true);
    lbits zgsz3219;
    CREATE(lbits)(&zgsz3219);
    CONVERT_OF(lbits, fbits)(&zgsz3219, zgaz3110, UINT64_C(11) , true);
    lbits zgsz3220;
    CREATE(lbits)(&zgsz3220);
    append(&zgsz3220, zgsz3218, zgsz3219);
    zcbz335 = CONVERT_OF(fbits, lbits)(zgsz3220, true);
    KILL(lbits)(&zgsz3220);
    KILL(lbits)(&zgsz3219);
    KILL(lbits)(&zgsz3218);
  }


end_function_142: ;
  return zcbz335;
end_block_exception_143: ;

  return UINT64_C(0xdeadc0de);
}

static struct zCapability sailgen_encCapabilityToCapability(bool, struct zEncCapability);

static struct zCapability sailgen_encCapabilityToCapability(bool zt, struct zEncCapability zc)
{
  __label__ cleanup_146, end_cleanup_147, end_function_145, end_block_exception_148;

  struct zCapability zcbz336;
  bool zinternal_E;
  {
    fbits zgaz3182;
    zgaz3182 = zc.zinternal_E;
    zinternal_E = sailgen_bit_to_bool(zgaz3182);
  
  }
  uint64_t zE;
  zE = sailgen_zzzz7nzzJzzK6z3zzeros_implicit(INT64_C(6));
  uint64_t zBs;
  zBs = sailgen_zzzz7nzzJzzK8z3zzeros_implicit(INT64_C(8));
  uint64_t zT;
  {
    int64_t zgaz3181;
    {
      sail_int zgsz3221;
      CREATE(sail_int)(&zgsz3221);
      CONVERT_OF(sail_int, mach_int)(&zgsz3221, INT64_C(8));
      sail_int zgsz3222;
      CREATE(sail_int)(&zgsz3222);
      CONVERT_OF(sail_int, mach_int)(&zgsz3222, INT64_C(2));
      sail_int zgsz3223;
      CREATE(sail_int)(&zgsz3223);
      sub_int(&zgsz3223, zgsz3221, zgsz3222);
      zgaz3181 = CONVERT_OF(mach_int, sail_int)(zgsz3223);
      KILL(sail_int)(&zgsz3223);
      KILL(sail_int)(&zgsz3222);
      KILL(sail_int)(&zgsz3221);
    }
    {
      sail_int zgsz3224;
      CREATE(sail_int)(&zgsz3224);
      CONVERT_OF(sail_int, mach_int)(&zgsz3224, zgaz3181);
      lbits zgsz3225;
      CREATE(lbits)(&zgsz3225);
      sailgen_zzz3zzz3zzeros_implicit(&zgsz3225, zgsz3224);
      zT = CONVERT_OF(fbits, lbits)(zgsz3225, true);
      KILL(lbits)(&zgsz3225);
      KILL(sail_int)(&zgsz3224);
    }
  
  }
  uint64_t zlenMSBs;
  zlenMSBs = sailgen_zzzz7nzzJzzK2z3zzeros_implicit(INT64_C(2));
  {
    unit zgsz3226;
    if (zinternal_E) {
    {
      uint64_t zgaz3115;
      {
        uint64_t zgaz3111;
        zgaz3111 = zc.zT;
        int64_t zgaz3112;
        {
          sail_int zgsz3229;
          CREATE(sail_int)(&zgsz3229);
          CONVERT_OF(sail_int, mach_int)(&zgsz3229, zinternal_E_take_bits);
          sail_int zgsz3230;
          CREATE(sail_int)(&zgsz3230);
          CONVERT_OF(sail_int, mach_int)(&zgsz3230, INT64_C(1));
          sail_int zgsz3231;
          CREATE(sail_int)(&zgsz3231);
          sub_int(&zgsz3231, zgsz3229, zgsz3230);
          zgaz3112 = CONVERT_OF(mach_int, sail_int)(zgsz3231);
          KILL(sail_int)(&zgsz3231);
          KILL(sail_int)(&zgsz3230);
          KILL(sail_int)(&zgsz3229);
        }
        {
          lbits zgsz3232;
          CREATE(lbits)(&zgsz3232);
          CONVERT_OF(lbits, fbits)(&zgsz3232, zgaz3111, UINT64_C(6) , true);
          sail_int zgsz3233;
          CREATE(sail_int)(&zgsz3233);
          CONVERT_OF(sail_int, mach_int)(&zgsz3233, zgaz3112);
          sail_int zgsz3234;
          CREATE(sail_int)(&zgsz3234);
          CONVERT_OF(sail_int, mach_int)(&zgsz3234, INT64_C(0));
          lbits zgsz3235;
          CREATE(lbits)(&zgsz3235);
          vector_subrange_lbits(&zgsz3235, zgsz3232, zgsz3233, zgsz3234);
          zgaz3115 = CONVERT_OF(fbits, lbits)(zgsz3235, true);
          KILL(lbits)(&zgsz3235);
          KILL(sail_int)(&zgsz3234);
          KILL(sail_int)(&zgsz3233);
          KILL(lbits)(&zgsz3232);
        }
      
      
      }
      uint64_t zgaz3116;
      {
        uint64_t zgaz3113;
        zgaz3113 = zc.zB;
        int64_t zgaz3114;
        {
          sail_int zgsz3236;
          CREATE(sail_int)(&zgsz3236);
          CONVERT_OF(sail_int, mach_int)(&zgsz3236, zinternal_E_take_bits);
          sail_int zgsz3237;
          CREATE(sail_int)(&zgsz3237);
          CONVERT_OF(sail_int, mach_int)(&zgsz3237, INT64_C(1));
          sail_int zgsz3238;
          CREATE(sail_int)(&zgsz3238);
          sub_int(&zgsz3238, zgsz3236, zgsz3237);
          zgaz3114 = CONVERT_OF(mach_int, sail_int)(zgsz3238);
          KILL(sail_int)(&zgsz3238);
          KILL(sail_int)(&zgsz3237);
          KILL(sail_int)(&zgsz3236);
        }
        {
          lbits zgsz3239;
          CREATE(lbits)(&zgsz3239);
          CONVERT_OF(lbits, fbits)(&zgsz3239, zgaz3113, UINT64_C(8) , true);
          sail_int zgsz3240;
          CREATE(sail_int)(&zgsz3240);
          CONVERT_OF(sail_int, mach_int)(&zgsz3240, zgaz3114);
          sail_int zgsz3241;
          CREATE(sail_int)(&zgsz3241);
          CONVERT_OF(sail_int, mach_int)(&zgsz3241, INT64_C(0));
          lbits zgsz3242;
          CREATE(lbits)(&zgsz3242);
          vector_subrange_lbits(&zgsz3242, zgsz3239, zgsz3240, zgsz3241);
          zgaz3116 = CONVERT_OF(fbits, lbits)(zgsz3242, true);
          KILL(lbits)(&zgsz3242);
          KILL(sail_int)(&zgsz3241);
          KILL(sail_int)(&zgsz3240);
          KILL(lbits)(&zgsz3239);
        }
      
      
      }
      {
        lbits zgsz3243;
        CREATE(lbits)(&zgsz3243);
        CONVERT_OF(lbits, fbits)(&zgsz3243, zgaz3115, UINT64_C(3) , true);
        lbits zgsz3244;
        CREATE(lbits)(&zgsz3244);
        CONVERT_OF(lbits, fbits)(&zgsz3244, zgaz3116, UINT64_C(3) , true);
        lbits zgsz3245;
        CREATE(lbits)(&zgsz3245);
        append(&zgsz3245, zgsz3243, zgsz3244);
        zE = CONVERT_OF(fbits, lbits)(zgsz3245, true);
        KILL(lbits)(&zgsz3245);
        KILL(lbits)(&zgsz3244);
        KILL(lbits)(&zgsz3243);
      }
      unit zgsz3258;
      zgsz3258 = UNIT;
    
    
    }
    {
      zlenMSBs = UINT64_C(0b01);
      unit zgsz3257;
      zgsz3257 = UNIT;
    }
    {
      uint64_t zgaz3119;
      {
        uint64_t zgaz3117;
        zgaz3117 = zc.zT;
        int64_t zgaz3118;
        {
          sail_int zgsz3246;
          CREATE(sail_int)(&zgsz3246);
          CONVERT_OF(sail_int, mach_int)(&zgsz3246, zcap_mantissa_width);
          sail_int zgsz3247;
          CREATE(sail_int)(&zgsz3247);
          CONVERT_OF(sail_int, mach_int)(&zgsz3247, INT64_C(3));
          sail_int zgsz3248;
          CREATE(sail_int)(&zgsz3248);
          sub_int(&zgsz3248, zgsz3246, zgsz3247);
          zgaz3118 = CONVERT_OF(mach_int, sail_int)(zgsz3248);
          KILL(sail_int)(&zgsz3248);
          KILL(sail_int)(&zgsz3247);
          KILL(sail_int)(&zgsz3246);
        }
        {
          lbits zgsz3249;
          CREATE(lbits)(&zgsz3249);
          CONVERT_OF(lbits, fbits)(&zgsz3249, zgaz3117, UINT64_C(6) , true);
          sail_int zgsz3250;
          CREATE(sail_int)(&zgsz3250);
          CONVERT_OF(sail_int, mach_int)(&zgsz3250, zgaz3118);
          sail_int zgsz3251;
          CREATE(sail_int)(&zgsz3251);
          CONVERT_OF(sail_int, mach_int)(&zgsz3251, INT64_C(3));
          lbits zgsz3252;
          CREATE(lbits)(&zgsz3252);
          vector_subrange_lbits(&zgsz3252, zgsz3249, zgsz3250, zgsz3251);
          zgaz3119 = CONVERT_OF(fbits, lbits)(zgsz3252, true);
          KILL(lbits)(&zgsz3252);
          KILL(sail_int)(&zgsz3251);
          KILL(sail_int)(&zgsz3250);
          KILL(lbits)(&zgsz3249);
        }
      
      
      }
      uint64_t zgaz3120;
      zgaz3120 = sailgen_zzzz7nzzJzzK3z3zzeros_implicit(zinternal_E_take_bits);
      {
        lbits zgsz3253;
        CREATE(lbits)(&zgsz3253);
        CONVERT_OF(lbits, fbits)(&zgsz3253, zgaz3119, UINT64_C(3) , true);
        lbits zgsz3254;
        CREATE(lbits)(&zgsz3254);
        CONVERT_OF(lbits, fbits)(&zgsz3254, zgaz3120, UINT64_C(3) , true);
        lbits zgsz3255;
        CREATE(lbits)(&zgsz3255);
        append(&zgsz3255, zgsz3253, zgsz3254);
        zT = CONVERT_OF(fbits, lbits)(zgsz3255, true);
        KILL(lbits)(&zgsz3255);
        KILL(lbits)(&zgsz3254);
        KILL(lbits)(&zgsz3253);
      }
      unit zgsz3256;
      zgsz3256 = UNIT;
    
    
    }
    uint64_t zgaz3123;
    {
      uint64_t zgaz3121;
      zgaz3121 = zc.zB;
      int64_t zgaz3122;
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
        zgaz3122 = CONVERT_OF(mach_int, sail_int)(zgsz3261);
        KILL(sail_int)(&zgsz3261);
        KILL(sail_int)(&zgsz3260);
        KILL(sail_int)(&zgsz3259);
      }
      {
        lbits zgsz3262;
        CREATE(lbits)(&zgsz3262);
        CONVERT_OF(lbits, fbits)(&zgsz3262, zgaz3121, UINT64_C(8) , true);
        sail_int zgsz3263;
        CREATE(sail_int)(&zgsz3263);
        CONVERT_OF(sail_int, mach_int)(&zgsz3263, zgaz3122);
        sail_int zgsz3264;
        CREATE(sail_int)(&zgsz3264);
        CONVERT_OF(sail_int, mach_int)(&zgsz3264, INT64_C(3));
        lbits zgsz3265;
        CREATE(lbits)(&zgsz3265);
        vector_subrange_lbits(&zgsz3265, zgsz3262, zgsz3263, zgsz3264);
        zgaz3123 = CONVERT_OF(fbits, lbits)(zgsz3265, true);
        KILL(lbits)(&zgsz3265);
        KILL(sail_int)(&zgsz3264);
        KILL(sail_int)(&zgsz3263);
        KILL(lbits)(&zgsz3262);
      }
    
    
    }
    uint64_t zgaz3124;
    zgaz3124 = sailgen_zzzz7nzzJzzK3z3zzeros_implicit(zinternal_E_take_bits);
    {
      lbits zgsz3266;
      CREATE(lbits)(&zgsz3266);
      CONVERT_OF(lbits, fbits)(&zgsz3266, zgaz3123, UINT64_C(5) , true);
      lbits zgsz3267;
      CREATE(lbits)(&zgsz3267);
      CONVERT_OF(lbits, fbits)(&zgsz3267, zgaz3124, UINT64_C(3) , true);
      lbits zgsz3268;
      CREATE(lbits)(&zgsz3268);
      append(&zgsz3268, zgsz3266, zgsz3267);
      zBs = CONVERT_OF(fbits, lbits)(zgsz3268, true);
      KILL(lbits)(&zgsz3268);
      KILL(lbits)(&zgsz3267);
      KILL(lbits)(&zgsz3266);
    }
    zgsz3226 = UNIT;
  
  
    } else {
    {
      zlenMSBs = UINT64_C(0b00);
      unit zgsz3228;
      zgsz3228 = UNIT;
    }
    {
      zT = zc.zT;
      unit zgsz3227;
      zgsz3227 = UNIT;
    }
    zBs = zc.zB;
    zgsz3226 = UNIT;
    }
  }
  uint64_t zcarry_out;
  {
    bool zgaz3180;
    {
      uint64_t zgaz3179;
      {
        int64_t zgaz3178;
        {
          sail_int zgsz3269;
          CREATE(sail_int)(&zgsz3269);
          CONVERT_OF(sail_int, mach_int)(&zgsz3269, zcap_mantissa_width);
          sail_int zgsz3270;
          CREATE(sail_int)(&zgsz3270);
          CONVERT_OF(sail_int, mach_int)(&zgsz3270, INT64_C(3));
          sail_int zgsz3271;
          CREATE(sail_int)(&zgsz3271);
          sub_int(&zgsz3271, zgsz3269, zgsz3270);
          zgaz3178 = CONVERT_OF(mach_int, sail_int)(zgsz3271);
          KILL(sail_int)(&zgsz3271);
          KILL(sail_int)(&zgsz3270);
          KILL(sail_int)(&zgsz3269);
        }
        {
          lbits zgsz3272;
          CREATE(lbits)(&zgsz3272);
          CONVERT_OF(lbits, fbits)(&zgsz3272, zBs, UINT64_C(8) , true);
          sail_int zgsz3273;
          CREATE(sail_int)(&zgsz3273);
          CONVERT_OF(sail_int, mach_int)(&zgsz3273, zgaz3178);
          sail_int zgsz3274;
          CREATE(sail_int)(&zgsz3274);
          CONVERT_OF(sail_int, mach_int)(&zgsz3274, INT64_C(0));
          lbits zgsz3275;
          CREATE(lbits)(&zgsz3275);
          vector_subrange_lbits(&zgsz3275, zgsz3272, zgsz3273, zgsz3274);
          zgaz3179 = CONVERT_OF(fbits, lbits)(zgsz3275, true);
          KILL(lbits)(&zgsz3275);
          KILL(sail_int)(&zgsz3274);
          KILL(sail_int)(&zgsz3273);
          KILL(lbits)(&zgsz3272);
        }
      
      }
      {
        lbits zgsz3277;
        CREATE(lbits)(&zgsz3277);
        CONVERT_OF(lbits, fbits)(&zgsz3277, zgaz3179, UINT64_C(6) , true);
        lbits zgsz3276;
        CREATE(lbits)(&zgsz3276);
        CONVERT_OF(lbits, fbits)(&zgsz3276, zT, UINT64_C(6) , true);
        zgaz3180 = sailgen_z8operatorz0zzz3zzz3zI_uz9(zgsz3276, zgsz3277);
        KILL(lbits)(&zgsz3277);
        KILL(lbits)(&zgsz3276);
      }
    
    }
    if (zgaz3180) {  zcarry_out = UINT64_C(0b01);  } else {  zcarry_out = UINT64_C(0b00);  }
  
  }
  uint64_t zTtop2;
  {
    uint64_t zgaz3177;
    {
      uint64_t zgaz3176;
      {
        int64_t zgaz3174;
        {
          sail_int zgsz3278;
          CREATE(sail_int)(&zgsz3278);
          CONVERT_OF(sail_int, mach_int)(&zgsz3278, zcap_mantissa_width);
          sail_int zgsz3279;
          CREATE(sail_int)(&zgsz3279);
          CONVERT_OF(sail_int, mach_int)(&zgsz3279, INT64_C(1));
          sail_int zgsz3280;
          CREATE(sail_int)(&zgsz3280);
          sub_int(&zgsz3280, zgsz3278, zgsz3279);
          zgaz3174 = CONVERT_OF(mach_int, sail_int)(zgsz3280);
          KILL(sail_int)(&zgsz3280);
          KILL(sail_int)(&zgsz3279);
          KILL(sail_int)(&zgsz3278);
        }
        int64_t zgaz3175;
        {
          sail_int zgsz3281;
          CREATE(sail_int)(&zgsz3281);
          CONVERT_OF(sail_int, mach_int)(&zgsz3281, zcap_mantissa_width);
          sail_int zgsz3282;
          CREATE(sail_int)(&zgsz3282);
          CONVERT_OF(sail_int, mach_int)(&zgsz3282, INT64_C(2));
          sail_int zgsz3283;
          CREATE(sail_int)(&zgsz3283);
          sub_int(&zgsz3283, zgsz3281, zgsz3282);
          zgaz3175 = CONVERT_OF(mach_int, sail_int)(zgsz3283);
          KILL(sail_int)(&zgsz3283);
          KILL(sail_int)(&zgsz3282);
          KILL(sail_int)(&zgsz3281);
        }
        {
          lbits zgsz3284;
          CREATE(lbits)(&zgsz3284);
          CONVERT_OF(lbits, fbits)(&zgsz3284, zBs, UINT64_C(8) , true);
          sail_int zgsz3285;
          CREATE(sail_int)(&zgsz3285);
          CONVERT_OF(sail_int, mach_int)(&zgsz3285, zgaz3174);
          sail_int zgsz3286;
          CREATE(sail_int)(&zgsz3286);
          CONVERT_OF(sail_int, mach_int)(&zgsz3286, zgaz3175);
          lbits zgsz3287;
          CREATE(lbits)(&zgsz3287);
          vector_subrange_lbits(&zgsz3287, zgsz3284, zgsz3285, zgsz3286);
          zgaz3176 = CONVERT_OF(fbits, lbits)(zgsz3287, true);
          KILL(lbits)(&zgsz3287);
          KILL(sail_int)(&zgsz3286);
          KILL(sail_int)(&zgsz3285);
          KILL(lbits)(&zgsz3284);
        }
      
      
      }
      {
        lbits zgsz3288;
        CREATE(lbits)(&zgsz3288);
        CONVERT_OF(lbits, fbits)(&zgsz3288, zgaz3176, UINT64_C(2) , true);
        lbits zgsz3289;
        CREATE(lbits)(&zgsz3289);
        CONVERT_OF(lbits, fbits)(&zgsz3289, zlenMSBs, UINT64_C(2) , true);
        lbits zgsz3290;
        CREATE(lbits)(&zgsz3290);
        add_bits(&zgsz3290, zgsz3288, zgsz3289);
        zgaz3177 = CONVERT_OF(fbits, lbits)(zgsz3290, true);
        KILL(lbits)(&zgsz3290);
        KILL(lbits)(&zgsz3289);
        KILL(lbits)(&zgsz3288);
      }
    
    }
    {
      lbits zgsz3291;
      CREATE(lbits)(&zgsz3291);
      CONVERT_OF(lbits, fbits)(&zgsz3291, zgaz3177, UINT64_C(2) , true);
      lbits zgsz3292;
      CREATE(lbits)(&zgsz3292);
      CONVERT_OF(lbits, fbits)(&zgsz3292, zcarry_out, UINT64_C(2) , true);
      lbits zgsz3293;
      CREATE(lbits)(&zgsz3293);
      add_bits(&zgsz3293, zgsz3291, zgsz3292);
      zTtop2 = CONVERT_OF(fbits, lbits)(zgsz3293, true);
      KILL(lbits)(&zgsz3293);
      KILL(lbits)(&zgsz3292);
      KILL(lbits)(&zgsz3291);
    }
  
  }
  struct zCapability zgaz3173;
  {
    uint64_t zgaz3129;
    {
      bool zgaz3125;
      {
        sail_int zgsz3295;
        CREATE(sail_int)(&zgsz3295);
        CONVERT_OF(sail_int, mach_int)(&zgsz3295, INT64_C(0));
        sail_int zgsz3294;
        CREATE(sail_int)(&zgsz3294);
        CONVERT_OF(sail_int, mach_int)(&zgsz3294, zcap_uperms_width);
        zgaz3125 = gt(zgsz3294, zgsz3295);
        KILL(sail_int)(&zgsz3295);
        KILL(sail_int)(&zgsz3294);
      }
      zgaz3129 = UINT64_C(0);
    
    }
    bool zgaz3132;
    {
      fbits zgaz3131;
      {
        uint64_t zgaz3130;
        zgaz3130 = zc.zperms;
        {
          sail_int zgsz3297;
          CREATE(sail_int)(&zgsz3297);
          CONVERT_OF(sail_int, mach_int)(&zgsz3297, INT64_C(11));
          lbits zgsz3296;
          CREATE(lbits)(&zgsz3296);
          CONVERT_OF(lbits, fbits)(&zgsz3296, zgaz3130, UINT64_C(12) , true);
          zgaz3131 = bitvector_access(zgsz3296, zgsz3297);
          KILL(sail_int)(&zgsz3297);
          KILL(lbits)(&zgsz3296);
        }
      
      }
      zgaz3132 = sailgen_bit_to_bool(zgaz3131);
    
    }
    bool zgaz3135;
    {
      fbits zgaz3134;
      {
        uint64_t zgaz3133;
        zgaz3133 = zc.zperms;
        {
          sail_int zgsz3299;
          CREATE(sail_int)(&zgsz3299);
          CONVERT_OF(sail_int, mach_int)(&zgsz3299, INT64_C(10));
          lbits zgsz3298;
          CREATE(lbits)(&zgsz3298);
          CONVERT_OF(lbits, fbits)(&zgsz3298, zgaz3133, UINT64_C(12) , true);
          zgaz3134 = bitvector_access(zgsz3298, zgsz3299);
          KILL(sail_int)(&zgsz3299);
          KILL(lbits)(&zgsz3298);
        }
      
      }
      zgaz3135 = sailgen_bit_to_bool(zgaz3134);
    
    }
    bool zgaz3138;
    {
      fbits zgaz3137;
      {
        uint64_t zgaz3136;
        zgaz3136 = zc.zperms;
        {
          sail_int zgsz3301;
          CREATE(sail_int)(&zgsz3301);
          CONVERT_OF(sail_int, mach_int)(&zgsz3301, INT64_C(9));
          lbits zgsz3300;
          CREATE(lbits)(&zgsz3300);
          CONVERT_OF(lbits, fbits)(&zgsz3300, zgaz3136, UINT64_C(12) , true);
          zgaz3137 = bitvector_access(zgsz3300, zgsz3301);
          KILL(sail_int)(&zgsz3301);
          KILL(lbits)(&zgsz3300);
        }
      
      }
      zgaz3138 = sailgen_bit_to_bool(zgaz3137);
    
    }
    bool zgaz3141;
    {
      fbits zgaz3140;
      {
        uint64_t zgaz3139;
        zgaz3139 = zc.zperms;
        {
          sail_int zgsz3303;
          CREATE(sail_int)(&zgsz3303);
          CONVERT_OF(sail_int, mach_int)(&zgsz3303, INT64_C(8));
          lbits zgsz3302;
          CREATE(lbits)(&zgsz3302);
          CONVERT_OF(lbits, fbits)(&zgsz3302, zgaz3139, UINT64_C(12) , true);
          zgaz3140 = bitvector_access(zgsz3302, zgsz3303);
          KILL(sail_int)(&zgsz3303);
          KILL(lbits)(&zgsz3302);
        }
      
      }
      zgaz3141 = sailgen_bit_to_bool(zgaz3140);
    
    }
    bool zgaz3144;
    {
      fbits zgaz3143;
      {
        uint64_t zgaz3142;
        zgaz3142 = zc.zperms;
        {
          sail_int zgsz3305;
          CREATE(sail_int)(&zgsz3305);
          CONVERT_OF(sail_int, mach_int)(&zgsz3305, INT64_C(7));
          lbits zgsz3304;
          CREATE(lbits)(&zgsz3304);
          CONVERT_OF(lbits, fbits)(&zgsz3304, zgaz3142, UINT64_C(12) , true);
          zgaz3143 = bitvector_access(zgsz3304, zgsz3305);
          KILL(sail_int)(&zgsz3305);
          KILL(lbits)(&zgsz3304);
        }
      
      }
      zgaz3144 = sailgen_bit_to_bool(zgaz3143);
    
    }
    bool zgaz3147;
    {
      fbits zgaz3146;
      {
        uint64_t zgaz3145;
        zgaz3145 = zc.zperms;
        {
          sail_int zgsz3307;
          CREATE(sail_int)(&zgsz3307);
          CONVERT_OF(sail_int, mach_int)(&zgsz3307, INT64_C(6));
          lbits zgsz3306;
          CREATE(lbits)(&zgsz3306);
          CONVERT_OF(lbits, fbits)(&zgsz3306, zgaz3145, UINT64_C(12) , true);
          zgaz3146 = bitvector_access(zgsz3306, zgsz3307);
          KILL(sail_int)(&zgsz3307);
          KILL(lbits)(&zgsz3306);
        }
      
      }
      zgaz3147 = sailgen_bit_to_bool(zgaz3146);
    
    }
    bool zgaz3150;
    {
      fbits zgaz3149;
      {
        uint64_t zgaz3148;
        zgaz3148 = zc.zperms;
        {
          sail_int zgsz3309;
          CREATE(sail_int)(&zgsz3309);
          CONVERT_OF(sail_int, mach_int)(&zgsz3309, INT64_C(5));
          lbits zgsz3308;
          CREATE(lbits)(&zgsz3308);
          CONVERT_OF(lbits, fbits)(&zgsz3308, zgaz3148, UINT64_C(12) , true);
          zgaz3149 = bitvector_access(zgsz3308, zgsz3309);
          KILL(sail_int)(&zgsz3309);
          KILL(lbits)(&zgsz3308);
        }
      
      }
      zgaz3150 = sailgen_bit_to_bool(zgaz3149);
    
    }
    bool zgaz3153;
    {
      fbits zgaz3152;
      {
        uint64_t zgaz3151;
        zgaz3151 = zc.zperms;
        {
          sail_int zgsz3311;
          CREATE(sail_int)(&zgsz3311);
          CONVERT_OF(sail_int, mach_int)(&zgsz3311, INT64_C(4));
          lbits zgsz3310;
          CREATE(lbits)(&zgsz3310);
          CONVERT_OF(lbits, fbits)(&zgsz3310, zgaz3151, UINT64_C(12) , true);
          zgaz3152 = bitvector_access(zgsz3310, zgsz3311);
          KILL(sail_int)(&zgsz3311);
          KILL(lbits)(&zgsz3310);
        }
      
      }
      zgaz3153 = sailgen_bit_to_bool(zgaz3152);
    
    }
    bool zgaz3156;
    {
      fbits zgaz3155;
      {
        uint64_t zgaz3154;
        zgaz3154 = zc.zperms;
        {
          sail_int zgsz3313;
          CREATE(sail_int)(&zgsz3313);
          CONVERT_OF(sail_int, mach_int)(&zgsz3313, INT64_C(3));
          lbits zgsz3312;
          CREATE(lbits)(&zgsz3312);
          CONVERT_OF(lbits, fbits)(&zgsz3312, zgaz3154, UINT64_C(12) , true);
          zgaz3155 = bitvector_access(zgsz3312, zgsz3313);
          KILL(sail_int)(&zgsz3313);
          KILL(lbits)(&zgsz3312);
        }
      
      }
      zgaz3156 = sailgen_bit_to_bool(zgaz3155);
    
    }
    bool zgaz3159;
    {
      fbits zgaz3158;
      {
        uint64_t zgaz3157;
        zgaz3157 = zc.zperms;
        {
          sail_int zgsz3315;
          CREATE(sail_int)(&zgsz3315);
          CONVERT_OF(sail_int, mach_int)(&zgsz3315, INT64_C(2));
          lbits zgsz3314;
          CREATE(lbits)(&zgsz3314);
          CONVERT_OF(lbits, fbits)(&zgsz3314, zgaz3157, UINT64_C(12) , true);
          zgaz3158 = bitvector_access(zgsz3314, zgsz3315);
          KILL(sail_int)(&zgsz3315);
          KILL(lbits)(&zgsz3314);
        }
      
      }
      zgaz3159 = sailgen_bit_to_bool(zgaz3158);
    
    }
    bool zgaz3162;
    {
      fbits zgaz3161;
      {
        uint64_t zgaz3160;
        zgaz3160 = zc.zperms;
        {
          sail_int zgsz3317;
          CREATE(sail_int)(&zgsz3317);
          CONVERT_OF(sail_int, mach_int)(&zgsz3317, INT64_C(1));
          lbits zgsz3316;
          CREATE(lbits)(&zgsz3316);
          CONVERT_OF(lbits, fbits)(&zgsz3316, zgaz3160, UINT64_C(12) , true);
          zgaz3161 = bitvector_access(zgsz3316, zgsz3317);
          KILL(sail_int)(&zgsz3317);
          KILL(lbits)(&zgsz3316);
        }
      
      }
      zgaz3162 = sailgen_bit_to_bool(zgaz3161);
    
    }
    bool zgaz3165;
    {
      fbits zgaz3164;
      {
        uint64_t zgaz3163;
        zgaz3163 = zc.zperms;
        {
          sail_int zgsz3319;
          CREATE(sail_int)(&zgsz3319);
          CONVERT_OF(sail_int, mach_int)(&zgsz3319, INT64_C(0));
          lbits zgsz3318;
          CREATE(lbits)(&zgsz3318);
          CONVERT_OF(lbits, fbits)(&zgsz3318, zgaz3163, UINT64_C(12) , true);
          zgaz3164 = bitvector_access(zgsz3318, zgsz3319);
          KILL(sail_int)(&zgsz3319);
          KILL(lbits)(&zgsz3318);
        }
      
      }
      zgaz3165 = sailgen_bit_to_bool(zgaz3164);
    
    }
    uint64_t zgaz3166;
    zgaz3166 = zc.zreserved;
    bool zgaz3169;
    {
      fbits zgaz3168;
      {
        uint64_t zgaz3167;
        zgaz3167 = zc.zflags;
        {
          sail_int zgsz3321;
          CREATE(sail_int)(&zgsz3321);
          CONVERT_OF(sail_int, mach_int)(&zgsz3321, INT64_C(0));
          lbits zgsz3320;
          CREATE(lbits)(&zgsz3320);
          CONVERT_OF(lbits, fbits)(&zgsz3320, zgaz3167, UINT64_C(1) , true);
          zgaz3168 = bitvector_access(zgsz3320, zgsz3321);
          KILL(sail_int)(&zgsz3321);
          KILL(lbits)(&zgsz3320);
        }
      
      }
      zgaz3169 = sailgen_bit_to_bool(zgaz3168);
    
    }
    uint64_t zgaz3170;
    {
      lbits zgsz3322;
      CREATE(lbits)(&zgsz3322);
      CONVERT_OF(lbits, fbits)(&zgsz3322, zTtop2, UINT64_C(2) , true);
      lbits zgsz3323;
      CREATE(lbits)(&zgsz3323);
      CONVERT_OF(lbits, fbits)(&zgsz3323, zT, UINT64_C(6) , true);
      lbits zgsz3324;
      CREATE(lbits)(&zgsz3324);
      append(&zgsz3324, zgsz3322, zgsz3323);
      zgaz3170 = CONVERT_OF(fbits, lbits)(zgsz3324, true);
      KILL(lbits)(&zgsz3324);
      KILL(lbits)(&zgsz3323);
      KILL(lbits)(&zgsz3322);
    }
    uint64_t zgaz3171;
    zgaz3171 = zc.zotype;
    uint64_t zgaz3172;
    zgaz3172 = zc.zaddress;
    struct zCapability zgsz3325;
    zgsz3325.zB = zBs;
    zgsz3325.zE = zE;
    zgsz3325.zT = zgaz3170;
    zgsz3325.zaccess_system_regs = zgaz3135;
    zgsz3325.zaddress = zgaz3172;
    zgsz3325.zflag_cap_mode = zgaz3169;
    zgsz3325.zglobal = zgaz3165;
    zgsz3325.zinternal_E = zinternal_E;
    zgsz3325.zotype = zgaz3171;
    zgsz3325.zpermit_cinvoke = zgaz3141;
    zgsz3325.zpermit_execute = zgaz3162;
    zgsz3325.zpermit_load = zgaz3159;
    zgsz3325.zpermit_load_cap = zgaz3153;
    zgsz3325.zpermit_seal = zgaz3144;
    zgsz3325.zpermit_set_CID = zgaz3132;
    zgsz3325.zpermit_store = zgaz3156;
    zgsz3325.zpermit_store_cap = zgaz3150;
    zgsz3325.zpermit_store_local_cap = zgaz3147;
    zgsz3325.zpermit_unseal = zgaz3138;
    zgsz3325.zreserved = zgaz3166;
    zgsz3325.ztag = zt;
    zgsz3325.zuperms = zgaz3129;
    zgaz3173 = zgsz3325;
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  
  }
  zcbz336 = zgaz3173;
  goto cleanup_146;
  /* unreachable after return */








  goto end_cleanup_147;
cleanup_146: ;








  goto end_function_145;
end_cleanup_147: ;
end_function_145: ;
  return zcbz336;
end_block_exception_148: ;
  struct zCapability zcbz361 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz361;
}

static struct zCapability sailgen_capBitsToCapability(bool, uint64_t);

static struct zCapability sailgen_capBitsToCapability(bool zt, uint64_t zc)
{
  __label__ end_function_150, end_block_exception_151;

  struct zCapability zcbz337;
  struct zEncCapability zgaz3183;
  zgaz3183 = sailgen_capBitsToEncCapability(zc);
  zcbz337 = sailgen_encCapabilityToCapability(zt, zgaz3183);

end_function_150: ;
  return zcbz337;
end_block_exception_151: ;
  struct zCapability zcbz362 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz362;
}

static struct zEncCapability sailgen_capToEncCap(struct zCapability);

static struct zEncCapability sailgen_capToEncCap(struct zCapability zcap)
{
  __label__ cleanup_154, end_cleanup_155, end_function_153, end_block_exception_156;

  struct zEncCapability zcbz338;
  uint64_t zt_hi;
  {
    uint64_t zgaz3210;
    zgaz3210 = zcap.zT;
    int64_t zgaz3211;
    {
      sail_int zgsz3326;
      CREATE(sail_int)(&zgsz3326);
      CONVERT_OF(sail_int, mach_int)(&zgsz3326, zcap_mantissa_width);
      sail_int zgsz3327;
      CREATE(sail_int)(&zgsz3327);
      CONVERT_OF(sail_int, mach_int)(&zgsz3327, INT64_C(3));
      sail_int zgsz3328;
      CREATE(sail_int)(&zgsz3328);
      sub_int(&zgsz3328, zgsz3326, zgsz3327);
      zgaz3211 = CONVERT_OF(mach_int, sail_int)(zgsz3328);
      KILL(sail_int)(&zgsz3328);
      KILL(sail_int)(&zgsz3327);
      KILL(sail_int)(&zgsz3326);
    }
    {
      lbits zgsz3329;
      CREATE(lbits)(&zgsz3329);
      CONVERT_OF(lbits, fbits)(&zgsz3329, zgaz3210, UINT64_C(8) , true);
      sail_int zgsz3330;
      CREATE(sail_int)(&zgsz3330);
      CONVERT_OF(sail_int, mach_int)(&zgsz3330, zgaz3211);
      sail_int zgsz3331;
      CREATE(sail_int)(&zgsz3331);
      CONVERT_OF(sail_int, mach_int)(&zgsz3331, zinternal_E_take_bits);
      lbits zgsz3332;
      CREATE(lbits)(&zgsz3332);
      vector_subrange_lbits(&zgsz3332, zgsz3329, zgsz3330, zgsz3331);
      zt_hi = CONVERT_OF(fbits, lbits)(zgsz3332, true);
      KILL(lbits)(&zgsz3332);
      KILL(sail_int)(&zgsz3331);
      KILL(sail_int)(&zgsz3330);
      KILL(lbits)(&zgsz3329);
    }
  
  
  }
  uint64_t zt_lo;
  {
    uint64_t zgaz3208;
    zgaz3208 = zcap.zT;
    int64_t zgaz3209;
    {
      sail_int zgsz3333;
      CREATE(sail_int)(&zgsz3333);
      CONVERT_OF(sail_int, mach_int)(&zgsz3333, zinternal_E_take_bits);
      sail_int zgsz3334;
      CREATE(sail_int)(&zgsz3334);
      CONVERT_OF(sail_int, mach_int)(&zgsz3334, INT64_C(1));
      sail_int zgsz3335;
      CREATE(sail_int)(&zgsz3335);
      sub_int(&zgsz3335, zgsz3333, zgsz3334);
      zgaz3209 = CONVERT_OF(mach_int, sail_int)(zgsz3335);
      KILL(sail_int)(&zgsz3335);
      KILL(sail_int)(&zgsz3334);
      KILL(sail_int)(&zgsz3333);
    }
    {
      lbits zgsz3336;
      CREATE(lbits)(&zgsz3336);
      CONVERT_OF(lbits, fbits)(&zgsz3336, zgaz3208, UINT64_C(8) , true);
      sail_int zgsz3337;
      CREATE(sail_int)(&zgsz3337);
      CONVERT_OF(sail_int, mach_int)(&zgsz3337, zgaz3209);
      sail_int zgsz3338;
      CREATE(sail_int)(&zgsz3338);
      CONVERT_OF(sail_int, mach_int)(&zgsz3338, INT64_C(0));
      lbits zgsz3339;
      CREATE(lbits)(&zgsz3339);
      vector_subrange_lbits(&zgsz3339, zgsz3336, zgsz3337, zgsz3338);
      zt_lo = CONVERT_OF(fbits, lbits)(zgsz3339, true);
      KILL(lbits)(&zgsz3339);
      KILL(sail_int)(&zgsz3338);
      KILL(sail_int)(&zgsz3337);
      KILL(lbits)(&zgsz3336);
    }
  
  
  }
  uint64_t zb_hi;
  {
    uint64_t zgaz3206;
    zgaz3206 = zcap.zB;
    int64_t zgaz3207;
    {
      sail_int zgsz3340;
      CREATE(sail_int)(&zgsz3340);
      CONVERT_OF(sail_int, mach_int)(&zgsz3340, zcap_mantissa_width);
      sail_int zgsz3341;
      CREATE(sail_int)(&zgsz3341);
      CONVERT_OF(sail_int, mach_int)(&zgsz3341, INT64_C(1));
      sail_int zgsz3342;
      CREATE(sail_int)(&zgsz3342);
      sub_int(&zgsz3342, zgsz3340, zgsz3341);
      zgaz3207 = CONVERT_OF(mach_int, sail_int)(zgsz3342);
      KILL(sail_int)(&zgsz3342);
      KILL(sail_int)(&zgsz3341);
      KILL(sail_int)(&zgsz3340);
    }
    {
      lbits zgsz3343;
      CREATE(lbits)(&zgsz3343);
      CONVERT_OF(lbits, fbits)(&zgsz3343, zgaz3206, UINT64_C(8) , true);
      sail_int zgsz3344;
      CREATE(sail_int)(&zgsz3344);
      CONVERT_OF(sail_int, mach_int)(&zgsz3344, zgaz3207);
      sail_int zgsz3345;
      CREATE(sail_int)(&zgsz3345);
      CONVERT_OF(sail_int, mach_int)(&zgsz3345, zinternal_E_take_bits);
      lbits zgsz3346;
      CREATE(lbits)(&zgsz3346);
      vector_subrange_lbits(&zgsz3346, zgsz3343, zgsz3344, zgsz3345);
      zb_hi = CONVERT_OF(fbits, lbits)(zgsz3346, true);
      KILL(lbits)(&zgsz3346);
      KILL(sail_int)(&zgsz3345);
      KILL(sail_int)(&zgsz3344);
      KILL(lbits)(&zgsz3343);
    }
  
  
  }
  uint64_t zb_lo;
  {
    uint64_t zgaz3204;
    zgaz3204 = zcap.zB;
    int64_t zgaz3205;
    {
      sail_int zgsz3347;
      CREATE(sail_int)(&zgsz3347);
      CONVERT_OF(sail_int, mach_int)(&zgsz3347, zinternal_E_take_bits);
      sail_int zgsz3348;
      CREATE(sail_int)(&zgsz3348);
      CONVERT_OF(sail_int, mach_int)(&zgsz3348, INT64_C(1));
      sail_int zgsz3349;
      CREATE(sail_int)(&zgsz3349);
      sub_int(&zgsz3349, zgsz3347, zgsz3348);
      zgaz3205 = CONVERT_OF(mach_int, sail_int)(zgsz3349);
      KILL(sail_int)(&zgsz3349);
      KILL(sail_int)(&zgsz3348);
      KILL(sail_int)(&zgsz3347);
    }
    {
      lbits zgsz3350;
      CREATE(lbits)(&zgsz3350);
      CONVERT_OF(lbits, fbits)(&zgsz3350, zgaz3204, UINT64_C(8) , true);
      sail_int zgsz3351;
      CREATE(sail_int)(&zgsz3351);
      CONVERT_OF(sail_int, mach_int)(&zgsz3351, zgaz3205);
      sail_int zgsz3352;
      CREATE(sail_int)(&zgsz3352);
      CONVERT_OF(sail_int, mach_int)(&zgsz3352, INT64_C(0));
      lbits zgsz3353;
      CREATE(lbits)(&zgsz3353);
      vector_subrange_lbits(&zgsz3353, zgsz3350, zgsz3351, zgsz3352);
      zb_lo = CONVERT_OF(fbits, lbits)(zgsz3353, true);
      KILL(lbits)(&zgsz3353);
      KILL(sail_int)(&zgsz3352);
      KILL(sail_int)(&zgsz3351);
      KILL(lbits)(&zgsz3350);
    }
  
  
  }
  {
    bool zgaz3184;
    zgaz3184 = zcap.zinternal_E;
    unit zgsz3354;
    if (zgaz3184) {
    {
      uint64_t zgaz3186;
      zgaz3186 = zcap.zE;
      int64_t zgaz3187;
      {
        int64_t zgaz3185;
        {
          sail_int zgsz3355;
          CREATE(sail_int)(&zgsz3355);
          CONVERT_OF(sail_int, mach_int)(&zgsz3355, INT64_C(2));
          sail_int zgsz3356;
          CREATE(sail_int)(&zgsz3356);
          CONVERT_OF(sail_int, mach_int)(&zgsz3356, zinternal_E_take_bits);
          sail_int zgsz3357;
          CREATE(sail_int)(&zgsz3357);
          mult_int(&zgsz3357, zgsz3355, zgsz3356);
          zgaz3185 = CONVERT_OF(mach_int, sail_int)(zgsz3357);
          KILL(sail_int)(&zgsz3357);
          KILL(sail_int)(&zgsz3356);
          KILL(sail_int)(&zgsz3355);
        }
        {
          sail_int zgsz3358;
          CREATE(sail_int)(&zgsz3358);
          CONVERT_OF(sail_int, mach_int)(&zgsz3358, zgaz3185);
          sail_int zgsz3359;
          CREATE(sail_int)(&zgsz3359);
          CONVERT_OF(sail_int, mach_int)(&zgsz3359, INT64_C(1));
          sail_int zgsz3360;
          CREATE(sail_int)(&zgsz3360);
          sub_int(&zgsz3360, zgsz3358, zgsz3359);
          zgaz3187 = CONVERT_OF(mach_int, sail_int)(zgsz3360);
          KILL(sail_int)(&zgsz3360);
          KILL(sail_int)(&zgsz3359);
          KILL(sail_int)(&zgsz3358);
        }
      
      }
      {
        lbits zgsz3361;
        CREATE(lbits)(&zgsz3361);
        CONVERT_OF(lbits, fbits)(&zgsz3361, zgaz3186, UINT64_C(6) , true);
        sail_int zgsz3362;
        CREATE(sail_int)(&zgsz3362);
        CONVERT_OF(sail_int, mach_int)(&zgsz3362, zgaz3187);
        sail_int zgsz3363;
        CREATE(sail_int)(&zgsz3363);
        CONVERT_OF(sail_int, mach_int)(&zgsz3363, zinternal_E_take_bits);
        lbits zgsz3364;
        CREATE(lbits)(&zgsz3364);
        vector_subrange_lbits(&zgsz3364, zgsz3361, zgsz3362, zgsz3363);
        zt_lo = CONVERT_OF(fbits, lbits)(zgsz3364, true);
        KILL(lbits)(&zgsz3364);
        KILL(sail_int)(&zgsz3363);
        KILL(sail_int)(&zgsz3362);
        KILL(lbits)(&zgsz3361);
      }
      unit zgsz3365;
      zgsz3365 = UNIT;
    
    
    }
    uint64_t zgaz3188;
    zgaz3188 = zcap.zE;
    int64_t zgaz3189;
    {
      sail_int zgsz3366;
      CREATE(sail_int)(&zgsz3366);
      CONVERT_OF(sail_int, mach_int)(&zgsz3366, zinternal_E_take_bits);
      sail_int zgsz3367;
      CREATE(sail_int)(&zgsz3367);
      CONVERT_OF(sail_int, mach_int)(&zgsz3367, INT64_C(1));
      sail_int zgsz3368;
      CREATE(sail_int)(&zgsz3368);
      sub_int(&zgsz3368, zgsz3366, zgsz3367);
      zgaz3189 = CONVERT_OF(mach_int, sail_int)(zgsz3368);
      KILL(sail_int)(&zgsz3368);
      KILL(sail_int)(&zgsz3367);
      KILL(sail_int)(&zgsz3366);
    }
    {
      lbits zgsz3369;
      CREATE(lbits)(&zgsz3369);
      CONVERT_OF(lbits, fbits)(&zgsz3369, zgaz3188, UINT64_C(6) , true);
      sail_int zgsz3370;
      CREATE(sail_int)(&zgsz3370);
      CONVERT_OF(sail_int, mach_int)(&zgsz3370, zgaz3189);
      sail_int zgsz3371;
      CREATE(sail_int)(&zgsz3371);
      CONVERT_OF(sail_int, mach_int)(&zgsz3371, INT64_C(0));
      lbits zgsz3372;
      CREATE(lbits)(&zgsz3372);
      vector_subrange_lbits(&zgsz3372, zgsz3369, zgsz3370, zgsz3371);
      zb_lo = CONVERT_OF(fbits, lbits)(zgsz3372, true);
      KILL(lbits)(&zgsz3372);
      KILL(sail_int)(&zgsz3371);
      KILL(sail_int)(&zgsz3370);
      KILL(lbits)(&zgsz3369);
    }
    zgsz3354 = UNIT;
  
  
    } else {  zgsz3354 = UNIT;  }
  
  }
  struct zEncCapability zgaz3203;
  {
    uint64_t zgaz3192;
    {
      uint64_t zgaz3190;
      zgaz3190 = zcap.zuperms;
      uint64_t zgaz3191;
      zgaz3191 = sailgen_getCapHardPerms(zcap);
      {
        lbits zgsz3373;
        CREATE(lbits)(&zgsz3373);
        CONVERT_OF(lbits, fbits)(&zgsz3373, zgaz3190, UINT64_C(0) , true);
        lbits zgsz3374;
        CREATE(lbits)(&zgsz3374);
        CONVERT_OF(lbits, fbits)(&zgsz3374, zgaz3191, UINT64_C(12) , true);
        lbits zgsz3375;
        CREATE(lbits)(&zgsz3375);
        append(&zgsz3375, zgsz3373, zgsz3374);
        zgaz3192 = CONVERT_OF(fbits, lbits)(zgsz3375, true);
        KILL(lbits)(&zgsz3375);
        KILL(lbits)(&zgsz3374);
        KILL(lbits)(&zgsz3373);
      }
    
    
    }
    uint64_t zgaz3193;
    zgaz3193 = zcap.zreserved;
    uint64_t zgaz3195;
    {
      bool zgaz3194;
      zgaz3194 = zcap.zflag_cap_mode;
      zgaz3195 = sailgen_bool_to_bits(zgaz3194);
    
    }
    uint64_t zgaz3196;
    zgaz3196 = zcap.zotype;
    fbits zgaz3199;
    {
      uint64_t zgaz3198;
      {
        bool zgaz3197;
        zgaz3197 = zcap.zinternal_E;
        zgaz3198 = sailgen_bool_to_bits(zgaz3197);
      
      }
      {
        sail_int zgsz3377;
        CREATE(sail_int)(&zgsz3377);
        CONVERT_OF(sail_int, mach_int)(&zgsz3377, INT64_C(0));
        lbits zgsz3376;
        CREATE(lbits)(&zgsz3376);
        CONVERT_OF(lbits, fbits)(&zgsz3376, zgaz3198, UINT64_C(1) , true);
        zgaz3199 = bitvector_access(zgsz3376, zgsz3377);
        KILL(sail_int)(&zgsz3377);
        KILL(lbits)(&zgsz3376);
      }
    
    }
    uint64_t zgaz3200;
    {
      lbits zgsz3378;
      CREATE(lbits)(&zgsz3378);
      CONVERT_OF(lbits, fbits)(&zgsz3378, zt_hi, UINT64_C(3) , true);
      lbits zgsz3379;
      CREATE(lbits)(&zgsz3379);
      CONVERT_OF(lbits, fbits)(&zgsz3379, zt_lo, UINT64_C(3) , true);
      lbits zgsz3380;
      CREATE(lbits)(&zgsz3380);
      append(&zgsz3380, zgsz3378, zgsz3379);
      zgaz3200 = CONVERT_OF(fbits, lbits)(zgsz3380, true);
      KILL(lbits)(&zgsz3380);
      KILL(lbits)(&zgsz3379);
      KILL(lbits)(&zgsz3378);
    }
    uint64_t zgaz3201;
    {
      lbits zgsz3381;
      CREATE(lbits)(&zgsz3381);
      CONVERT_OF(lbits, fbits)(&zgsz3381, zb_hi, UINT64_C(5) , true);
      lbits zgsz3382;
      CREATE(lbits)(&zgsz3382);
      CONVERT_OF(lbits, fbits)(&zgsz3382, zb_lo, UINT64_C(3) , true);
      lbits zgsz3383;
      CREATE(lbits)(&zgsz3383);
      append(&zgsz3383, zgsz3381, zgsz3382);
      zgaz3201 = CONVERT_OF(fbits, lbits)(zgsz3383, true);
      KILL(lbits)(&zgsz3383);
      KILL(lbits)(&zgsz3382);
      KILL(lbits)(&zgsz3381);
    }
    uint64_t zgaz3202;
    zgaz3202 = zcap.zaddress;
    struct zEncCapability zgsz3384;
    zgsz3384.zB = zgaz3201;
    zgsz3384.zT = zgaz3200;
    zgsz3384.zaddress = zgaz3202;
    zgsz3384.zflags = zgaz3195;
    zgsz3384.zinternal_E = zgaz3199;
    zgsz3384.zotype = zgaz3196;
    zgsz3384.zperms = zgaz3192;
    zgsz3384.zreserved = zgaz3193;
    zgaz3203 = zgsz3384;
  
  
  
  
  
  
  
  
  
  }
  zcbz338 = zgaz3203;
  goto cleanup_154;
  /* unreachable after return */





  goto end_cleanup_155;
cleanup_154: ;





  goto end_function_153;
end_cleanup_155: ;
end_function_153: ;
  return zcbz338;
end_block_exception_156: ;
  struct zEncCapability zcbz363 = { .zreserved = UINT64_C(0xdeadc0de), .zperms = UINT64_C(0xdeadc0de), .zotype = UINT64_C(0xdeadc0de), .zinternal_E = UINT64_C(0), .zflags = UINT64_C(0xdeadc0de), .zaddress = UINT64_C(0xdeadc0de), .zT = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz363;
}

static uint64_t sailgen_capToBits(struct zCapability);

static uint64_t sailgen_capToBits(struct zCapability zcap)
{
  __label__ end_function_158, end_block_exception_159;

  uint64_t zcbz339;
  struct zEncCapability zgaz3212;
  zgaz3212 = sailgen_capToEncCap(zcap);
  zcbz339 = sailgen_encCapToBits(zgaz3212);

end_function_158: ;
  return zcbz339;
end_block_exception_159: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t znull_cap_bits;
static void create_letbind_27(void) {  


  uint64_t zgsz3385;
  zgsz3385 = sailgen_capToBits(znull_cap);
  znull_cap_bits = zgsz3385;

let_end_160: ;
}
static void kill_letbind_27(void) {  
}

static uint64_t sailgen_capToMemBits(struct zCapability);

static uint64_t sailgen_capToMemBits(struct zCapability zcap)
{
  __label__ end_function_162, end_block_exception_163;

  uint64_t zcbz340;
  uint64_t zgaz3213;
  zgaz3213 = sailgen_capToBits(zcap);
  {
    lbits zgsz3386;
    CREATE(lbits)(&zgsz3386);
    CONVERT_OF(lbits, fbits)(&zgsz3386, zgaz3213, UINT64_C(64) , true);
    lbits zgsz3387;
    CREATE(lbits)(&zgsz3387);
    CONVERT_OF(lbits, fbits)(&zgsz3387, znull_cap_bits, UINT64_C(64) , true);
    lbits zgsz3388;
    CREATE(lbits)(&zgsz3388);
    xor_bits(&zgsz3388, zgsz3386, zgsz3387);
    zcbz340 = CONVERT_OF(fbits, lbits)(zgsz3388, true);
    KILL(lbits)(&zgsz3388);
    KILL(lbits)(&zgsz3387);
    KILL(lbits)(&zgsz3386);
  }

end_function_162: ;
  return zcbz340;
end_block_exception_163: ;

  return UINT64_C(0xdeadc0de);
}

static struct zCapability sailgen_memBitsToCapability(bool, uint64_t);

static struct zCapability sailgen_memBitsToCapability(bool ztag, uint64_t zb)
{
  __label__ end_function_165, end_block_exception_166;

  struct zCapability zcbz341;
  uint64_t zgaz3214;
  {
    lbits zgsz3389;
    CREATE(lbits)(&zgsz3389);
    CONVERT_OF(lbits, fbits)(&zgsz3389, zb, UINT64_C(64) , true);
    lbits zgsz3390;
    CREATE(lbits)(&zgsz3390);
    CONVERT_OF(lbits, fbits)(&zgsz3390, znull_cap_bits, UINT64_C(64) , true);
    lbits zgsz3391;
    CREATE(lbits)(&zgsz3391);
    xor_bits(&zgsz3391, zgsz3389, zgsz3390);
    zgaz3214 = CONVERT_OF(fbits, lbits)(zgsz3391, true);
    KILL(lbits)(&zgsz3391);
    KILL(lbits)(&zgsz3390);
    KILL(lbits)(&zgsz3389);
  }
  zcbz341 = sailgen_capBitsToCapability(ztag, zgaz3214);

end_function_165: ;
  return zcbz341;
end_block_exception_166: ;
  struct zCapability zcbz364 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz364;
}

// struct tuple_(%bv32, %bv33)
struct ztuple_z8z5bv32zCz0z5bv33z9 {
  uint64_t ztup0;
  uint64_t ztup1;
};

static void COPY(ztuple_z8z5bv32zCz0z5bv33z9)(struct ztuple_z8z5bv32zCz0z5bv33z9 *rop, const struct ztuple_z8z5bv32zCz0z5bv33z9 op) {
  rop->ztup0 = op.ztup0;
  rop->ztup1 = op.ztup1;
}

static bool EQUAL(ztuple_z8z5bv32zCz0z5bv33z9)(struct ztuple_z8z5bv32zCz0z5bv33z9 op1, struct ztuple_z8z5bv32zCz0z5bv33z9 op2) {
  return EQUAL(fbits)(op1.ztup0, op2.ztup0) && EQUAL(fbits)(op1.ztup1, op2.ztup1);
}

static struct ztuple_z8z5bv32zCz0z5bv33z9 sailgen_getCapBoundsBits(struct zCapability);

static struct ztuple_z8z5bv32zCz0z5bv33z9 sailgen_getCapBoundsBits(struct zCapability zc)
{
  __label__ end_function_168, end_block_exception_169;

  struct ztuple_z8z5bv32zCz0z5bv33z9 zcbz342;
  int64_t zE;
  {
    int64_t zgaz3248;
    {
      uint64_t zgaz3247;
      zgaz3247 = zc.zE;
      zgaz3248 = ((mach_int) zgaz3247);
    
    }
    {
      sail_int zgsz3392;
      CREATE(sail_int)(&zgsz3392);
      CONVERT_OF(sail_int, mach_int)(&zgsz3392, zcap_max_E);
      sail_int zgsz3393;
      CREATE(sail_int)(&zgsz3393);
      CONVERT_OF(sail_int, mach_int)(&zgsz3393, zgaz3248);
      sail_int zgsz3394;
      CREATE(sail_int)(&zgsz3394);
      min_int(&zgsz3394, zgsz3392, zgsz3393);
      zE = CONVERT_OF(mach_int, sail_int)(zgsz3394);
      KILL(sail_int)(&zgsz3394);
      KILL(sail_int)(&zgsz3393);
      KILL(sail_int)(&zgsz3392);
    }
  
  }
  uint64_t za;
  za = zc.zaddress;
  uint64_t za3;
  {
    uint64_t zgaz3246;
    {
      int64_t zgaz3245;
      {
        int64_t zgaz3244;
        {
          sail_int zgsz3395;
          CREATE(sail_int)(&zgsz3395);
          CONVERT_OF(sail_int, mach_int)(&zgsz3395, zE);
          sail_int zgsz3396;
          CREATE(sail_int)(&zgsz3396);
          CONVERT_OF(sail_int, mach_int)(&zgsz3396, zcap_mantissa_width);
          sail_int zgsz3397;
          CREATE(sail_int)(&zgsz3397);
          add_int(&zgsz3397, zgsz3395, zgsz3396);
          zgaz3244 = CONVERT_OF(mach_int, sail_int)(zgsz3397);
          KILL(sail_int)(&zgsz3397);
          KILL(sail_int)(&zgsz3396);
          KILL(sail_int)(&zgsz3395);
        }
        {
          sail_int zgsz3398;
          CREATE(sail_int)(&zgsz3398);
          CONVERT_OF(sail_int, mach_int)(&zgsz3398, zgaz3244);
          sail_int zgsz3399;
          CREATE(sail_int)(&zgsz3399);
          CONVERT_OF(sail_int, mach_int)(&zgsz3399, INT64_C(3));
          sail_int zgsz3400;
          CREATE(sail_int)(&zgsz3400);
          sub_int(&zgsz3400, zgsz3398, zgsz3399);
          zgaz3245 = CONVERT_OF(mach_int, sail_int)(zgsz3400);
          KILL(sail_int)(&zgsz3400);
          KILL(sail_int)(&zgsz3399);
          KILL(sail_int)(&zgsz3398);
        }
      
      }
      {
        lbits zgsz3401;
        CREATE(lbits)(&zgsz3401);
        CONVERT_OF(lbits, fbits)(&zgsz3401, za, UINT64_C(32) , true);
        sail_int zgsz3402;
        CREATE(sail_int)(&zgsz3402);
        CONVERT_OF(sail_int, mach_int)(&zgsz3402, zgaz3245);
        lbits zgsz3403;
        CREATE(lbits)(&zgsz3403);
        shiftr(&zgsz3403, zgsz3401, zgsz3402);
        zgaz3246 = CONVERT_OF(fbits, lbits)(zgsz3403, true);
        KILL(lbits)(&zgsz3403);
        KILL(sail_int)(&zgsz3402);
        KILL(lbits)(&zgsz3401);
      }
    
    }
    {
      lbits zgsz3404;
      CREATE(lbits)(&zgsz3404);
      CONVERT_OF(lbits, fbits)(&zgsz3404, zgaz3246, UINT64_C(32) , true);
      sail_int zgsz3405;
      CREATE(sail_int)(&zgsz3405);
      CONVERT_OF(sail_int, mach_int)(&zgsz3405, INT64_C(3));
      lbits zgsz3406;
      CREATE(lbits)(&zgsz3406);
      sail_truncate(&zgsz3406, zgsz3404, zgsz3405);
      za3 = CONVERT_OF(fbits, lbits)(zgsz3406, true);
      KILL(lbits)(&zgsz3406);
      KILL(sail_int)(&zgsz3405);
      KILL(lbits)(&zgsz3404);
    }
  
  }
  uint64_t zB3;
  {
    uint64_t zgaz3243;
    zgaz3243 = zc.zB;
    {
      lbits zgsz3407;
      CREATE(lbits)(&zgsz3407);
      CONVERT_OF(lbits, fbits)(&zgsz3407, zgaz3243, UINT64_C(8) , true);
      sail_int zgsz3408;
      CREATE(sail_int)(&zgsz3408);
      CONVERT_OF(sail_int, mach_int)(&zgsz3408, INT64_C(3));
      lbits zgsz3409;
      CREATE(lbits)(&zgsz3409);
      sail_truncateLSB(&zgsz3409, zgsz3407, zgsz3408);
      zB3 = CONVERT_OF(fbits, lbits)(zgsz3409, true);
      KILL(lbits)(&zgsz3409);
      KILL(sail_int)(&zgsz3408);
      KILL(lbits)(&zgsz3407);
    }
  
  }
  uint64_t zT3;
  {
    uint64_t zgaz3242;
    zgaz3242 = zc.zT;
    {
      lbits zgsz3410;
      CREATE(lbits)(&zgsz3410);
      CONVERT_OF(lbits, fbits)(&zgsz3410, zgaz3242, UINT64_C(8) , true);
      sail_int zgsz3411;
      CREATE(sail_int)(&zgsz3411);
      CONVERT_OF(sail_int, mach_int)(&zgsz3411, INT64_C(3));
      lbits zgsz3412;
      CREATE(lbits)(&zgsz3412);
      sail_truncateLSB(&zgsz3412, zgsz3410, zgsz3411);
      zT3 = CONVERT_OF(fbits, lbits)(zgsz3412, true);
      KILL(lbits)(&zgsz3412);
      KILL(sail_int)(&zgsz3411);
      KILL(lbits)(&zgsz3410);
    }
  
  }
  uint64_t zR3;
  {
    lbits zgsz3413;
    CREATE(lbits)(&zgsz3413);
    CONVERT_OF(lbits, fbits)(&zgsz3413, zB3, UINT64_C(3) , true);
    lbits zgsz3414;
    CREATE(lbits)(&zgsz3414);
    CONVERT_OF(lbits, fbits)(&zgsz3414, UINT64_C(0b001), UINT64_C(3) , true);
    lbits zgsz3415;
    CREATE(lbits)(&zgsz3415);
    sub_bits(&zgsz3415, zgsz3413, zgsz3414);
    zR3 = CONVERT_OF(fbits, lbits)(zgsz3415, true);
    KILL(lbits)(&zgsz3415);
    KILL(lbits)(&zgsz3414);
    KILL(lbits)(&zgsz3413);
  }
  int64_t zaHi;
  {
    bool zgaz3241;
    zgaz3241 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(za3, zR3);
    if (zgaz3241) {  zaHi = INT64_C(1);  } else {  zaHi = INT64_C(0);  }
  
  }
  int64_t zbHi;
  {
    bool zgaz3240;
    zgaz3240 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(zB3, zR3);
    if (zgaz3240) {  zbHi = INT64_C(1);  } else {  zbHi = INT64_C(0);  }
  
  }
  int64_t ztHi;
  {
    bool zgaz3239;
    zgaz3239 = sailgen_z8operatorz0zzzz7nzzJzzK3z3zI_uz9(zT3, zR3);
    if (zgaz3239) {  ztHi = INT64_C(1);  } else {  ztHi = INT64_C(0);  }
  
  }
  int64_t zcorrection_base;
  {
    sail_int zgsz3416;
    CREATE(sail_int)(&zgsz3416);
    CONVERT_OF(sail_int, mach_int)(&zgsz3416, zbHi);
    sail_int zgsz3417;
    CREATE(sail_int)(&zgsz3417);
    CONVERT_OF(sail_int, mach_int)(&zgsz3417, zaHi);
    sail_int zgsz3418;
    CREATE(sail_int)(&zgsz3418);
    sub_int(&zgsz3418, zgsz3416, zgsz3417);
    zcorrection_base = CONVERT_OF(mach_int, sail_int)(zgsz3418);
    KILL(sail_int)(&zgsz3418);
    KILL(sail_int)(&zgsz3417);
    KILL(sail_int)(&zgsz3416);
  }
  int64_t zcorrection_top;
  {
    sail_int zgsz3419;
    CREATE(sail_int)(&zgsz3419);
    CONVERT_OF(sail_int, mach_int)(&zgsz3419, ztHi);
    sail_int zgsz3420;
    CREATE(sail_int)(&zgsz3420);
    CONVERT_OF(sail_int, mach_int)(&zgsz3420, zaHi);
    sail_int zgsz3421;
    CREATE(sail_int)(&zgsz3421);
    sub_int(&zgsz3421, zgsz3419, zgsz3420);
    zcorrection_top = CONVERT_OF(mach_int, sail_int)(zgsz3421);
    KILL(sail_int)(&zgsz3421);
    KILL(sail_int)(&zgsz3420);
    KILL(sail_int)(&zgsz3419);
  }
  uint64_t za_top;
  {
    int64_t zgaz3238;
    {
      sail_int zgsz3422;
      CREATE(sail_int)(&zgsz3422);
      CONVERT_OF(sail_int, mach_int)(&zgsz3422, zE);
      sail_int zgsz3423;
      CREATE(sail_int)(&zgsz3423);
      CONVERT_OF(sail_int, mach_int)(&zgsz3423, zcap_mantissa_width);
      sail_int zgsz3424;
      CREATE(sail_int)(&zgsz3424);
      add_int(&zgsz3424, zgsz3422, zgsz3423);
      zgaz3238 = CONVERT_OF(mach_int, sail_int)(zgsz3424);
      KILL(sail_int)(&zgsz3424);
      KILL(sail_int)(&zgsz3423);
      KILL(sail_int)(&zgsz3422);
    }
    {
      lbits zgsz3425;
      CREATE(lbits)(&zgsz3425);
      CONVERT_OF(lbits, fbits)(&zgsz3425, za, UINT64_C(32) , true);
      sail_int zgsz3426;
      CREATE(sail_int)(&zgsz3426);
      CONVERT_OF(sail_int, mach_int)(&zgsz3426, zgaz3238);
      lbits zgsz3427;
      CREATE(lbits)(&zgsz3427);
      shiftr(&zgsz3427, zgsz3425, zgsz3426);
      za_top = CONVERT_OF(fbits, lbits)(zgsz3427, true);
      KILL(lbits)(&zgsz3427);
      KILL(sail_int)(&zgsz3426);
      KILL(lbits)(&zgsz3425);
    }
  
  }
  uint64_t zbase;
  {
    lbits zgaz3237;
    CREATE(lbits)(&zgaz3237);
    {
      uint64_t zgaz3235;
      {
        lbits zgsz3428;
        CREATE(lbits)(&zgsz3428);
        CONVERT_OF(lbits, fbits)(&zgsz3428, za_top, UINT64_C(32) , true);
        sail_int zgsz3429;
        CREATE(sail_int)(&zgsz3429);
        CONVERT_OF(sail_int, mach_int)(&zgsz3429, zcorrection_base);
        lbits zgsz3430;
        CREATE(lbits)(&zgsz3430);
        add_bits_int(&zgsz3430, zgsz3428, zgsz3429);
        zgaz3235 = CONVERT_OF(fbits, lbits)(zgsz3430, true);
        KILL(lbits)(&zgsz3430);
        KILL(sail_int)(&zgsz3429);
        KILL(lbits)(&zgsz3428);
      }
      sbits zgaz3236;
      {
        uint64_t zgaz3233;
        zgaz3233 = zc.zB;
        sbits zgaz3234;
        {
          sail_int zgsz3431;
          CREATE(sail_int)(&zgsz3431);
          CONVERT_OF(sail_int, mach_int)(&zgsz3431, zE);
          lbits zgsz3432;
          CREATE(lbits)(&zgsz3432);
          sailgen_zzz3zzz3zzeros_implicit(&zgsz3432, zgsz3431);
          zgaz3234 = CONVERT_OF(sbits, lbits)(zgsz3432, true);
          KILL(lbits)(&zgsz3432);
          KILL(sail_int)(&zgsz3431);
        }
        {
          lbits zgsz3433;
          CREATE(lbits)(&zgsz3433);
          CONVERT_OF(lbits, fbits)(&zgsz3433, zgaz3233, UINT64_C(8) , true);
          lbits zgsz3434;
          CREATE(lbits)(&zgsz3434);
          CONVERT_OF(lbits, sbits)(&zgsz3434, zgaz3234, true);
          lbits zgsz3435;
          CREATE(lbits)(&zgsz3435);
          append(&zgsz3435, zgsz3433, zgsz3434);
          zgaz3236 = CONVERT_OF(sbits, lbits)(zgsz3435, true);
          KILL(lbits)(&zgsz3435);
          KILL(lbits)(&zgsz3434);
          KILL(lbits)(&zgsz3433);
        }
      
      
      }
      {
        lbits zgsz3437;
        CREATE(lbits)(&zgsz3437);
        CONVERT_OF(lbits, sbits)(&zgsz3437, zgaz3236, true);
        lbits zgsz3436;
        CREATE(lbits)(&zgsz3436);
        CONVERT_OF(lbits, fbits)(&zgsz3436, zgaz3235, UINT64_C(32) , true);
        append(&zgaz3237, zgsz3436, zgsz3437);
        KILL(lbits)(&zgsz3437);
        KILL(lbits)(&zgsz3436);
      }
    
    
    }
    {
      sail_int zgsz3438;
      CREATE(sail_int)(&zgsz3438);
      CONVERT_OF(sail_int, mach_int)(&zgsz3438, zcap_len_width);
      lbits zgsz3439;
      CREATE(lbits)(&zgsz3439);
      sail_truncate(&zgsz3439, zgaz3237, zgsz3438);
      zbase = CONVERT_OF(fbits, lbits)(zgsz3439, true);
      KILL(lbits)(&zgsz3439);
      KILL(sail_int)(&zgsz3438);
    }
    KILL(lbits)(&zgaz3237);
  }
  uint64_t ztop;
  {
    lbits zgaz3232;
    CREATE(lbits)(&zgaz3232);
    {
      uint64_t zgaz3230;
      {
        lbits zgsz3440;
        CREATE(lbits)(&zgsz3440);
        CONVERT_OF(lbits, fbits)(&zgsz3440, za_top, UINT64_C(32) , true);
        sail_int zgsz3441;
        CREATE(sail_int)(&zgsz3441);
        CONVERT_OF(sail_int, mach_int)(&zgsz3441, zcorrection_top);
        lbits zgsz3442;
        CREATE(lbits)(&zgsz3442);
        add_bits_int(&zgsz3442, zgsz3440, zgsz3441);
        zgaz3230 = CONVERT_OF(fbits, lbits)(zgsz3442, true);
        KILL(lbits)(&zgsz3442);
        KILL(sail_int)(&zgsz3441);
        KILL(lbits)(&zgsz3440);
      }
      sbits zgaz3231;
      {
        uint64_t zgaz3228;
        zgaz3228 = zc.zT;
        sbits zgaz3229;
        {
          sail_int zgsz3443;
          CREATE(sail_int)(&zgsz3443);
          CONVERT_OF(sail_int, mach_int)(&zgsz3443, zE);
          lbits zgsz3444;
          CREATE(lbits)(&zgsz3444);
          sailgen_zzz3zzz3zzeros_implicit(&zgsz3444, zgsz3443);
          zgaz3229 = CONVERT_OF(sbits, lbits)(zgsz3444, true);
          KILL(lbits)(&zgsz3444);
          KILL(sail_int)(&zgsz3443);
        }
        {
          lbits zgsz3445;
          CREATE(lbits)(&zgsz3445);
          CONVERT_OF(lbits, fbits)(&zgsz3445, zgaz3228, UINT64_C(8) , true);
          lbits zgsz3446;
          CREATE(lbits)(&zgsz3446);
          CONVERT_OF(lbits, sbits)(&zgsz3446, zgaz3229, true);
          lbits zgsz3447;
          CREATE(lbits)(&zgsz3447);
          append(&zgsz3447, zgsz3445, zgsz3446);
          zgaz3231 = CONVERT_OF(sbits, lbits)(zgsz3447, true);
          KILL(lbits)(&zgsz3447);
          KILL(lbits)(&zgsz3446);
          KILL(lbits)(&zgsz3445);
        }
      
      
      }
      {
        lbits zgsz3449;
        CREATE(lbits)(&zgsz3449);
        CONVERT_OF(lbits, sbits)(&zgsz3449, zgaz3231, true);
        lbits zgsz3448;
        CREATE(lbits)(&zgsz3448);
        CONVERT_OF(lbits, fbits)(&zgsz3448, zgaz3230, UINT64_C(32) , true);
        append(&zgaz3232, zgsz3448, zgsz3449);
        KILL(lbits)(&zgsz3449);
        KILL(lbits)(&zgsz3448);
      }
    
    
    }
    {
      sail_int zgsz3450;
      CREATE(sail_int)(&zgsz3450);
      CONVERT_OF(sail_int, mach_int)(&zgsz3450, zcap_len_width);
      lbits zgsz3451;
      CREATE(lbits)(&zgsz3451);
      sail_truncate(&zgsz3451, zgaz3232, zgsz3450);
      ztop = CONVERT_OF(fbits, lbits)(zgsz3451, true);
      KILL(lbits)(&zgsz3451);
      KILL(sail_int)(&zgsz3450);
    }
    KILL(lbits)(&zgaz3232);
  }
  uint64_t zbase2;
  {
    uint64_t zgaz3227;
    {
      fbits zgaz3226;
      {
        int64_t zgaz3225;
        {
          sail_int zgsz3452;
          CREATE(sail_int)(&zgsz3452);
          CONVERT_OF(sail_int, mach_int)(&zgsz3452, zcap_addr_width);
          sail_int zgsz3453;
          CREATE(sail_int)(&zgsz3453);
          CONVERT_OF(sail_int, mach_int)(&zgsz3453, INT64_C(1));
          sail_int zgsz3454;
          CREATE(sail_int)(&zgsz3454);
          sub_int(&zgsz3454, zgsz3452, zgsz3453);
          zgaz3225 = CONVERT_OF(mach_int, sail_int)(zgsz3454);
          KILL(sail_int)(&zgsz3454);
          KILL(sail_int)(&zgsz3453);
          KILL(sail_int)(&zgsz3452);
        }
        {
          sail_int zgsz3456;
          CREATE(sail_int)(&zgsz3456);
          CONVERT_OF(sail_int, mach_int)(&zgsz3456, zgaz3225);
          lbits zgsz3455;
          CREATE(lbits)(&zgsz3455);
          CONVERT_OF(lbits, fbits)(&zgsz3455, zbase, UINT64_C(33) , true);
          zgaz3226 = bitvector_access(zgsz3455, zgsz3456);
          KILL(sail_int)(&zgsz3456);
          KILL(lbits)(&zgsz3455);
        }
      
      }
      uint64_t zgsz3457;
      zgsz3457 = UINT64_C(0b0);
      zgsz3457 = update_fbits(zgsz3457, INT64_C(0), zgaz3226);
      zgaz3227 = zgsz3457;
    
    }
    {
      lbits zgsz3458;
      CREATE(lbits)(&zgsz3458);
      CONVERT_OF(lbits, fbits)(&zgsz3458, UINT64_C(0b0), UINT64_C(1) , true);
      lbits zgsz3459;
      CREATE(lbits)(&zgsz3459);
      CONVERT_OF(lbits, fbits)(&zgsz3459, zgaz3227, UINT64_C(1) , true);
      lbits zgsz3460;
      CREATE(lbits)(&zgsz3460);
      append(&zgsz3460, zgsz3458, zgsz3459);
      zbase2 = CONVERT_OF(fbits, lbits)(zgsz3460, true);
      KILL(lbits)(&zgsz3460);
      KILL(lbits)(&zgsz3459);
      KILL(lbits)(&zgsz3458);
    }
  
  }
  uint64_t ztop2;
  {
    int64_t zgaz3224;
    {
      sail_int zgsz3461;
      CREATE(sail_int)(&zgsz3461);
      CONVERT_OF(sail_int, mach_int)(&zgsz3461, zcap_addr_width);
      sail_int zgsz3462;
      CREATE(sail_int)(&zgsz3462);
      CONVERT_OF(sail_int, mach_int)(&zgsz3462, INT64_C(1));
      sail_int zgsz3463;
      CREATE(sail_int)(&zgsz3463);
      sub_int(&zgsz3463, zgsz3461, zgsz3462);
      zgaz3224 = CONVERT_OF(mach_int, sail_int)(zgsz3463);
      KILL(sail_int)(&zgsz3463);
      KILL(sail_int)(&zgsz3462);
      KILL(sail_int)(&zgsz3461);
    }
    {
      lbits zgsz3464;
      CREATE(lbits)(&zgsz3464);
      CONVERT_OF(lbits, fbits)(&zgsz3464, ztop, UINT64_C(33) , true);
      sail_int zgsz3465;
      CREATE(sail_int)(&zgsz3465);
      CONVERT_OF(sail_int, mach_int)(&zgsz3465, zcap_addr_width);
      sail_int zgsz3466;
      CREATE(sail_int)(&zgsz3466);
      CONVERT_OF(sail_int, mach_int)(&zgsz3466, zgaz3224);
      lbits zgsz3467;
      CREATE(lbits)(&zgsz3467);
      vector_subrange_lbits(&zgsz3467, zgsz3464, zgsz3465, zgsz3466);
      ztop2 = CONVERT_OF(fbits, lbits)(zgsz3467, true);
      KILL(lbits)(&zgsz3467);
      KILL(sail_int)(&zgsz3466);
      KILL(sail_int)(&zgsz3465);
      KILL(lbits)(&zgsz3464);
    }
  
  }
  {
    bool zgaz3219;
    {
      bool zgaz3218;
      {
        int64_t zgaz3215;
        {
          sail_int zgsz3468;
          CREATE(sail_int)(&zgsz3468);
          CONVERT_OF(sail_int, mach_int)(&zgsz3468, zcap_max_E);
          sail_int zgsz3469;
          CREATE(sail_int)(&zgsz3469);
          CONVERT_OF(sail_int, mach_int)(&zgsz3469, INT64_C(1));
          sail_int zgsz3470;
          CREATE(sail_int)(&zgsz3470);
          sub_int(&zgsz3470, zgsz3468, zgsz3469);
          zgaz3215 = CONVERT_OF(mach_int, sail_int)(zgsz3470);
          KILL(sail_int)(&zgsz3470);
          KILL(sail_int)(&zgsz3469);
          KILL(sail_int)(&zgsz3468);
        }
        {
          sail_int zgsz3472;
          CREATE(sail_int)(&zgsz3472);
          CONVERT_OF(sail_int, mach_int)(&zgsz3472, zgaz3215);
          sail_int zgsz3471;
          CREATE(sail_int)(&zgsz3471);
          CONVERT_OF(sail_int, mach_int)(&zgsz3471, zE);
          zgaz3218 = lt(zgsz3471, zgsz3472);
          KILL(sail_int)(&zgsz3472);
          KILL(sail_int)(&zgsz3471);
        }
      
      }
      bool zgsz3476;
      if (zgaz3218) {
      int64_t zgaz3217;
      {
        uint64_t zgaz3216;
        {
          lbits zgsz3473;
          CREATE(lbits)(&zgsz3473);
          CONVERT_OF(lbits, fbits)(&zgsz3473, ztop2, UINT64_C(2) , true);
          lbits zgsz3474;
          CREATE(lbits)(&zgsz3474);
          CONVERT_OF(lbits, fbits)(&zgsz3474, zbase2, UINT64_C(2) , true);
          lbits zgsz3475;
          CREATE(lbits)(&zgsz3475);
          sub_bits(&zgsz3475, zgsz3473, zgsz3474);
          zgaz3216 = CONVERT_OF(fbits, lbits)(zgsz3475, true);
          KILL(lbits)(&zgsz3475);
          KILL(lbits)(&zgsz3474);
          KILL(lbits)(&zgsz3473);
        }
        zgaz3217 = ((mach_int) zgaz3216);
      
      }
      {
        sail_int zgsz3478;
        CREATE(sail_int)(&zgsz3478);
        CONVERT_OF(sail_int, mach_int)(&zgsz3478, INT64_C(1));
        sail_int zgsz3477;
        CREATE(sail_int)(&zgsz3477);
        CONVERT_OF(sail_int, mach_int)(&zgsz3477, zgaz3217);
        zgsz3476 = gt(zgsz3477, zgsz3478);
        KILL(sail_int)(&zgsz3478);
        KILL(sail_int)(&zgsz3477);
      }
    
      } else {  zgsz3476 = false;  }
      zgaz3219 = zgsz3476;
    
    }
    unit zgsz3479;
    if (zgaz3219) {
    fbits zgaz3221;
    {
      fbits zgaz3220;
      {
        sail_int zgsz3481;
        CREATE(sail_int)(&zgsz3481);
        CONVERT_OF(sail_int, mach_int)(&zgsz3481, zcap_addr_width);
        lbits zgsz3480;
        CREATE(lbits)(&zgsz3480);
        CONVERT_OF(lbits, fbits)(&zgsz3480, ztop, UINT64_C(33) , true);
        zgaz3220 = bitvector_access(zgsz3480, zgsz3481);
        KILL(sail_int)(&zgsz3481);
        KILL(lbits)(&zgsz3480);
      }
      zgaz3221 = sailgen_not_bit(zgaz3220);
    
    }
    {
      lbits zgsz3482;
      CREATE(lbits)(&zgsz3482);
      CONVERT_OF(lbits, fbits)(&zgsz3482, ztop, UINT64_C(33) , true);
      sail_int zgsz3483;
      CREATE(sail_int)(&zgsz3483);
      CONVERT_OF(sail_int, mach_int)(&zgsz3483, zcap_addr_width);
      lbits zgsz3484;
      CREATE(lbits)(&zgsz3484);
      update_lbits(&zgsz3484, zgsz3482, zgsz3483, zgaz3221);
      ztop = CONVERT_OF(fbits, lbits)(zgsz3484, true);
      KILL(lbits)(&zgsz3484);
      KILL(sail_int)(&zgsz3483);
      KILL(lbits)(&zgsz3482);
    }
    zgsz3479 = UNIT;
  
    } else {  zgsz3479 = UNIT;  }
  
  }
  uint64_t zgaz3223;
  {
    int64_t zgaz3222;
    {
      sail_int zgsz3485;
      CREATE(sail_int)(&zgsz3485);
      CONVERT_OF(sail_int, mach_int)(&zgsz3485, zcap_addr_width);
      sail_int zgsz3486;
      CREATE(sail_int)(&zgsz3486);
      CONVERT_OF(sail_int, mach_int)(&zgsz3486, INT64_C(1));
      sail_int zgsz3487;
      CREATE(sail_int)(&zgsz3487);
      sub_int(&zgsz3487, zgsz3485, zgsz3486);
      zgaz3222 = CONVERT_OF(mach_int, sail_int)(zgsz3487);
      KILL(sail_int)(&zgsz3487);
      KILL(sail_int)(&zgsz3486);
      KILL(sail_int)(&zgsz3485);
    }
    {
      lbits zgsz3488;
      CREATE(lbits)(&zgsz3488);
      CONVERT_OF(lbits, fbits)(&zgsz3488, zbase, UINT64_C(33) , true);
      sail_int zgsz3489;
      CREATE(sail_int)(&zgsz3489);
      CONVERT_OF(sail_int, mach_int)(&zgsz3489, zgaz3222);
      sail_int zgsz3490;
      CREATE(sail_int)(&zgsz3490);
      CONVERT_OF(sail_int, mach_int)(&zgsz3490, INT64_C(0));
      lbits zgsz3491;
      CREATE(lbits)(&zgsz3491);
      vector_subrange_lbits(&zgsz3491, zgsz3488, zgsz3489, zgsz3490);
      zgaz3223 = CONVERT_OF(fbits, lbits)(zgsz3491, true);
      KILL(lbits)(&zgsz3491);
      KILL(sail_int)(&zgsz3490);
      KILL(sail_int)(&zgsz3489);
      KILL(lbits)(&zgsz3488);
    }
  
  }
  struct ztuple_z8z5bv32zCz0z5bv33z9 zgsz3492;
  zgsz3492.ztup0 = zgaz3223;
  zgsz3492.ztup1 = ztop;
  zcbz342 = zgsz3492;


















end_function_168: ;
  return zcbz342;
end_block_exception_169: ;
  struct ztuple_z8z5bv32zCz0z5bv33z9 zcbz365 = { .ztup1 = UINT64_C(0xdeadc0de), .ztup0 = UINT64_C(0xdeadc0de) };
  return zcbz365;
}

// struct tuple_(%i64, %i64)
struct ztuple_z8z5i64zCz0z5i64z9 {
  int64_t ztup0;
  int64_t ztup1;
};

static void COPY(ztuple_z8z5i64zCz0z5i64z9)(struct ztuple_z8z5i64zCz0z5i64z9 *rop, const struct ztuple_z8z5i64zCz0z5i64z9 op) {
  rop->ztup0 = op.ztup0;
  rop->ztup1 = op.ztup1;
}

static bool EQUAL(ztuple_z8z5i64zCz0z5i64z9)(struct ztuple_z8z5i64zCz0z5i64z9 op1, struct ztuple_z8z5i64zCz0z5i64z9 op2) {
  return EQUAL(mach_int)(op1.ztup0, op2.ztup0) && EQUAL(mach_int)(op1.ztup1, op2.ztup1);
}

static struct ztuple_z8z5i64zCz0z5i64z9 sailgen_getCapBounds(struct zCapability);


static struct ztuple_z8z5i64zCz0z5i64z9 sailgen_getCapBounds(struct zCapability zcap)
{
  __label__ case_172, finish_match_171, end_function_173, end_block_exception_174;

  struct ztuple_z8z5i64zCz0z5i64z9 zcbz343;
  struct ztuple_z8z5bv32zCz0z5bv33z9 zgaz3249;
  zgaz3249 = sailgen_getCapBoundsBits(zcap);
  struct ztuple_z8z5i64zCz0z5i64z9 zgsz3493;
  {
    uint64_t zbase;
    zbase = zgaz3249.ztup0;
    uint64_t ztop;
    ztop = zgaz3249.ztup1;
    int64_t zgaz3250;
    zgaz3250 = ((mach_int) zbase);
    int64_t zgaz3251;
    zgaz3251 = ((mach_int) ztop);
    struct ztuple_z8z5i64zCz0z5i64z9 zgsz3494;
    zgsz3494.ztup0 = zgaz3250;
    zgsz3494.ztup1 = zgaz3251;
    zgsz3493 = zgsz3494;
  
  
  
  
  
    goto finish_match_171;
  }
case_172: ;
  sail_match_failure("getCapBounds");
finish_match_171: ;
  zcbz343 = zgsz3493;


end_function_173: ;
  return zcbz343;
end_block_exception_174: ;
  struct ztuple_z8z5i64zCz0z5i64z9 zcbz366 = { .ztup1 = INT64_C(0xdeadc0de), .ztup0 = INT64_C(0xdeadc0de) };
  return zcbz366;
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

static struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 sailgen_setCapBounds(struct zCapability, uint64_t, uint64_t);

static struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 sailgen_setCapBounds(struct zCapability zcap, uint64_t zbase, uint64_t ztop)
{
  __label__ end_function_176, end_block_exception_177;

  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zcbz344;
  uint64_t zext_base;
  {
    lbits zgsz3496;
    CREATE(lbits)(&zgsz3496);
    CONVERT_OF(lbits, fbits)(&zgsz3496, UINT64_C(0b0), UINT64_C(1) , true);
    lbits zgsz3497;
    CREATE(lbits)(&zgsz3497);
    CONVERT_OF(lbits, fbits)(&zgsz3497, zbase, UINT64_C(32) , true);
    lbits zgsz3498;
    CREATE(lbits)(&zgsz3498);
    append(&zgsz3498, zgsz3496, zgsz3497);
    zext_base = CONVERT_OF(fbits, lbits)(zgsz3498, true);
    KILL(lbits)(&zgsz3498);
    KILL(lbits)(&zgsz3497);
    KILL(lbits)(&zgsz3496);
  }
  uint64_t zlength;
  {
    lbits zgsz3499;
    CREATE(lbits)(&zgsz3499);
    CONVERT_OF(lbits, fbits)(&zgsz3499, ztop, UINT64_C(33) , true);
    lbits zgsz3500;
    CREATE(lbits)(&zgsz3500);
    CONVERT_OF(lbits, fbits)(&zgsz3500, zext_base, UINT64_C(33) , true);
    lbits zgsz3501;
    CREATE(lbits)(&zgsz3501);
    sub_bits(&zgsz3501, zgsz3499, zgsz3500);
    zlength = CONVERT_OF(fbits, lbits)(zgsz3501, true);
    KILL(lbits)(&zgsz3501);
    KILL(lbits)(&zgsz3500);
    KILL(lbits)(&zgsz3499);
  }
  int64_t ze;
  {
    int64_t zgaz3285;
    {
      uint64_t zgaz3284;
      {
        int64_t zgaz3283;
        {
          sail_int zgsz3502;
          CREATE(sail_int)(&zgsz3502);
          CONVERT_OF(sail_int, mach_int)(&zgsz3502, zcap_mantissa_width);
          sail_int zgsz3503;
          CREATE(sail_int)(&zgsz3503);
          CONVERT_OF(sail_int, mach_int)(&zgsz3503, INT64_C(1));
          sail_int zgsz3504;
          CREATE(sail_int)(&zgsz3504);
          sub_int(&zgsz3504, zgsz3502, zgsz3503);
          zgaz3283 = CONVERT_OF(mach_int, sail_int)(zgsz3504);
          KILL(sail_int)(&zgsz3504);
          KILL(sail_int)(&zgsz3503);
          KILL(sail_int)(&zgsz3502);
        }
        {
          lbits zgsz3505;
          CREATE(lbits)(&zgsz3505);
          CONVERT_OF(lbits, fbits)(&zgsz3505, zlength, UINT64_C(33) , true);
          sail_int zgsz3506;
          CREATE(sail_int)(&zgsz3506);
          CONVERT_OF(sail_int, mach_int)(&zgsz3506, zcap_addr_width);
          sail_int zgsz3507;
          CREATE(sail_int)(&zgsz3507);
          CONVERT_OF(sail_int, mach_int)(&zgsz3507, zgaz3283);
          lbits zgsz3508;
          CREATE(lbits)(&zgsz3508);
          vector_subrange_lbits(&zgsz3508, zgsz3505, zgsz3506, zgsz3507);
          zgaz3284 = CONVERT_OF(fbits, lbits)(zgsz3508, true);
          KILL(lbits)(&zgsz3508);
          KILL(sail_int)(&zgsz3507);
          KILL(sail_int)(&zgsz3506);
          KILL(lbits)(&zgsz3505);
        }
      
      }
      {
        lbits zgsz3509;
        CREATE(lbits)(&zgsz3509);
        CONVERT_OF(lbits, fbits)(&zgsz3509, zgaz3284, UINT64_C(26) , true);
        sail_int zgsz3510;
        CREATE(sail_int)(&zgsz3510);
        count_leading_zeros(&zgsz3510, zgsz3509);
        zgaz3285 = CONVERT_OF(mach_int, sail_int)(zgsz3510);
        KILL(sail_int)(&zgsz3510);
        KILL(lbits)(&zgsz3509);
      }
    
    }
    {
      sail_int zgsz3511;
      CREATE(sail_int)(&zgsz3511);
      CONVERT_OF(sail_int, mach_int)(&zgsz3511, zcap_max_E);
      sail_int zgsz3512;
      CREATE(sail_int)(&zgsz3512);
      CONVERT_OF(sail_int, mach_int)(&zgsz3512, zgaz3285);
      sail_int zgsz3513;
      CREATE(sail_int)(&zgsz3513);
      sub_int(&zgsz3513, zgsz3511, zgsz3512);
      ze = CONVERT_OF(mach_int, sail_int)(zgsz3513);
      KILL(sail_int)(&zgsz3513);
      KILL(sail_int)(&zgsz3512);
      KILL(sail_int)(&zgsz3511);
    }
  
  }
  bool zie;
  {
    bool zgaz3282;
    {
      sail_int zgsz3514;
      CREATE(sail_int)(&zgsz3514);
      CONVERT_OF(sail_int, mach_int)(&zgsz3514, ze);
      zgaz3282 = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgsz3514, INT64_C(0));
      KILL(sail_int)(&zgsz3514);
    }
    bool zgsz3520;
    if (zgaz3282) {  zgsz3520 = true;  } else {
    fbits zgaz3281;
    {
      int64_t zgaz3280;
      {
        sail_int zgsz3515;
        CREATE(sail_int)(&zgsz3515);
        CONVERT_OF(sail_int, mach_int)(&zgsz3515, zcap_mantissa_width);
        sail_int zgsz3516;
        CREATE(sail_int)(&zgsz3516);
        CONVERT_OF(sail_int, mach_int)(&zgsz3516, INT64_C(2));
        sail_int zgsz3517;
        CREATE(sail_int)(&zgsz3517);
        sub_int(&zgsz3517, zgsz3515, zgsz3516);
        zgaz3280 = CONVERT_OF(mach_int, sail_int)(zgsz3517);
        KILL(sail_int)(&zgsz3517);
        KILL(sail_int)(&zgsz3516);
        KILL(sail_int)(&zgsz3515);
      }
      {
        sail_int zgsz3519;
        CREATE(sail_int)(&zgsz3519);
        CONVERT_OF(sail_int, mach_int)(&zgsz3519, zgaz3280);
        lbits zgsz3518;
        CREATE(lbits)(&zgsz3518);
        CONVERT_OF(lbits, fbits)(&zgsz3518, zlength, UINT64_C(33) , true);
        zgaz3281 = bitvector_access(zgsz3518, zgsz3519);
        KILL(sail_int)(&zgsz3519);
        KILL(lbits)(&zgsz3518);
      }
    
    }
    zgsz3520 = eq_bit(zgaz3281, UINT64_C(1));
  
    }
    zie = zgsz3520;
  
  }
  uint64_t zBbits;
  {
    lbits zgsz3521;
    CREATE(lbits)(&zgsz3521);
    CONVERT_OF(lbits, fbits)(&zgsz3521, zbase, UINT64_C(32) , true);
    sail_int zgsz3522;
    CREATE(sail_int)(&zgsz3522);
    CONVERT_OF(sail_int, mach_int)(&zgsz3522, zcap_mantissa_width);
    lbits zgsz3523;
    CREATE(lbits)(&zgsz3523);
    sail_truncate(&zgsz3523, zgsz3521, zgsz3522);
    zBbits = CONVERT_OF(fbits, lbits)(zgsz3523, true);
    KILL(lbits)(&zgsz3523);
    KILL(sail_int)(&zgsz3522);
    KILL(lbits)(&zgsz3521);
  }
  uint64_t zTbits;
  {
    lbits zgsz3524;
    CREATE(lbits)(&zgsz3524);
    CONVERT_OF(lbits, fbits)(&zgsz3524, ztop, UINT64_C(33) , true);
    sail_int zgsz3525;
    CREATE(sail_int)(&zgsz3525);
    CONVERT_OF(sail_int, mach_int)(&zgsz3525, zcap_mantissa_width);
    lbits zgsz3526;
    CREATE(lbits)(&zgsz3526);
    sail_truncate(&zgsz3526, zgsz3524, zgsz3525);
    zTbits = CONVERT_OF(fbits, lbits)(zgsz3526, true);
    KILL(lbits)(&zgsz3526);
    KILL(sail_int)(&zgsz3525);
    KILL(lbits)(&zgsz3524);
  }
  bool zlostSignificantTop;
  zlostSignificantTop = false;
  bool zlostSignificantBase;
  zlostSignificantBase = false;
  bool zincE;
  zincE = false;
  {
    unit zgsz3527;
    if (zie) {
    uint64_t zB_ie;
    {
      uint64_t zgaz3275;
      {
        int64_t zgaz3274;
        {
          sail_int zgsz3528;
          CREATE(sail_int)(&zgsz3528);
          CONVERT_OF(sail_int, mach_int)(&zgsz3528, ze);
          sail_int zgsz3529;
          CREATE(sail_int)(&zgsz3529);
          CONVERT_OF(sail_int, mach_int)(&zgsz3529, INT64_C(3));
          sail_int zgsz3530;
          CREATE(sail_int)(&zgsz3530);
          add_int(&zgsz3530, zgsz3528, zgsz3529);
          zgaz3274 = CONVERT_OF(mach_int, sail_int)(zgsz3530);
          KILL(sail_int)(&zgsz3530);
          KILL(sail_int)(&zgsz3529);
          KILL(sail_int)(&zgsz3528);
        }
        {
          lbits zgsz3531;
          CREATE(lbits)(&zgsz3531);
          CONVERT_OF(lbits, fbits)(&zgsz3531, zbase, UINT64_C(32) , true);
          sail_int zgsz3532;
          CREATE(sail_int)(&zgsz3532);
          CONVERT_OF(sail_int, mach_int)(&zgsz3532, zgaz3274);
          lbits zgsz3533;
          CREATE(lbits)(&zgsz3533);
          shiftr(&zgsz3533, zgsz3531, zgsz3532);
          zgaz3275 = CONVERT_OF(fbits, lbits)(zgsz3533, true);
          KILL(lbits)(&zgsz3533);
          KILL(sail_int)(&zgsz3532);
          KILL(lbits)(&zgsz3531);
        }
      
      }
      int64_t zgaz3276;
      {
        sail_int zgsz3534;
        CREATE(sail_int)(&zgsz3534);
        CONVERT_OF(sail_int, mach_int)(&zgsz3534, zcap_mantissa_width);
        sail_int zgsz3535;
        CREATE(sail_int)(&zgsz3535);
        CONVERT_OF(sail_int, mach_int)(&zgsz3535, INT64_C(3));
        sail_int zgsz3536;
        CREATE(sail_int)(&zgsz3536);
        sub_int(&zgsz3536, zgsz3534, zgsz3535);
        zgaz3276 = CONVERT_OF(mach_int, sail_int)(zgsz3536);
        KILL(sail_int)(&zgsz3536);
        KILL(sail_int)(&zgsz3535);
        KILL(sail_int)(&zgsz3534);
      }
      {
        lbits zgsz3537;
        CREATE(lbits)(&zgsz3537);
        CONVERT_OF(lbits, fbits)(&zgsz3537, zgaz3275, UINT64_C(32) , true);
        sail_int zgsz3538;
        CREATE(sail_int)(&zgsz3538);
        CONVERT_OF(sail_int, mach_int)(&zgsz3538, zgaz3276);
        lbits zgsz3539;
        CREATE(lbits)(&zgsz3539);
        sail_truncate(&zgsz3539, zgsz3537, zgsz3538);
        zB_ie = CONVERT_OF(fbits, lbits)(zgsz3539, true);
        KILL(lbits)(&zgsz3539);
        KILL(sail_int)(&zgsz3538);
        KILL(lbits)(&zgsz3537);
      }
    
    
    }
    uint64_t zT_ie;
    {
      uint64_t zgaz3272;
      {
        int64_t zgaz3271;
        {
          sail_int zgsz3540;
          CREATE(sail_int)(&zgsz3540);
          CONVERT_OF(sail_int, mach_int)(&zgsz3540, ze);
          sail_int zgsz3541;
          CREATE(sail_int)(&zgsz3541);
          CONVERT_OF(sail_int, mach_int)(&zgsz3541, INT64_C(3));
          sail_int zgsz3542;
          CREATE(sail_int)(&zgsz3542);
          add_int(&zgsz3542, zgsz3540, zgsz3541);
          zgaz3271 = CONVERT_OF(mach_int, sail_int)(zgsz3542);
          KILL(sail_int)(&zgsz3542);
          KILL(sail_int)(&zgsz3541);
          KILL(sail_int)(&zgsz3540);
        }
        {
          lbits zgsz3543;
          CREATE(lbits)(&zgsz3543);
          CONVERT_OF(lbits, fbits)(&zgsz3543, ztop, UINT64_C(33) , true);
          sail_int zgsz3544;
          CREATE(sail_int)(&zgsz3544);
          CONVERT_OF(sail_int, mach_int)(&zgsz3544, zgaz3271);
          lbits zgsz3545;
          CREATE(lbits)(&zgsz3545);
          shiftr(&zgsz3545, zgsz3543, zgsz3544);
          zgaz3272 = CONVERT_OF(fbits, lbits)(zgsz3545, true);
          KILL(lbits)(&zgsz3545);
          KILL(sail_int)(&zgsz3544);
          KILL(lbits)(&zgsz3543);
        }
      
      }
      int64_t zgaz3273;
      {
        sail_int zgsz3546;
        CREATE(sail_int)(&zgsz3546);
        CONVERT_OF(sail_int, mach_int)(&zgsz3546, zcap_mantissa_width);
        sail_int zgsz3547;
        CREATE(sail_int)(&zgsz3547);
        CONVERT_OF(sail_int, mach_int)(&zgsz3547, INT64_C(3));
        sail_int zgsz3548;
        CREATE(sail_int)(&zgsz3548);
        sub_int(&zgsz3548, zgsz3546, zgsz3547);
        zgaz3273 = CONVERT_OF(mach_int, sail_int)(zgsz3548);
        KILL(sail_int)(&zgsz3548);
        KILL(sail_int)(&zgsz3547);
        KILL(sail_int)(&zgsz3546);
      }
      {
        lbits zgsz3549;
        CREATE(lbits)(&zgsz3549);
        CONVERT_OF(lbits, fbits)(&zgsz3549, zgaz3272, UINT64_C(33) , true);
        sail_int zgsz3550;
        CREATE(sail_int)(&zgsz3550);
        CONVERT_OF(sail_int, mach_int)(&zgsz3550, zgaz3273);
        lbits zgsz3551;
        CREATE(lbits)(&zgsz3551);
        sail_truncate(&zgsz3551, zgsz3549, zgsz3550);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3551, true);
        KILL(lbits)(&zgsz3551);
        KILL(sail_int)(&zgsz3550);
        KILL(lbits)(&zgsz3549);
      }
    
    
    }
    uint64_t zmaskLo;
    {
      int64_t zgaz3269;
      {
        sail_int zgsz3552;
        CREATE(sail_int)(&zgsz3552);
        CONVERT_OF(sail_int, mach_int)(&zgsz3552, INT64_C(32));
        sail_int zgsz3553;
        CREATE(sail_int)(&zgsz3553);
        CONVERT_OF(sail_int, mach_int)(&zgsz3553, INT64_C(1));
        sail_int zgsz3554;
        CREATE(sail_int)(&zgsz3554);
        add_int(&zgsz3554, zgsz3552, zgsz3553);
        zgaz3269 = CONVERT_OF(mach_int, sail_int)(zgsz3554);
        KILL(sail_int)(&zgsz3554);
        KILL(sail_int)(&zgsz3553);
        KILL(sail_int)(&zgsz3552);
      }
      sbits zgaz3270;
      {
        int64_t zgaz3268;
        {
          sail_int zgsz3555;
          CREATE(sail_int)(&zgsz3555);
          CONVERT_OF(sail_int, mach_int)(&zgsz3555, ze);
          sail_int zgsz3556;
          CREATE(sail_int)(&zgsz3556);
          CONVERT_OF(sail_int, mach_int)(&zgsz3556, INT64_C(3));
          sail_int zgsz3557;
          CREATE(sail_int)(&zgsz3557);
          add_int(&zgsz3557, zgsz3555, zgsz3556);
          zgaz3268 = CONVERT_OF(mach_int, sail_int)(zgsz3557);
          KILL(sail_int)(&zgsz3557);
          KILL(sail_int)(&zgsz3556);
          KILL(sail_int)(&zgsz3555);
        }
        {
          sail_int zgsz3558;
          CREATE(sail_int)(&zgsz3558);
          CONVERT_OF(sail_int, mach_int)(&zgsz3558, zgaz3268);
          lbits zgsz3559;
          CREATE(lbits)(&zgsz3559);
          sailgen_zzz3zzz3ones(&zgsz3559, zgsz3558);
          zgaz3270 = CONVERT_OF(sbits, lbits)(zgsz3559, true);
          KILL(lbits)(&zgsz3559);
          KILL(sail_int)(&zgsz3558);
        }
      
      }
      {
        sail_int zgsz3560;
        CREATE(sail_int)(&zgsz3560);
        CONVERT_OF(sail_int, mach_int)(&zgsz3560, zgaz3269);
        lbits zgsz3561;
        CREATE(lbits)(&zgsz3561);
        CONVERT_OF(lbits, sbits)(&zgsz3561, zgaz3270, true);
        lbits zgsz3562;
        CREATE(lbits)(&zgsz3562);
        sailgen_zzz3zzz3EXTZ(&zgsz3562, zgsz3560, zgsz3561);
        zmaskLo = CONVERT_OF(fbits, lbits)(zgsz3562, true);
        KILL(lbits)(&zgsz3562);
        KILL(lbits)(&zgsz3561);
        KILL(sail_int)(&zgsz3560);
      }
    
    
    }
    {
      int64_t zgaz3253;
      {
        uint64_t zgaz3252;
        {
          lbits zgsz3563;
          CREATE(lbits)(&zgsz3563);
          CONVERT_OF(lbits, fbits)(&zgsz3563, zext_base, UINT64_C(33) , true);
          lbits zgsz3564;
          CREATE(lbits)(&zgsz3564);
          CONVERT_OF(lbits, fbits)(&zgsz3564, zmaskLo, UINT64_C(33) , true);
          lbits zgsz3565;
          CREATE(lbits)(&zgsz3565);
          and_bits(&zgsz3565, zgsz3563, zgsz3564);
          zgaz3252 = CONVERT_OF(fbits, lbits)(zgsz3565, true);
          KILL(lbits)(&zgsz3565);
          KILL(lbits)(&zgsz3564);
          KILL(lbits)(&zgsz3563);
        }
        zgaz3253 = ((mach_int) zgaz3252);
      
      }
      {
        sail_int zgsz3566;
        CREATE(sail_int)(&zgsz3566);
        CONVERT_OF(sail_int, mach_int)(&zgsz3566, zgaz3253);
        zlostSignificantBase = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgsz3566, INT64_C(0));
        KILL(sail_int)(&zgsz3566);
      }
      unit zgsz3576;
      zgsz3576 = UNIT;
    
    }
    {
      int64_t zgaz3255;
      {
        uint64_t zgaz3254;
        {
          lbits zgsz3567;
          CREATE(lbits)(&zgsz3567);
          CONVERT_OF(lbits, fbits)(&zgsz3567, ztop, UINT64_C(33) , true);
          lbits zgsz3568;
          CREATE(lbits)(&zgsz3568);
          CONVERT_OF(lbits, fbits)(&zgsz3568, zmaskLo, UINT64_C(33) , true);
          lbits zgsz3569;
          CREATE(lbits)(&zgsz3569);
          and_bits(&zgsz3569, zgsz3567, zgsz3568);
          zgaz3254 = CONVERT_OF(fbits, lbits)(zgsz3569, true);
          KILL(lbits)(&zgsz3569);
          KILL(lbits)(&zgsz3568);
          KILL(lbits)(&zgsz3567);
        }
        zgaz3255 = ((mach_int) zgaz3254);
      
      }
      {
        sail_int zgsz3570;
        CREATE(sail_int)(&zgsz3570);
        CONVERT_OF(sail_int, mach_int)(&zgsz3570, zgaz3255);
        zlostSignificantTop = sailgen_zzz3zzzz7mzzJzzK0z3neq_int(zgsz3570, INT64_C(0));
        KILL(sail_int)(&zgsz3570);
      }
      unit zgsz3575;
      zgsz3575 = UNIT;
    
    }
    {
      unit zgsz3571;
      if (zlostSignificantTop) {
      {
        lbits zgsz3572;
        CREATE(lbits)(&zgsz3572);
        CONVERT_OF(lbits, fbits)(&zgsz3572, zT_ie, UINT64_C(5) , true);
        sail_int zgsz3573;
        CREATE(sail_int)(&zgsz3573);
        CONVERT_OF(sail_int, mach_int)(&zgsz3573, INT64_C(1));
        lbits zgsz3574;
        CREATE(lbits)(&zgsz3574);
        add_bits_int(&zgsz3574, zgsz3572, zgsz3573);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3574, true);
        KILL(lbits)(&zgsz3574);
        KILL(sail_int)(&zgsz3573);
        KILL(lbits)(&zgsz3572);
      }
      zgsz3571 = UNIT;
      } else {  zgsz3571 = UNIT;  }
    }
    uint64_t zlen_ie;
    {
      lbits zgsz3577;
      CREATE(lbits)(&zgsz3577);
      CONVERT_OF(lbits, fbits)(&zgsz3577, zT_ie, UINT64_C(5) , true);
      lbits zgsz3578;
      CREATE(lbits)(&zgsz3578);
      CONVERT_OF(lbits, fbits)(&zgsz3578, zB_ie, UINT64_C(5) , true);
      lbits zgsz3579;
      CREATE(lbits)(&zgsz3579);
      sub_bits(&zgsz3579, zgsz3577, zgsz3578);
      zlen_ie = CONVERT_OF(fbits, lbits)(zgsz3579, true);
      KILL(lbits)(&zgsz3579);
      KILL(lbits)(&zgsz3578);
      KILL(lbits)(&zgsz3577);
    }
    {
      bool zgaz3258;
      {
        fbits zgaz3257;
        {
          int64_t zgaz3256;
          {
            sail_int zgsz3580;
            CREATE(sail_int)(&zgsz3580);
            CONVERT_OF(sail_int, mach_int)(&zgsz3580, zcap_mantissa_width);
            sail_int zgsz3581;
            CREATE(sail_int)(&zgsz3581);
            CONVERT_OF(sail_int, mach_int)(&zgsz3581, INT64_C(4));
            sail_int zgsz3582;
            CREATE(sail_int)(&zgsz3582);
            sub_int(&zgsz3582, zgsz3580, zgsz3581);
            zgaz3256 = CONVERT_OF(mach_int, sail_int)(zgsz3582);
            KILL(sail_int)(&zgsz3582);
            KILL(sail_int)(&zgsz3581);
            KILL(sail_int)(&zgsz3580);
          }
          {
            sail_int zgsz3584;
            CREATE(sail_int)(&zgsz3584);
            CONVERT_OF(sail_int, mach_int)(&zgsz3584, zgaz3256);
            lbits zgsz3583;
            CREATE(lbits)(&zgsz3583);
            CONVERT_OF(lbits, fbits)(&zgsz3583, zlen_ie, UINT64_C(5) , true);
            zgaz3257 = bitvector_access(zgsz3583, zgsz3584);
            KILL(sail_int)(&zgsz3584);
            KILL(lbits)(&zgsz3583);
          }
        
        }
        zgaz3258 = eq_bit(zgaz3257, UINT64_C(1));
      
      }
      unit zgsz3589;
      if (zgaz3258) {
      {
        zincE = true;
        unit zgsz3611;
        zgsz3611 = UNIT;
      }
      {
        bool zgsz3592;
        if (zlostSignificantBase) {  zgsz3592 = true;  } else {
        fbits zgaz3259;
        {
          sail_int zgsz3591;
          CREATE(sail_int)(&zgsz3591);
          CONVERT_OF(sail_int, mach_int)(&zgsz3591, INT64_C(0));
          lbits zgsz3590;
          CREATE(lbits)(&zgsz3590);
          CONVERT_OF(lbits, fbits)(&zgsz3590, zB_ie, UINT64_C(5) , true);
          zgaz3259 = bitvector_access(zgsz3590, zgsz3591);
          KILL(sail_int)(&zgsz3591);
          KILL(lbits)(&zgsz3590);
        }
        zgsz3592 = eq_bit(zgaz3259, UINT64_C(1));
      
        }
        zlostSignificantBase = zgsz3592;
        unit zgsz3610;
        zgsz3610 = UNIT;
      }
      {
        bool zgsz3595;
        if (zlostSignificantTop) {  zgsz3595 = true;  } else {
        fbits zgaz3260;
        {
          sail_int zgsz3594;
          CREATE(sail_int)(&zgsz3594);
          CONVERT_OF(sail_int, mach_int)(&zgsz3594, INT64_C(0));
          lbits zgsz3593;
          CREATE(lbits)(&zgsz3593);
          CONVERT_OF(lbits, fbits)(&zgsz3593, zT_ie, UINT64_C(5) , true);
          zgaz3260 = bitvector_access(zgsz3593, zgsz3594);
          KILL(sail_int)(&zgsz3594);
          KILL(lbits)(&zgsz3593);
        }
        zgsz3595 = eq_bit(zgaz3260, UINT64_C(1));
      
        }
        zlostSignificantTop = zgsz3595;
        unit zgsz3609;
        zgsz3609 = UNIT;
      }
      {
        uint64_t zgaz3262;
        {
          int64_t zgaz3261;
          {
            sail_int zgsz3596;
            CREATE(sail_int)(&zgsz3596);
            CONVERT_OF(sail_int, mach_int)(&zgsz3596, ze);
            sail_int zgsz3597;
            CREATE(sail_int)(&zgsz3597);
            CONVERT_OF(sail_int, mach_int)(&zgsz3597, INT64_C(4));
            sail_int zgsz3598;
            CREATE(sail_int)(&zgsz3598);
            add_int(&zgsz3598, zgsz3596, zgsz3597);
            zgaz3261 = CONVERT_OF(mach_int, sail_int)(zgsz3598);
            KILL(sail_int)(&zgsz3598);
            KILL(sail_int)(&zgsz3597);
            KILL(sail_int)(&zgsz3596);
          }
          {
            lbits zgsz3599;
            CREATE(lbits)(&zgsz3599);
            CONVERT_OF(lbits, fbits)(&zgsz3599, zbase, UINT64_C(32) , true);
            sail_int zgsz3600;
            CREATE(sail_int)(&zgsz3600);
            CONVERT_OF(sail_int, mach_int)(&zgsz3600, zgaz3261);
            lbits zgsz3601;
            CREATE(lbits)(&zgsz3601);
            shiftr(&zgsz3601, zgsz3599, zgsz3600);
            zgaz3262 = CONVERT_OF(fbits, lbits)(zgsz3601, true);
            KILL(lbits)(&zgsz3601);
            KILL(sail_int)(&zgsz3600);
            KILL(lbits)(&zgsz3599);
          }
        
        }
        int64_t zgaz3263;
        {
          sail_int zgsz3602;
          CREATE(sail_int)(&zgsz3602);
          CONVERT_OF(sail_int, mach_int)(&zgsz3602, zcap_mantissa_width);
          sail_int zgsz3603;
          CREATE(sail_int)(&zgsz3603);
          CONVERT_OF(sail_int, mach_int)(&zgsz3603, INT64_C(3));
          sail_int zgsz3604;
          CREATE(sail_int)(&zgsz3604);
          sub_int(&zgsz3604, zgsz3602, zgsz3603);
          zgaz3263 = CONVERT_OF(mach_int, sail_int)(zgsz3604);
          KILL(sail_int)(&zgsz3604);
          KILL(sail_int)(&zgsz3603);
          KILL(sail_int)(&zgsz3602);
        }
        {
          lbits zgsz3605;
          CREATE(lbits)(&zgsz3605);
          CONVERT_OF(lbits, fbits)(&zgsz3605, zgaz3262, UINT64_C(32) , true);
          sail_int zgsz3606;
          CREATE(sail_int)(&zgsz3606);
          CONVERT_OF(sail_int, mach_int)(&zgsz3606, zgaz3263);
          lbits zgsz3607;
          CREATE(lbits)(&zgsz3607);
          sail_truncate(&zgsz3607, zgsz3605, zgsz3606);
          zB_ie = CONVERT_OF(fbits, lbits)(zgsz3607, true);
          KILL(lbits)(&zgsz3607);
          KILL(sail_int)(&zgsz3606);
          KILL(lbits)(&zgsz3605);
        }
        unit zgsz3608;
        zgsz3608 = UNIT;
      
      
      }
      int64_t zincT;
      if (zlostSignificantTop) {  zincT = INT64_C(1);  } else {  zincT = INT64_C(0);  }
      uint64_t zgaz3267;
      {
        uint64_t zgaz3265;
        {
          int64_t zgaz3264;
          {
            sail_int zgsz3612;
            CREATE(sail_int)(&zgsz3612);
            CONVERT_OF(sail_int, mach_int)(&zgsz3612, ze);
            sail_int zgsz3613;
            CREATE(sail_int)(&zgsz3613);
            CONVERT_OF(sail_int, mach_int)(&zgsz3613, INT64_C(4));
            sail_int zgsz3614;
            CREATE(sail_int)(&zgsz3614);
            add_int(&zgsz3614, zgsz3612, zgsz3613);
            zgaz3264 = CONVERT_OF(mach_int, sail_int)(zgsz3614);
            KILL(sail_int)(&zgsz3614);
            KILL(sail_int)(&zgsz3613);
            KILL(sail_int)(&zgsz3612);
          }
          {
            lbits zgsz3615;
            CREATE(lbits)(&zgsz3615);
            CONVERT_OF(lbits, fbits)(&zgsz3615, ztop, UINT64_C(33) , true);
            sail_int zgsz3616;
            CREATE(sail_int)(&zgsz3616);
            CONVERT_OF(sail_int, mach_int)(&zgsz3616, zgaz3264);
            lbits zgsz3617;
            CREATE(lbits)(&zgsz3617);
            shiftr(&zgsz3617, zgsz3615, zgsz3616);
            zgaz3265 = CONVERT_OF(fbits, lbits)(zgsz3617, true);
            KILL(lbits)(&zgsz3617);
            KILL(sail_int)(&zgsz3616);
            KILL(lbits)(&zgsz3615);
          }
        
        }
        int64_t zgaz3266;
        {
          sail_int zgsz3618;
          CREATE(sail_int)(&zgsz3618);
          CONVERT_OF(sail_int, mach_int)(&zgsz3618, zcap_mantissa_width);
          sail_int zgsz3619;
          CREATE(sail_int)(&zgsz3619);
          CONVERT_OF(sail_int, mach_int)(&zgsz3619, INT64_C(3));
          sail_int zgsz3620;
          CREATE(sail_int)(&zgsz3620);
          sub_int(&zgsz3620, zgsz3618, zgsz3619);
          zgaz3266 = CONVERT_OF(mach_int, sail_int)(zgsz3620);
          KILL(sail_int)(&zgsz3620);
          KILL(sail_int)(&zgsz3619);
          KILL(sail_int)(&zgsz3618);
        }
        {
          lbits zgsz3621;
          CREATE(lbits)(&zgsz3621);
          CONVERT_OF(lbits, fbits)(&zgsz3621, zgaz3265, UINT64_C(33) , true);
          sail_int zgsz3622;
          CREATE(sail_int)(&zgsz3622);
          CONVERT_OF(sail_int, mach_int)(&zgsz3622, zgaz3266);
          lbits zgsz3623;
          CREATE(lbits)(&zgsz3623);
          sail_truncate(&zgsz3623, zgsz3621, zgsz3622);
          zgaz3267 = CONVERT_OF(fbits, lbits)(zgsz3623, true);
          KILL(lbits)(&zgsz3623);
          KILL(sail_int)(&zgsz3622);
          KILL(lbits)(&zgsz3621);
        }
      
      
      }
      {
        lbits zgsz3624;
        CREATE(lbits)(&zgsz3624);
        CONVERT_OF(lbits, fbits)(&zgsz3624, zgaz3267, UINT64_C(5) , true);
        sail_int zgsz3625;
        CREATE(sail_int)(&zgsz3625);
        CONVERT_OF(sail_int, mach_int)(&zgsz3625, zincT);
        lbits zgsz3626;
        CREATE(lbits)(&zgsz3626);
        add_bits_int(&zgsz3626, zgsz3624, zgsz3625);
        zT_ie = CONVERT_OF(fbits, lbits)(zgsz3626, true);
        KILL(lbits)(&zgsz3626);
        KILL(sail_int)(&zgsz3625);
        KILL(lbits)(&zgsz3624);
      }
      zgsz3589 = UNIT;
    
    
      } else {  zgsz3589 = UNIT;  }
    
    }
    {
      {
        lbits zgsz3585;
        CREATE(lbits)(&zgsz3585);
        CONVERT_OF(lbits, fbits)(&zgsz3585, zB_ie, UINT64_C(5) , true);
        lbits zgsz3586;
        CREATE(lbits)(&zgsz3586);
        CONVERT_OF(lbits, fbits)(&zgsz3586, UINT64_C(0b000), UINT64_C(3) , true);
        lbits zgsz3587;
        CREATE(lbits)(&zgsz3587);
        append(&zgsz3587, zgsz3585, zgsz3586);
        zBbits = CONVERT_OF(fbits, lbits)(zgsz3587, true);
        KILL(lbits)(&zgsz3587);
        KILL(lbits)(&zgsz3586);
        KILL(lbits)(&zgsz3585);
      }
      unit zgsz3588;
      zgsz3588 = UNIT;
    }
    {
      lbits zgsz3627;
      CREATE(lbits)(&zgsz3627);
      CONVERT_OF(lbits, fbits)(&zgsz3627, zT_ie, UINT64_C(5) , true);
      lbits zgsz3628;
      CREATE(lbits)(&zgsz3628);
      CONVERT_OF(lbits, fbits)(&zgsz3628, UINT64_C(0b000), UINT64_C(3) , true);
      lbits zgsz3629;
      CREATE(lbits)(&zgsz3629);
      append(&zgsz3629, zgsz3627, zgsz3628);
      zTbits = CONVERT_OF(fbits, lbits)(zgsz3629, true);
      KILL(lbits)(&zgsz3629);
      KILL(lbits)(&zgsz3628);
      KILL(lbits)(&zgsz3627);
    }
    zgsz3527 = UNIT;
  
  
  
  
    } else {  zgsz3527 = UNIT;  }
  }
  struct zCapability znewCap;
  {
    uint64_t zgaz3279;
    {
      int64_t zgaz3278;
      if (zincE) {
      {
        sail_int zgsz3630;
        CREATE(sail_int)(&zgsz3630);
        CONVERT_OF(sail_int, mach_int)(&zgsz3630, ze);
        sail_int zgsz3631;
        CREATE(sail_int)(&zgsz3631);
        CONVERT_OF(sail_int, mach_int)(&zgsz3631, INT64_C(1));
        sail_int zgsz3632;
        CREATE(sail_int)(&zgsz3632);
        add_int(&zgsz3632, zgsz3630, zgsz3631);
        zgaz3278 = CONVERT_OF(mach_int, sail_int)(zgsz3632);
        KILL(sail_int)(&zgsz3632);
        KILL(sail_int)(&zgsz3631);
        KILL(sail_int)(&zgsz3630);
      }
      } else {  zgaz3278 = ze;  }
      {
        sail_int zgsz3633;
        CREATE(sail_int)(&zgsz3633);
        CONVERT_OF(sail_int, mach_int)(&zgsz3633, zgaz3278);
        zgaz3279 = sailgen_zzzz7lzzJzzK6z3to_bits(INT64_C(6), zgsz3633);
        KILL(sail_int)(&zgsz3633);
      }
    
    }
    struct zCapability zgsz3634;
    zgsz3634 = zcap;
    zgsz3634.zB = zBbits;
    zgsz3634.zE = zgaz3279;
    zgsz3634.zT = zTbits;
    zgsz3634.zaddress = zbase;
    zgsz3634.zinternal_E = zie;
    znewCap = zgsz3634;
  
  
  }
  bool zexact;
  {
    bool zgaz3277;
    {
      bool zgsz3635;
      if (zlostSignificantBase) {  zgsz3635 = true;  } else {  zgsz3635 = zlostSignificantTop;  }
      zgaz3277 = zgsz3635;
    }
    zexact = not(zgaz3277);
  
  }
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgsz3636;
  zgsz3636.ztup0 = zexact;
  zgsz3636.ztup1 = znewCap;
  zcbz344 = zgsz3636;












end_function_176: ;
  return zcbz344;
end_block_exception_177: ;
  struct zCapability zcbz368 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zcbz367 = { .ztup1 = zcbz368, .ztup0 = false };
  return zcbz367;
}

static uint64_t sailgen_getCapPerms(struct zCapability);

static uint64_t sailgen_getCapPerms(struct zCapability zcap)
{
  __label__ end_function_179, end_block_exception_180;

  uint64_t zcbz345;
  uint64_t zperms;
  {
    uint64_t zgaz3287;
    zgaz3287 = sailgen_getCapHardPerms(zcap);
    zperms = sailgen_zzzz7mzzJzzK15zzCzz0zz7nzzJzzK12z3EXTZ(INT64_C(15), zgaz3287);
  
  }
  uint64_t zgaz3286;
  zgaz3286 = zcap.zuperms;
  {
    lbits zgsz3637;
    CREATE(lbits)(&zgsz3637);
    CONVERT_OF(lbits, fbits)(&zgsz3637, zgaz3286, UINT64_C(0) , true);
    lbits zgsz3638;
    CREATE(lbits)(&zgsz3638);
    CONVERT_OF(lbits, fbits)(&zgsz3638, zperms, UINT64_C(15) , true);
    lbits zgsz3639;
    CREATE(lbits)(&zgsz3639);
    append(&zgsz3639, zgsz3637, zgsz3638);
    zcbz345 = CONVERT_OF(fbits, lbits)(zgsz3639, true);
    KILL(lbits)(&zgsz3639);
    KILL(lbits)(&zgsz3638);
    KILL(lbits)(&zgsz3637);
  }


end_function_179: ;
  return zcbz345;
end_block_exception_180: ;

  return UINT64_C(0xdeadc0de);
}

static struct zCapability sailgen_setCapPerms(struct zCapability, uint64_t);

static struct zCapability sailgen_setCapPerms(struct zCapability zcap, uint64_t zperms)
{
  __label__ end_function_182, end_block_exception_183;

  struct zCapability zcbz346;
  uint64_t zgaz3289;
  {
    uint64_t zgaz3288;
    {
      lbits zgsz3640;
      CREATE(lbits)(&zgsz3640);
      CONVERT_OF(lbits, fbits)(&zgsz3640, zperms, UINT64_C(15) , true);
      sail_int zgsz3641;
      CREATE(sail_int)(&zgsz3641);
      CONVERT_OF(sail_int, mach_int)(&zgsz3641, zcap_uperms_shift);
      lbits zgsz3642;
      CREATE(lbits)(&zgsz3642);
      shiftr(&zgsz3642, zgsz3640, zgsz3641);
      zgaz3288 = CONVERT_OF(fbits, lbits)(zgsz3642, true);
      KILL(lbits)(&zgsz3642);
      KILL(sail_int)(&zgsz3641);
      KILL(lbits)(&zgsz3640);
    }
    {
      lbits zgsz3643;
      CREATE(lbits)(&zgsz3643);
      CONVERT_OF(lbits, fbits)(&zgsz3643, zgaz3288, UINT64_C(15) , true);
      sail_int zgsz3644;
      CREATE(sail_int)(&zgsz3644);
      CONVERT_OF(sail_int, mach_int)(&zgsz3644, zcap_uperms_width);
      lbits zgsz3645;
      CREATE(lbits)(&zgsz3645);
      sail_truncate(&zgsz3645, zgsz3643, zgsz3644);
      zgaz3289 = CONVERT_OF(fbits, lbits)(zgsz3645, true);
      KILL(lbits)(&zgsz3645);
      KILL(sail_int)(&zgsz3644);
      KILL(lbits)(&zgsz3643);
    }
  
  }
  bool zgaz3291;
  {
    fbits zgaz3290;
    {
      sail_int zgsz3647;
      CREATE(sail_int)(&zgsz3647);
      CONVERT_OF(sail_int, mach_int)(&zgsz3647, INT64_C(11));
      lbits zgsz3646;
      CREATE(lbits)(&zgsz3646);
      CONVERT_OF(lbits, fbits)(&zgsz3646, zperms, UINT64_C(15) , true);
      zgaz3290 = bitvector_access(zgsz3646, zgsz3647);
      KILL(sail_int)(&zgsz3647);
      KILL(lbits)(&zgsz3646);
    }
    zgaz3291 = sailgen_bit_to_bool(zgaz3290);
  
  }
  bool zgaz3293;
  {
    fbits zgaz3292;
    {
      sail_int zgsz3649;
      CREATE(sail_int)(&zgsz3649);
      CONVERT_OF(sail_int, mach_int)(&zgsz3649, INT64_C(10));
      lbits zgsz3648;
      CREATE(lbits)(&zgsz3648);
      CONVERT_OF(lbits, fbits)(&zgsz3648, zperms, UINT64_C(15) , true);
      zgaz3292 = bitvector_access(zgsz3648, zgsz3649);
      KILL(sail_int)(&zgsz3649);
      KILL(lbits)(&zgsz3648);
    }
    zgaz3293 = sailgen_bit_to_bool(zgaz3292);
  
  }
  bool zgaz3295;
  {
    fbits zgaz3294;
    {
      sail_int zgsz3651;
      CREATE(sail_int)(&zgsz3651);
      CONVERT_OF(sail_int, mach_int)(&zgsz3651, INT64_C(9));
      lbits zgsz3650;
      CREATE(lbits)(&zgsz3650);
      CONVERT_OF(lbits, fbits)(&zgsz3650, zperms, UINT64_C(15) , true);
      zgaz3294 = bitvector_access(zgsz3650, zgsz3651);
      KILL(sail_int)(&zgsz3651);
      KILL(lbits)(&zgsz3650);
    }
    zgaz3295 = sailgen_bit_to_bool(zgaz3294);
  
  }
  bool zgaz3297;
  {
    fbits zgaz3296;
    {
      sail_int zgsz3653;
      CREATE(sail_int)(&zgsz3653);
      CONVERT_OF(sail_int, mach_int)(&zgsz3653, INT64_C(8));
      lbits zgsz3652;
      CREATE(lbits)(&zgsz3652);
      CONVERT_OF(lbits, fbits)(&zgsz3652, zperms, UINT64_C(15) , true);
      zgaz3296 = bitvector_access(zgsz3652, zgsz3653);
      KILL(sail_int)(&zgsz3653);
      KILL(lbits)(&zgsz3652);
    }
    zgaz3297 = sailgen_bit_to_bool(zgaz3296);
  
  }
  bool zgaz3299;
  {
    fbits zgaz3298;
    {
      sail_int zgsz3655;
      CREATE(sail_int)(&zgsz3655);
      CONVERT_OF(sail_int, mach_int)(&zgsz3655, INT64_C(7));
      lbits zgsz3654;
      CREATE(lbits)(&zgsz3654);
      CONVERT_OF(lbits, fbits)(&zgsz3654, zperms, UINT64_C(15) , true);
      zgaz3298 = bitvector_access(zgsz3654, zgsz3655);
      KILL(sail_int)(&zgsz3655);
      KILL(lbits)(&zgsz3654);
    }
    zgaz3299 = sailgen_bit_to_bool(zgaz3298);
  
  }
  bool zgaz3301;
  {
    fbits zgaz3300;
    {
      sail_int zgsz3657;
      CREATE(sail_int)(&zgsz3657);
      CONVERT_OF(sail_int, mach_int)(&zgsz3657, INT64_C(6));
      lbits zgsz3656;
      CREATE(lbits)(&zgsz3656);
      CONVERT_OF(lbits, fbits)(&zgsz3656, zperms, UINT64_C(15) , true);
      zgaz3300 = bitvector_access(zgsz3656, zgsz3657);
      KILL(sail_int)(&zgsz3657);
      KILL(lbits)(&zgsz3656);
    }
    zgaz3301 = sailgen_bit_to_bool(zgaz3300);
  
  }
  bool zgaz3303;
  {
    fbits zgaz3302;
    {
      sail_int zgsz3659;
      CREATE(sail_int)(&zgsz3659);
      CONVERT_OF(sail_int, mach_int)(&zgsz3659, INT64_C(5));
      lbits zgsz3658;
      CREATE(lbits)(&zgsz3658);
      CONVERT_OF(lbits, fbits)(&zgsz3658, zperms, UINT64_C(15) , true);
      zgaz3302 = bitvector_access(zgsz3658, zgsz3659);
      KILL(sail_int)(&zgsz3659);
      KILL(lbits)(&zgsz3658);
    }
    zgaz3303 = sailgen_bit_to_bool(zgaz3302);
  
  }
  bool zgaz3305;
  {
    fbits zgaz3304;
    {
      sail_int zgsz3661;
      CREATE(sail_int)(&zgsz3661);
      CONVERT_OF(sail_int, mach_int)(&zgsz3661, INT64_C(4));
      lbits zgsz3660;
      CREATE(lbits)(&zgsz3660);
      CONVERT_OF(lbits, fbits)(&zgsz3660, zperms, UINT64_C(15) , true);
      zgaz3304 = bitvector_access(zgsz3660, zgsz3661);
      KILL(sail_int)(&zgsz3661);
      KILL(lbits)(&zgsz3660);
    }
    zgaz3305 = sailgen_bit_to_bool(zgaz3304);
  
  }
  bool zgaz3307;
  {
    fbits zgaz3306;
    {
      sail_int zgsz3663;
      CREATE(sail_int)(&zgsz3663);
      CONVERT_OF(sail_int, mach_int)(&zgsz3663, INT64_C(3));
      lbits zgsz3662;
      CREATE(lbits)(&zgsz3662);
      CONVERT_OF(lbits, fbits)(&zgsz3662, zperms, UINT64_C(15) , true);
      zgaz3306 = bitvector_access(zgsz3662, zgsz3663);
      KILL(sail_int)(&zgsz3663);
      KILL(lbits)(&zgsz3662);
    }
    zgaz3307 = sailgen_bit_to_bool(zgaz3306);
  
  }
  bool zgaz3309;
  {
    fbits zgaz3308;
    {
      sail_int zgsz3665;
      CREATE(sail_int)(&zgsz3665);
      CONVERT_OF(sail_int, mach_int)(&zgsz3665, INT64_C(2));
      lbits zgsz3664;
      CREATE(lbits)(&zgsz3664);
      CONVERT_OF(lbits, fbits)(&zgsz3664, zperms, UINT64_C(15) , true);
      zgaz3308 = bitvector_access(zgsz3664, zgsz3665);
      KILL(sail_int)(&zgsz3665);
      KILL(lbits)(&zgsz3664);
    }
    zgaz3309 = sailgen_bit_to_bool(zgaz3308);
  
  }
  bool zgaz3311;
  {
    fbits zgaz3310;
    {
      sail_int zgsz3667;
      CREATE(sail_int)(&zgsz3667);
      CONVERT_OF(sail_int, mach_int)(&zgsz3667, INT64_C(1));
      lbits zgsz3666;
      CREATE(lbits)(&zgsz3666);
      CONVERT_OF(lbits, fbits)(&zgsz3666, zperms, UINT64_C(15) , true);
      zgaz3310 = bitvector_access(zgsz3666, zgsz3667);
      KILL(sail_int)(&zgsz3667);
      KILL(lbits)(&zgsz3666);
    }
    zgaz3311 = sailgen_bit_to_bool(zgaz3310);
  
  }
  bool zgaz3313;
  {
    fbits zgaz3312;
    {
      sail_int zgsz3669;
      CREATE(sail_int)(&zgsz3669);
      CONVERT_OF(sail_int, mach_int)(&zgsz3669, INT64_C(0));
      lbits zgsz3668;
      CREATE(lbits)(&zgsz3668);
      CONVERT_OF(lbits, fbits)(&zgsz3668, zperms, UINT64_C(15) , true);
      zgaz3312 = bitvector_access(zgsz3668, zgsz3669);
      KILL(sail_int)(&zgsz3669);
      KILL(lbits)(&zgsz3668);
    }
    zgaz3313 = sailgen_bit_to_bool(zgaz3312);
  
  }
  struct zCapability zgsz3670;
  zgsz3670 = zcap;
  zgsz3670.zaccess_system_regs = zgaz3293;
  zgsz3670.zglobal = zgaz3313;
  zgsz3670.zpermit_cinvoke = zgaz3297;
  zgsz3670.zpermit_execute = zgaz3311;
  zgsz3670.zpermit_load = zgaz3309;
  zgsz3670.zpermit_load_cap = zgaz3305;
  zgsz3670.zpermit_seal = zgaz3299;
  zgsz3670.zpermit_set_CID = zgaz3291;
  zgsz3670.zpermit_store = zgaz3307;
  zgsz3670.zpermit_store_cap = zgaz3303;
  zgsz3670.zpermit_store_local_cap = zgaz3301;
  zgsz3670.zpermit_unseal = zgaz3295;
  zgsz3670.zuperms = zgaz3289;
  zcbz346 = zgsz3670;














end_function_182: ;
  return zcbz346;
end_block_exception_183: ;
  struct zCapability zcbz369 = { .zuperms = UINT64_C(0xdeadc0de), .ztag = false, .zreserved = UINT64_C(0xdeadc0de), .zpermit_unseal = false, .zpermit_store_local_cap = false, .zpermit_store_cap = false, .zpermit_store = false, .zpermit_set_CID = false, .zpermit_seal = false, .zpermit_load_cap = false, .zpermit_load = false, .zpermit_execute = false, .zpermit_cinvoke = false, .zotype = UINT64_C(0xdeadc0de), .zinternal_E = false, .zglobal = false, .zflag_cap_mode = false, .zaddress = UINT64_C(0xdeadc0de), .zaccess_system_regs = false, .zT = UINT64_C(0xdeadc0de), .zE = UINT64_C(0xdeadc0de), .zB = UINT64_C(0xdeadc0de) };
  return zcbz369;
}

static uint64_t sailgen_getCapFlags(struct zCapability);

static uint64_t sailgen_getCapFlags(struct zCapability zcap)
{
  __label__ end_function_185, end_block_exception_186;

  uint64_t zcbz347;
  bool zgaz3314;
  zgaz3314 = zcap.zflag_cap_mode;
  zcbz347 = sailgen_bool_to_bits(zgaz3314);

end_function_185: ;
  return zcbz347;
end_block_exception_186: ;

  return UINT64_C(0xdeadc0de);
}

static bool sailgen_isCapSealed(struct zCapability);

static bool sailgen_isCapSealed(struct zCapability zcap)
{
  __label__ end_function_188, end_block_exception_189;

  bool zcbz348;
  int64_t zgaz3316;
  {
    uint64_t zgaz3315;
    zgaz3315 = zcap.zotype;
    zgaz3316 = fast_signed(zgaz3315, 4);
  
  }
  {
    sail_int zgsz3671;
    CREATE(sail_int)(&zgsz3671);
    CONVERT_OF(sail_int, mach_int)(&zgsz3671, zgaz3316);
    zcbz348 = sailgen_zzz3zzzz7mzzJzzKzzD1z3neq_int(zgsz3671, zotype_unsealed);
    KILL(sail_int)(&zgsz3671);
  }

end_function_188: ;
  return zcbz348;
end_block_exception_189: ;

  return false;
}

static bool sailgen_hasReservedOType(struct zCapability);

static bool sailgen_hasReservedOType(struct zCapability zcap)
{
  __label__ end_function_191, end_block_exception_192;

  bool zcbz349;
  int64_t zgaz3318;
  {
    uint64_t zgaz3317;
    zgaz3317 = zcap.zotype;
    zgaz3318 = ((mach_int) zgaz3317);
  
  }
  {
    sail_int zgsz3673;
    CREATE(sail_int)(&zgsz3673);
    CONVERT_OF(sail_int, mach_int)(&zgsz3673, zcap_max_otype);
    sail_int zgsz3672;
    CREATE(sail_int)(&zgsz3672);
    CONVERT_OF(sail_int, mach_int)(&zgsz3672, zgaz3318);
    zcbz349 = gt(zgsz3672, zgsz3673);
    KILL(sail_int)(&zgsz3673);
    KILL(sail_int)(&zgsz3672);
  }

end_function_191: ;
  return zcbz349;
end_block_exception_192: ;

  return false;
}

static uint64_t sailgen_getCapBaseBits(struct zCapability);

static uint64_t sailgen_getCapBaseBits(struct zCapability zc)
{
  __label__ case_195, finish_match_194, end_function_196, end_block_exception_197;

  uint64_t zcbz350;
  struct ztuple_z8z5bv32zCz0z5bv33z9 zgaz3319;
  zgaz3319 = sailgen_getCapBoundsBits(zc);
  uint64_t zgsz3674;
  {
    uint64_t zbase;
    zbase = zgaz3319.ztup0;
    zgsz3674 = zbase;
  
    goto finish_match_194;
  }
case_195: ;
  sail_match_failure("getCapBaseBits");
finish_match_194: ;
  zcbz350 = zgsz3674;


end_function_196: ;
  return zcbz350;
end_block_exception_197: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_getCapOffsetBits(struct zCapability);

static uint64_t sailgen_getCapOffsetBits(struct zCapability zc)
{
  __label__ end_function_199, end_block_exception_200;

  uint64_t zcbz351;
  uint64_t zbase;
  zbase = sailgen_getCapBaseBits(zc);
  uint64_t zgaz3320;
  zgaz3320 = zc.zaddress;
  {
    lbits zgsz3676;
    CREATE(lbits)(&zgsz3676);
    CONVERT_OF(lbits, fbits)(&zgsz3676, zgaz3320, UINT64_C(32) , true);
    lbits zgsz3677;
    CREATE(lbits)(&zgsz3677);
    CONVERT_OF(lbits, fbits)(&zgsz3677, zbase, UINT64_C(32) , true);
    lbits zgsz3678;
    CREATE(lbits)(&zgsz3678);
    sub_bits(&zgsz3678, zgsz3676, zgsz3677);
    zcbz351 = CONVERT_OF(fbits, lbits)(zgsz3678, true);
    KILL(lbits)(&zgsz3678);
    KILL(lbits)(&zgsz3677);
    KILL(lbits)(&zgsz3676);
  }


end_function_199: ;
  return zcbz351;
end_block_exception_200: ;

  return UINT64_C(0xdeadc0de);
}

static int64_t sailgen_getCapLength(struct zCapability);

static int64_t sailgen_getCapLength(struct zCapability zc)
{
  __label__ case_203, finish_match_202, end_function_204, end_block_exception_205;

  int64_t zcbz352;
  struct ztuple_z8z5i64zCz0z5i64z9 zgaz3321;
  zgaz3321 = sailgen_getCapBounds(zc);
  int64_t zgsz3679;
  {
    int64_t zbase;
    zbase = zgaz3321.ztup0;
    int64_t ztop;
    ztop = zgaz3321.ztup1;
    {
      bool zgaz3324;
      {
        bool zgaz3323;
        {
          bool zgaz3322;
          zgaz3322 = zc.ztag;
          zgaz3323 = not(zgaz3322);
        
        }
        bool zgsz3680;
        if (zgaz3323) {  zgsz3680 = true;  } else {
        {
          sail_int zgsz3682;
          CREATE(sail_int)(&zgsz3682);
          CONVERT_OF(sail_int, mach_int)(&zgsz3682, zbase);
          sail_int zgsz3681;
          CREATE(sail_int)(&zgsz3681);
          CONVERT_OF(sail_int, mach_int)(&zgsz3681, ztop);
          zgsz3680 = gteq(zgsz3681, zgsz3682);
          KILL(sail_int)(&zgsz3682);
          KILL(sail_int)(&zgsz3681);
        }
        }
        zgaz3324 = zgsz3680;
      
      }
      unit zgsz3683;
      zgsz3683 = sail_assert(zgaz3324, "src/cheri_cap_common.sail 425:40 - 425:41");
    
    }
    int64_t zgaz3325;
    {
      sail_int zgsz3684;
      CREATE(sail_int)(&zgsz3684);
      CONVERT_OF(sail_int, mach_int)(&zgsz3684, ztop);
      sail_int zgsz3685;
      CREATE(sail_int)(&zgsz3685);
      CONVERT_OF(sail_int, mach_int)(&zgsz3685, zbase);
      sail_int zgsz3686;
      CREATE(sail_int)(&zgsz3686);
      sub_int(&zgsz3686, zgsz3684, zgsz3685);
      zgaz3325 = CONVERT_OF(mach_int, sail_int)(zgsz3686);
      KILL(sail_int)(&zgsz3686);
      KILL(sail_int)(&zgsz3685);
      KILL(sail_int)(&zgsz3684);
    }
    int64_t zgaz3326;
    {
      sail_int zgsz3687;
      CREATE(sail_int)(&zgsz3687);
      CONVERT_OF(sail_int, mach_int)(&zgsz3687, zcap_len_width);
      sail_int zgsz3688;
      CREATE(sail_int)(&zgsz3688);
      pow2(&zgsz3688, zgsz3687);
      zgaz3326 = CONVERT_OF(mach_int, sail_int)(zgsz3688);
      KILL(sail_int)(&zgsz3688);
      KILL(sail_int)(&zgsz3687);
    }
    {
      sail_int zgsz3690;
      CREATE(sail_int)(&zgsz3690);
      CONVERT_OF(sail_int, mach_int)(&zgsz3690, zgaz3325);
      sail_int zgsz3691;
      CREATE(sail_int)(&zgsz3691);
      CONVERT_OF(sail_int, mach_int)(&zgsz3691, zgaz3326);
      sail_int zgsz3692;
      CREATE(sail_int)(&zgsz3692);
      emod_int(&zgsz3692, zgsz3690, zgsz3691);
      zgsz3679 = CONVERT_OF(mach_int, sail_int)(zgsz3692);
      KILL(sail_int)(&zgsz3692);
      KILL(sail_int)(&zgsz3691);
      KILL(sail_int)(&zgsz3690);
    }
  
  
  
  
    goto finish_match_202;
  }
case_203: ;
  sail_match_failure("getCapLength");
finish_match_202: ;
  zcbz352 = zgsz3679;


end_function_204: ;
  return zcbz352;
end_block_exception_205: ;

  return INT64_C(0xdeadc0de);
}

static bool sailgen_fastRepCheck(struct zCapability, uint64_t);

static bool sailgen_fastRepCheck(struct zCapability zc, uint64_t zi)
{
  __label__ end_function_207, end_block_exception_208;

  bool zcbz353;
  int64_t zE;
  {
    uint64_t zgaz3340;
    zgaz3340 = zc.zE;
    zE = ((mach_int) zgaz3340);
  
  }
  bool zgaz3328;
  {
    int64_t zgaz3327;
    {
      sail_int zgsz3693;
      CREATE(sail_int)(&zgsz3693);
      CONVERT_OF(sail_int, mach_int)(&zgsz3693, zcap_max_E);
      sail_int zgsz3694;
      CREATE(sail_int)(&zgsz3694);
      CONVERT_OF(sail_int, mach_int)(&zgsz3694, INT64_C(2));
      sail_int zgsz3695;
      CREATE(sail_int)(&zgsz3695);
      sub_int(&zgsz3695, zgsz3693, zgsz3694);
      zgaz3327 = CONVERT_OF(mach_int, sail_int)(zgsz3695);
      KILL(sail_int)(&zgsz3695);
      KILL(sail_int)(&zgsz3694);
      KILL(sail_int)(&zgsz3693);
    }
    {
      sail_int zgsz3697;
      CREATE(sail_int)(&zgsz3697);
      CONVERT_OF(sail_int, mach_int)(&zgsz3697, zgaz3327);
      sail_int zgsz3696;
      CREATE(sail_int)(&zgsz3696);
      CONVERT_OF(sail_int, mach_int)(&zgsz3696, zE);
      zgaz3328 = gteq(zgsz3696, zgsz3697);
      KILL(sail_int)(&zgsz3697);
      KILL(sail_int)(&zgsz3696);
    }
  
  }
  if (zgaz3328) {  zcbz353 = true;  } else {
  int64_t zi_top;
  {
    uint64_t zgaz3339;
    {
      int64_t zgaz3338;
      {
        sail_int zgsz3698;
        CREATE(sail_int)(&zgsz3698);
        CONVERT_OF(sail_int, mach_int)(&zgsz3698, zE);
        sail_int zgsz3699;
        CREATE(sail_int)(&zgsz3699);
        CONVERT_OF(sail_int, mach_int)(&zgsz3699, zcap_mantissa_width);
        sail_int zgsz3700;
        CREATE(sail_int)(&zgsz3700);
        add_int(&zgsz3700, zgsz3698, zgsz3699);
        zgaz3338 = CONVERT_OF(mach_int, sail_int)(zgsz3700);
        KILL(sail_int)(&zgsz3700);
        KILL(sail_int)(&zgsz3699);
        KILL(sail_int)(&zgsz3698);
      }
      {
        lbits zgsz3701;
        CREATE(lbits)(&zgsz3701);
        CONVERT_OF(lbits, fbits)(&zgsz3701, zi, UINT64_C(32) , true);
        sail_int zgsz3702;
        CREATE(sail_int)(&zgsz3702);
        CONVERT_OF(sail_int, mach_int)(&zgsz3702, zgaz3338);
        lbits zgsz3703;
        CREATE(lbits)(&zgsz3703);
        arith_shiftr(&zgsz3703, zgsz3701, zgsz3702);
        zgaz3339 = CONVERT_OF(fbits, lbits)(zgsz3703, true);
        KILL(lbits)(&zgsz3703);
        KILL(sail_int)(&zgsz3702);
        KILL(lbits)(&zgsz3701);
      }
    
    }
    zi_top = fast_signed(zgaz3339, 32);
  
  }
  uint64_t zi_mid;
  {
    uint64_t zgaz3337;
    {
      lbits zgsz3704;
      CREATE(lbits)(&zgsz3704);
      CONVERT_OF(lbits, fbits)(&zgsz3704, zi, UINT64_C(32) , true);
      sail_int zgsz3705;
      CREATE(sail_int)(&zgsz3705);
      CONVERT_OF(sail_int, mach_int)(&zgsz3705, zE);
      lbits zgsz3706;
      CREATE(lbits)(&zgsz3706);
      shiftr(&zgsz3706, zgsz3704, zgsz3705);
      zgaz3337 = CONVERT_OF(fbits, lbits)(zgsz3706, true);
      KILL(lbits)(&zgsz3706);
      KILL(sail_int)(&zgsz3705);
      KILL(lbits)(&zgsz3704);
    }
    {
      lbits zgsz3707;
      CREATE(lbits)(&zgsz3707);
      CONVERT_OF(lbits, fbits)(&zgsz3707, zgaz3337, UINT64_C(32) , true);
      sail_int zgsz3708;
      CREATE(sail_int)(&zgsz3708);
      CONVERT_OF(sail_int, mach_int)(&zgsz3708, zcap_mantissa_width);
      lbits zgsz3709;
      CREATE(lbits)(&zgsz3709);
      sail_truncate(&zgsz3709, zgsz3707, zgsz3708);
      zi_mid = CONVERT_OF(fbits, lbits)(zgsz3709, true);
      KILL(lbits)(&zgsz3709);
      KILL(sail_int)(&zgsz3708);
      KILL(lbits)(&zgsz3707);
    }
  
  }
  uint64_t za_mid;
  {
    uint64_t zgaz3336;
    {
      uint64_t zgaz3335;
      zgaz3335 = zc.zaddress;
      {
        lbits zgsz3710;
        CREATE(lbits)(&zgsz3710);
        CONVERT_OF(lbits, fbits)(&zgsz3710, zgaz3335, UINT64_C(32) , true);
        sail_int zgsz3711;
        CREATE(sail_int)(&zgsz3711);
        CONVERT_OF(sail_int, mach_int)(&zgsz3711, zE);
        lbits zgsz3712;
        CREATE(lbits)(&zgsz3712);
        shiftr(&zgsz3712, zgsz3710, zgsz3711);
        zgaz3336 = CONVERT_OF(fbits, lbits)(zgsz3712, true);
        KILL(lbits)(&zgsz3712);
        KILL(sail_int)(&zgsz3711);
        KILL(lbits)(&zgsz3710);
      }
    
    }
    {
      lbits zgsz3713;
      CREATE(lbits)(&zgsz3713);
      CONVERT_OF(lbits, fbits)(&zgsz3713, zgaz3336, UINT64_C(32) , true);
      sail_int zgsz3714;
      CREATE(sail_int)(&zgsz3714);
      CONVERT_OF(sail_int, mach_int)(&zgsz3714, zcap_mantissa_width);
      lbits zgsz3715;
      CREATE(lbits)(&zgsz3715);
      sail_truncate(&zgsz3715, zgsz3713, zgsz3714);
      za_mid = CONVERT_OF(fbits, lbits)(zgsz3715, true);
      KILL(lbits)(&zgsz3715);
      KILL(sail_int)(&zgsz3714);
      KILL(lbits)(&zgsz3713);
    }
  
  }
  uint64_t zB3;
  {
    uint64_t zgaz3334;
    zgaz3334 = zc.zB;
    {
      lbits zgsz3716;
      CREATE(lbits)(&zgsz3716);
      CONVERT_OF(lbits, fbits)(&zgsz3716, zgaz3334, UINT64_C(8) , true);
      sail_int zgsz3717;
      CREATE(sail_int)(&zgsz3717);
      CONVERT_OF(sail_int, mach_int)(&zgsz3717, INT64_C(3));
      lbits zgsz3718;
      CREATE(lbits)(&zgsz3718);
      sail_truncateLSB(&zgsz3718, zgsz3716, zgsz3717);
      zB3 = CONVERT_OF(fbits, lbits)(zgsz3718, true);
      KILL(lbits)(&zgsz3718);
      KILL(sail_int)(&zgsz3717);
      KILL(lbits)(&zgsz3716);
    }
  
  }
  uint64_t zR3;
  {
    lbits zgsz3719;
    CREATE(lbits)(&zgsz3719);
    CONVERT_OF(lbits, fbits)(&zgsz3719, zB3, UINT64_C(3) , true);
    lbits zgsz3720;
    CREATE(lbits)(&zgsz3720);
    CONVERT_OF(lbits, fbits)(&zgsz3720, UINT64_C(0b001), UINT64_C(3) , true);
    lbits zgsz3721;
    CREATE(lbits)(&zgsz3721);
    sub_bits(&zgsz3721, zgsz3719, zgsz3720);
    zR3 = CONVERT_OF(fbits, lbits)(zgsz3721, true);
    KILL(lbits)(&zgsz3721);
    KILL(lbits)(&zgsz3720);
    KILL(lbits)(&zgsz3719);
  }
  uint64_t zR;
  {
    uint64_t zgaz3333;
    {
      int64_t zgaz3332;
      {
        sail_int zgsz3722;
        CREATE(sail_int)(&zgsz3722);
        CONVERT_OF(sail_int, mach_int)(&zgsz3722, zcap_mantissa_width);
        sail_int zgsz3723;
        CREATE(sail_int)(&zgsz3723);
        CONVERT_OF(sail_int, mach_int)(&zgsz3723, INT64_C(3));
        sail_int zgsz3724;
        CREATE(sail_int)(&zgsz3724);
        sub_int(&zgsz3724, zgsz3722, zgsz3723);
        zgaz3332 = CONVERT_OF(mach_int, sail_int)(zgsz3724);
        KILL(sail_int)(&zgsz3724);
        KILL(sail_int)(&zgsz3723);
        KILL(sail_int)(&zgsz3722);
      }
      zgaz3333 = sailgen_zzzz7nzzJzzK5z3zzeros_implicit(zgaz3332);
    
    }
    {
      lbits zgsz3725;
      CREATE(lbits)(&zgsz3725);
      CONVERT_OF(lbits, fbits)(&zgsz3725, zR3, UINT64_C(3) , true);
      lbits zgsz3726;
      CREATE(lbits)(&zgsz3726);
      CONVERT_OF(lbits, fbits)(&zgsz3726, zgaz3333, UINT64_C(5) , true);
      lbits zgsz3727;
      CREATE(lbits)(&zgsz3727);
      append(&zgsz3727, zgsz3725, zgsz3726);
      zR = CONVERT_OF(fbits, lbits)(zgsz3727, true);
      KILL(lbits)(&zgsz3727);
      KILL(lbits)(&zgsz3726);
      KILL(lbits)(&zgsz3725);
    }
  
  }
  uint64_t zdiff;
  {
    lbits zgsz3728;
    CREATE(lbits)(&zgsz3728);
    CONVERT_OF(lbits, fbits)(&zgsz3728, zR, UINT64_C(8) , true);
    lbits zgsz3729;
    CREATE(lbits)(&zgsz3729);
    CONVERT_OF(lbits, fbits)(&zgsz3729, za_mid, UINT64_C(8) , true);
    lbits zgsz3730;
    CREATE(lbits)(&zgsz3730);
    sub_bits(&zgsz3730, zgsz3728, zgsz3729);
    zdiff = CONVERT_OF(fbits, lbits)(zgsz3730, true);
    KILL(lbits)(&zgsz3730);
    KILL(lbits)(&zgsz3729);
    KILL(lbits)(&zgsz3728);
  }
  uint64_t zdiff1;
  {
    lbits zgsz3731;
    CREATE(lbits)(&zgsz3731);
    CONVERT_OF(lbits, fbits)(&zgsz3731, zdiff, UINT64_C(8) , true);
    sail_int zgsz3732;
    CREATE(sail_int)(&zgsz3732);
    CONVERT_OF(sail_int, mach_int)(&zgsz3732, INT64_C(1));
    lbits zgsz3733;
    CREATE(lbits)(&zgsz3733);
    sub_bits_int(&zgsz3733, zgsz3731, zgsz3732);
    zdiff1 = CONVERT_OF(fbits, lbits)(zgsz3733, true);
    KILL(lbits)(&zgsz3733);
    KILL(sail_int)(&zgsz3732);
    KILL(lbits)(&zgsz3731);
  }
  bool zgaz3329;
  {
    sail_int zgsz3735;
    CREATE(sail_int)(&zgsz3735);
    CONVERT_OF(sail_int, mach_int)(&zgsz3735, INT64_C(0));
    sail_int zgsz3734;
    CREATE(sail_int)(&zgsz3734);
    CONVERT_OF(sail_int, mach_int)(&zgsz3734, zi_top);
    zgaz3329 = eq_int(zgsz3734, zgsz3735);
    KILL(sail_int)(&zgsz3735);
    KILL(sail_int)(&zgsz3734);
  }
  if (zgaz3329) {  zcbz353 = sailgen_z8operatorz0zzzz7nzzJzzK8z3zI_uz9(zi_mid, zdiff1);  } else {
  bool zgaz3330;
  {
    sail_int zgsz3737;
    CREATE(sail_int)(&zgsz3737);
    CONVERT_OF(sail_int, mach_int)(&zgsz3737, INT64_C(-1));
    sail_int zgsz3736;
    CREATE(sail_int)(&zgsz3736);
    CONVERT_OF(sail_int, mach_int)(&zgsz3736, zi_top);
    zgaz3330 = eq_int(zgsz3736, zgsz3737);
    KILL(sail_int)(&zgsz3737);
    KILL(sail_int)(&zgsz3736);
  }
  if (zgaz3330) {
  bool zgaz3331;
  zgaz3331 = sailgen_z8operatorz0zzzz7nzzJzzK8z3zKzJ_uz9(zi_mid, zdiff);
  bool zgsz3738;
  if (zgaz3331) {
  {
    lbits zgsz3740;
    CREATE(lbits)(&zgsz3740);
    CONVERT_OF(lbits, fbits)(&zgsz3740, za_mid, UINT64_C(8) , true);
    lbits zgsz3739;
    CREATE(lbits)(&zgsz3739);
    CONVERT_OF(lbits, fbits)(&zgsz3739, zR, UINT64_C(8) , true);
    zgsz3738 = neq_bits(zgsz3739, zgsz3740);
    KILL(lbits)(&zgsz3740);
    KILL(lbits)(&zgsz3739);
  }
  } else {  zgsz3738 = false;  }
  zcbz353 = zgsz3738;

  } else {  zcbz353 = false;  }

  }









  }


end_function_207: ;
  return zcbz353;
end_block_exception_208: ;

  return false;
}

static void sailgen_capToString(sail_string *rop, struct zCapability);

static void sailgen_capToString(sail_string *zcbz354, struct zCapability zcap)
{
  __label__ end_function_210, end_block_exception_211, end_function_230;

  int64_t zlen;
  zlen = sailgen_getCapLength(zcap);
  if (have_exception) {

  goto end_block_exception_211;
  }
  sail_string zlen_str;
  CREATE(sail_string)(&zlen_str);
  {
    uint64_t zgaz3369;
    {
      int64_t zgaz3368;
      {
        sail_int zgsz3741;
        CREATE(sail_int)(&zgsz3741);
        CONVERT_OF(sail_int, mach_int)(&zgsz3741, zcap_len_width);
        sail_int zgsz3742;
        CREATE(sail_int)(&zgsz3742);
        CONVERT_OF(sail_int, mach_int)(&zgsz3742, INT64_C(3));
        sail_int zgsz3743;
        CREATE(sail_int)(&zgsz3743);
        add_int(&zgsz3743, zgsz3741, zgsz3742);
        zgaz3368 = CONVERT_OF(mach_int, sail_int)(zgsz3743);
        KILL(sail_int)(&zgsz3743);
        KILL(sail_int)(&zgsz3742);
        KILL(sail_int)(&zgsz3741);
      }
      {
        sail_int zgsz3744;
        CREATE(sail_int)(&zgsz3744);
        CONVERT_OF(sail_int, mach_int)(&zgsz3744, zlen);
        zgaz3369 = sailgen_zzzz7lzzJzzK36z3to_bits(zgaz3368, zgsz3744);
        KILL(sail_int)(&zgsz3744);
      }
    
    }
    {
      lbits zgsz3745;
      CREATE(lbits)(&zgsz3745);
      CONVERT_OF(lbits, fbits)(&zgsz3745, zgaz3369, UINT64_C(36) , true);
      string_of_lbits(&zlen_str, zgsz3745);
      KILL(lbits)(&zgsz3745);
    }
  
  }
  uint64_t zotype64;
  {
    bool zgaz3365;
    zgaz3365 = sailgen_hasReservedOType(zcap);
    if (zgaz3365) {
    uint64_t zgaz3366;
    zgaz3366 = zcap.zotype;
    zotype64 = sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTS(INT64_C(32), zgaz3366);
  
    } else {
    uint64_t zgaz3367;
    zgaz3367 = zcap.zotype;
    zotype64 = sailgen_zzzz7mzzJzzK32zzCzz0zz7nzzJzzK4z3EXTZ(INT64_C(32), zgaz3367);
  
    }
  
  }
  sail_string zgaz3364;
  CREATE(sail_string)(&zgaz3364);
  {
    sail_string zgaz3362;
    CREATE(sail_string)(&zgaz3362);
    {
      bool zgaz3341;
      zgaz3341 = zcap.ztag;
      if (zgaz3341) {  COPY(sail_string)(&zgaz3362, "1");  } else {  COPY(sail_string)(&zgaz3362, "0");  }
    
    }
    sail_string zgaz3363;
    CREATE(sail_string)(&zgaz3363);
    {
      sail_string zgaz3361;
      CREATE(sail_string)(&zgaz3361);
      {
        sail_string zgaz3359;
        CREATE(sail_string)(&zgaz3359);
        {
          bool zgaz3342;
          zgaz3342 = sailgen_isCapSealed(zcap);
          if (zgaz3342) {  COPY(sail_string)(&zgaz3359, "1");  } else {  COPY(sail_string)(&zgaz3359, "0");  }
        
        }
        sail_string zgaz3360;
        CREATE(sail_string)(&zgaz3360);
        {
          sail_string zgaz3358;
          CREATE(sail_string)(&zgaz3358);
          {
            sail_string zgaz3356;
            CREATE(sail_string)(&zgaz3356);
            {
              uint64_t zgaz3344;
              {
                uint64_t zgaz3343;
                zgaz3343 = sailgen_getCapPerms(zcap);
                {
                  lbits zgsz3746;
                  CREATE(lbits)(&zgsz3746);
                  CONVERT_OF(lbits, fbits)(&zgsz3746, UINT64_C(0b0), UINT64_C(1) , true);
                  lbits zgsz3747;
                  CREATE(lbits)(&zgsz3747);
                  CONVERT_OF(lbits, fbits)(&zgsz3747, zgaz3343, UINT64_C(15) , true);
                  lbits zgsz3748;
                  CREATE(lbits)(&zgsz3748);
                  append(&zgsz3748, zgsz3746, zgsz3747);
                  zgaz3344 = CONVERT_OF(fbits, lbits)(zgsz3748, true);
                  KILL(lbits)(&zgsz3748);
                  KILL(lbits)(&zgsz3747);
                  KILL(lbits)(&zgsz3746);
                }
              
              }
              {
                lbits zgsz3749;
                CREATE(lbits)(&zgsz3749);
                CONVERT_OF(lbits, fbits)(&zgsz3749, zgaz3344, UINT64_C(16) , true);
                string_of_lbits(&zgaz3356, zgsz3749);
                KILL(lbits)(&zgsz3749);
              }
            
            }
            sail_string zgaz3357;
            CREATE(sail_string)(&zgaz3357);
            {
              sail_string zgaz3355;
              CREATE(sail_string)(&zgaz3355);
              {
                sail_string zgaz3353;
                CREATE(sail_string)(&zgaz3353);
                {
                  lbits zgsz3750;
                  CREATE(lbits)(&zgsz3750);
                  CONVERT_OF(lbits, fbits)(&zgsz3750, zotype64, UINT64_C(32) , true);
                  string_of_lbits(&zgaz3353, zgsz3750);
                  KILL(lbits)(&zgsz3750);
                }
                sail_string zgaz3354;
                CREATE(sail_string)(&zgaz3354);
                {
                  sail_string zgaz3352;
                  CREATE(sail_string)(&zgaz3352);
                  {
                    sail_string zgaz3350;
                    CREATE(sail_string)(&zgaz3350);
                    {
                      uint64_t zgaz3345;
                      zgaz3345 = sailgen_getCapOffsetBits(zcap);
                      {
                        lbits zgsz3751;
                        CREATE(lbits)(&zgsz3751);
                        CONVERT_OF(lbits, fbits)(&zgsz3751, zgaz3345, UINT64_C(32) , true);
                        string_of_lbits(&zgaz3350, zgsz3751);
                        KILL(lbits)(&zgsz3751);
                      }
                    
                    }
                    sail_string zgaz3351;
                    CREATE(sail_string)(&zgaz3351);
                    {
                      sail_string zgaz3349;
                      CREATE(sail_string)(&zgaz3349);
                      {
                        sail_string zgaz3347;
                        CREATE(sail_string)(&zgaz3347);
                        {
                          uint64_t zgaz3346;
                          zgaz3346 = sailgen_getCapBaseBits(zcap);
                          {
                            lbits zgsz3752;
                            CREATE(lbits)(&zgsz3752);
                            CONVERT_OF(lbits, fbits)(&zgsz3752, zgaz3346, UINT64_C(32) , true);
                            string_of_lbits(&zgaz3347, zgsz3752);
                            KILL(lbits)(&zgsz3752);
                          }
                        
                        }
                        sail_string zgaz3348;
                        CREATE(sail_string)(&zgaz3348);
                        concat_str(&zgaz3348, " length:", zlen_str);
                        concat_str(&zgaz3349, zgaz3347, zgaz3348);
                        KILL(sail_string)(&zgaz3348);
                        KILL(sail_string)(&zgaz3347);
                      }
                      concat_str(&zgaz3351, " base:", zgaz3349);
                      KILL(sail_string)(&zgaz3349);
                    }
                    concat_str(&zgaz3352, zgaz3350, zgaz3351);
                    KILL(sail_string)(&zgaz3351);
                    KILL(sail_string)(&zgaz3350);
                  }
                  concat_str(&zgaz3354, " offset:", zgaz3352);
                  KILL(sail_string)(&zgaz3352);
                }
                concat_str(&zgaz3355, zgaz3353, zgaz3354);
                KILL(sail_string)(&zgaz3354);
                KILL(sail_string)(&zgaz3353);
              }
              concat_str(&zgaz3357, " type:", zgaz3355);
              KILL(sail_string)(&zgaz3355);
            }
            concat_str(&zgaz3358, zgaz3356, zgaz3357);
            KILL(sail_string)(&zgaz3357);
            KILL(sail_string)(&zgaz3356);
          }
          concat_str(&zgaz3360, " perms:", zgaz3358);
          KILL(sail_string)(&zgaz3358);
        }
        concat_str(&zgaz3361, zgaz3359, zgaz3360);
        KILL(sail_string)(&zgaz3360);
        KILL(sail_string)(&zgaz3359);
      }
      concat_str(&zgaz3363, " s:", zgaz3361);
      KILL(sail_string)(&zgaz3361);
    }
    concat_str(&zgaz3364, zgaz3362, zgaz3363);
    KILL(sail_string)(&zgaz3363);
    KILL(sail_string)(&zgaz3362);
  }
  concat_str((*(&zcbz354)), " t:", zgaz3364);
  KILL(sail_string)(&zgaz3364);

  KILL(sail_string)(&zlen_str);

end_function_210: ;
  goto end_function_230;
end_block_exception_211: ;
  goto end_function_230;
end_function_230: ;
}

static uint64_t sailgen_getRepresentableAlignmentMask(uint64_t);

static uint64_t sailgen_getRepresentableAlignmentMask(uint64_t zlen)
{
  __label__ case_214, finish_match_213, end_function_215, end_block_exception_216;

  uint64_t zcbz355;
  struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgaz3372;
  {
    uint64_t zgaz3370;
    {
      sail_int zgsz3753;
      CREATE(sail_int)(&zgsz3753);
      CONVERT_OF(sail_int, mach_int)(&zgsz3753, INT64_C(32));
      sail_int zgsz3754;
      CREATE(sail_int)(&zgsz3754);
      CONVERT_OF(sail_int, mach_int)(&zgsz3754, INT64_C(0));
      lbits zgsz3755;
      CREATE(lbits)(&zgsz3755);
      sailgen_to_bits(&zgsz3755, zgsz3753, zgsz3754);
      zgaz3370 = CONVERT_OF(fbits, lbits)(zgsz3755, true);
      KILL(lbits)(&zgsz3755);
      KILL(sail_int)(&zgsz3754);
      KILL(sail_int)(&zgsz3753);
    }
    uint64_t zgaz3371;
    {
      lbits zgsz3756;
      CREATE(lbits)(&zgsz3756);
      CONVERT_OF(lbits, fbits)(&zgsz3756, UINT64_C(0b0), UINT64_C(1) , true);
      lbits zgsz3757;
      CREATE(lbits)(&zgsz3757);
      CONVERT_OF(lbits, fbits)(&zgsz3757, zlen, UINT64_C(32) , true);
      lbits zgsz3758;
      CREATE(lbits)(&zgsz3758);
      append(&zgsz3758, zgsz3756, zgsz3757);
      zgaz3371 = CONVERT_OF(fbits, lbits)(zgsz3758, true);
      KILL(lbits)(&zgsz3758);
      KILL(lbits)(&zgsz3757);
      KILL(lbits)(&zgsz3756);
    }
    zgaz3372 = sailgen_setCapBounds(zdefault_cap, zgaz3370, zgaz3371);
  
  
  }
  uint64_t zgsz3759;
  {
    bool zexact;
    zexact = zgaz3372.ztup0;
    struct zCapability zc;
    zc = zgaz3372.ztup1;
    int64_t ze;
    {
      int64_t zgaz3378;
      {
        uint64_t zgaz3377;
        zgaz3377 = zc.zE;
        zgaz3378 = ((mach_int) zgaz3377);
      
      }
      {
        sail_int zgsz3760;
        CREATE(sail_int)(&zgsz3760);
        CONVERT_OF(sail_int, mach_int)(&zgsz3760, zgaz3378);
        sail_int zgsz3761;
        CREATE(sail_int)(&zgsz3761);
        CONVERT_OF(sail_int, mach_int)(&zgsz3761, zcap_max_E);
        sail_int zgsz3762;
        CREATE(sail_int)(&zgsz3762);
        min_int(&zgsz3762, zgsz3760, zgsz3761);
        ze = CONVERT_OF(mach_int, sail_int)(zgsz3762);
        KILL(sail_int)(&zgsz3762);
        KILL(sail_int)(&zgsz3761);
        KILL(sail_int)(&zgsz3760);
      }
    
    }
    int64_t zez7;
    {
      bool zgaz3376;
      zgaz3376 = zc.zinternal_E;
      if (zgaz3376) {
      {
        sail_int zgsz3763;
        CREATE(sail_int)(&zgsz3763);
        CONVERT_OF(sail_int, mach_int)(&zgsz3763, ze);
        sail_int zgsz3764;
        CREATE(sail_int)(&zgsz3764);
        CONVERT_OF(sail_int, mach_int)(&zgsz3764, zinternal_E_take_bits);
        sail_int zgsz3765;
        CREATE(sail_int)(&zgsz3765);
        add_int(&zgsz3765, zgsz3763, zgsz3764);
        zez7 = CONVERT_OF(mach_int, sail_int)(zgsz3765);
        KILL(sail_int)(&zgsz3765);
        KILL(sail_int)(&zgsz3764);
        KILL(sail_int)(&zgsz3763);
      }
      } else {  zez7 = INT64_C(0);  }
    
    }
    sbits zgaz3374;
    {
      int64_t zgaz3373;
      {
        sail_int zgsz3766;
        CREATE(sail_int)(&zgsz3766);
        CONVERT_OF(sail_int, mach_int)(&zgsz3766, INT64_C(32));
        sail_int zgsz3767;
        CREATE(sail_int)(&zgsz3767);
        CONVERT_OF(sail_int, mach_int)(&zgsz3767, zez7);
        sail_int zgsz3768;
        CREATE(sail_int)(&zgsz3768);
        sub_int(&zgsz3768, zgsz3766, zgsz3767);
        zgaz3373 = CONVERT_OF(mach_int, sail_int)(zgsz3768);
        KILL(sail_int)(&zgsz3768);
        KILL(sail_int)(&zgsz3767);
        KILL(sail_int)(&zgsz3766);
      }
      {
        sail_int zgsz3769;
        CREATE(sail_int)(&zgsz3769);
        CONVERT_OF(sail_int, mach_int)(&zgsz3769, zgaz3373);
        lbits zgsz3770;
        CREATE(lbits)(&zgsz3770);
        sailgen_zzz3zzz3ones(&zgsz3770, zgsz3769);
        zgaz3374 = CONVERT_OF(sbits, lbits)(zgsz3770, true);
        KILL(lbits)(&zgsz3770);
        KILL(sail_int)(&zgsz3769);
      }
    
    }
    sbits zgaz3375;
    {
      sail_int zgsz3771;
      CREATE(sail_int)(&zgsz3771);
      CONVERT_OF(sail_int, mach_int)(&zgsz3771, zez7);
      lbits zgsz3772;
      CREATE(lbits)(&zgsz3772);
      sailgen_zzz3zzz3zzeros_implicit(&zgsz3772, zgsz3771);
      zgaz3375 = CONVERT_OF(sbits, lbits)(zgsz3772, true);
      KILL(lbits)(&zgsz3772);
      KILL(sail_int)(&zgsz3771);
    }
    {
      lbits zgsz3774;
      CREATE(lbits)(&zgsz3774);
      CONVERT_OF(lbits, sbits)(&zgsz3774, zgaz3374, true);
      lbits zgsz3775;
      CREATE(lbits)(&zgsz3775);
      CONVERT_OF(lbits, sbits)(&zgsz3775, zgaz3375, true);
      lbits zgsz3776;
      CREATE(lbits)(&zgsz3776);
      append(&zgsz3776, zgsz3774, zgsz3775);
      zgsz3759 = CONVERT_OF(fbits, lbits)(zgsz3776, true);
      KILL(lbits)(&zgsz3776);
      KILL(lbits)(&zgsz3775);
      KILL(lbits)(&zgsz3774);
    }
  
  
  
  
  
  
    goto finish_match_213;
  }
case_214: ;
  sail_match_failure("getRepresentableAlignmentMask");
finish_match_213: ;
  zcbz355 = zgsz3759;


end_function_215: ;
  return zcbz355;
end_block_exception_216: ;

  return UINT64_C(0xdeadc0de);
}

static uint64_t sailgen_getRepresentableLength(uint64_t);

static uint64_t sailgen_getRepresentableLength(uint64_t zlen)
{
  __label__ end_function_218, end_block_exception_219;

  uint64_t zcbz356;
  uint64_t zm;
  zm = sailgen_getRepresentableAlignmentMask(zlen);
  uint64_t zgaz3380;
  {
    uint64_t zgaz3379;
    {
      lbits zgsz3777;
      CREATE(lbits)(&zgsz3777);
      CONVERT_OF(lbits, fbits)(&zgsz3777, zm, UINT64_C(32) , true);
      lbits zgsz3778;
      CREATE(lbits)(&zgsz3778);
      not_bits(&zgsz3778, zgsz3777);
      zgaz3379 = CONVERT_OF(fbits, lbits)(zgsz3778, true);
      KILL(lbits)(&zgsz3778);
      KILL(lbits)(&zgsz3777);
    }
    {
      lbits zgsz3779;
      CREATE(lbits)(&zgsz3779);
      CONVERT_OF(lbits, fbits)(&zgsz3779, zlen, UINT64_C(32) , true);
      lbits zgsz3780;
      CREATE(lbits)(&zgsz3780);
      CONVERT_OF(lbits, fbits)(&zgsz3780, zgaz3379, UINT64_C(32) , true);
      lbits zgsz3781;
      CREATE(lbits)(&zgsz3781);
      add_bits(&zgsz3781, zgsz3779, zgsz3780);
      zgaz3380 = CONVERT_OF(fbits, lbits)(zgsz3781, true);
      KILL(lbits)(&zgsz3781);
      KILL(lbits)(&zgsz3780);
      KILL(lbits)(&zgsz3779);
    }
  
  }
  {
    lbits zgsz3782;
    CREATE(lbits)(&zgsz3782);
    CONVERT_OF(lbits, fbits)(&zgsz3782, zgaz3380, UINT64_C(32) , true);
    lbits zgsz3783;
    CREATE(lbits)(&zgsz3783);
    CONVERT_OF(lbits, fbits)(&zgsz3783, zm, UINT64_C(32) , true);
    lbits zgsz3784;
    CREATE(lbits)(&zgsz3784);
    and_bits(&zgsz3784, zgsz3782, zgsz3783);
    zcbz356 = CONVERT_OF(fbits, lbits)(zgsz3784, true);
    KILL(lbits)(&zgsz3784);
    KILL(lbits)(&zgsz3783);
    KILL(lbits)(&zgsz3782);
  }


end_function_218: ;
  return zcbz356;
end_block_exception_219: ;

  return UINT64_C(0xdeadc0de);
}

// register TestCap
static struct zCapability zTestCap;

// register TestAddr
static uint64_t zTestAddr;

// register TestLen
static uint64_t zTestLen;

static unit sailgen_main(unit);


static unit sailgen_main(unit zgsz3785)
{
  __label__ end_function_225, end_block_exception_226;

  unit zcbz357;
  {
    zTestCap = znull_cap;
    unit zgsz3786;
    zgsz3786 = UNIT;
  }
  uint64_t zbits1;
  zbits1 = sailgen_capToBits(zTestCap);
  uint64_t zbits2;
  zbits2 = sailgen_capToMemBits(zTestCap);
  struct zCapability zcap1;
  {
    bool zgaz3386;
    zgaz3386 = zTestCap.ztag;
    zcap1 = sailgen_capBitsToCapability(zgaz3386, zbits1);
  
  }
  struct zCapability zcap2;
  {
    bool zgaz3385;
    zgaz3385 = zTestCap.ztag;
    zcap2 = sailgen_memBitsToCapability(zgaz3385, zbits2);
  
  }
  bool zrep;
  zrep = sailgen_fastRepCheck(zTestCap, zTestAddr);
  {
    __label__ case_222, finish_match_221;

    struct ztuple_z8z5boolzCz0z5structz0zzCapabilityz9 zgaz3381;
    zgaz3381 = sailgen_setCapBounds(zTestCap, zTestAddr, zTestLen);
    unit zgsz3787;
    {
      bool ztup__0;
      ztup__0 = zgaz3381.ztup0;
      struct zCapability ztup__1;
      ztup__1 = zgaz3381.ztup1;
      bool zexact;
      zexact = ztup__0;
      struct zCapability zcap;
      zcap = ztup__1;
      zgsz3787 = UNIT;
    
    
    
    
      goto finish_match_221;
    }
  case_222: ;
    sail_match_failure("main");
  finish_match_221: ;
    unit zgsz3792;
    zgsz3792 = zgsz3787;
  
  
  }
  {
    __label__ case_224, finish_match_223;

    struct ztuple_z8z5i64zCz0z5i64z9 zgaz3382;
    zgaz3382 = sailgen_getCapBounds(zTestCap);
    unit zgsz3789;
    {
      int64_t ztup__0;
      ztup__0 = zgaz3382.ztup0;
      int64_t ztup__1;
      ztup__1 = zgaz3382.ztup1;
      int64_t zbase;
      zbase = ztup__0;
      int64_t ztop;
      ztop = ztup__1;
      zgsz3789 = UNIT;
    
    
    
    
      goto finish_match_223;
    }
  case_224: ;
    sail_match_failure("main");
  finish_match_223: ;
    unit zgsz3791;
    zgsz3791 = zgsz3789;
  
  
  }
  uint64_t zflags;
  zflags = sailgen_getCapFlags(zTestCap);
  struct zCapability znew_cap;
  {
    uint64_t zgaz3384;
    {
      int64_t zgaz3383;
      {
        sail_int zgsz3793;
        CREATE(sail_int)(&zgsz3793);
        CONVERT_OF(sail_int, mach_int)(&zgsz3793, INT64_C(15));
        sail_int zgsz3794;
        CREATE(sail_int)(&zgsz3794);
        CONVERT_OF(sail_int, mach_int)(&zgsz3794, INT64_C(0));
        sail_int zgsz3795;
        CREATE(sail_int)(&zgsz3795);
        add_int(&zgsz3795, zgsz3793, zgsz3794);
        zgaz3383 = CONVERT_OF(mach_int, sail_int)(zgsz3795);
        KILL(sail_int)(&zgsz3795);
        KILL(sail_int)(&zgsz3794);
        KILL(sail_int)(&zgsz3793);
      }
      {
        sail_int zgsz3796;
        CREATE(sail_int)(&zgsz3796);
        CONVERT_OF(sail_int, mach_int)(&zgsz3796, zgaz3383);
        lbits zgsz3797;
        CREATE(lbits)(&zgsz3797);
        sailgen_zzz3zzz3ones(&zgsz3797, zgsz3796);
        zgaz3384 = CONVERT_OF(fbits, lbits)(zgsz3797, true);
        KILL(lbits)(&zgsz3797);
        KILL(sail_int)(&zgsz3796);
      }
    
    }
    znew_cap = sailgen_setCapPerms(zTestCap, zgaz3384);
  
  }
  sail_string zcap_str;
  CREATE(sail_string)(&zcap_str);
  sailgen_capToString(&zcap_str, zTestCap);
  if (have_exception) {







  KILL(sail_string)(&zcap_str);
  goto end_block_exception_226;
  }
  uint64_t zlen;
  zlen = sailgen_getRepresentableLength(zTestAddr);
  uint64_t zmask;
  zmask = sailgen_getRepresentableAlignmentMask(zTestAddr);
  zcbz357 = UNIT;


  KILL(sail_string)(&zcap_str);







end_function_225: ;
  return zcbz357;
end_block_exception_226: ;

  return UNIT;
}

static unit sailgen_initializze_registers(unit);

static unit sailgen_initializze_registers(unit zgsz3798)
{
  __label__ end_function_228, end_block_exception_229;

  unit zcbz358;
  {
    zTestCap = sailgen_undefined_Capability(UNIT);
    unit zgsz3802;
    zgsz3802 = UNIT;
  }
  {
    {
      sail_int zgsz3799;
      CREATE(sail_int)(&zgsz3799);
      CONVERT_OF(sail_int, mach_int)(&zgsz3799, INT64_C(32));
      lbits zgsz3800;
      CREATE(lbits)(&zgsz3800);
      UNDEFINED(lbits)(&zgsz3800, zgsz3799);
      zTestAddr = CONVERT_OF(fbits, lbits)(zgsz3800, true);
      KILL(lbits)(&zgsz3800);
      KILL(sail_int)(&zgsz3799);
    }
    unit zgsz3801;
    zgsz3801 = UNIT;
  }
  int64_t zgaz3387;
  {
    sail_int zgsz3803;
    CREATE(sail_int)(&zgsz3803);
    CONVERT_OF(sail_int, mach_int)(&zgsz3803, INT64_C(32));
    sail_int zgsz3804;
    CREATE(sail_int)(&zgsz3804);
    CONVERT_OF(sail_int, mach_int)(&zgsz3804, INT64_C(1));
    sail_int zgsz3805;
    CREATE(sail_int)(&zgsz3805);
    add_int(&zgsz3805, zgsz3803, zgsz3804);
    zgaz3387 = CONVERT_OF(mach_int, sail_int)(zgsz3805);
    KILL(sail_int)(&zgsz3805);
    KILL(sail_int)(&zgsz3804);
    KILL(sail_int)(&zgsz3803);
  }
  {
    sail_int zgsz3806;
    CREATE(sail_int)(&zgsz3806);
    CONVERT_OF(sail_int, mach_int)(&zgsz3806, zgaz3387);
    lbits zgsz3807;
    CREATE(lbits)(&zgsz3807);
    UNDEFINED(lbits)(&zgsz3807, zgsz3806);
    zTestLen = CONVERT_OF(fbits, lbits)(zgsz3807, true);
    KILL(lbits)(&zgsz3807);
    KILL(sail_int)(&zgsz3806);
  }
  zcbz358 = UNIT;

end_function_228: ;
  return zcbz358;
end_block_exception_229: ;

  return UNIT;
}

static void model_init(void)
{
  setup_rts();
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


