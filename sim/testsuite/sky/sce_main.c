
/****************************************************/
/* This is a (Toronto created) wrapper program      */
/* to drive the sce_tests                           */
/*                                                  */
/* Copyright (C) 1998, Cygnus Solutions             */
/****************************************************/ 

extern int printf(const char *, ...);

extern char My_dma_start[];
extern char gpu_refresh;


/* ------------- VU defines --------------*/

#define VPU_STAT		(volatile int *)0x110073d0
#define VPU_STAT_VBS1_MASK	0x00000100

/* ----------end of VU defines -----------*/

                                
/* ------------- VIF defines -------------*/
#define VIF1_STAT		(volatile int *) 0x10003C00
#define VIF1_STAT_FQC_MASK 0x1F000000
#define VIF1_STAT_PPS_MASK 0x00000003
                                                 
/* ----------end of VIF defines -----------*/


/* -------------- DMA defines -------------*/
#define DMA_D0_CHCR	(volatile int*)0x10008000
#define DMA_D0_MADR	(volatile int*)0x10008010
#define DMA_D0_QWC	(volatile int*)0x10008020
#define DMA_D0_TADR	(volatile int*)0x10008030
#define DMA_D0_ASR0	(volatile int*)0x10008040
#define DMA_D0_ASR1	(volatile int*)0x10008050
#define DMA_D0_PKTFLAG	(volatile int*)0x10008060	/* virtual reg to indicate presence of tag in data */

#define DMA_D1_CHCR	(volatile int*)0x10009000
#define DMA_D1_MADR	(volatile int*)0x10009010
#define DMA_D1_QWC	(volatile int*)0x10009020
#define DMA_D1_TADR	(volatile int*)0x10009030
#define DMA_D1_ASR0	(volatile int*)0x10009040
#define DMA_D1_ASR1	(volatile int*)0x10009050
#define DMA_D1_PKTFLAG	(volatile int*)0x10009060	/* virtual reg to indicate presence of tag in data */ 

#define DMA_D2_CHCR	(volatile int*)0x1000a000
#define DMA_D2_MADR	(volatile int*)0x1000a010
#define DMA_D2_QWC	(volatile int*)0x1000a020
#define DMA_D2_TADR	(volatile int*)0x1000a030
#define DMA_D2_ASR0	(volatile int*)0x1000a040
#define DMA_D2_ASR1	(volatile int*)0x1000a050
#define DMA_D2_PKTFLAG	(volatile int*)0x1000a060	/* virtual reg to indicate presence of tag in data */ 

#define DMA_D_CTRL	(volatile int*)0x1000e000
#define DMA_D_CTRL__DMAE	0x00000001
#define DMA_D_STAT	(volatile int*)0x1000e010
#define DMA_D_STAT__TOGGLE	0x63ff0000
#define DMA_D_STAT__CLEAR	0x0000e3ff
#define DMA_D_PCR	(volatile int*)0x1000e020
#define DMA_D_PCR__PCE		0x80000000
#define DMA_D_PCR__CDE		0x03ff0000
#define DMA_D_SQWC	(volatile int*)0x1000e030
#define DMA_D_RBSR	(volatile int*)0x1000e040
#define DMA_D_RBOR	(volatile int*)0x1000e050
#define DMA_D_STADR	(volatile int*)0x1000e060

/* Defines for DMA tag fields. */    
#define DMA_TAG_ID	0x70000000
#define DMA_TAG_ID__REFE	0
#define DMA_TAG_ID__CNT		1
#define DMA_TAG_ID__NEXT	2
#define DMA_TAG_ID__REF		3
#define DMA_TAG_ID__REFS	4
#define DMA_TAG_ID__CALL	5
#define DMA_TAG_ID__RET		6
#define DMA_TAG_ID__END		7

/* Dn_CHCR definition values */
#define MODE_NORM       0
#define MODE_CHAIN      (1 << 2)
#define MODE_INTR       (2 << 2)
#define DMA_START       (1 << 8)
#define DMA_Dn_CHCR__TTE  0x00000040


/* ----------end of VIF defines -----------*/



void DMA_enable(void) {
    *DMA_D_CTRL = 0x01; /* DMA enable */
}

/* If DMA mode is source chain */ 
void start_DMA_ch1_source_chain(void* data) {
    *DMA_D_CTRL  = 0x01; /* DMA enable */
    *DMA_D1_QWC  = 0x00;
    *DMA_D1_TADR = (int)data; 
    *DMA_D1_CHCR = MODE_CHAIN | DMA_START | DMA_Dn_CHCR__TTE;

}

/* If DMA mode is normal */
void start_DMA_ch1_normal(void* data, int qwc) {
    *DMA_D_CTRL  = 0x01; /* DMA enable */
    *DMA_D1_QWC  = qwc; /* 8 is sample */
    *DMA_D1_MADR = (int)data; 
    *DMA_D1_CHCR = MODE_NORM | DMA_START | DMA_Dn_CHCR__TTE;

}

void wait_until_idle() {
    /* Hmmm... Not sure exactly what the right code is for this.  I'll look for
     * VIF_STAT.PPS = 0 && VIF_STAT.FQC == 0 && VPU_STAT.VBS1 == 0 */

    int vif1_stat, vpu_stat;
    do {
	vif1_stat = *VIF1_STAT;
	vpu_stat = *VPU_STAT;
    } while (!(   (vif1_stat & VIF1_STAT_PPS_MASK) == 0
	       && (vif1_stat & VIF1_STAT_FQC_MASK) == 0
	       && (vpu_stat  & VPU_STAT_VBS1_MASK) == 0));
}

void wait_a_while() {
    int i;
    for (i = 0; i<200000; i++) {}
}

int main() {
    start_DMA_ch1_source_chain(&My_dma_start);
    wait_until_idle();
    start_DMA_ch1_source_chain(&gpu_refresh);
    wait_a_while();

    return 0;
}
