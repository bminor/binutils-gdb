#define ARC4_JTAG 1
#define CONFIG_OSABI		GDB_OSABI_UNKNOWN

struct gdbarch *arc_jtag_init (struct gdbarch *gdbarch);
#define CONFIG_INIT_TDEP	arc_jtag_init

/* The core regnums here are the same as the hardware register numbers.  We
   cannot do that for aux registers, because the aux regs on the h/w do not
   have contiguous numbers.  */
enum arc4_jtag_regnums
  {
    ARC_FP_REGNUM       = 27,
    ARC_SP_REGNUM           ,
    ARC_ILINK1_REGNUM	    ,
    ARC_ILINK2_REGNUM	    ,
    ARC_BLINK_REGNUM        ,
    /* Extension core regs are 32..59 inclusive.  */
    ARC_LP_COUNT_REGNUM = 60,
    /* 61 is reserved, 62 is not a real register.  */
    ARC_PCL_REGNUM      = 63,

    /* Now the aux registers.  */
    
    ARC_STATUS_REGNUM   	= 64,
    ARC_SEMAPHORE_REGNUM	    ,
    ARC_LP_START_REGNUM		    ,
    ARC_LP_END_REGNUM		    ,
    ARC_IDENTITY_REGNUM		    ,
    ARC_DEBUG_REGNUM		    ,
#ifndef ARC4_JTAG
    ARC_PC_REGNUM		    ,
    ARC_STATUS32_REGNUM		    ,
    ARC_STATUS32_L1_REGNUM	    ,
    ARC_STATUS32_L2_REGNUM	    ,

    ARC_COUNT0_REGNUM               , 
    ARC_CONTROL0_REGNUM		    , 
    ARC_LIMIT0_REGNUM               , 
    ARC_INT_VECTOR_BASE_REGNUM	    , 
    ARC_AUX_MACMODE_REGNUM	    , 
    ARC_AUX_IRQ_LV12_REGNUM	    , 

    ARC_COUNT1_REGNUM                ,
    ARC_CONTROL1_REGNUM		     ,
    ARC_LIMIT1_REGNUM		     ,
    ARC_AUX_IRQ_LEV_REGNUM	     ,
    ARC_AUX_IRQ_HINT_REGNUM	     ,
    ARC_ERET_REGNUM		     ,
    ARC_ERBTA_REGNUM		     ,
    ARC_ERSTATUS_REGNUM		     ,
    ARC_ECR_REGNUM		     ,
    ARC_EFA_REGNUM		     ,
    ARC_ICAUSE1_REGNUM		     ,
    ARC_ICAUSE2_REGNUM		     ,
    ARC_AUX_IENABLE_REGNUM	     ,
    ARC_AUX_ITRIGGER_REGNUM	     ,
    ARC_XPU_REGNUM                   ,
    ARC_BTA_REGNUM		     ,
    ARC_BTA_L1_REGNUM		     ,
    ARC_BTA_L2_REGNUM		     ,
    ARC_AUX_IRQ_PULSE_CANCEL_REGNUM  ,
    ARC_AUX_IRQ_PENDING_REGNUM       ,

    /* Build configuration registers.  */
    ARC_BCR_0_REGNUM		    , 
    ARC_BCR_1_REGNUM		    , 
    ARC_BCR_2_REGNUM		    , 
    ARC_BCR_3_REGNUM		    , 
    ARC_BCR_4_REGNUM		    , 
    ARC_BCR_5_REGNUM		    , 
    ARC_BCR_6_REGNUM		    , 
    ARC_BCR_7_REGNUM		    , 
    ARC_BCR_8_REGNUM		    , 
    ARC_BCR_9_REGNUM		    , 
    ARC_BCR_A_REGNUM		    , 
    ARC_BCR_B_REGNUM		    , 
    ARC_BCR_C_REGNUM		    , 
    ARC_BCR_D_REGNUM		    , 
    ARC_BCR_E_REGNUM		    , 
    ARC_BCR_F_REGNUM		    , 
    ARC_BCR_10_REGNUM		    , 
    ARC_BCR_11_REGNUM		    , 
    ARC_BCR_12_REGNUM		    ,
    				
    ARC_BCR_13_REGNUM		    ,
    ARC_BCR_14_REGNUM		     ,
    ARC_BCR_15_REGNUM		     ,
    ARC_BCR_16_REGNUM		     ,
    ARC_BCR_17_REGNUM		     ,
    ARC_BCR_18_REGNUM		     ,
    ARC_BCR_19_REGNUM		     ,
    ARC_BCR_1A_REGNUM		     ,
    ARC_BCR_1B_REGNUM		     ,
    ARC_BCR_1C_REGNUM		     ,
    ARC_BCR_1D_REGNUM		     ,
    ARC_BCR_1E_REGNUM		     ,
    ARC_BCR_1F_REGNUM		     ,

#endif    
    ARC_NR_REGS

  };

