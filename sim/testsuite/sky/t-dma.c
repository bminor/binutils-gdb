#include <stdio.h>
#include <string.h>

#define error(x) { printf(x); exit( 999); }

void wrtest( unsigned *reg, unsigned w, unsigned e)
{
    unsigned r;

    *reg = w;
    r = *reg;
    if( r != e)
    {
	printf( "wrtest: at 0x%08lx wrote=0x%08x read=0x%08x expected=0x%08x\n",
		reg, w, r, e);
	exit( 999);
    }
}

/*
DMA tag in target format (LE).
BE=0x00000000_00000000_aaaaaaa0_ip00cccc
*/
typedef struct { unsigned tagL __attribute__ ((aligned (16))), tagH, pke1, pke0; } quad;

			quad data0 = {  3,  2,  1,  0};

/* DMArefe 1, data0 */	quad dma_refe =	{ 0x0c000001, 000, 0, 0};
/* DMAcnt 2 */		quad dma_cnt =	{ 0x10000002, 0, 0, 0};
			quad data1 =	{  7,  6,  5,  4};
			quad data2 =	{ 11, 10,  9,  8};
/* DMAnext 3,dma_ref */	quad dma_next =	{ 0x20000003, 000, 0, 0};
			quad data3 =	{ 15, 14, 13, 12};
			quad data4 =	{ 19, 18, 17, 16};
			quad data5 =	{ 23, 22, 21, 20};

			quad data6 =	{ 27, 26, 25, 24};
			quad data7 =	{ 31, 30, 29, 28};
			quad data8 =	{ 35, 34, 33, 32};
/* DMAref 1,data6 */	quad dma_ref =	{ 0x30000001, 000, 0, 0};
/* DMArefs 2,data7 */	quad dma_refs =	{ 0x40000002, 000, 0, 0};
/* DMAcall 3,dma_cal2 */quad dma_call =	{ 0x50000003, 000, 0, 0};
			quad data9 =	{ 39, 38, 37, 36};
			quad dataA =	{ 43, 42, 41, 40};
			quad dataB =	{ 47, 46, 45, 44};
/* DMAend 1 */  	quad dma_end =	{ 0x78000001, 0, 0, 0};
			quad dataE =	{ 59, 58, 57, 56};

/* DMAcall 1,dma_rt2 */	quad dma_cal2 =	{ 0x50000001, 000, 0, 0 };
			quad dataC =	{ 51, 50, 49, 48};
/* DMAret 0 */		quad dma_ret =	{ 0x60000000, 0, 0, 0}; 

/* DMAret 1 */		quad dma_rt2 =	{ 0x60000001, 0, 0, 0};
			quad dataD =	{ 55, 54, 53, 52};

