/*  Copyright (C) 1998, Cygnus Solutions

    */

#include "sim-main.h"

#include "device.h"
#include "vu1.h"
#include "libvpe.h"
#include "vu.h"

VectorUnitState vu1_state;

static char vu1_umem_buffer[VU1_MEM0_SIZE];
static char vu1_mem_buffer[VU1_MEM1_SIZE];

void init_vu1();
void init_vu(VectorUnitState *state, char* umem_buffer, char* mem_buffer);

void 
vu1_issue() 
{
      if (vu1_state.runState == VU_RUN)
          vpecallms_cycle(&vu1_state);
}

int
vu1_io_read_register_window(device *me,
                   void *dest,
                   int space,
                   address_word addr,
                   unsigned nr_bytes,
                   sim_cpu *processor,
                   sim_cia cia)
{
	/* Slow and crappy hack ... */

	int i;

	char source_buffer[VU1_REGISTER_WINDOW_SIZE];
	char* src;

	memcpy(source_buffer, &vu1_state.regs.VF[0][0], 0x200);	/* copy VF registers */	
	for (i = 0; i<16; i++ ) {
	    *(short*)&source_buffer[0x200 + i*16] = vu1_state.regs.VI[i];
	}
	*(u_long*)&source_buffer[VU1_MST - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MST;
	*(u_long*)&source_buffer[VU1_MMC - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MMC;
	*(u_long*)&source_buffer[VU1_MCP - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MCP;
	*(u_long*)&source_buffer[VU1_MR  - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MR;
	*(u_long*)&source_buffer[VU1_MI  - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MI;
	*(u_long*)&source_buffer[VU1_MQ  - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MQ;
	*(u_long*)&source_buffer[VU1_MP  - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MP;
	*(u_long*)&source_buffer[VU1_MTPC - VU1_REGISTER_WINDOW_START] = vu1_state.regs.MTPC;
	*(VpeStat*)&source_buffer[VPE1_STAT - VU1_REGISTER_WINDOW_START] = vu1_state.regs.VPE_STAT;

	printf("%s: Read: %x, %d, dest: %x, space: %d, %x!\n", me->name, addr, nr_bytes, dest, space, vu1_state.regs.VPE_STAT);
	printf("	vu1_state.regs.VPE_STAT = %x\n", vu1_state.regs.VPE_STAT);

	if (addr + nr_bytes > VU1_REGISTER_WINDOW_END) {
	    fprintf(stderr, "Error: Read past end of vu1 register window!!!\n");
	    exit(1);
	}

	src = &source_buffer[0] + (addr - VU1_REGISTER_WINDOW_START);
	memcpy(dest, src, nr_bytes);
	return nr_bytes;
}

int
vu1_io_write_register_window(device *me,
                    const void *source,
                    int space,
                    address_word addr,
                    unsigned nr_bytes,
                    sim_cpu *processor,
                    sim_cia cia)
{
	char *dest;

	if (addr == VPE1_STAT && nr_bytes == 4) {
	    /* Magic to switch VU to run state, until other methods are available. */
	    vu1_state.runState = VU_RUN;
	    vu1_state.regs.VPE_STAT.vbs = 1;
printf("Magic start run...\n");
printf("%x,%x,%x,%x\n", &vu1_state.regs.VF[0][0], &vu1_state.regs.VPE_STAT,
			((char*)&vu1_state.regs.VPE_STAT) - ((char*)&vu1_state.regs.VF[0][0]),
			((char*)&vu1_state.regs.VPE_STAT) - ((char*)&vu1_state.regs.VF[0][0]) + VU1_REGISTER_WINDOW_START
     );

printf("%x,%x,%x,%x\n", &vu1_state.regs.VF[0][0], &vu1_state.regs.VI[0],
			((char*)&vu1_state.regs.VI[0]) - ((char*)&vu1_state.regs.VF[0][0]),
			((char*)&vu1_state.regs.VI[0]) - ((char*)&vu1_state.regs.VF[0][0]) + VU1_REGISTER_WINDOW_START
     );
printf("%x,%x,%x,%x\n", &vu1_state.regs.VF[0][0], &vu1_state.regs.MST,
			((char*)&vu1_state.regs.MST) - ((char*)&vu1_state.regs.VF[0][0]),
			((char*)&vu1_state.regs.MST) - ((char*)&vu1_state.regs.VF[0][0]) + VU1_REGISTER_WINDOW_START
     );
	    return nr_bytes;
	}

	printf("%s: Write: %x, %d, source: %x, space: %d!\n", me->name, addr, nr_bytes, source, space);

	if (addr + nr_bytes > VU1_REGISTER_WINDOW_END) {
	    fprintf(stderr, "Error: Read past end of vu1 register window!!!\n");
	    exit(1);
	}

	dest = ((char*) (&vu1_state.regs)) + (addr - VU1_REGISTER_WINDOW_START);

	memcpy(dest, source, nr_bytes);

	return nr_bytes;
}

device vu1_device = 
  { 
    "vu1", 
    &vu1_io_read_register_window,
    &vu1_io_write_register_window 
  };

void 
vu1_init(SIM_DESC sd) 
{

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_REGISTER_WINDOW_START,
                   VU1_REGISTER_WINDOW_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   &vu1_device,
                   NULL /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_MEM0_WINDOW_START,
                   VU1_MEM0_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   &vu1_umem_buffer /*buffer*/);

  sim_core_attach (sd,
		   NULL,
                   0 /*level*/,
                   access_read_write,
                   0 /*space ???*/,
                   VU1_MEM1_WINDOW_START,
                   VU1_MEM1_SIZE /*nr_bytes*/,
                   0 /*modulo*/,
                   0 /*device*/,
                   &vu1_mem_buffer /*buffer*/);

  init_vu1();
  /*initvpe();*/
  vpecallms_init(&vu1_state);
}

/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*      VPE1 simulator                                                      */
/*                                                                          */
/****************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <strings.h>
#include "libvpe.h"

char	ifilename[64] = "vu.bin";
char	ofilename[64] = "";
char	pfilename[64] = "";

void abend2(char *fmt, char* p) {
    fprintf(stderr, fmt, p);
    exit(1);
}

void getoption();

void init_vu1() {
    init_vu(&vu1_state, &vu1_umem_buffer[0], &vu1_mem_buffer[0]);
}

void init_vu(VectorUnitState *state, char* umem_buffer, char* mem_buffer)
{
	FILE *fp;
	int i, j;
	u_long	data[4];

	/* set up memory buffers */
	state->uMEM = (uMEM_Entry_Type *) umem_buffer;
	state->MEM =  (MEM_Entry_Type*)   mem_buffer;

	/* set up run state */
	state->runState = VU_READY;

	/* read option */
	getoption();

	/* read instruction file (mandatory) */
	if (*ifilename) {
		if((fp = fopen(ifilename, "r")) == NULL)
			abend2("%s: can not open\n", ifilename);

		for (i = 0; fread(&data[0], 4, 1, fp) != 0; i++) {
			fread(&data[1], 4, 1, fp);
			LoadMMem(state, i, data, 1);
		}
		fclose(fp);
	}
	
	/* PKE dirven simvpe */
	if (*pfilename) {
		/* initpke(pfilename); */
		initvpe(&vu1_state);
		/* while (simpke() != -1) 
			simvpe(); */
	}
	
	/* conventional simvpe */
	else {
		initvpe(&vu1_state);
		/*simvpe();*/
	}
	
	/* write result memory image (optional) */
	if (*ofilename) {
		if((fp = fopen(ofilename, "w")) == NULL)
			abend2("%s: can not open\n", ofilename);

		for(i = 0; i < 2048; i++){
			StoreVUMem(i, data, 1);
			for(j = 0; j < 4; j++)
				fwrite(&data[j], 4, 1, fp);
		}
		fclose(fp);
	}
}

static void Usage(void)
{
	fprintf(stderr, "Usage: simvpe [options]\n");
	fprintf(stderr, "\t\t-i instruction-file\n");
	fprintf(stderr, "\t\t-o output-memory-file\n");
	fprintf(stderr, "\t\t-t PKE-file (text type)\n");
	fprintf(stderr, "\t\t-s start-address [default = 0]\n");
	fprintf(stderr, "\t\t-d [interactive mode enable: default desable]\n");
	fprintf(stderr, "\t\t-v [statistics mode enable: default desable]\n");
	fprintf(stderr, "\t\t-p [debug print mode enable: default desable]\n");
}

void getoption()
{
	int startline = 0;
	int count = 1;

	_is_dbg = 1;
	_vpepc = 0;
	_is_verb = 0;
	_is_dump = 0;
	_pgpuif	 = 2;
	_ITOP = 20;
	_TOP = 10;

#if 0
	while(argc - count){
		if(argv[count][0] == '-'){
			switch(argv[count][1]){
				case 'i':
					strcpy(ifilename, argv[count+1]);
					count += 2;
					break;
				case 'o':
					strcpy(ofilename, argv[count+1]);
					count += 2;
					break;
				case 't':
					strcpy(pfilename, argv[count+1]);
					count += 2;
					break;
				case 's':
					sscanf(argv[count+1], "%d", &startline);
					_vpepc = startline;
					count += 2;
					break;
				case 'd':
					_is_dbg = 1;
					count += 1;
					break;
				case 'v':
					_is_verb = 1;
					count += 1;
					break;
				case 'p':
					_is_dump = 1;
					count += 1;
					break;
				case 'h':
				case '?':
					Usage();
					exit(1);
					break;
				default:
					Usage();
					exit(1);
			}
		}else{
			Usage();
			exit(1);
		}
	}
#endif
}