int main()
{

#define DMA_D0_START	0x10008000
#define DMA_D0_CHCR	(unsigned*) 0x10008000
#define DMA_Dn_CHCR__STR	0x00000100
#define DMA_Dn_CHCR__TIE	0x00000080
#define DMA_Dn_CHCR__TTE	0x00000040
#define DMA_Dn_CHCR__MOD	0x0000000c
#define DMA_Dn_CHCR__MOD_NORM	0x00000000
#define DMA_Dn_CHCR__MOD_CHAIN	0x00000004
#define DMA_Dn_CHCR__DIR	0x00000001
#define DMA_D0_MADR	(unsigned*) 0x10008010
#define DMA_D0_QWC	(unsigned*) 0x10008020
#define DMA_D0_TADR	(unsigned*) 0x10008030
#define DMA_D0_ASR0	(unsigned*) 0x10008040
#define DMA_D0_ASR1	(unsigned*) 0x10008050
#define DMA_D0_PKTFLAG	(unsigned*) 0x10008060	/* virtual reg to indicate presence of tag in data */
#define DMA_D0_END	0x10008070

#define DMA_D1_START	0x10009000
#define DMA_D1_CHCR	(unsigned*) 0x10009000
#define DMA_D1_MADR	(unsigned*) 0x10009010
#define DMA_D1_QWC	(unsigned*) 0x10009020
#define DMA_D1_TADR	(unsigned*) 0x10009030
#define DMA_D1_ASR0	(unsigned*) 0x10009040
#define DMA_D1_ASR1	(unsigned*) 0x10009050
#define DMA_D1_PKTFLAG	(unsigned*) 0x10009060	/* virtual reg to indicate presence of tag in data */ 
#define DMA_D1_END	0x10009070

#define DMA_D2_START	0x1000a000
#define DMA_D2_CHCR	(unsigned*) 0x1000a000
#define DMA_D2_MADR	(unsigned*) 0x1000a010
#define DMA_D2_QWC	(unsigned*) 0x1000a020
#define DMA_D2_TADR	(unsigned*) 0x1000a030
#define DMA_D2_ASR0	(unsigned*) 0x1000a040
#define DMA_D2_ASR1	(unsigned*) 0x1000a050
#define DMA_D2_PKTFLAG	(unsigned*) 0x1000a060	/* virtual reg to indicate presence of tag in data */ 
#define DMA_D2_END	0x1000a070

#define DMA_D_CTRL	(unsigned*) 0x1000e000
#define DMA_D_CTRL__DMAE	0x00000001
#define DMA_D_STAT	(unsigned*) 0x1000e010
#define DMA_D_STAT__TOGGLE	0x63ff0000
#define DMA_D_STAT__CLEAR	0x0000e3ff
#define DMA_D_PCR	(unsigned*) 0x1000e020
#define DMA_D_PCR__PCE		0x80000000
#define DMA_D_PCR__CDE		0x03ff0000
#define DMA_D_PCR__CDE0		0x00010000
#define DMA_D_PCR__CDE1		0x00020000
#define DMA_D_PCR__CDE2		0x00040000
#define DMA_D_SQWC	(unsigned*) 0x1000e030
#define DMA_D_RBSR	(unsigned*) 0x1000e040
#define DMA_D_RBOR	(unsigned*) 0x1000e050
#define DMA_D_STADR	(unsigned*) 0x1000e060

#define DMA_TAG_IRQ	0x80000000;


    unsigned *reg, temp;

    printf( "Testing DMA_D_CTRL...\n");
    reg = DMA_D_CTRL;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x00000555, 0x00000555);
    wrtest( reg, 0x000002aa, 0x000002aa);
    *reg = 0;

    printf( "Testing DMA_D_STAT...\n");
    reg = DMA_D_STAT;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0xffffffff, 0x63ff0000);
    wrtest( reg, 0x63ff0000, 0x00000000);

    printf( "Testing DMA_D_PCR...\n");
    reg = DMA_D_PCR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x80000000, 0x83ff0000);
    wrtest( reg, 0x8fff0000, 0x83ff0000);
    wrtest( reg, 0x05550000, 0x01550000);
    wrtest( reg, 0x0155aaaa, 0x015502aa);
    wrtest( reg, 0x00005555, 0x00000155);
    *reg = 0;

    printf( "Testing DMA_D_SQWC...\n");
    reg = DMA_D_SQWC;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x005500aa);
    wrtest( reg, 0xaaaa5555, 0x00aa0055);
    *reg = 0;

    printf( "Testing DMA_D_RBOR...\n");
    reg = DMA_D_RBOR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0xaaaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D_RBSR...\n");
    reg = DMA_D_RBSR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0xaaaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D_STADR...\n");
    reg = DMA_D_STADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0xaaaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D0_CHCR...\n");
    reg = DMA_D0_CHCR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x000000aa);
    wrtest( reg, 0xaaaa5555, 0x00000155);
    *reg = 0;

    printf( "Testing DMA_D0_MADR...\n");
    reg = DMA_D0_MADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D0_TADR...\n");
    reg = DMA_D0_TADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D0_ASR0...\n");
    reg = DMA_D0_ASR0;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D0_ASR1...\n");
    reg = DMA_D0_ASR1;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D0_QWC...\n");
    reg = DMA_D0_QWC;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x0000aaaa);
    wrtest( reg, 0xaaaa5555, 0x00005555);
    *reg = 0;

    printf( "Testing DMA_D0_PKTFLAG...\n");
    reg = DMA_D0_PKTFLAG;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaaa);
    wrtest( reg, 0xaaaa5555, 0xaaaa5555);
    *reg = 0;

    printf( "Testing DMA_D1_CHCR...\n");
    reg = DMA_D1_CHCR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x000000aa);
    wrtest( reg, 0xaaaa5555, 0x00000155);
    *reg = 0;

    printf( "Testing DMA_D1_MADR...\n");
    reg = DMA_D1_MADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D1_TADR...\n");
    reg = DMA_D1_TADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D1_ASR0...\n");
    reg = DMA_D1_ASR0;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D1_ASR1...\n");
    reg = DMA_D1_ASR1;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D1_QWC...\n");
    reg = DMA_D1_QWC;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x0000aaaa);
    wrtest( reg, 0xaaaa5555, 0x00005555);
    *reg = 0;

    printf( "Testing DMA_D1_PKTFLAG...\n");
    reg = DMA_D1_PKTFLAG;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaaa);
    wrtest( reg, 0xaaaa5555, 0xaaaa5555);
    *reg = 0;

    printf( "Testing DMA_D2_CHCR...\n");
    reg = DMA_D2_CHCR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x000000aa);
    wrtest( reg, 0xaaaa5555, 0x00000155);
    *reg = 0;

    printf( "Testing DMA_D2_MADR...\n");
    reg = DMA_D2_MADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D2_TADR...\n");
    reg = DMA_D2_TADR;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D2_ASR0...\n");
    reg = DMA_D2_ASR0;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D2_ASR1...\n");
    reg = DMA_D2_ASR1;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaa0);
    wrtest( reg, 0x2aaa5555, 0x2aaa5550);
    *reg = 0;

    printf( "Testing DMA_D2_QWC...\n");
    reg = DMA_D2_QWC;
    if( *reg)
    {
	printf ("Not initialized to zero (0x%08x).\n", *reg);
	exit (999);
    }
    wrtest( reg, 0x5555aaaa, 0x0000aaaa);
    wrtest( reg, 0xaaaa5555, 0x00005555);
    *reg = 0;

    printf( "Testing DMA_D2_PKTFLAG...\n");
    reg = DMA_D2_PKTFLAG;
    if( *reg) error( "Not initialized to zero.\n");
    wrtest( reg, 0x5555aaaa, 0x5555aaaa);
    wrtest( reg, 0xaaaa5555, 0xaaaa5555);
    *reg = 0;

    /* Initiate a non-chained 0-length transfer. */
    printf ("\nPreparing for a DMA1 NORM transfer...\n");
    reg = DMA_D_CTRL;			/* enable DMA */
    *reg = DMA_D_CTRL__DMAE;
    reg = DMA_D1_QWC;			/* set # of quad words to xfr */
    *reg = 0;
    reg = DMA_D1_MADR;			/* set data address */
    *reg = 0x00012340;
    reg = DMA_D1_CHCR;
    printf ("Ready...\n");
    *reg = DMA_Dn_CHCR__STR | DMA_Dn_CHCR__MOD_NORM;

    reg = DMA_D1_QWC;
    if (*reg)
    {
	printf ("D1_QWC is not zero after transfer.\n");
	exit (999);
    }
    reg = DMA_D1_MADR;
    if (*reg != 0x00012340)
    {
	printf ("D1_MADR value is wrong after transfer.\n");
	exit (999);
    }
    reg = DMA_D1_CHCR;
    if ((*reg & DMA_Dn_CHCR__STR) != 0)
    {
	printf ("D1_CHCR.STR is not reset after transfer.\n");
	exit (999);
    }
    printf ("Completed OK...\n");

    /* Initiate a chained transfer. */

    /* Some of the "tag" fields couldn't be initialised statically. */
    /* The first address is unaligned to force a warning. */
    dma_refe.tagH = (unsigned)&data0 + 1;
    dma_next.tagH = (unsigned)&dma_ref;
    dma_ref.tagH = (unsigned)&data6;
    dma_refs.tagH = (unsigned)&data7;
    dma_call.tagH = (unsigned)&dma_cal2;
    dma_cal2.tagH = (unsigned)&dma_rt2;

    printf ("\nPreparing for a DMA0 TAG transfer...\n");
    reg = DMA_D_PCR;			/* disable DMA0 */
    *reg &= ~DMA_D_PCR__CDE0;
    *reg |= DMA_D_PCR__PCE;
    reg = DMA_D_CTRL;			/* enable DMA */
    *reg = DMA_D_CTRL__DMAE;
    reg = DMA_D0_TADR;			/* set tag address */
    *reg = (unsigned) & dma_refe;
    reg = DMA_D0_QWC;			/* set fictitious count */
    *reg = 51;
    printf ("Ready...\n");
    reg = DMA_D0_CHCR;
    *reg = DMA_Dn_CHCR__STR | DMA_Dn_CHCR__TTE | DMA_Dn_CHCR__MOD_CHAIN;

    reg = DMA_D0_QWC;
    if (*reg != 51)
    {
	printf ("DMA0 transfer not inhibited by DMA_D_PCR__CDE0 (%d).\n", *reg);
	exit (999);
    }
    reg = DMA_D_PCR;			/* enable DMA0 */
    *reg &= ~DMA_D_PCR__PCE;

    reg = DMA_D0_QWC;
    if (*reg)
    {
	printf ("D0_QWC is not zero after transfer.\n");
	exit (999);
    }
    reg = DMA_D0_MADR;
    if (*reg != (unsigned)&dma_cal2)
    {
	printf ("D0_MADR value is wrong after transfer.\n");
	exit (999);
    }
    reg = DMA_D0_CHCR;
    if ((*reg & DMA_Dn_CHCR__STR) != 0)
    {
	printf ("D0_CHCR.STR is not reset after transfer.\n");
	exit (999);
    }
    printf ("Completed OK...\n");

    /* Initiate a chained transfer that will be suspended
    and restarted. */

    /* Request suspension part way through. */
    dma_ref.tagL |= DMA_TAG_IRQ;

    printf ("\nPreparing for a DMA2 TAG transfer with suspension...\n");
    reg = DMA_D_CTRL;			/* enable DMA */
    *reg = DMA_D_CTRL__DMAE;
    reg = DMA_D2_TADR;			/* set tag address */
    *reg = (unsigned) & dma_refe;
    printf ("Ready...\n");
    reg = DMA_D2_CHCR;
    *reg = DMA_Dn_CHCR__STR | DMA_Dn_CHCR__MOD_CHAIN;
    printf ("returns...\n");

    reg = DMA_D2_QWC;
    if (*reg)
    {
	printf ("D2_QWC (0x%08x) is not zero after suspension.\n", *reg);
	exit (999);
    }
    reg = DMA_D2_MADR;
    if (*reg != (unsigned)&data7)
    {
	printf ("D2_MADR value (0x%08x) is wrong after suspension.\n", *reg);
	exit (999);
    }
    reg = DMA_D2_TADR;
    if (*reg != (unsigned)&dma_refs)
    {
	printf ("D2_TADR value (0x%08x) is wrong after suspension.\n", *reg);
	exit (999);
    }
    reg = DMA_D2_CHCR;
    if ((*reg & DMA_Dn_CHCR__STR) != 0)
    {
	printf ("D2_CHCR.STR (0x%08x) is not reset after suspension.\n", *reg);
	exit (999);
    }
    reg = DMA_D_PCR;
    if ((*reg & DMA_D_PCR__PCE) == 0)
    {
	printf ("DMA_D_PCR__PCE not set by transfer.\n");
	exit (999);
    }
    printf ("Suspended OK...\n");

    /* Reset the suspension request and continue. */
    dma_ref.tagL &= ~DMA_TAG_IRQ;
    reg = DMA_D2_CHCR;
    *reg |= DMA_Dn_CHCR__STR;

    reg = DMA_D2_QWC;
    if (*reg)
    {
	printf ("D2_QWC (0x%08x) is not zero after transfer.\n", *reg);
	exit (999);
    }
    reg = DMA_D2_MADR;
    if (*reg != (unsigned)&dma_cal2)
    {
	printf ("D2_MADR value (0x%08x) is wrong after transfer.\n", *reg);
	exit (999);
    }
    reg = DMA_D2_CHCR;
    if ((*reg & DMA_Dn_CHCR__STR) != 0)
    {
	printf ("D2_CHCR.STR (0x%08x) is not reset after transfer.\n", *reg);
	exit (999);
    }
    reg = DMA_D_PCR;
    if (*reg & DMA_D_PCR__PCE)
    {
	printf ("DMA_D_PCR__PCE not reset by transfer.\n");
	exit (999);
    }
    printf ("Completed OK...\n");

    exit (0);
}
