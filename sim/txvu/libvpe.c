/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*      VU simulator                                                        */
/*                                                                          */
/****************************************************************************/

#include "vpe.h"
#include "libvpe.h"
#include <stdio.h>
#include <strings.h>

unsigned long _ITOP;
unsigned long _TOP;
unsigned long _vpepc;
int	_is_dbg;
int	_is_verb;
int	_is_dump;
int	_pgpuif;
FILE *_fp_gpu;

int _GIF_SIM_OFF;
int _GIF_BUSY;
int _GIF_VUCALL;
int _GIF_VUADDR;


/****************************************************************************/

/* Some external declarations ... */
void GpuIfKick(VectorUnitState *state, int addr);
void opcode_analyze(u_long *opcode);

#define VF state->regs.VF
#define VI state->regs.VI

void initvpe(VectorUnitState *state)
{
/* 
[name]		initvpe
[desc]		Initialize internal status of VU simulator.
[args]		void
[return]	void
*/
	int i, j;

	_fp_gpu = stdout;
	VF[0][0] = 0.0;
	VF[0][1] = 0.5;
	VF[0][2] = -1.0;
	VF[0][3] = 1.0;
	VI[0] = 0;
	eflag = 0;
	jflag = 0;
	peflag = 0;
	sflag = 0;
	pc = _vpepc;
	jaddr = 0;
	bp = 0xffffffff;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < 2; j++) {
			pipe[i][j].flag = 0;
		}
	}
	for (i = 0; i < 7; i++) {
		qpipe[i].flag = 0;
	}
	for (i = 0; i < 13; i++) {
		rqpipe[i].flag = 0;
	}
	ipipe[0].flag = 0;
	ipipe[1].flag = 0;
	apipe.flag = 0;
	Ipipe.flag = 0;

	spipe.no = 0;
	spipe.flag = 0;
	for (i = 1; i < 16; i++) {
		VI[i] = 0;
	}
	for (i = 1; i < 32; i++) {
		for (j = 0; j < 4; j++) {
			VF[i][j] = 0.0;
		}
	}
	/* EFU internal regstars */
	VN[8] = 1.0;
	VN[9] = 1.0;		/* S1 */
	VN[10] = -0.166666567325592;	/* S2 */
	VN[11] = 0.008333025500178;	/* S3 */
	VN[12] = -0.000198074136279;	/* S4 */
	VN[13] = 0.000002601886990;	/* S5 */
	VN[14] = 0.999999344348907;	/* T1 */
	VN[15] = -0.333298563957214;	/* T2 */
	VN[16] = 0.199465364217758;	/* T3 */
	VN[17] = -0.139085337519646;	/* T4 */
	VN[18] = 0.096420042216778;	/* T5 */
	VN[19] = -0.055909886956215;	/* T6 */
	VN[20] = 0.021861229091883;	/* T7 */
	VN[21] = -0.004054057877511;	/* T8 */
	VN[22] = 0.7853981633974483;	/* PI/4 */
	VN[23] = 0.2499986842;	/* E1 */
	VN[24] = 0.0312575832;	/* E2 */
	VN[25] = 0.0025913712;	/* E3 */
	VN[26] = 0.0001715620;	/* E4 */
	VN[27] = 0.0000054302;	/* E5 */
	VN[28] = 0.0000006906;	/* E6 */

	_GIF_SIM_OFF = 0;
	_GIF_BUSY = 0;
	_GIF_VUCALL = 0;
	_GIF_VUADDR = 0;
	intr_mode = _is_dbg;
	ecount = 0;
}

int get_dest(u_long opcode)
{
/*  
[name]		get_dest
[desc.]		get dest field of instruction.
[args]		opcode: instruction(32bits)
[return]	dest field value(4bits)
*/
	return ((int) ((opcode >> 21) & 0xf));
}

int	get_ft(u_long opcode)
{
/*
[name]		get_ft
[desc.]		get ft field of instruction 
[args]		opcode: instruction(32bits)
[return]	ft field value(5bits)
*/
	return ((int) ((opcode >> 16) & 0x1f));
}

int get_fs(u_long opcode)
{
/*
[name]		get_fs
[desc.]		get fs field of instruction 
[args]		opcode: instruction(32bits)
[return]	fs field value(5bits)
*/
	return ((int) ((opcode >> 11) & 0x1f));
}

int get_fd(u_long opcode)
{
/*
[name]		get_fd
[desc.]		get fd field of instruction 
[args]		opcode: instruction(32bits)
[return]	fd field value(5bits)
*/
	return ((int) ((opcode >> 6) & 0x1f));
}

int get_bc(u_long opcode)
{
/*
[name]		get_bc
[desc.]		get bc field of instruction
[args]		opcode: instruction(32bit)
[return]	bc field value(2bits);
*/
	return ((int) (opcode & 0x3));
}

int get_ic(u_long opcode)
{
/*
[name]		get_ic
[desc.]		get bit25 and bit26 in instruction
[args]		opcode: instruction(32bit)
[return]	bit 25 and bit26 value(2bits)
*/
	return ((int) ((opcode >> 25) & 0x3));
}

int get_fsf(u_long opcode)
{
/*
[name]		get_fsf
[desc.]		get fsf field of instruction
[args]		opcode: instruction(32bit)
[return]	fsf field value(2bits)
*/
	return ((opcode >> 21) & 0x3);
}

int get_ftf(u_long opcode)
{
/*
[name]		get_ftf
[desc.]		get ftf field of instruction
[args]		opcode: instruction(32bit)
[return]	ftf field value(2bits)
*/
	return ((opcode >> 23) & 0x3);
}

void statuscheck(float arg, u_long * status, int no)
{
/*
[name]		statuscheck
[desc.]		FMAC status decide when FMAC calculates.
[args]		arg: calculated value by FMAC.
			status: FMAC status pointer.
			no: FMAC number
[return]	void
*/
	/* write status flag's value */
	if (arg == 0.0)
		*status = *status | 1;
	if (arg < 0.0)
		*status = *status | 2;
	/* write MACflag's value */
	switch (no) {
		case 0:
			if (arg == 0.0)
				*status |= 0x80000;
			if (arg < 0.0)
				*status |= 0x800000;
			break;
		case 1:
			if (arg == 0.0)
				*status |= 0x40000;
			if (arg < 0.0)
				*status |= 0x400000;
			break;
		case 2:
			if (arg == 0.0)
				*status |= 0x20000;
			if (arg < 0.0)
				*status |= 0x200000;
			break;
		case 3:
			if (arg == 0.0)
				*status |= 0x10000;
			if (arg < 0.0)
				*status |= 0x100000;
			break;
	}
}

int hazardcheck(int fs, int ft, int dest, int type)
{
/*
[name]		hazardcheck

			If destinatin registers in pipeline is same as source register,
			data hazard occured.
[args]		fs: If fs field assigns source register, set source register number
				in this argment.
			ft: If ft field assigns source register, set souce register number
				in this argment
			dest: specify which units calculate
			type: source register type
				1: source register is ft field
				2: source register is fs field
				3: source registers are ft and fs field
[return]	0: hazard not occured
			1: hazard occured
*/

	int i, j;

	for (i = 3; i >= 0; i--) {
		for (j = 0; j < 2; j++) {
			if (pipe[i][j].flag == 1) {
				if (type & 0x1) {
					if ((pipe[i][j].no == ft) && (pipe[i][j].mask | dest)) {
						sflag = 1;
						hc_count++;
						if (verb_mode)
							printf("[data hazard %04d:VF%02d]\n", pc - 1, ft);
						return 1;
					}
				}
				if (type & 0x2) {
					if ((pipe[i][j].no == fs) && (pipe[i][j].mask | dest)) {
						sflag = 1;
						hc_count++;
						if (verb_mode)
							printf("[data hazard %04d:VF%02d]\n", pc - 1, fs);
						return 1;
					}
				}
			}
			if (pipe[i][j].flag == 6) {
				if (type & 0x1) {
					if ((pipe[i][j].no == ft) && (pipe[i][j].mask | dest)) {
						sflag = 1;
						hc_count++;
						if (verb_mode)
							printf("[data hazard %04d:VF%02d]\n", pc - 1, ft);
						return 1;
					}
				}
				if (type & 0x2) {
					if ((pipe[i][j].no == fs) && (pipe[i][j].mask | dest)) {
						sflag = 1;
						hc_count++;
						if (verb_mode)
							printf("[data hazard %04d:VF%02d]\n", pc - 1, fs);
						return 1;
					}
				}
			}
		}
	}
	sflag = 0;
	return 0;
}

int hazardcheckVI(int fs, int ft, int type)
{
/*
[name]		hazardcheckVI
[desc.]		data hazarde check. (VI register)
			If destinatin registers in pipeline is same as source register,
			data hazard occured.
[args]		fs: If fs field assigns source register, set source register number
				in this argment.
			ft: If ft field assigns source register, set souce register number
				in this argment
			type: source register type
				1: source register is ft field
				2: source register is fs field
				3: source registers are ft and fs field
[return]	0: hazard not occured
			1: hazard occured
*/

	int i;

	for (i = 1; i >= 0; i--) {
		if (ipipe[i].flag == 1) {
			if (type & 0x1) {
				if (ipipe[i].no == ft) {
					hc_count++;
					if (verb_mode)
						printf("[data hazard %04d:VI%02d]\n", pc - 1, ft);
					sflag = 1;
					return 1;
				}
			}
			if (type & 0x2) {
				if (ipipe[i].no == fs) {
					hc_count++;
					if (verb_mode)
						printf("[data hazard %04d:VI%02d]\n", pc - 1, fs);
					sflag = 1;
					return 1;
				}
			}
		}
	}
	sflag = 0;
	return 0;
}

int hazardcheckFTF(int fs, int ft, int fsf, int ftf, int type)
{
/*
[name]		hazardcheckFTF
[desc.]		data hazarde check. (VF register)
			If destinatin registers in pipeline is same as source register,
			data hazard occured.
[args]		fs: If fs field assigns source register, set source register number
				in this argment.
			ft: If ft field assigns source register, set souce register number
				in this argment
			fsf: If fsf field assigns source register, set source register
				number in this argment.
			ftf: If ftf field assigns source register, set souce register
				number in this argment
			type: source register type
				1: source register is ft field
				2: source register is fs field
				3: source registers are ft and fs field
[return]	0: hazard not occured
			1: hazard occured
*/

	int i, j;

	for (i = 3; i >= 0; i--) {
		for (j = 0; j < 2; j++) {
			if (pipe[i][j].flag == 1) {
				if (type & 0x1) {
					if (pipe[i][j].no == ft) {
						switch (ftf) {
							case 0:
								if (pipe[i][j].mask & DEST_X) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 1:
								if (pipe[i][j].mask & DEST_Y) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 2:
								if (pipe[i][j].mask & DEST_Z) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 3:
								if (pipe[i][j].mask & DEST_W) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
						}
					}
				}
				if (type & 0x2) {
					if (pipe[i][j].no == fs) {
						switch (fsf) {
							case 0:
								if (pipe[i][j].mask & DEST_X) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, fs);
									sflag = 1;
									return 1;
								}
								break;
							case 1:
								if (pipe[i][j].mask & DEST_Y) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, fs);
									sflag = 1;
									return 1;
								}
								break;
							case 2:
								if (pipe[i][j].mask & DEST_Z) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, fs);
									sflag = 1;
									return 1;
								}
								break;
							case 3:
								if (pipe[i][j].mask & DEST_W) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, fs);
									sflag = 1;
									return 1;
								}
								break;
						}
					}
				}
			}
		}
	}
	sflag = 0;
	return 0;
}

int hazardcheckBC(int fs, int ft, int dest, int bc, int type)
{
/*
[name]		hazardcheckBC
[desc.]		data hazarde check. (VF register)
			If destinatin registers in pipeline is same as source register,
			data hazard occured.
[args]		fs: If fs field assigns source register, set source register number
				in this argment.
			ft: If ft field assigns source register, set souce register number
				in this argment
			dest: specify which units calculate
			bc: specify which broadcast units
			type: source register type
				1: source register is ft field
				2: source register is fs field
				3: source registers are ft and fs field
[return]	0: hazard not occured
			1: hazard occured
*/

	int i, j;

	for (i = 3; i >= 0; i--) {
		for (j = 0; j < 2; j++) {
			if ((pipe[i][j].flag == 1) || (pipe[i][j].flag == 7)) {
				if (type & 0x1) {
					if (pipe[i][j].no == ft) {
						switch (bc) {
							case 0:
								if (pipe[i][j].mask & DEST_X) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 1:
								if (pipe[i][j].mask & DEST_Y) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 2:
								if (pipe[i][j].mask & DEST_Z) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
							case 3:
								if (pipe[i][j].mask & DEST_W) {
									hc_count++;
									if (verb_mode)
										printf("[data hazard %04d:VF%02d]\n",
											pc - 1, ft);
									sflag = 1;
									return 1;
								}
								break;
						}
					}
				}
				if (type & 0x2) {
					if ((pipe[i][j].no == fs) && (pipe[i][j].mask | dest)) {
						hc_count++;
						if (verb_mode)
							printf("[data hazard %04d:VF%02d]\n", pc - 1, ft);
						sflag = 1;
						return 1;
					}
				}
			}
		}
	}
	sflag = 0;
	return 0;
}

int hazardcheckQ(void)
{
/*
[name]		hazardcheckQ
[desc.]		resource hazarde check. (FDIV unit)
			If instruction use FDIV unit when FDIV is calculating,
			resource hazard occured.
[args]		void
[return]	0: hazard not occured
			1: hazard occured
*/
	int i;

	for (i = 5; i >= 0; i--) {
		if (qpipe[i].flag == 1) {
			hc_count++;
			if (verb_mode)
				printf("[resource hazard %04d:FDIV]\n", pc - 1);
			sflag = 1;
			return 1;
		}
	}
	for (i = 11; i >= 0; i--) {
		if (rqpipe[i].flag == 1) {
			hc_count++;
			if (verb_mode)
				printf("[resource hazard %04d:FDIV]\n", pc - 1);
			sflag = 1;
			return 1;
		}
	}
	sflag = 0;
	return 0;
}

int hazardcheckP(void)
{
/*
[name]		hazardcheckP
[desc.]		resource hazarde check. (EFU)
			If instruction use EFU unit when EFU is calculating,
			resource hazard occured.
[args]		void
[return]	0: hazard not occured
			1: hazard occured
*/
	if (spipe.no != 0) {
		hc_count++;
		if (verb_mode)
			printf("[resource hazard %04d:EFU]\n", pc - 1);
		sflag = 1;
		return 1;
	}
	sflag = 0;
	return 0;
}

int Lower_special(VectorUnitState *state, u_long iL)
{
/*
[name]		Lower_special
[desc.]		Micro Special OPCODE (Lower instruction) execute
[args]		iL: lower instruction(32bits)
[return]	0: executed
			1: stall ( hazard occured)
*/

	double  dval, ddval;
	float   fval;
	u_long  ival;

	switch ((iL >> 6) & 0x1f) {
		case 12:	/* MOVE, MR32, ---, --- */
			switch (get_bc(iL)) {
				case 0:	/* MOVE */
					if ((get_fs(iL) != 0) || (get_ft(iL) != 0)) {
						if (hazardcheck(get_fs(iL), 0, get_dest(iL), 2)) {
							pipe[0][0].flag = 0;
							apipe.flag = 0;
							return 1;
						}
						pipe[0][1].no = get_ft(iL);
						pipe[0][1].mask = get_dest(iL);
						pipe[0][1].flag = 1;
						strcpy(pipe[0][1].code, "MOVE");
						if (get_dest(iL) & DEST_X)
							pipe[0][1].vf[0] = VF[get_fs(iL)][0];
						if (get_dest(iL) & DEST_Y)
							pipe[0][1].vf[1] = VF[get_fs(iL)][1];
						if (get_dest(iL) & DEST_Z)
							pipe[0][1].vf[2] = VF[get_fs(iL)][2];
						if (get_dest(iL) & DEST_W)
							pipe[0][1].vf[3] = VF[get_fs(iL)][3];
					}
					break;
				case 1:	/* MR32 */
					if (hazardcheck(get_fs(iL), 0, get_dest(iL), 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "MR32");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VF[get_fs(iL)][1];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VF[get_fs(iL)][2];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VF[get_fs(iL)][3];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VF[get_fs(iL)][0];
					break;
				default:
					fprintf(stderr, "Undefined opcode(%08x)\n", iL);
					exit(1);
			}
			break;
		case 13:	/* LQI, SQI, LQD, SQD */
			switch (get_bc(iL)) {
				case 0:	/* LQI */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "LQI");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = *(float *) &state->MEM[VI[get_fs(iL)]][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = *(float *) &state->MEM[VI[get_fs(iL)]][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = *(float *) &state->MEM[VI[get_fs(iL)]][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = *(float *) &state->MEM[VI[get_fs(iL)]][3];
					VI[get_fs(iL)] += 1;
					break;
				case 1:	/* SQI */
/*printf("SQI\n");
printpipe();
printf("fs:%d ft:%d dest:%d)\n", get_fs(iL), get_ft(iL), get_dest(iL));
printf("hazardcheck(%d, %d, %d, %d)\n", 0, get_ft(iL), get_dest(iL), 1); */
					//if (hazardcheck(0, get_ft(iL), get_dest(iL), 1) ||
					  //hazardcheckVI(get_fs(iL), 0, 2)) {
					if (hazardcheck(0, get_fs(iL), get_dest(iL), 1) ||
					  hazardcheckVI(get_ft(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 2;
					pipe[0][1].addr = VI[get_ft(iL)];
					strcpy(pipe[0][1].code, "SQI");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VF[get_fs(iL)][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VF[get_fs(iL)][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VF[get_fs(iL)][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VF[get_fs(iL)][3];
					VI[get_ft(iL)] += 1;
					break;
				case 2:	/* LQD */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "LQD");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] =
							*(float *) &state->MEM[VI[get_fs(iL)] - 1][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] =
							*(float *) &state->MEM[VI[get_fs(iL)] - 1][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] =
							*(float *) &state->MEM[VI[get_fs(iL)] - 1][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] =
							*(float *) &state->MEM[VI[get_fs(iL)] - 1][3];
					VI[get_fs(iL)] -= 1;
					break;
				case 3:	/* SQD */
					if (hazardcheck(0, get_ft(iL), get_dest(iL), 1) ||
					  hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 2;
					pipe[0][1].addr = VI[get_ft(iL)] - 1;
					strcpy(pipe[0][1].code, "SQD");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VF[get_fs(iL)][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VF[get_fs(iL)][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VF[get_fs(iL)][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VF[get_fs(iL)][3];
					VI[get_ft(iL)] -= 1;
					break;
			}
			break;
		case 14:	/* DIV, SQRT, RSQRT, WAITQ */
			switch (get_bc(iL)) {
				case 0:	/* DIV */
					if (hazardcheckFTF(get_fs(iL), get_ft(iL),
					     get_fsf(iL), get_ftf(iL), 3) ||
					    hazardcheckQ()) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					qpipe[0].flag = 1;
					if(VF[get_ft(iL)][get_ftf(iL)] == 0){
						*(u_long *)&qpipe[0].vf[0] = 0x7fffffff;
						if(VF[get_fs(iL)][get_fsf(iL)] == 0)
							qpipe[0].status |= 0x10;
						else
							qpipe[0].status |= 0x20;
					}else{
						qpipe[0].vf[0] = VF[get_fs(iL)][get_fsf(iL)] /
							VF[get_ft(iL)][get_ftf(iL)];
						qpipe[0].status = 0;
					}
					strcpy(qpipe[0].code, "DIV");
					break;
				case 1:	/* SQRT */
					if (hazardcheckFTF(0, get_ft(iL), 0, get_ftf(iL), 1) ||
					    hazardcheckQ()) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					qpipe[0].flag = 1;
					dval = (double) VF[get_ft(iL)][get_ftf(iL)];
					if(dval < 0){
						dval = 0 - dval;
						qpipe[0].status |= 0x10;
					}else{
						qpipe[0].status = 0;
					}
					ddval = sqrt(dval);
					qpipe[0].vf[0] = (float) ddval;
					strcpy(qpipe[0].code, "SQRT");
					break;
				case 2:	/* RSQRT */
					if (hazardcheckFTF(0, get_ft(iL), 0, get_ftf(iL), 1) ||
					    hazardcheckQ()) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					rqpipe[0].flag = 1;
					dval = (double) (VF[get_ft(iL)][get_ftf(iL)]);
					if(dval < 0){
						dval = 0 - dval;
						qpipe[0].status |= 0x10;
					}
					ddval = sqrt(dval);
					dval = (double) (VF[get_fs(iL)][get_fsf(iL)]);
					if(ddval < 0){
						qpipe[0].status |= 0x20;
						*(u_long *)&rqpipe[0].vf[0] = 0x7fffffff;
					}else{
						rqpipe[0].vf[0] = (float) (dval / ddval);
					}
					break;
				case 3:	/* WAITQ */
					if (hazardcheckQ()) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					break;
			}
			break;
		case 15:	/* MTIR, MFIR, ILWR, ISWR */
			switch (get_bc(iL)) {
				case 0:	/* MTIR */
					if (hazardcheck(get_fs(iL), 0, get_dest(iL), 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL) + 32;
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "MTIR");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VF[get_fs(iL)][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VF[get_fs(iL)][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VF[get_fs(iL)][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VF[get_fs(iL)][3];
					break;
				case 1:	/* MFIR */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "MFIR");
					ival = VI[get_fs(iL)];
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = *(float *) &ival;
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = *(float *) &ival;
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = *(float *) &ival;
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = *(float *) &ival;
					break;
				case 2:	/* ILWR */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL) + 32;
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "ILWR");
					if (get_dest(iL) & DEST_X)
						*(long *) &pipe[0][1].vf[0] = state->MEM[VI[get_fs(iL)]][0];
					if (get_dest(iL) & DEST_Y)
						*(long *) &pipe[0][1].vf[1] = state->MEM[VI[get_fs(iL)]][1];
					if (get_dest(iL) & DEST_Z)
						*(long *) &pipe[0][1].vf[2] = state->MEM[VI[get_fs(iL)]][2];
					if (get_dest(iL) & DEST_W)
						*(long *) &pipe[0][1].vf[3] = state->MEM[VI[get_fs(iL)]][3];
					break;
				case 3:	/* ISWR */
					if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 2;
					pipe[0][1].addr = VI[get_fs(iL)];
					strcpy(pipe[0][1].code, "ISWR");
					if (get_dest(iL) & DEST_X)
						*(long *) &pipe[0][1].vf[0] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_Y)
						*(long *) &pipe[0][1].vf[1] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_Z)
						*(long *) &pipe[0][1].vf[2] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_W)
						*(long *) &pipe[0][1].vf[3] = (VI[get_ft(iL)]) & 0xffff;
					break;
			}
			break;
		case 16: /* RNEXT, RGET, RINIT, RXOR */
			switch (get_bc(iL)) {
				case 0: /* RNEXT */
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "RNEXT");
					ival = 0;
					if(((R >> 22) & 0x1) ^ ((R >> 4) & 0x1))
						ival = 1;
					R = ((R << 1) + ival) & 0x007fffff;
					if (get_dest(iL) & DEST_X)
						*(u_long *)&pipe[0][1].vf[0] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_Y)
						*(u_long *)&pipe[0][1].vf[1] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_Z)
						*(u_long *)&pipe[0][1].vf[2] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_W)
						*(u_long *)&pipe[0][1].vf[3] = 0x3f800000 | R;
					break;
				case 1: /* RGET */
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "RGET");
					if (get_dest(iL) & DEST_X)
						*(u_long *)&pipe[0][1].vf[0] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_Y)
						*(u_long *)&pipe[0][1].vf[1] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_Z)
						*(u_long *)&pipe[0][1].vf[2] = 0x3f800000 | R;
					if (get_dest(iL) & DEST_W)
						*(u_long *)&pipe[0][1].vf[3] = 0x3f800000 | R;
					break;
				case 2: /* RINIT */
					if (hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					R = *(u_long *)&VF[get_fs(iL)][get_fsf(iL)] & 0x7fffff;
					break;
				case 3: /* RXOR */
					if (hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					R = R ^ (*(u_long *)&VF[get_fs(iL)][get_fsf(iL)] & 0x7fffff);
					break;
			}
			break;
		case 25:	/* MFP, ---, ---, --- */
			switch (get_bc(iL)) {
				case 0:	/* MFP */
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 7;
					strcpy(pipe[0][1].code, "MFP");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VN[4];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VN[4];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VN[4];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VN[4];
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08x)\n", iL);
					exit(1);
					break;
			}
			break;
		case 26:	/* XITOP, XTOP */
			switch (get_bc(iL)) {
				case 0:	/* XTOP */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = (short) _TOP;
					strcpy(ipipe[0].code, "XTOP");
					break;
				case 1:	/* XITOP */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = (short) _ITOP;
					strcpy(ipipe[0].code, "XITOP");
					break;
			}
			break;
		case 27:	/* XGKICK */
			GpuIfKick(state, VI[get_fs(iL)]); 
			break;
		case 28:	/* ESADD, ERSADD, ELENG, ERLENG */
			switch (get_bc(iL)) {
				case 0:	/* ESADD */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xe, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[1] = VF[get_fs(iL)][1];
					VN[2] = VF[get_fs(iL)][2];
					spipe.vn = VN[0] * VN[0] + VN[1] * VN[1] + VN[2] * VN[2];
					spipe.no = 9;
					break;
				case 1:	/* ERSADD */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xe, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[1] = VF[get_fs(iL)][1];
					VN[2] = VF[get_fs(iL)][2];
					VN[5] = VN[0] * VN[0] + VN[1] * VN[1] + VN[2] * VN[2];
					spipe.vn = 1.0 / VN[5];
					spipe.no = 16;
					break;
				case 2:	/* ELENG */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xe, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[1] = VF[get_fs(iL)][1];
					VN[2] = VF[get_fs(iL)][2];
					dval = (double) (VN[0] * VN[0] + VN[1] * VN[1] + VN[2] * VN[2]);
					spipe.vn = (float) sqrt(dval);
					spipe.no = 16;
					break;
				case 3:	/* ERLENG */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xe, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[1] = VF[get_fs(iL)][1];
					VN[2] = VF[get_fs(iL)][2];
					dval = (double) (VN[0] * VN[0] + VN[1] * VN[1] + VN[2] * VN[2]);
					spipe.vn = 1.0 / (float) sqrt(dval);
					spipe.no = 22;
					break;
			}
			break;
		case 29:	/* EATANxy, EATANxz, ESUM, --- */
			switch (get_bc(iL)) {
				case 0:	/* EATANxy */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xc, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[1] = VF[get_fs(iL)][1];
					VN[5] = (VN[1] - VN[0]) / (VN[1] + VN[0]);
					fval = VN[5];
					spipe.vn = VN[14] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[15] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[16] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[17] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[18] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[19] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[20] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[21] * fval;
					spipe.no = 52;
					break;
				case 1:	/* EATANxz */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xa, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][0];
					VN[2] = VF[get_fs(iL)][2];
					VN[5] = (VN[2] - VN[0]) / (VN[2] + VN[0]);
					fval = VN[5];
					spipe.vn = VN[14] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[15] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[16] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[17] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[18] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[19] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[20] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[21] * fval;
					spipe.no = 52;
					break;
				case 2:	/* ESUM */
					if (hazardcheckP() || hazardcheck(0, get_ft(iL), 0xf, 1)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					spipe.vn = VF[get_fs(iL)][0] + VF[get_fs(iL)][1]
						+ VF[get_fs(iL)][2] + VF[get_fs(iL)][3];
					spipe.no = 10;
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08x)\n", iL);
					exit(1);
					break;
			}
			break;
		case 30:	/* ESQRT, ERSQRT, ERCPR, WAITP */
			switch (get_bc(iL)) {
				case 0:	/* ESQRT */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					dval = (double) VN[0];
					spipe.vn = (float) sqrt(dval);
					spipe.no = 10;
					break;
				case 1:	/* ERSQRT */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					dval = (double) VN[0];
					spipe.vn = 1.0 / (float) sqrt(dval);
					spipe.no = 16;
					break;
				case 2:	/* ERCPR */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					spipe.vn = 1.0 / VN[0];
					spipe.no = 10;
					break;
				case 3:	/* WAITP */
					if (hazardcheckP()) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					break;
			}
			break;
		case 31:	/* ESIN, EATAN, EEXP, --- */
			switch (get_bc(iL)) {
				case 0:	/* SIN */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					fval = VN[0];
					spipe.vn = VN[9] * fval;
					fval = fval * VN[0] * VN[0];
					spipe.vn += VN[10] * fval;
					fval = fval * VN[0] * VN[0];
					spipe.vn += VN[11] * fval;
					fval = fval * VN[0] * VN[0];
					spipe.vn += VN[12] * fval;
					fval = fval * VN[0] * VN[0];
					spipe.vn += VN[13] * fval;
					spipe.no = 27;
					break;
				case 1:	/* EATAN */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					VN[5] = (VN[0] - 1.0) / (VN[0] + 1.0);
					fval = VN[5];
					spipe.vn = VN[14] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[15] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[16] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[17] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[18] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[19] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[20] * fval;
					fval = fval * VN[5] * VN[5];
					spipe.vn += VN[21] * fval;
					spipe.no = 52;
					break;
				case 2:	/* EEXP */
					if (hazardcheckP() ||
					    hazardcheckFTF(get_fs(iL), 0, get_fsf(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					VN[0] = VF[get_fs(iL)][get_fsf(iL)];
					fval = 0.0 - VN[0];
					VN[5] = fval;
					VN[6] = 1.0;
					VN[6] += VN[23] * VN[5];
					VN[5] *= fval;
					VN[6] += VN[24] * VN[5];
					VN[5] *= fval;
					VN[6] += VN[25] * VN[5];
					VN[5] *= fval;
					VN[6] += VN[26] * VN[5];
					VN[5] *= fval;
					VN[6] += VN[27] * VN[5];
					VN[5] *= fval;
					VN[6] += VN[28] * VN[5];
					fval = VN[6] * VN[6] * VN[6] * VN[6];
					spipe.vn = 1.0 / fval;
					spipe.no = 42;
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08x)\n", iL);
					exit(1);
					break;
			}
			break;
		default:
			fprintf(stderr, "Undefined opcode(LowerOp:%08x)\n", iL);
			exit(1);
	}
	return 0;
}

int Upper_special(VectorUnitState *state, u_long iH, u_long iL)
{
/*
[name]		Upper_special
[desc.]		Micro Special OPCODE (Upper instruction) execute
[args]		iH: Upper instruction(32bits)
			iL: lower instruction(32bits)
[return]	0: executed
			1: stall ( hazard occured)
*/

	float w;
	double dval, dval2;
	long ix, iy, iz, iw;

	switch ((iH >> 6) & 0x1f) {	/* bit42-38 */
		case 0:	/* ADDAx, ADDAy, ADDAz, ADDAw */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].flag = 4;
			strcpy(pipe[0][0].code, "ADDA");
			apipe.flag = 1;
			apipe.mask = get_dest(iH);
			if (get_dest(iH) & DEST_X) {
				apipe.acc[0] = VF[get_fs(iH)][0] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				apipe.acc[1] = VF[get_fs(iH)][1] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				apipe.acc[2] = VF[get_fs(iH)][2] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				apipe.acc[3] = VF[get_fs(iH)][3] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
			}
			break;
		case 1:	/* SUBAx, SUBAy, SUBAz, SUBAw */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].flag = 4;
			strcpy(pipe[0][0].code, "SUBA");
			apipe.flag = 1;
			apipe.mask = get_dest(iH);
			if (get_dest(iH) & DEST_X) {
				apipe.acc[0] = VF[get_fs(iH)][0] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				apipe.acc[1] = VF[get_fs(iH)][1] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				apipe.acc[2] = VF[get_fs(iH)][2] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				apipe.acc[3] = VF[get_fs(iH)][3] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
			}
			break;
		case 2:	/* MADDAx, MADDAy, MADDAz, MADDAw */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].flag = 4;
			strcpy(pipe[0][0].code, "MADDA");
			apipe.flag = 1;
			apipe.mask = get_dest(iH);
			if (get_dest(iH) & DEST_X) {
				apipe.acc[0] = ACC[0] +
					VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				apipe.acc[1] = ACC[1] +
					VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				apipe.acc[2] = ACC[2] +
					VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				apipe.acc[3] = ACC[3] +
					VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
			}
			break;
		case 3:	/* MSUBAx, MSUBAy, MSUBAz, MSUBAw */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].flag = 4;
			strcpy(pipe[0][0].code, "MSUBA");
			apipe.flag = 1;
			apipe.mask = get_dest(iH);
			if (get_dest(iH) & DEST_X) {
				apipe.acc[0] = ACC[0] -
					VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				apipe.acc[1] = ACC[1] -
					VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				apipe.acc[2] = ACC[2] -
					VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				apipe.acc[3] = ACC[3] -
					VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
			}
			break;
		case 4: /* ITOF0, ITOF4, ITOF12, ITOF15 */
			if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
				return 1;
			pipe[0][0].no = get_ft(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			switch (get_bc(iH)) {
				case 0:	/* ITOF0 */
					strcpy(pipe[0][0].code, "ITOF0");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = *(long *) &VF[get_fs(iH)][0] / 1.0;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = *(long *) &VF[get_fs(iH)][1] / 1.0;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = *(long *) &VF[get_fs(iH)][2] / 1.0;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = *(long *) &VF[get_fs(iH)][3] / 1.0;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* ITOF4 */
					strcpy(pipe[0][0].code, "ITOF4");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = *(long *) &VF[get_fs(iH)][0] / 16.0;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = *(long *) &VF[get_fs(iH)][1] / 16.0;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = *(long *) &VF[get_fs(iH)][2] / 16.0;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = *(long *) &VF[get_fs(iH)][3] / 16.0;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* ITOF12 */
					strcpy(pipe[0][0].code, "ITOF12");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = *(long *)&VF[get_fs(iH)][0] / 4096.0;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = *(long *)&VF[get_fs(iH)][1] / 4096.0;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = *(long *)&VF[get_fs(iH)][2] / 4096.0;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = *(long *)&VF[get_fs(iH)][3] / 4096.0;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* ITOF15 */
					strcpy(pipe[0][0].code, "ITOF15");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] =
							*(long *) &VF[get_fs(iH)][0] / 32768.0;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] =
							*(long *) &VF[get_fs(iH)][1] / 32768.0;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] =
							*(long *) &VF[get_fs(iH)][2] / 32768.0;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] =
							*(long *) &VF[get_fs(iH)][3] / 32768.0;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
			}
			break;
		case 5: /* FTOI0, FTOI4, FTOI12, FTOI15 */
			if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
				return 1;
			pipe[0][0].no = get_ft(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			switch (get_bc(iH)) {
				case 0:	/* FTOI0 */
					strcpy(pipe[0][0].code, "FTOI0");
					if (get_dest(iH) & DEST_X) {
						ix = VF[get_fs(iH)][0];
						*(long *) &pipe[0][0].vf[0] = ix;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						iy = VF[get_fs(iH)][1];
						*(long *) &pipe[0][0].vf[1] = iy;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						iz = VF[get_fs(iH)][2];
						*(long *) &pipe[0][0].vf[2] = iz;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						iw = VF[get_fs(iH)][3];
						*(long *) &pipe[0][0].vf[3] = iw;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* FTOI4 */
					strcpy(pipe[0][0].code, "FTOI4");
					if (get_dest(iH) & DEST_X) {
						ix = VF[get_fs(iH)][0] * 16;
						*(long *) &pipe[0][0].vf[0] = ix;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						iy = VF[get_fs(iH)][1] * 16;
						*(long *) &pipe[0][0].vf[1] = iy;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						iz = VF[get_fs(iH)][2] * 16;
						*(long *) &pipe[0][0].vf[2] = iz;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						iw = VF[get_fs(iH)][3] * 16;
						*(long *) &pipe[0][0].vf[3] = iw;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* FTOI12 */
					strcpy(pipe[0][0].code, "FTOI12");
					if (get_dest(iH) & DEST_X) {
						ix = VF[get_fs(iH)][0] * 4096;
						*(long *) &pipe[0][0].vf[0] = ix;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						iy = VF[get_fs(iH)][1] * 4096;
						*(long *) &pipe[0][0].vf[1] = iy;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						iz = VF[get_fs(iH)][2] * 4096;
						*(long *) &pipe[0][0].vf[2] = iz;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						iw = VF[get_fs(iH)][3] * 4096;
						*(long *) &pipe[0][0].vf[3] = iw;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* FTOI15 */
					strcpy(pipe[0][0].code, "FTOI15");
					if (get_dest(iH) & DEST_X) {
						ix = VF[get_fs(iH)][0] * 32768;
						*(long *) &pipe[0][0].vf[0] = ix;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						iy = VF[get_fs(iH)][1] * 32768;
						*(long *) &pipe[0][0].vf[1] = iy;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						iz = VF[get_fs(iH)][2] * 32768;
						*(long *) &pipe[0][0].vf[2] = iz;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						iw = VF[get_fs(iH)][3] * 32768;
						*(long *) &pipe[0][0].vf[3] = iw;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
			}
			break;
		case 6:	/* MULAx, MULAy, MULAz, MULAw */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].flag = 4;
			strcpy(pipe[0][0].code, "MULA");
			apipe.flag = 1;
			apipe.mask = get_dest(iH);
			if (get_dest(iH) & DEST_X) {
				apipe.acc[0] = VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				apipe.acc[1] = VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				apipe.acc[2] = VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				apipe.acc[3] = VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
			}
			break;
		case 7: /* MULAq, ABS, MULAi, CLIP */
			switch (get_bc(iH)) {
				case 0:	/* MULAq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MULAq");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] * Q;
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] * Q;
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] * Q;
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] * Q;
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* ABS */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_ft(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "ABS");
					if (get_dest(iH) & DEST_X) {
						if (VF[get_fs(iH)][0] > 0) {
							pipe[0][0].vf[0] = VF[get_fs(iH)][0];
						} else {
							pipe[0][0].vf[0] = 0 - VF[get_fs(iH)][0];
						}
						statuscheck(VF[get_ft(iH)][0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						if (VF[get_fs(iH)][1] > 0) {
							pipe[0][0].vf[1] = VF[get_fs(iH)][1];
						} else {
							pipe[0][0].vf[1] = 0 - VF[get_fs(iH)][1];
						}
						statuscheck(VF[get_ft(iH)][1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						if (VF[get_fs(iH)][2] > 0) {
							pipe[0][0].vf[2] = VF[get_fs(iH)][2];
						} else {
							pipe[0][0].vf[2] = 0 - VF[get_fs(iH)][2];
						}
						statuscheck(VF[get_ft(iH)][2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						if (VF[get_fs(iH)][3] > 0) {
							pipe[0][0].vf[3] = VF[get_fs(iH)][3];
						} else {
							pipe[0][0].vf[3] = 0 - VF[get_fs(iH)][3];
						}
						statuscheck(VF[get_ft(iH)][3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* MULAi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MULAi");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (iH & 0x80000000) {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] * (*(float *) &iL);
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] * (*(float *) &iL);
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] * (*(float *) &iL);
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] * (*(float *) &iL);
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					} else {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] * I;
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] * I;
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] * I;
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] * I;
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					}
					break;
				case 3:	/* CLIP */
					if (hazardcheck(get_fs(iH), 0, 0xf, 2))
						return 1;
					pipe[0][0].flag = 5;
					pipe[0][0].status = 0;
					strcpy(pipe[0][0].code, "CLIP");
					if (VF[get_ft(iH)][3] < 0)
						w = 0 - VF[get_ft(iH)][3];
					else
						w = VF[get_ft(iH)][3];
					if (VF[get_fs(iH)][0] > w)
						pipe[0][0].status |= 0x1;
					if (VF[get_fs(iH)][0] < (0 - w))
						pipe[0][0].status |= 0x2;
					if (VF[get_fs(iH)][1] > w)
						pipe[0][0].status |= 0x4;
					if (VF[get_fs(iH)][1] < (0 - w))
						pipe[0][0].status |= 0x8;
					if (VF[get_fs(iH)][2] > w)
						pipe[0][0].status |= 0x10;
					if (VF[get_fs(iH)][2] < (0 - w))
						pipe[0][0].status |= 0x20;
					break;
			}
			break;
		case 8: /* ADDAq, MADDAq, ADDAi, MADDAi */
			switch (get_bc(iH)) {
				case 0:	/* ADDAq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "ADDAq");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] + Q;
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] + Q;
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] + Q;
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] + Q;
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MADDAq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MADDAq");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = ACC[0] + VF[get_fs(iH)][0] * Q;
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = ACC[1] + VF[get_fs(iH)][1] * Q;
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = ACC[2] + VF[get_fs(iH)][2] * Q;
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = ACC[3] + VF[get_fs(iH)][3] * Q;
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* ADDAi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "ADDAi");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (iH & 0x80000000) {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] + (*(float *) &iL);
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] + (*(float *) &iL);
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] + (*(float *) &iL);
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] + (*(float *) &iL);
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					} else {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] + I;
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] + I;
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] + I;
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] + I;
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					}
					break;
				case 3:	/* MADDAi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MADDAi");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (iH & 0x80000000) {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] =
								ACC[0] + VF[get_fs(iH)][0] * (*(float *) &iL);
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] =
								ACC[1] + VF[get_fs(iH)][1] * (*(float *) &iL);
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] =
								ACC[2] + VF[get_fs(iH)][2] * (*(float *) &iL);
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] =
								ACC[3] + VF[get_fs(iH)][3] * (*(float *) &iL);
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					} else {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = ACC[0] + VF[get_fs(iH)][0] * I;
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = ACC[1] + VF[get_fs(iH)][1] * I;
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = ACC[2] + VF[get_fs(iH)][2] * I;
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = ACC[3] + VF[get_fs(iH)][3] * I;
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					}
					break;
			}
			break;
		case 9: /* SUBAq, MSUBAq, SUBAi, MSUBAi */
			switch (get_bc(iH)) {
				case 0:	/* SUBAq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "SUBAq");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] - Q;
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] - Q;
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] - Q;
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] - Q;
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MSUBAq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MSUBAq");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = ACC[0] - VF[get_fs(iH)][0] * Q;
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = ACC[1] - VF[get_fs(iH)][1] * Q;
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = ACC[2] - VF[get_fs(iH)][2] * Q;
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = ACC[3] - VF[get_fs(iH)][3] * Q;
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* SUBAi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "SUBAi");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (iH & 0x80000000) {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] - (*(float *) &iL);
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] - (*(float *) &iL);
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] - (*(float *) &iL);
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] - (*(float *) &iL);
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					} else {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = VF[get_fs(iH)][0] - I;
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = VF[get_fs(iH)][1] - I;
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = VF[get_fs(iH)][2] - I;
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = VF[get_fs(iH)][3] - I;
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					}
					break;
				case 3:	/* MSUBAi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MSUBAi");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (iH & 0x80000000) {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] =
								ACC[0] - VF[get_fs(iH)][0] * (*(float *) &iL);
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] =
								ACC[1] - VF[get_fs(iH)][1] * (*(float *) &iL);
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] =
								ACC[2] - VF[get_fs(iH)][2] * (*(float *) &iL);
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] =
								ACC[3] - VF[get_fs(iH)][3] * (*(float *) &iL);
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					} else {
						if (get_dest(iH) & DEST_X) {
							apipe.acc[0] = ACC[0] - VF[get_fs(iH)][0] * I;
							statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
						}
						if (get_dest(iH) & DEST_Y) {
							apipe.acc[1] = ACC[1] - VF[get_fs(iH)][1] * I;
							statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
						}
						if (get_dest(iH) & DEST_Z) {
							apipe.acc[2] = ACC[2] - VF[get_fs(iH)][2] * I;
							statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
						}
						if (get_dest(iH) & DEST_W) {
							apipe.acc[3] = ACC[3] - VF[get_fs(iH)][3] * I;
							statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
						}
					}
					break;
			}
			break;
		case 10: /* ADDA, MADDA, MULA, --- */
			switch (get_bc(iH)) {
				case 0:	/* ADDA */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "ADDA");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] + VF[get_ft(iH)][0];
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] + VF[get_ft(iH)][1];
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] + VF[get_ft(iH)][2];
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] + VF[get_ft(iH)][3];
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MADDA */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MADDA");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = 
							ACC[0] + VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = 
							ACC[1] + VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = 
							ACC[2] + VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = 
							ACC[3] + VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* MULA */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MULA");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:
					fprintf(stderr, "Undefined opcode(UpperOp:%08x)\n", iH);
					break;
			}
			break;
		case 11: /* SUBA, MSUBA, OPMULA, NOP */
			switch (get_bc(iH)) {
				case 0:	/* SUBA */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "SUBA");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = VF[get_fs(iH)][0] - VF[get_ft(iH)][0];
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = VF[get_fs(iH)][1] - VF[get_ft(iH)][1];
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = VF[get_fs(iH)][2] - VF[get_ft(iH)][2];
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = VF[get_fs(iH)][3] - VF[get_ft(iH)][3];
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MSUBA */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].flag = 4;
					strcpy(pipe[0][0].code, "MSUBA");
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					if (get_dest(iH) & DEST_X) {
						apipe.acc[0] = 
							ACC[0] - VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						apipe.acc[1] = 
							ACC[1] - VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						apipe.acc[2] = 
							ACC[2] - VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						apipe.acc[3] = 
							ACC[3] - VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(apipe.acc[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* OPMULA */
					if (hazardcheck(get_fs(iH), get_ft(iH), 0xe, 3))
						return 1;
					pipe[0][0].flag = 4;
					apipe.flag = 1;
					apipe.mask = get_dest(iH);
					strcpy(pipe[0][0].code, "OPMULA");
					apipe.acc[0] = VF[get_fs(iH)][1] * VF[get_ft(iH)][2];
					statuscheck(apipe.acc[0], &pipe[0][0].status, 0);
					apipe.acc[1] = VF[get_fs(iH)][2] * VF[get_ft(iH)][0];
					statuscheck(apipe.acc[1], &pipe[0][0].status, 1);
					apipe.acc[2] = VF[get_fs(iH)][0] * VF[get_ft(iH)][1];
					statuscheck(apipe.acc[2], &pipe[0][0].status, 2);
					break;
				case 3:	/* NOP */
					break;
			}
			break;
		default:
			fprintf(stderr, "Undefined opcode(%08x)\n", iH);
			exit(1);
	}
	return 0;
}

int exec_inst(VectorUnitState *state)
{
/*
[name]		exec_inst
[desc.]		execute instruction ( second stage of pipeline)
[args]		void
[return]	0: finished
			1: stall (hazard occured)
			2: executed
*/
	double dval;
	float fval;
	u_long iH, iL;
	long imm12, imm11, imm15, imm5, imm24;

	if (peflag == -1)
		return 0;

	if (peflag == 1)
		peflag = -1;
	if (instbuf[0] & 0x40000000)
		eflag = 1;

	iH = instbuf[0];
	iL = instbuf[1];

	pipe[0][0].status = 0;
	pipe[0][1].status = 0;
	qpipe[0].status = 0;
	rqpipe[0].status = 0;

	if (iH & 0x80000000) {
		Ipipe.flag = 1;
		Ipipe.val = *(float *) &iL;
		/* pipe[0][1].flag = 3;
		pipe[0][1].vf[0] = *(float *)&iL; */
	}
	/* UpperOp execute */
	switch ((iH >> 2) & 0xf) {	/* bit37-34 */
		case 0:	/* ADDx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "ADD");
			if (get_dest(iH) & DEST_X) {
				pipe[0][0].vf[0] =
					VF[get_fs(iH)][0] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				pipe[0][0].vf[1] =
					VF[get_fs(iH)][1] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Z) {
				pipe[0][0].vf[2] =
					VF[get_fs(iH)][2] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_W) {
				pipe[0][0].vf[3] =
					VF[get_fs(iH)][3] + VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 0);
			}
			break;
		case 1:	/* SUBx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "SUB");
			if (get_dest(iH) & DEST_X) {
				pipe[0][0].vf[0] =
					VF[get_fs(iH)][0] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				pipe[0][0].vf[1] =
					VF[get_fs(iH)][1] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				pipe[0][0].vf[2] =
					VF[get_fs(iH)][2] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				pipe[0][0].vf[3] =
					VF[get_fs(iH)][3] - VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
			}
			break;
		case 2:	/* MADDx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "MADD");
			if (get_dest(iH) & DEST_X) {
				pipe[0][0].vf[0] = ACC[0] +
					VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				pipe[0][0].vf[1] = ACC[1] +
					VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				pipe[0][0].vf[2] = ACC[2] +
					VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				pipe[0][0].vf[3] = ACC[3] +
					VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
			}
			break;
		case 3:	/* MSUBx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "MSUB");
			if (get_dest(iH) & DEST_X) {
				pipe[0][0].vf[0] = ACC[0] -
					VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				pipe[0][0].vf[1] = ACC[1] -
					VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				pipe[0][0].vf[2] = ACC[2] -
					VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				pipe[0][0].vf[3] = ACC[3] -
					VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
			}
			break;
		case 4:	/* MAXx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "MAX");
			if (get_dest(iH) & DEST_X) {
				if (VF[get_fs(iH)][0] > VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[0] = VF[get_fs(iH)][0];
				else
					pipe[0][0].vf[0] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_Y) {
				if (VF[get_fs(iH)][1] > VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[1] = VF[get_fs(iH)][1];
				else
					pipe[0][0].vf[1] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_Z) {
				if (VF[get_fs(iH)][2] > VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[2] = VF[get_fs(iH)][2];
				else
					pipe[0][0].vf[2] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_W) {
				if (VF[get_fs(iH)][3] > VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[3] = VF[get_fs(iH)][3];
				else
					pipe[0][0].vf[3] = VF[get_ft(iH)][get_bc(iH)];
			}
			break;
		case 5:	/* MINIx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "MINI");
			if (get_dest(iH) & DEST_X) {
				if (VF[get_fs(iH)][0] < VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[0] = VF[get_fs(iH)][0];
				else
					pipe[0][0].vf[0] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_Y) {
				if (VF[get_fs(iH)][1] < VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[1] = VF[get_fs(iH)][1];
				else
					pipe[0][0].vf[1] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_Z) {
				if (VF[get_fs(iH)][2] < VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[2] = VF[get_fs(iH)][2];
				else
					pipe[0][0].vf[2] = VF[get_ft(iH)][get_bc(iH)];
			}
			if (get_dest(iH) & DEST_W) {
				if (VF[get_fs(iH)][3] < VF[get_ft(iH)][get_bc(iH)])
					pipe[0][0].vf[3] = VF[get_fs(iH)][3];
				else
					pipe[0][0].vf[3] = VF[get_ft(iH)][get_bc(iH)];
			}
			break;
		case 6:	/* MULx,y,z,w */
			if (hazardcheckBC(get_fs(iH), get_ft(iH), get_dest(iH), get_bc(iH), 3))
				return 1;
			pipe[0][0].no = get_fd(iH);
			pipe[0][0].mask = get_dest(iH);
			pipe[0][0].flag = 1;
			strcpy(pipe[0][0].code, "MUL");
			if (get_dest(iH) & DEST_X) {
				pipe[0][0].vf[0] =
					VF[get_fs(iH)][0] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
			}
			if (get_dest(iH) & DEST_Y) {
				pipe[0][0].vf[1] =
					VF[get_fs(iH)][1] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
			}
			if (get_dest(iH) & DEST_Z) {
				pipe[0][0].vf[2] =
					VF[get_fs(iH)][2] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
			}
			if (get_dest(iH) & DEST_W) {
				pipe[0][0].vf[3] =
					VF[get_fs(iH)][3] * VF[get_ft(iH)][get_bc(iH)];
				statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
			}
			break;
		case 7: /* MULq, MAXi, MULi, MINIi */
			switch (get_bc(iH)) {
				case 0:	/* MULq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MULq");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] * Q;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] * Q;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] * Q;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] * Q;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MAXi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MAXi");
					fval = I;
					if (get_dest(iH) & DEST_X) {
						if (VF[get_fs(iH)][0] > fval) {
							pipe[0][0].vf[0] = VF[get_fs(iH)][0];
						} else {
							pipe[0][0].vf[0] = fval;
						}
					}
					if (get_dest(iH) & DEST_Y) {
						if (VF[get_fs(iH)][1] > fval) {
							pipe[0][0].vf[1] = VF[get_fs(iH)][1];
						} else {
							pipe[0][0].vf[1] = fval;
						}
					}
					if (get_dest(iH) & DEST_Z) {
						if (VF[get_fs(iH)][2] > fval) {
							pipe[0][0].vf[2] = VF[get_fs(iH)][2];
						} else {
							pipe[0][0].vf[2] = fval;
						}
					}
					if (get_dest(iH) & DEST_W) {
						if (VF[get_fs(iH)][3] > fval) {
							pipe[0][0].vf[3] = VF[get_fs(iH)][3];
						} else {
							pipe[0][0].vf[3] = fval;
						}
					}
					break;
				case 2:	/* MULi */
					/* if(iH & 0x80000000){
						   if(hazardcheckI())
							return 1; 
						} */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;

					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MULi");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] * I;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] * I;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] * I;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] * I;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* MINIi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MINIi");
					fval = I;
					if (get_dest(iH) & DEST_X) {
						if (VF[get_fs(iH)][0] < fval) {
							pipe[0][0].vf[0] = VF[get_fs(iH)][0];
						} else {
							pipe[0][0].vf[0] = fval;
						}
					}
					if (get_dest(iH) & DEST_Y) {
						if (VF[get_fs(iH)][1] < fval) {
							pipe[0][0].vf[1] = VF[get_fs(iH)][1];
						} else {
							pipe[0][0].vf[1] = fval;
						}
					}
					if (get_dest(iH) & DEST_Z) {
						if (VF[get_fs(iH)][2] < fval) {
							pipe[0][0].vf[2] = VF[get_fs(iH)][2];
						} else {
							pipe[0][0].vf[2] = fval;
						}
					}
					if (get_dest(iH) & DEST_W) {
						if (VF[get_fs(iH)][3] < fval) {
							pipe[0][0].vf[3] = VF[get_fs(iH)][3];
						} else {
							pipe[0][0].vf[3] = fval;
						}
					}
					break;
			}
			break;
		case 8: /* ADDq, MADDq, ADDi, MADDi */
			switch (get_bc(iH)) {
				case 0:	/* ADDq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "ADDq");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] + Q;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] + Q;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] + Q;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] + Q;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MADDq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MADDq");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] + VF[get_fs(iH)][0] * Q;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] + VF[get_fs(iH)][1] * Q;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] + VF[get_fs(iH)][2] * Q;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] + VF[get_fs(iH)][3] * Q;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* ADDi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;

					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "ADDi");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] + I;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] + I;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] + I;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] + I;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* MADDi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;

					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MADDi");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] + VF[get_fs(iH)][0] * I;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] + VF[get_fs(iH)][1] * I;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] + VF[get_fs(iH)][2] * I;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] + VF[get_fs(iH)][3] * I;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
			}
			break;
		case 9: /* SUBq, MSUBq, SUBi, MSUBi */
			switch (get_bc(iH)) {
				case 0:	/* SUBq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "SUBq");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] - Q;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] - Q;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] - Q;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] - Q;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MSUBq */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MSUBq");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] - VF[get_fs(iH)][0] * Q;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] - VF[get_fs(iH)][1] * Q;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] - VF[get_fs(iH)][2] * Q;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] - VF[get_fs(iH)][3] * Q;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* SUBi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;

					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "SUBi");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = VF[get_fs(iH)][0] - I;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = VF[get_fs(iH)][1] - I;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = VF[get_fs(iH)][2] - I;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = VF[get_fs(iH)][3] - I;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* MSUBi */
					if (hazardcheck(get_fs(iH), 0, get_dest(iH), 2))
						return 1;

					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MSUBi");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] - VF[get_fs(iH)][0] * I;
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] - VF[get_fs(iH)][1] * I;
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] - VF[get_fs(iH)][2] * I;
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] - VF[get_fs(iH)][3] * I;
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
			}
			break;
		case 10: /* ADD, MADD, MUL, MAX */
			switch (get_bc(iH)) {
				case 0:	/* ADD */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "ADD");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] =
							VF[get_fs(iH)][0] + VF[get_ft(iH)][0];
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] =
							VF[get_fs(iH)][1] + VF[get_ft(iH)][1];
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] =
							VF[get_fs(iH)][2] + VF[get_ft(iH)][2];
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] =
							VF[get_fs(iH)][3] + VF[get_ft(iH)][3];
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MADD */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MADD");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] +
							VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] +
							VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] +
							VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] +
							VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* MUL */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MUL");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] =
							VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] =
							VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] =
							VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] =
							VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 3:	/* MAX */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MAX");
					if (get_dest(iH) & DEST_X) {
						if (VF[get_fs(iH)][0] > VF[get_ft(iH)][0]) {
							pipe[0][0].vf[0] = VF[get_fs(iH)][0];
						} else {
							pipe[0][0].vf[0] = VF[get_ft(iH)][0];
						}
					}
					if (get_dest(iH) & DEST_Y) {
						if (VF[get_fs(iH)][1] > VF[get_ft(iH)][1]) {
							pipe[0][0].vf[1] = VF[get_fs(iH)][1];
						} else {
							pipe[0][0].vf[1] = VF[get_ft(iH)][1];
						}
					}
					if (get_dest(iH) & DEST_Z) {
						if (VF[get_fs(iH)][2] > VF[get_ft(iH)][2]) {
							pipe[0][0].vf[2] = VF[get_fs(iH)][2];
						} else {
							pipe[0][0].vf[2] = VF[get_ft(iH)][2];
						}
					}
					if (get_dest(iH) & DEST_W) {
						if (VF[get_fs(iH)][3] > VF[get_ft(iH)][3]) {
							pipe[0][0].vf[3] = VF[get_fs(iH)][3];
						} else {
							pipe[0][0].vf[3] = VF[get_ft(iH)][3];
						}
					}
					break;
			}
			break;
		case 11: /* SUB, MSUB, OPMSUB, MINI */
			switch (get_bc(iH)) {
				case 0:	/* SUB */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "SUB");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] =
							VF[get_fs(iH)][0] - VF[get_ft(iH)][0];
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] =
							VF[get_fs(iH)][1] - VF[get_ft(iH)][1];
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] =
							VF[get_fs(iH)][2] - VF[get_ft(iH)][2];
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] =
							VF[get_fs(iH)][3] - VF[get_ft(iH)][3];
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 1:	/* MSUB */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "MSUB");
					if (get_dest(iH) & DEST_X) {
						pipe[0][0].vf[0] = ACC[0] -
							VF[get_fs(iH)][0] * VF[get_ft(iH)][0];
						statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					}
					if (get_dest(iH) & DEST_Y) {
						pipe[0][0].vf[1] = ACC[1] -
							VF[get_fs(iH)][1] * VF[get_ft(iH)][1];
						statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					}
					if (get_dest(iH) & DEST_Z) {
						pipe[0][0].vf[2] = ACC[2] -
							VF[get_fs(iH)][2] * VF[get_ft(iH)][2];
						statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					}
					if (get_dest(iH) & DEST_W) {
						pipe[0][0].vf[3] = ACC[3] -
							VF[get_fs(iH)][3] * VF[get_ft(iH)][3];
						statuscheck(pipe[0][0].vf[3], &pipe[0][0].status, 3);
					}
					break;
				case 2:	/* OPMSUB */
					if (hazardcheck(get_fs(iH), get_ft(iH), 0xe, 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = 0xe;
					pipe[0][0].flag = 1;
					strcpy(pipe[0][0].code, "OPMSUB");
					pipe[0][0].vf[0] = ACC[0] -
						VF[get_fs(iH)][1] * VF[get_ft(iH)][2];
					statuscheck(pipe[0][0].vf[0], &pipe[0][0].status, 0);
					pipe[0][0].vf[1] = ACC[1] -
						VF[get_fs(iH)][2] * VF[get_ft(iH)][0];
					statuscheck(pipe[0][0].vf[1], &pipe[0][0].status, 1);
					pipe[0][0].vf[2] = ACC[2] -
						VF[get_fs(iH)][0] * VF[get_ft(iH)][1];
					statuscheck(pipe[0][0].vf[2], &pipe[0][0].status, 2);
					break;
				case 3:	/* MINI */
					if (hazardcheck(get_fs(iH), get_ft(iH), get_dest(iH), 3))
						return 1;
					pipe[0][0].no = get_fd(iH);
					pipe[0][0].mask = get_dest(iH);
					pipe[0][0].flag = 1;
					if (get_dest(iH) & DEST_X) {
						if (VF[get_fs(iH)][0] < VF[get_ft(iH)][0]) {
							pipe[0][0].vf[0] = VF[get_fs(iH)][0];
						} else {
							pipe[0][0].vf[0] = VF[get_ft(iH)][0];
						}
					}
					if (get_dest(iH) & DEST_Y) {
						if (VF[get_fs(iH)][1] < VF[get_ft(iH)][1]) {
							pipe[0][0].vf[1] = VF[get_fs(iH)][1];
						} else {
							pipe[0][0].vf[1] = VF[get_ft(iH)][1];
						}
					}
					if (get_dest(iH) & DEST_Z) {
						if (VF[get_fs(iH)][2] < VF[get_ft(iH)][2]) {
							pipe[0][0].vf[2] = VF[get_fs(iH)][2];
						} else {
							pipe[0][0].vf[2] = VF[get_ft(iH)][2];
						}
					}
					if (get_dest(iH) & DEST_W) {
						if (VF[get_fs(iH)][3] < VF[get_ft(iH)][3]) {
							pipe[0][0].vf[3] = VF[get_fs(iH)][3];
						} else {
							pipe[0][0].vf[3] = VF[get_ft(iH)][3];
						}
					}
					break;
			}
			break;
		case 15:
			if (Upper_special(state, iH, iL))
				return 1;
			break;
		default:
			fprintf(stderr, "Undefined opcode(%08x)\n", iH);
			exit(1);
	}

	if (iH & 0x80000000)
		return 1;
	if (iH & 0x04000000) {
		/* DEBUG PRINT */
		if (dpr_mode)
			printf("\"%08lx\"\n", iL);
		return 1;
	}
	/* LowerOp execute */
	switch ((iL >> 27) & 0x1f) {
		case 0:	/* LQ, SQ, ---, --- */
			if (iL & 0x00000400)
				imm11 = (iL & 0x07ff) | 0xfffff800;
			else
				imm11 = iL & 0x07ff;
			switch (get_ic(iL)) {
				case 0:	/* LQ */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL);
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "LQ");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] =
							*(float *) &state->MEM[imm11 + VI[get_fs(iL)]][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] =
							*(float *) &state->MEM[imm11 + VI[get_fs(iL)]][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] =
							*(float *) &state->MEM[imm11 + VI[get_fs(iL)]][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] =
							*(float *) &state->MEM[imm11 + VI[get_fs(iL)]][3];
					break;
				case 1:	/* SQ */
					if (hazardcheck(0, get_ft(iL), get_dest(iL), 1) ||
					  hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 2;
					pipe[0][1].addr = imm11 + VI[get_ft(iL)];
					strcpy(pipe[0][1].code, "SQ");
					if (get_dest(iL) & DEST_X)
						pipe[0][1].vf[0] = VF[get_fs(iL)][0];
					if (get_dest(iL) & DEST_Y)
						pipe[0][1].vf[1] = VF[get_fs(iL)][1];
					if (get_dest(iL) & DEST_Z)
						pipe[0][1].vf[2] = VF[get_fs(iL)][2];
					if (get_dest(iL) & DEST_W)
						pipe[0][1].vf[3] = VF[get_fs(iL)][3];
					break;
				default:
					fprintf(stderr, "Undefined opcode(%08lx)\n", iL);
					exit(1);
			}
			break;
		case 1:	/* ILW, ISW, ---, --- */
			if (iL & 0x00000400)
				imm11 = (iL & 0x07ff) | 0xfffff800;
			else
				imm11 = iL & 0x07ff;
			switch (get_ic(iL)) {
				case 0:	/* ILW */
					if (hazardcheckVI(get_fs(iL), 0, 2)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].no = get_ft(iL) + 32;
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 1;
					strcpy(pipe[0][1].code, "ILW");
					if (get_dest(iL) & DEST_X)
						*(long *) &pipe[0][1].vf[0] =
							state->MEM[imm11 + VI[get_fs(iL)]][0];
					if (get_dest(iL) & DEST_Y)
						*(long *) &pipe[0][1].vf[1] =
							state->MEM[imm11 + VI[get_fs(iL)]][1];
					if (get_dest(iL) & DEST_Z)
						*(long *) &pipe[0][1].vf[2] =
							state->MEM[imm11 + VI[get_fs(iL)]][2];
					if (get_dest(iL) & DEST_W)
						*(long *) &pipe[0][1].vf[3] =
							state->MEM[imm11 + VI[get_fs(iL)]][3];
					break;
				case 1:	/* ISW */
					if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
						pipe[0][0].flag = 0;
						apipe.flag = 0;
						return 1;
					}
					pipe[0][1].mask = get_dest(iL);
					pipe[0][1].flag = 2;
					pipe[0][1].addr = imm11 + VI[get_fs(iL)];
					strcpy(pipe[0][1].code, "ISW");
					if (get_dest(iL) & DEST_X)
						*(long *) &pipe[0][1].vf[0] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_Y)
						*(long *) &pipe[0][1].vf[1] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_Z)
						*(long *) &pipe[0][1].vf[2] = (VI[get_ft(iL)]) & 0xffff;
					if (get_dest(iL) & DEST_W)
						*(long *) &pipe[0][1].vf[3] = (VI[get_ft(iL)]) & 0xffff;
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 2:	/* IADDIU, ISUBIU, ---, --- */
			if (hazardcheckVI(get_fs(iL), 0, 2)) {
				pipe[0][0].flag = 0;
				apipe.flag = 0;
				return 1;
			}
			imm15 = ((iL >> 10) & 0x7800) | (iL & 0x7ff);
			switch (get_ic(iL)) {
				case 0:	/* IADDIU */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = VI[get_fs(iL)] + imm15;
					strcpy(ipipe[0].code, "IADDIU");
					break;
				case 1:	/* ISUBIU */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = VI[get_fs(iL)] - imm15;
					strcpy(ipipe[0].code, "ISUBIU");
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 3:
			fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
			exit(1);
			break;
		case 4: /* FCEQ, FCSET, FCAND, FCOR */
			imm24 = iL & 0x00ffffff;
			switch (get_ic(iL)) {
				case 0:	/* FCEQ */
					ipipe[0].no = 1;
					ipipe[0].flag = 1;
					if(clipflag == imm24)
						ipipe[0].vi = 1;
					else
						ipipe[0].vi = 0;
					strcpy(ipipe[0].code, "FCEQ");
					break;
				case 1:	/* FCSET */
					pipe[0][1].flag = 5;
					pipe[0][1].status = imm24;
					strcpy(pipe[0][1].code, "FCSET");
					break;
				case 2: /* FCAND */
					ipipe[0].no = 1;
					ipipe[0].flag = 1;
					if(clipflag & imm24)
						ipipe[0].vi = 1;
					else
						ipipe[0].vi = 0;
					strcpy(ipipe[0].code, "FCAND");
					break;
				case 3: /* FCOR */
					ipipe[0].no = 1;
					ipipe[0].flag = 1;
					if((clipflag | imm24) == 0x00ffffff)
						ipipe[0].vi = 1;
					else
						ipipe[0].vi = 0;
					strcpy(ipipe[0].code, "FCOR");
					break;
			}
			break;
		case 5: /* FSEQ, FSSET, FSAND, FSOR */
			imm12 = ((iL >> 10) & 0x0800) | (iL & 0x7ff);
			switch (get_ic(iL)) {
				case 0:	/* FSEQ */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					if(statusflag == imm12)
						ipipe[0].vi = 1;
					else
						ipipe[0].vi = 0;
					strcpy(ipipe[0].code, "FSEQ");
					break;
				case 1:	/* FSSET */
					pipe[0][1].flag = 4;
					pipe[0][1].status = imm12 & 0xfc0;
					strcpy(pipe[0][1].code, "FSSET");
					break;
				case 2: /* FSAND */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = statusflag & imm12;
					strcpy(ipipe[0].code, "FSAND");
					break;
				case 3: /* FSOR */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = statusflag | imm12;
					strcpy(ipipe[0].code, "FSOR");
					break;
			}
			break;
		case 6:	/* FMEQ, NULL, FMAND, FMOR */
			switch (get_ic(iL)) {
				case 0:	/* FMEQ */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					if ((short) MACflag == VI[get_fs(iL)])
						ipipe[0].vi = 1;
					else
						ipipe[0].vi = 0;
					strcpy(ipipe[0].code, "FMEQ");
					break;
				case 2:	/* FMAND */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = (MACflag & 0xffff) & VI[get_fs(iL)];
					strcpy(ipipe[0].code, "FMAND");
					break;
				case 3:	/* FMOR */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = (MACflag & 0xffff) | VI[get_fs(iL)];
					strcpy(ipipe[0].code, "FMOR");
					break;
			}
			break;
		case 7: /* FCGET, ---, ---, --- */
			switch (get_ic(iL)) {
				case 0:	/* FCGET */
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = clipflag & 0x0fff;
					strcpy(ipipe[0].code, "FCGET");
				break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 8:	/* B, BAL, ---, --- */
			if (iL & 0x00000400)
				imm11 = (iL & 0x07ff) | 0xfffff800;
			else
				imm11 = iL & 0x07ff;
			switch (get_ic(iL)) {
				case 0:	/* B */
					jaddr = pc + imm11;
					jflag = 1;
					break;
				case 1:	/* BAL */
					jaddr = pc + imm11;
					jflag = 1;
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = pc + 1;
					strcpy(ipipe[0].code, "BAL");
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 9:	/* JR, JALR, ---, --- */
			if (hazardcheckVI(get_fs(iL), 0, 2)) {
				pipe[0][0].flag = 0;
				apipe.flag = 0;
				return 1;
			}
			switch (get_ic(iL)) {
				case 0:	/* JR */
					jaddr = VI[get_fs(iL)];
					jflag = 1;
					break;
				case 1:	/* JALR */
					jaddr = VI[get_fs(iL)];
					jflag = 1;
					ipipe[0].no = get_ft(iL);
					ipipe[0].flag = 1;
					ipipe[0].vi = pc + 1;
					strcpy(ipipe[0].code, "JAL");
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 10: /* IBEQ, IBNE, NULL, NULL */
			if (iL & 0x00000400)
				imm11 = (iL & 0x07ff) | 0xfffff800;
			else
				imm11 = iL & 0x07ff;
			if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
				pipe[0][0].flag = 0;
				apipe.flag = 0;
				return 1;
			}
			switch (get_ic(iL)) {
				case 0:	/* IBEQ */
					if (VI[get_ft(iL)] == VI[get_fs(iL)]) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
				case 1:	/* IBNE */
					if (VI[get_ft(iL)] != VI[get_fs(iL)]) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
				default:
					fprintf(stderr, "Undefined opcode(LowerOp:%08lx)\n", iL);
					exit(1);
			}
			break;
		case 11: /* IBLTZ, IBGTZ, IBLEZ, IBGEZ */
			if (iL & 0x00000400)
				imm11 = (iL & 0x07ff) | 0xfffff800;
			else
				imm11 = iL & 0x07ff;
			if (hazardcheckVI(get_fs(iL), 0, 2)) {
				pipe[0][0].flag = 0;
				apipe.flag = 0;
				return 1;
			}
			switch (get_ic(iL)) {
				case 0:	/* IBLTZ */
					if (VI[get_fs(iL)] < 0) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
				case 1:	/* IBGTZ */
					if (VI[get_fs(iL)] > 0) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
				case 2:	/* IBLEZ */
					if (VI[get_fs(iL)] <= 0) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
				case 3:	/* IBGEZ */
					if (VI[get_fs(iL)] >= 0) {
						jaddr = pc + imm11;
						jflag = 1;
					}
					break;
			}
			break;
		case 16:
			switch ((iL >> 2) & 0xf) {
				case 12:	/* IADD, ISUB, IADDI, --- */
					switch (get_bc(iL)) {
						case 0:	/* IADD */
							if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
								pipe[0][0].flag = 0;
								apipe.flag = 0;
								return 1;
							}
							ipipe[0].no = get_fd(iL);
							ipipe[0].flag = 1;
							ipipe[0].vi = VI[get_fs(iL)] + VI[get_ft(iL)];
							strcpy(ipipe[0].code, "IADD");
							break;
						case 1:	/* ISUB */
							if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
								pipe[0][0].flag = 0;
								apipe.flag = 0;
								return 1;
							}
							ipipe[0].no = get_fd(iL);
							ipipe[0].flag = 1;
							ipipe[0].vi = VI[get_fs(iL)] - VI[get_ft(iL)];
							strcpy(ipipe[0].code, "ISUB");
							break;
						case 2:	/* IADDI */
							if (hazardcheckVI(get_fs(iL), 0, 2)) {
								pipe[0][0].flag = 0;
								apipe.flag = 0;
								return 1;
							}
							if (iL & 0x00000400)
								imm5 = ((iL >> 6) & 0x1f) | 0xffffffe0;
							else
								imm5 = (iL >> 6) & 0x1f;
							ipipe[0].no = get_ft(iL);
							ipipe[0].flag = 1;
							ipipe[0].vi = VI[get_fs(iL)] + imm5;
							strcpy(ipipe[0].code, "IADDI");
							break;
						default:
							fprintf(stderr, 
								"Undefined opcode(LowerOp:%08lx)\n", iL);
							exit(1);
					}
					break;
				case 13:	/* IAND, IOR */
					switch (get_bc(iL)) {
						case 0:	/* IAND */
							if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
								pipe[0][0].flag = 0;
								apipe.flag = 0;
								return 1;
							}
							ipipe[0].no = get_fd(iL);
							ipipe[0].flag = 1;
							ipipe[0].vi = VI[get_fs(iL)] & VI[get_ft(iL)];
							strcpy(ipipe[0].code, "IAND");
							break;
						case 1:	/* IOR */
							if (hazardcheckVI(get_fs(iL), get_ft(iL), 3)) {
								pipe[0][0].flag = 0;
								apipe.flag = 0;
								return 1;
							}
							ipipe[0].no = get_fd(iL);
							ipipe[0].flag = 1;
							ipipe[0].vi = VI[get_fs(iL)] | VI[get_ft(iL)];
							strcpy(ipipe[0].code, "IOR");
							break;
					}
					break;
				case 15:
					if (Lower_special(state, iL))
						return 1;
					break;
				default:
					fprintf(stderr, "Undefined opcode(%08lx)\n", iL);
					exit(1);
			}
			break;
		default:
			fprintf(stderr, "Undefined opcode(%08lx)\n", iL);
			exit(1);
	}

	return 2;
}

int fetch_inst(VectorUnitState *state)
{
/*
[name]		fetch_inst
[desc.]		instruction fetchs on Micro memory
[args]		void
[return]	0: finished 
			1: fetched
*/
	if (eflag != -1) {
		if (sflag != 1) {
			opc = pc;
			instbuf[0] = state->uMEM[pc][0];
			instbuf[1] = state->uMEM[pc][1];

/* ihc */
			
	printf("%04d: ", pc);
	opcode_analyze(instbuf);
	putchar('\n');

/* -=ihc=- */
			if (jflag == 1) {
				pc = jaddr;
				jflag = 0;
			} else
				if (eflag == 1) {
					eflag = -1;
					peflag = 1;
				} else {
					pc++;
				}
		}
		return 1;
	}
	return 0;
}

/* move pipeline stage */
int move_pipe(VectorUnitState *state)
{
/*
[name]		move_pipe
[desc.]		move value to next pipeline stage.
[args]		void
[return]	0: no value exist in all pipeline stage
			1: some values exist in pipeline stage
*/

	int i, j, move = 0;

	if (apipe.flag != 0) {
		if (apipe.mask & DEST_X) {
			ACC[0] = apipe.acc[0];
		}
		if (apipe.mask & DEST_Y) {
			ACC[1] = apipe.acc[1];
		}
		if (apipe.mask & DEST_Z) {
			ACC[2] = apipe.acc[2];
		}
		if (apipe.mask & DEST_W) {
			ACC[3] = apipe.acc[3];
		}
		apipe.flag = 0;
	}
	if (ipipe[0].flag != 0) {
		ipipe[0].flag = 0;
		VI[ipipe[0].no] = ipipe[0].vi;
	}
	if (ipipe[0].flag != 0) {
		ipipe[1].no = ipipe[0].no;
		ipipe[1].flag = ipipe[0].flag;
		ipipe[0].flag = 0;
		ipipe[1].vi = ipipe[0].vi;
		strcpy(ipipe[1].code, ipipe[0].code);
		move = 1;
	}

	if (pipe[3][0].flag != 0) {
		switch (pipe[3][0].flag) {
			case 1:/* move to VF-reg */
				if (pipe[3][0].mask & DEST_X) {
					VF[pipe[3][0].no][0] = pipe[3][0].vf[0];
				}
				if (pipe[3][0].mask & DEST_Y) {
					VF[pipe[3][0].no][1] = pipe[3][0].vf[1];
				}
				if (pipe[3][0].mask & DEST_Z) {
					VF[pipe[3][0].no][2] = pipe[3][0].vf[2];
				}
				if (pipe[3][0].mask & DEST_W) {
					VF[pipe[3][0].no][3] = pipe[3][0].vf[3];
				}
				statusflag = (statusflag & 0x00000ff0) |
					((pipe[3][0].status & 0x0f) << 6) |
					(pipe[3][0].status & 0x0f);
				MACflag = (pipe[3][0].status >> 16) & 0xffff;
				break;
			case 2:/* store from VF-reg */
				if (pipe[3][0].mask & DEST_X) {
					state->MEM[pipe[3][0].addr][0] = pipe[3][0].vf[0];
				}
				if (pipe[3][0].mask & DEST_Y) {
					state->MEM[pipe[3][0].addr][1] = pipe[3][0].vf[1];
				}
				if (pipe[3][0].mask & DEST_Z) {
					state->MEM[pipe[3][0].addr][2] = pipe[3][0].vf[2];
				}
				if (pipe[3][0].mask & DEST_W) {
					state->MEM[pipe[3][0].addr][3] = pipe[3][0].vf[3];
				}
				break;
			case 3:/* move to I-reg */
				I = pipe[3][0].vf[0];
				break;
			case 4:/* status only */
				statusflag = (statusflag & 0x00000ff0) |
					((pipe[3][0].status & 0x0f) << 6) |
					 (pipe[3][0].status & 0x0f);
				MACflag = (pipe[3][0].status >> 16) & 0xffff;
			case 5:
				clipflag = (clipflag << 6) & 0x00ffffc0 |
					(pipe[3][0].status & 0x3f);
				break;
		}
		pipe[3][0].flag = 0;
	}
	if (qpipe[6].flag != 0) {
		qpipe[6].flag = 0;
		Q = qpipe[6].vf[0];
		statusflag = (statusflag & 0xfcf) |
			(qpipe[6].status << 6) | qpipe[6].status;
	}
	for (i = 5; i >= 0; i--) {
		if (qpipe[i].flag != 0) {
			qpipe[i + 1].vf[0] = qpipe[i].vf[0];
			qpipe[i + 1].flag = qpipe[i].flag;
			qpipe[i + 1].status = qpipe[i].status;
			qpipe[i].flag = 0;
			strcpy(qpipe[i + 1].code, qpipe[i].code);
			move = 1;
		}
	}

	if (rqpipe[12].flag != 0) {
		rqpipe[12].flag = 0;
		Q = rqpipe[12].vf[0];
		statusflag = (statusflag & 0xfcf) |
			(rqpipe[12].status << 6) | rqpipe[12].status;
	}
	for (i = 11; i >= 0; i--) {
		if (rqpipe[i].flag != 0) {
			rqpipe[i + 1].vf[0] = rqpipe[i].vf[0];
			rqpipe[i + 1].flag = rqpipe[i].flag;
			rqpipe[i + 1].status = rqpipe[i].status;
			rqpipe[i].flag = 0;
			move = 1;
		}
	}
	if (pipe[3][1].flag != 0) {
		switch (pipe[3][1].flag) {
			case 1:
				if (pipe[3][1].no > 31) {
					if (pipe[3][1].mask & DEST_X) {
						VI[(pipe[3][1].no) & 0x1f] =
							*(long *) &pipe[3][1].vf[0] & 0xffff;
					}
					if (pipe[3][1].mask & DEST_Y) {
						VI[(pipe[3][1].no) & 0x1f] =
							*(long *) &pipe[3][1].vf[1] & 0xffff;
					}
					if (pipe[3][1].mask & DEST_Z) {
						VI[(pipe[3][1].no) & 0x1f] =
							*(long *) &pipe[3][1].vf[2] & 0xffff;
					}
					if (pipe[3][1].mask & DEST_W) {
						VI[(pipe[3][1].no) & 0x1f] =
							*(long *) &pipe[3][1].vf[3] & 0xffff;
					}
				} else {
					if (pipe[3][1].mask & DEST_X) {
						VF[pipe[3][1].no][0] = pipe[3][1].vf[0];
					}
					if (pipe[3][1].mask & DEST_Y) {
						VF[pipe[3][1].no][1] = pipe[3][1].vf[1];
					}
					if (pipe[3][1].mask & DEST_Z) {
						VF[pipe[3][1].no][2] = pipe[3][1].vf[2];
					}
					if (pipe[3][1].mask & DEST_W) {
						VF[pipe[3][1].no][3] = pipe[3][1].vf[3];
					}
				}
				break;
			case 2:
				if (strcmp(pipe[3][1].code, "ISW") == 0) {
					if (pipe[3][1].mask & DEST_X) {
						state->MEM[pipe[3][1].addr][0] =
							(*(long *) &pipe[3][1].vf[0]) & 0xffff;
					}
					if (pipe[3][1].mask & DEST_Y) {
						state->MEM[pipe[3][1].addr][1] =
							(*(long *) &pipe[3][1].vf[1]) & 0xffff;
					}
					if (pipe[3][1].mask & DEST_Z) {
						state->MEM[pipe[3][1].addr][2] =
							(*(long *) &pipe[3][1].vf[2]) & 0xffff;
					}
					if (pipe[3][1].mask & DEST_W) {
						state->MEM[pipe[3][1].addr][3] =
							(*(long *) &pipe[3][1].vf[3]) & 0xffff;
					}
				} else {
					if (pipe[3][1].mask & DEST_X) {
						state->MEM[pipe[3][1].addr][0] = *(long *) &pipe[3][1].vf[0];
					}
					if (pipe[3][1].mask & DEST_Y) {
						state->MEM[pipe[3][1].addr][1] = *(long *) &pipe[3][1].vf[1];
					}
					if (pipe[3][1].mask & DEST_Z) {
						state->MEM[pipe[3][1].addr][2] = *(long *) &pipe[3][1].vf[2];
					}
					if (pipe[3][1].mask & DEST_W) {
						state->MEM[pipe[3][1].addr][3] = *(long *) &pipe[3][1].vf[3];
					}
				}
				break;
			case 3:/* move to I-reg */
				I = pipe[3][1].vf[0];
				break;
			case 4:
				statusflag = pipe[3][1].status;
				break;
			case 5:
				clipflag = pipe[3][1].status;
				break;
			case 6:
				if (pipe[3][1].mask & DEST_X) {
					VN[0] = pipe[3][1].vf[0];
				}
				if (pipe[3][1].mask & DEST_Y) {
					VN[1] = pipe[3][1].vf[1];
				}
				if (pipe[3][1].mask & DEST_Z) {
					VN[2] = pipe[3][1].vf[2];
				}
				if (pipe[3][1].mask & DEST_W) {
					VN[3] = pipe[3][1].vf[3];
				}
				break;
			case 7:
				if (pipe[3][1].mask & DEST_X) {
					VF[pipe[3][1].no][0] = pipe[3][1].vf[0];
				}
				if (pipe[3][1].mask & DEST_Y) {
					VF[pipe[3][1].no][1] = pipe[3][1].vf[1];
				}
				if (pipe[3][1].mask & DEST_Z) {
					VF[pipe[3][1].no][2] = pipe[3][1].vf[2];
				}
				if (pipe[3][1].mask & DEST_W) {
					VF[pipe[3][1].no][3] = pipe[3][1].vf[3];
				}
				break;
		}
		pipe[3][1].flag = 0;
	}
	for (i = 2; i >= 0; i--) {
		if (pipe[i][0].flag != 0) {
			move = 1;
			pipe[i + 1][0].flag = pipe[i][0].flag;
			pipe[i][0].flag = 0;
			for (j = 0; j < 4; j++) {
				pipe[i + 1][0].vf[j] = pipe[i][0].vf[j];
			}
			pipe[i + 1][0].no = pipe[i][0].no;
			pipe[i + 1][0].mask = pipe[i][0].mask;
			pipe[i + 1][0].addr = pipe[i][0].addr;
			pipe[i + 1][0].status = pipe[i][0].status;
			pipe[i][0].status = 0;
			strcpy(pipe[i + 1][0].code, pipe[i][0].code);
		}
		if (pipe[i][1].flag != 0) {
			move = 1;
			pipe[i + 1][1].flag = pipe[i][1].flag;
			pipe[i][1].flag = 0;
			for (j = 0; j < 4; j++) {
				pipe[i + 1][1].vf[j] = pipe[i][1].vf[j];
			}
			pipe[i + 1][1].no = pipe[i][1].no;
			pipe[i + 1][1].mask = pipe[i][1].mask;
			pipe[i + 1][1].addr = pipe[i][1].addr;
			pipe[i + 1][1].status = pipe[i][1].status;
			pipe[i][1].status = 0;
			strcpy(pipe[i + 1][1].code, pipe[i][1].code);
		}
	}
	if (spipe.no != 0) {
		if (spipe.no == 1) {
			spipe.no = 0;
			if (spipe.flag == 1) {
				VN[0] = spipe.vn;
			} else {
				VN[4] = spipe.vn;
			}
			spipe.flag = 0;
		} else {
			spipe.no--;
		}
	}
	if (Ipipe.flag) {
		I = Ipipe.val;
		Ipipe.flag = 0;
	}
	return move;
}

void printpipe(void)
{
/*
[name]		printpipe
[desc.]		print out pipeline status to stdout. (for debug mode)
[args]		void
[return]	void
*/

	int i;
	for (i = 0; i < 4; i++) {
		switch (pipe[i][0].flag) {
			case 0:/* not charged */
				printf("Up:%d(nocharge)\n", i);
				break;
			case 1:/* move to reg */
				printf("Up:%d(%s) -> VF%02d\n",
					i, pipe[i][0].code, pipe[i][0].no);
				if (pipe[i][0].mask & DEST_W)
					printf("<%f>", pipe[i][0].vf[3]);
				else
					printf("<--->");
				if (pipe[i][0].mask & DEST_Z)
					printf("<%f>", pipe[i][0].vf[2]);
				else
					printf("<--->");
				if (pipe[i][0].mask & DEST_Y)
					printf("<%f>", pipe[i][0].vf[1]);
				else
					printf("<--->");
				if (pipe[i][0].mask & DEST_X)
					printf("<%f>\n", pipe[i][0].vf[0]);
				else
					printf("<--->\n");
				break;
			case 2:/* store from reg */
				printf("Up:%d(%s) -> 0x%x\n",
				       i, pipe[i][0].code, pipe[i][0].addr);
				break;
			case 4:/* only status */
				printf("Up:%d(%s) -> status <0x%08x>\n",
				     i, pipe[i][0].code, pipe[i][0].status);
				break;
			case 5:/* only clip */
				printf("Up:%d(%s) -> clip <0x%04x>\n",
				     i, pipe[i][0].code, pipe[i][0].status);
				break;
		}
	}
	for (i = 0; i < 4; i++) {
		switch (pipe[i][1].flag) {
			case 1:/* move to reg */
				if (pipe[i][1].no > 31) {
					printf("Lo:%d(%s) -> VI%02d",
					       i, pipe[i][1].code, pipe[i][1].no - 32);
					if (pipe[i][1].mask & DEST_X)
						printf(" <%f>\n", pipe[i][1].vf[0]);
					if (pipe[i][1].mask & DEST_Y)
						printf(" <%f>\n", pipe[i][1].vf[1]);
					if (pipe[i][1].mask & DEST_Z)
						printf(" <%f>\n", pipe[i][1].vf[2]);
					if (pipe[i][1].mask & DEST_W)
						printf(" <%f>\n", pipe[i][1].vf[3]);
				} else {
					printf("Lo:%d(%s) -> VF%02d\n",
					 i, pipe[i][1].code, pipe[i][1].no);
					if (pipe[i][1].mask & DEST_W)
						printf("<%f>", pipe[i][1].vf[3]);
					else
						printf("<--->");
					if (pipe[i][1].mask & DEST_Z)
						printf("<%f>", pipe[i][1].vf[2]);
					else
						printf("<--->");
					if (pipe[i][1].mask & DEST_Y)
						printf("<%f>", pipe[i][1].vf[1]);
					else
						printf("<--->");
					if (pipe[i][1].mask & DEST_X)
						printf("<%f>\n", pipe[i][1].vf[0]);
					else
						printf("<--->\n");
				}
				break;
			case 2:/* store from reg */
				printf("Lo:%d(%s) -> 0x%x\n",
				       i, pipe[i][1].code, pipe[i][1].addr);
				if (pipe[i][1].mask & DEST_W)
					printf("<%f>", pipe[i][1].vf[3]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_Z)
					printf("<%f>", pipe[i][1].vf[2]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_Y)
					printf("<%f>", pipe[i][1].vf[1]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_X)
					printf("<%f>\n", pipe[i][1].vf[0]);
				else
					printf("<--->\n");
				break;
			case 3:/* move to I-reg */
				printf("Lo:%d(LOI) -> I <%f>\n", i, pipe[i][1].vf[0]);
			case 4:/* only status */
				printf("Lo:%d(%s) -> status <0x%08x>\n",
				     i, pipe[i][1].code, pipe[i][1].status);
				break;
			case 5:/* only clip */
				printf("Lo:%d(%s) -> clip <0x%04x>\n",
				     i, pipe[i][1].code, pipe[i][1].status);
				break;
			case 6:/* move to EFU */
				printf("EFU:%d(MFP)\n", i);
				if (pipe[i][1].mask & DEST_W)
					printf("<%f>", pipe[i][1].vf[3]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_Z)
					printf("<%f>", pipe[i][1].vf[2]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_Y)
					printf("<%f>", pipe[i][1].vf[1]);
				else
					printf("<--->");
				if (pipe[i][1].mask & DEST_X)
					printf("<%f>\n", pipe[i][1].vf[0]);
				else
					printf("<--->\n");
				break;
			case 7:/* move from EFU */
				printf("EFU:%d(MFP->VF%02d)", i, pipe[i][1].no);
				if (pipe[i][1].mask & DEST_W)
					printf("<%f>", pipe[i][1].vf[3]);
				if (pipe[i][1].mask & DEST_Z)
					printf("<%f>", pipe[i][1].vf[2]);
				if (pipe[i][1].mask & DEST_Y)
					printf("<%f>", pipe[i][1].vf[1]);
				if (pipe[i][1].mask & DEST_X)
					printf("<%f>", pipe[i][1].vf[0]);
				printf("\n");
				break;
		}
	}
	for (i = 0; i < 2; i++) {
		if (ipipe[i].flag) {
			printf("Lo:%d(%s) -> VI%02d <%d>\n",
				i, ipipe[i].code, ipipe[i].no, ipipe[i].vi);
		}
	}
	if (spipe.no != 0) {
		printf("EFU(%d): <%f>\n", spipe.no, spipe.vn);
	}
	for (i = 0; i < 7; i++) {
		if (qpipe[i].flag) {
			printf("q_pipe(%d)<%s> -> %f\n",
				6 - i, qpipe[i].code, qpipe[i].vf[0]);
		}
	}
}


void printlist(VectorUnitState *state, char *buf)
{
/*
[name]		printlist
[desc.]		print out instruction list to stdout (for debug mode)
[args]		buf: input data from debug mode command line 
[return]	void
*/

	char dmy[32];
	int no, i;
	u_long inst[2];

	if (index(buf, ' ') != (char *) NULL) {
		sscanf(buf, "%s %d", dmy, &no);
	} else {
		no = opc;
	}
	for (i = 0; i < 10; i++) {
		inst[0] = state->uMEM[no + i][0];
		inst[1] = state->uMEM[no + i][1];
		printf("%04d: ", no + i);
		opcode_analyze(inst);
		putchar('\n');
	}
}

void printreg(VectorUnitState *state, char *buf)
{
/* 
[name]		printreg
[desc.]		print out register values(float or int) to stdout
[args]		buf: input data from debug mode command line 
[return]	void
*/

	int i;
	char dmy[32];

	if (buf[1] == 'F') {
		if(buf[2] == 0){
			for(i = 0; i < 32; i++){
				printf("VF%02d <%f> <%f> <%f> <%f>\n",
					i, VF[i][3], VF[i][2], VF[i][1], VF[i][0]);
			}
		}else{
			sscanf(buf, "VF%d", &i);
			printf("VF%02d <%f> <%f> <%f> <%f>\n",
				   i, VF[i][3], VF[i][2], VF[i][1], VF[i][0]);
		}
	} else {
		if (buf[1] == 'I') {
			if (buf[2] == 0){
				for(i = 0; i < 16; i++){
					printf("VI%02d <%d>\n", i, VI[i]);
				}
			}else{
				sscanf(buf, "VI%d", &i);
				printf("VI%02d <%d>\n", i, VI[i]);
			}
		}
	}
}

void printregI(VectorUnitState *state, char *buf)
{
/* 
[name]		printreg
[desc.]		print out register values(hex) to stdout
[args]		buf: input data from debug mode command line 
[return]	void
*/

	int i;
	char dmy[32];

	if (buf[1] == 'f') {
		if(buf[2] == 0) {
			for(i = 0; i < 32; i++){
				printf("VF%02d <%08lx> <%08lx> <%08lx> <%08lx>\n",
				   i, *(u_long *) & (VF[i][3]), *(u_long *) & (VF[i][2]),
				   *(u_long *) & (VF[i][1]), *(u_long *) & (VF[i][0]));
			}
		}else{
			sscanf(buf, "vf%d", &i);
			printf("VF%02d <%08lx> <%08lx> <%08lx> <%08lx>\n",
			   i, *(u_long *) & (VF[i][3]), *(u_long *) & (VF[i][2]),
			   *(u_long *) & (VF[i][1]), *(u_long *) & (VF[i][0]));
		}
	} else {
		if (buf[1] == 'i') {
			if (buf[2] == 0){
				for(i = 0; i < 16; i++){
					printf("VI%02d <%x>\n", i, VI[i]);
				}
			}else{
				sscanf(buf, "vi%d", &i);
				printf("VI%02d <%x>\n", i, VI[i]);
			}
		}
	}
}

void setVFreg(VectorUnitState *state, char *buf)
{
/* 
[name]		setVFreg
[desc.]		set value to VF regster (for debug mode)
[args]		buf: input data from debug mode command line 
[return]	void
*/

	int no;
	float val0, val1, val2, val3;

	sscanf(buf, "F%d %f %f %f %f\n", &no, &val0, &val1, &val2, &val3);
	if ((no <= 0) || (no > 31)) {
		printf("cannot set value VF%02d\n", no);
	} else {
		VF[no][0] = val3;
		VF[no][1] = val2;
		VF[no][2] = val1;
		VF[no][3] = val0;
		printf("VF%02d <%f> <%f> <%f> <%f>\n",
		       no, VF[no][3], VF[no][2], VF[no][1], VF[no][0]);
	}
}

void setVIreg(VectorUnitState *state, char *buf)
{
/* 
[name]		setVIreg
[desc.]		set value to VI regster (for debug mode)
[args]		buf: input data from debug mode command line 
[return]	void
*/

	int no, val;

	sscanf(buf, "I%d %d\n", &no, &val);
	if ((no <= 0) || (no > 24)) {
		printf("cannot set value VI%02d\n", no);
	} else {
		VI[no] = val;
		printf("VI%02d <%d>\n", no, VI[no]);
	}
}

void interactive(VectorUnitState *state)
{
/*
[name]		interactive
[desc.]		debug mode monitor
[args]		void
[return]	void
*/

	int stat = 1;
	char buf[128];
	char dmy[32];
	int addr;

	printf("%04d: ", opc);
	opcode_analyze(instbuf);
	putchar('\n');
	while (stat) {
		printf(">", opc);
		gets(buf);
		switch (buf[0]) {
			case 'b':	/* breakpoint */
				sscanf(buf, "%s %d", dmy, &bp);
				break;
			case 'p':	/* pipe */
				printpipe();
				break;
			case 'r':	/* run */
				sscanf(buf, "%s %d", dmy, &pc);
				intr_mode = 1;
				stat = 0;
				break;
			case 'e':	/* end count */
				sscanf(buf, "%s %d", dmy, &ecount);
				intr_mode = 4;
				stat = 0;
				break;
			case 'd':	/* dump state->MEM by hex */
				sscanf(buf, "%s %d", dmy, &addr);
				printf("%04d %08lx %08lx %08lx %08lx\n", addr,
					state->MEM[addr][3], state->MEM[addr][2], state->MEM[addr][1], state->MEM[addr][0]);
				break;
			case 'D':	/* dump state->MEM by float */
				sscanf(buf, "%s %d", dmy, &addr);
				printf("%04d %f %f %f %f\n", addr,
					*(float *) &state->MEM[addr][3], *(float *) &state->MEM[addr][2],
					*(float *) &state->MEM[addr][1], *(float *) &state->MEM[addr][0]);
				break;
			case 'l':	/* list */
				printlist(state, buf);
				break;
			case 's':	/* step */
				intr_mode = 2;
				stat = 0;
				break;
			case 'c':	/* continue */
				intr_mode = 3;
				stat = 0;
				break;
			case 'i':	/* print I */
				printf("I : %f\n", I);
				break;
			case 'V':	/* print reg */
				printreg(state, buf);
				break;
			case 'v':	/* print reg */
				printregI(state, buf);
				break;
			case 'F':	/* set val to VF reg */
				setVFreg(state, buf);
				break;
			case 'I':	/* set val to VI reg */
				setVIreg(state, buf);
				break;
			case 'Q':	/* print Q */
				printf("Q : %f\n", Q);
				break;
			case 'P':	/* print P */
				printf("P : %f\n", VN[4]);
				break;
			case 'R':	/* print R */
				printf("R : 0x%08lx\n", R);
				break;
			case 'A':	/* print ACC */
				printf("ACC:<%f> <%f> <%f> <%f>\n",
					ACC[0], ACC[1], ACC[2], ACC[3]);
				break;
			case 'M':	/* print MAC flag */
				printf("MAC:<%04lx>\n", MACflag);
				break;
			case 'S':	/* print status flag */
				printf("status:<%03lx>\n", statusflag);
				break;
			case 'C':	/* print clip flag */
				printf("clip:<%03lx>\n", clipflag);
				break;
			case 'h':	/* help */
				printf("COMMAND LIST\n");
				printf("r[un]\n");
				printf("b[reakpoint] <addr>\n");
				printf("c[ontinue]\n");
				printf("s[tep]\n");
				printf("l[ist] <addr>\n");
				printf("p[ipe]\n");
				printf("VF<num>(float)\n");
				printf("vf<num>(hex)\n");
				printf("VI<num>\n");
				printf("F<num> <val> <val> <val> <val>\n");
				printf("I<num> <val>\n");
				printf("Q\n");
				printf("P\n");
				printf("R\n");
				printf("M[ac flag]\n");
				printf("S[tatus flag]\n");
				printf("C[lip flag]\n");
				printf("d[ump state->MEM(hex)] <addr>\n");
				printf("D[ump state->MEM(float)] <addr>\n");
				printf("e[nd count] <num>\n");
				printf("h[elp]\n");
				break;
			default:
				break;
		}
	}
}


void simvpe(VectorUnitState *state)
{
/*
[name]		simvpe
[desc.]		VU simulation main routine.
			Need to set below variables and call initvpe,
			befor call this routine.
				_ITOP, _TOP, _vpepc, _is_dbg, _is_verb, _isdump, _pgpuif
[args]		void
[return]	void
*/

	int stat = 1;

	eflag = 0;
	jflag = 0;
	peflag = 0;
	sflag = 0;
	verb_mode = _is_verb;
	dpr_mode = _is_dump;
	pc = _vpepc;		
	if ((intr_mode > 0) && (intr_mode < 3)) {
		printf("simvpe interactive mode\n");
		bp = _vpepc + 1;
	}
	if ((intr_mode == 4) && (ecount == 0)){
		bp = _vpepc + 1;
	}
	fetch_inst(state);
	while (stat) {
		stat = 0;
		all_count++;
		if (((intr_mode == 1) && (pc == bp)) ||
		    ((intr_mode == 3) && (pc == bp)) ||
		    ((intr_mode == 4) && (pc == bp)) ||
		    (intr_mode == 2)) {
			interactive(state);
		}
		if (exec_inst(state))
			stat += 1;
		if (fetch_inst(state))
			stat += 2;
		if (move_pipe(state))
			stat += 8;
	}
	_vpepc = pc + 1;
	ecount--;

	if (verb_mode) {
		printf("\n>> all cycles = %d cycle\n", all_count);
		printf(">> all hazard stall cycles = %d cycle\n", hc_count);
	}
}

void LoadVUMem(VectorUnitState *state, int dst_addr, u_long * src_addr, int qsize)
{
/*
[name]		LoadVUMem
[desc.]		Set values to VU memory
[args]		dst_addr: destination address of VU memory to set values.
				(The byte order of destination values is big endian.)
			src_addr: source address of values to set. 
				(The byte order of source values is little endian)
			qsize: count of values to set ( quad ward )
[return]	void
*/

	int i;
	u_long eldata[4];

	for (i = 0; i < qsize; i++) {
		/* convert little endian to bigendian. */
		eldata[3] = src_addr[0] << 24 | ((src_addr[0] << 8) & 0xff0000) |
			((src_addr[0] >> 8) & 0xff00) | ((src_addr[0] >> 24) & 0xff);
		eldata[2] = src_addr[1] << 24 | ((src_addr[1] << 8) & 0xff0000) |
			((src_addr[1] >> 8) & 0xff00) | ((src_addr[1] >> 24) & 0xff);
		eldata[1] = src_addr[2] << 24 | ((src_addr[2] << 8) & 0xff0000) |
			((src_addr[2] >> 8) & 0xff00) | ((src_addr[2] >> 24) & 0xff);
		eldata[0] = src_addr[3] << 24 | ((src_addr[3] << 8) & 0xff0000) |
			((src_addr[3] >> 8) & 0xff00) | ((src_addr[3] >> 24) & 0xff);
		state->MEM[dst_addr + i][0] = eldata[0];
		state->MEM[dst_addr + i][1] = eldata[1];
		state->MEM[dst_addr + i][2] = eldata[2];
		state->MEM[dst_addr + i][3] = eldata[3];
		src_addr += 4;
	}
}

void LoadMMem(VectorUnitState *state, int dst_addr, u_long * src_addr, int dwsize)
{
/*
[name]		LoadMMem
[desc.]		Set instructions to Micro memory
[args]		dst_addr: destination address of Micro memory 
				to set instructions
				(The byte order of destination values is big endian.)
			src_addr: source address of instructions to set
				(The byte order of source values is little endian.)
			dwsize: count of values to set ( double ward )
[return]	void
*/

	int i;
	u_long elop[2];

	for (i = 0; i < dwsize; i++) {
		/* convert little endian to big endian. */
		elop[1] = src_addr[0] << 24 | ((src_addr[0] << 8) & 0xff0000) |
			((src_addr[0] >> 8) & 0xff00) | ((src_addr[0] >> 24) & 0xff);
		elop[0] = src_addr[1] << 24 | ((src_addr[1] << 8) & 0xff0000) |
			((src_addr[1] >> 8) & 0xff00) | ((src_addr[1] >> 24) & 0xff);
		state->uMEM[dst_addr + i][0] = elop[0];
		state->uMEM[dst_addr + i][1] = elop[1];
		src_addr += 2;
	}
}

void StoreVUMem(VectorUnitState *state, int src_addr, u_long * dst_addr, int qsize)
{
/*
[name]		StoreVUMem
[desc.]		Get values from VU memory
[args]		src_addr: source address of VU memory to get values
					(The byte order of source values is big endian.)
			dst_addr: destination address of values to get
					(The byte order of destination values is little endian.)
			qsize: count of values to get (quad ward)
[return]	void
*/

	int i;
	u_long data[4];

	for (i = 0; i < qsize; i++) {
		data[3] = state->MEM[src_addr + i][0];
		data[2] = state->MEM[src_addr + i][1];
		data[1] = state->MEM[src_addr + i][2];
		data[0] = state->MEM[src_addr + i][3];
		/* convert big endian to little endian. */
		dst_addr[3] = data[3] << 24 | ((data[3] << 8) & 0xff0000) |
			((data[3] >> 8) & 0xff00) | ((data[3] >> 24) & 0xff);
		dst_addr[2] = data[2] << 24 | ((data[2] << 8) & 0xff0000) |
			((data[2] >> 8) & 0xff00) | ((data[2] >> 24) & 0xff);
		dst_addr[1] = data[1] << 24 | ((data[1] << 8) & 0xff0000) |
			((data[1] >> 8) & 0xff00) | ((data[1] >> 24) & 0xff);
		dst_addr[0] = data[0] << 24 | ((data[0] << 8) & 0xff0000) |
			((data[0] >> 8) & 0xff00) | ((data[0] >> 24) & 0xff);
		
		dst_addr += 4;
	}
}

void StoreMMem(VectorUnitState *state, int src_addr, u_long * dst_addr, int dwsize)
{
/*
[name]		StoreMMem
[desc.]		Get instructions from Micro memory
[args]		src_addr: source address of Micro memory to get instructions
					(The byte order of source values is big endian.)
			dst_addr: destination address of instructions to get
					(The byte order of destination values is little endian.)
			qsize: count of instructions to get (quad ward)
[return]	void
*/

	int i;
	u_long data[2];

	for (i = 0; i < dwsize; i++) {
		data[1] = state->uMEM[src_addr + i][0];
		data[0] = state->uMEM[src_addr + i][1];
		/* convert big endian to little endian. */
		dst_addr[1] = data[1] << 24 | ((data[1] << 8) & 0xff0000) |
			((data[1] >> 8) & 0xff00) | ((data[1] >> 24) & 0xff);
		dst_addr[0] = data[0] << 24 | ((data[0] << 8) & 0xff0000) |
			((data[0] >> 8) & 0xff00) | ((data[0] >> 24) & 0xff);
		dst_addr += 2;
	}
}

void vpecallms_init(VectorUnitState *state)
{
/*
[name]		vpecallms_init
[desc.]		Initialize internal status of VU simulator 
			when start to execute a Micro program.
			This routine doesn't use with simvpe().
			This routine use with vpecallms_cycle() and initvpe().
[args]		void
[return]	void
*/

	eflag = 0;
	jflag = 0;
	peflag = 0;
	sflag = 0;
	intr_mode = _is_dbg;
	verb_mode = _is_verb;
	dpr_mode = _is_dump;
	pc = _vpepc;
}

void vpecallms_cycle(VectorUnitState* state)
{
/*
[name]		vpecallms_cycle
[desc.]		VU simulator execute 1cycle.
[args]		void
[return]	!0: VU is busy
			0: VU is idle
*/
	int stat = 0;

	if (fetch_inst(state)) stat = 1;
	if (exec_inst(state)) stat = 2;
	if (move_pipe(state)) stat = 4;
	_vpepc = pc + 1;

	if (stat) {
	    state->regs.VPE_STAT.vbs = 1;
	} else {
	    state->regs.VPE_STAT.vbs = 0;
	}
}

#if 0
void simvpe2(VectorUnitState *state)
{
/*
[name]		simvpe2
[desc.]		VU simulate same as simvpe(). ( sample routine )
[args]		void
[return]	void
*/

	vpecallms_init(state);
	while (vpecallms_cycle(state));
}
#endif

/*--------------------------------------------------------------------------*/

/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*      Experimental disassembler                                           */
/*                                                                          */
/****************************************************************************/

#include <sys/types.h>

void p_bc(u_long opcode)
{
	switch(opcode&0x3){
		case 0:
			putchar('x'); break;
		case 1:
			putchar('y'); break;
		case 2:
			putchar('z'); break;
		case 3:
			putchar('w'); break;
	}
}

void p_dest(u_long opcode)
{
	switch((opcode >> 21) & 0xf){
		case 1:
			printf("w"); break;
		case 2:
			printf("z"); break;
		case 3:
			printf("wz"); break;
		case 4:
			printf("y"); break;
		case 5:
			printf("yw"); break;
		case 6:
			printf("yz"); break;
		case 7:
			printf("yzw"); break;
		case 8:
			printf("x"); break;
		case 9:
			printf("xw"); break;
		case 10:
			printf("xz"); break;
		case 11:
			printf("xzw"); break;
		case 12:
			printf("xy"); break;
		case 13:
			printf("xyw"); break;
		case 14:
			printf("xyz"); break;
		case 15:
			printf("xyzw"); break;
	}
}

void bc_reg(u_long opcode)
{
	putchar('/'); p_dest(opcode);

	printf(" VF%02d", (opcode >> 6) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 16) & 0x1f); p_bc(opcode);
}

void par_reg3(u_long opcode)
{
	printf(" VF%02d", (opcode >> 6) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
}

void par_reg_ds(u_long opcode)
{
	printf(" VF%02d", (opcode >> 6) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
}

void mul_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("MULq/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", Q");
			break;
		case 1:
			printf("MAXi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
		case 2:
			printf("MULi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
		case 3:
			printf("MINIi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
	}
}

void add_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ADDq/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", Q");
			break;
		case 1:
			printf("MADDq/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", Q");
			break;
		case 2:
			printf("ADDi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
		case 3:
			printf("MADDi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
	}
}

void sub_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("SUBq/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", Q");
			break;
		case 1:
			printf("MSUBq/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", Q");
			break;
		case 2:
			printf("SUBi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
		case 3:
			printf("MSUBi/"); p_dest(opcode);
			par_reg_ds(opcode);
			printf(", I");
			break;
	}
}

void add(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ADD/"); p_dest(opcode);
			par_reg3(opcode);
			break;
		case 1:
			printf("MADD/"); p_dest(opcode);
			par_reg3(opcode);
			break;
		case 2:
			printf("MUL/"); p_dest(opcode);
			par_reg3(opcode);
			break;
		case 3:
			printf("MAX/"); p_dest(opcode);
			par_reg3(opcode);
			break;
	}
}

void sub(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("SUB/"); p_dest(opcode);
			par_reg3(opcode);
			break;
		case 1:
			printf("MSUB/"); p_dest(opcode);
			par_reg3(opcode);
			break;
		case 2:
			printf("OPMSUB/xyz, VF%02dxyz", (opcode >> 6) & 0x1f);
			printf(", VF%02dxyz, VF%02dxyz", (opcode >> 11) & 0x1f,
				(opcode >> 16) & 0x1f);
			break;
		case 3:
			printf("MINI/"); p_dest(opcode);
			par_reg3(opcode);
			break;
	}
}

void bc_acc(u_long opcode)
{
	putchar('/'); p_dest(opcode);

	printf(" ACC"); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 16) & 0x1f); p_bc(opcode);
}

void itof(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("0/"); break;
		case 1:
			printf("4/"); break;
		case 2:
			printf("12/"); break;
		case 3:
			printf("15/"); break;
	}
	p_dest(opcode);
	printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
}

void par_reg_s(u_long opcode)
{
	printf(" ACC"); p_dest(opcode);
	printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
}

void mula_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("MULAq/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", Q");
			break;
		case 1:
			printf("ABS/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 2:
			printf("MULAi/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", I");
			break;
		case 3:
			printf("CLIPw/xyz"); 
			printf(" VF%02dxyz", (opcode >> 11) & 0x1f);
			printf(", VF%02dw", (opcode >> 16) & 0x1f);
			break;
	}
}

void adda_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ADDAq/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", Q");
			break;
		case 1:
			printf("MADDAq/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", Q");
			break;
		case 2:
			printf("ADDAi/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", I");
			break;
		case 3:
			printf("MADDAi/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", I");
			break;
	}
}

void suba_q(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("SUBAq/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", Q");
			break;
		case 1:
			printf("MSUBAq/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", Q");
			break;
		case 2:
			printf("SUBAi/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", I");
			break;
		case 3:
			printf("MSUBAi/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", I");
			break;
	}
}
 
void adda(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ADDA/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 1:
			printf("MADDA/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 2:
			printf("MULA/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 3:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void suba(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("SUBA/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 1:
			printf("MSUBA/"); p_dest(opcode);
			par_reg_s(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 2:
			printf("OPMULA/xyz ACCxyz, VF%02d", (opcode >> 11) & 0x1f);
			p_dest(opcode);
			printf(", VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			break;
		case 3:
			printf("NOP");
			break;
	}
}

void special(u_long opcode)
{
	switch((opcode >> 6) & 0xf) { /* bit41-38 */
		case 0:
			printf("ADDA"); p_bc(opcode);
			bc_acc(opcode); break;
		case 1:
			printf("SUBA"); p_bc(opcode);
			bc_acc(opcode); break;
		case 2:
			printf("MADDA"); p_bc(opcode);
			bc_acc(opcode); break;
		case 3:
			printf("MSUBA"); p_bc(opcode);
			bc_acc(opcode); break;
		case 4:
			printf("ITOF");
			itof(opcode); break;
		case 5:
			printf("FTOI");
			itof(opcode); break;
		case 6:
			printf("MULA"); p_bc(opcode);
			bc_acc(opcode); break;
		case 7:
			mula_q(opcode); break;
		case 8:
			adda_q(opcode); break;
		case 9:
			suba_q(opcode); break;
		case 10:
			adda(opcode); break;
		case 11:
			suba(opcode); break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void quad_load(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("LQ/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", %d(VI%02d)", opcode & 0x7ff, (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("SQ/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			printf(", %d(VI%02d)", opcode & 0x7ff, (opcode >> 16) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void int_load(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("ILW/"); p_dest(opcode);
			printf(" VI%02d", (opcode >> 16) & 0x1f); 
			printf(", %d(VI%02d)", opcode & 0x7ff, (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("ISW/"); p_dest(opcode);
			printf(" VI%02d", (opcode >> 16) & 0x1f);
			printf(", %d(VI%02d)", opcode & 0x7ff, (opcode >> 11) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void int_br(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("IBEQ");
			printf(" VI%02d, VI%02d, 0x%x",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
		case 1:
			printf("IBNE");
			printf(" VI%02d, VI%02d, 0x%x",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void cond_int_br(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("IBLTZ");
			printf(" VI%02d, 0x%x", (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
		case 1:
			printf("IBGTZ");
			printf(" VI%02d, 0x%x", (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
		case 2:
			printf("IBLEZ");
			printf(" VI%02d, 0x%x", (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
		case 3:
			printf("IBGEZ");
			printf(" VI%02d, 0x%x", (opcode >> 11) & 0x1f, opcode & 0x7ff);
			break;
	}
}

void br(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("B 0x%x", opcode & 0x7ff);
			break;
		case 1:
			printf("BAL VI%02d, 0x%x", (opcode >> 16) & 0x1f, opcode & 0x7ff);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void jr(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("JR VI%02d", (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("JALR VI%02d, VI%02d", 
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void uint_add(u_long opcode)
{
	u_int imm;

	switch((opcode >> 25) & 0x3){
		case 0:
			printf("IADDIU");
			imm = ((opcode >> 10) & 0x7800) | (opcode & 0x7ff);
			printf(" VI%02d, VI%02d, %d",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, imm);
			break;
		case 1:
			printf("ISUBIU");
			imm = ((opcode >> 10) & 0x7800) | (opcode & 0x7ff);
			printf(" VI%02d, VI%02d, %d",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, imm);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void fceq(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("FCEQ 0x%x", opcode & 0x00ffffff);
			break;
		case 1:
			printf("FCSET 0x%x", opcode & 0x00ffffff);
			break;
		case 2:
			printf("FCAND 0x%x", opcode & 0x00ffffff);
			break;
		case 3:
			printf("FCOR 0x%x", opcode & 0x00ffffff);
			break;
	}
}

void fseq(u_long opcode)
{
	u_long imm12;

	imm12 = ((opcode & 0x00200000) >> 10) |
		(opcode & 0x7ff);
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("FSEQ VI%02d, 0x%x", (opcode >> 16) & 0x1f, imm12);
			break;
		case 1:
			printf("FSSET 0x%x", imm12);
			break;
		case 2:
			printf("FSAND VI%02d, 0x%x", (opcode >> 16) & 0x1f, imm12);
			break;
		case 3:
			printf("FSOR VI%02d, 0x%x", (opcode >> 16) & 0x1f, imm12);
			break;
	}
}

void fcget(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("FCGET VI%02d", (opcode >> 16) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void fmand(u_long opcode)
{
	switch((opcode >> 25) & 0x3){
		case 0:
			printf("FMEQ VI%02d, VI%02d",
				(opcode >> 11) & 0x1f, (opcode >> 16) & 0x1f);
			break;
		case 2:
			printf("FMAND VI%02d, VI%02d",
				(opcode >> 11) & 0x1f, (opcode >> 16) & 0x1f);
			break;
		case 3:
			printf("FMOR VI%02d, VI%02d",
				(opcode >> 11) & 0x1f, (opcode >> 16) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void iand(u_long opcode)
{
	int imm5;
	switch(opcode & 0x3){
		case 0:
			printf("IAND VI%02d, VI%02d, VI%02d",
				(opcode >> 6) & 0x1f, (opcode >> 11) & 0x1f,
				(opcode >> 16) & 0x1f);
			break;
		case 1:
			printf("IOR VI%02d, VI%02d, VI%02d",
				(opcode >> 6) & 0x1f, (opcode >> 11) & 0x1f,
				(opcode >> 16) & 0x1f);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}
void iadd(u_long opcode)
{
	int imm5;
	switch(opcode & 0x3){
		case 0:
			printf("IADD VI%02d, VI%02d, VI%02d",
				(opcode >> 6) & 0x1f, (opcode >> 11) & 0x1f,
				(opcode >> 16) & 0x1f);
			break;
		case 1:
			printf("ISUB VI%02d, VI%02d, VI%02d",
				(opcode >> 6) & 0x1f, (opcode >> 11) & 0x1f,
				(opcode >> 16) & 0x1f);
			break;
		case 2:
			if(opcode & 0x0400){
				imm5 = ((opcode >> 6) & 0x1f) | 0xffffffe0;
			}else
				imm5 = ((opcode >> 6) & 0x1f);
			printf("IADDI VI%02d, VI%02d, %d",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f, imm5);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void move(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			if(((opcode >> 11 & 0x1f) == 0) && ((opcode >> 16) & 0x1f) == 0){
				printf("NOP");
			}else{
				printf("MOVE/"); p_dest(opcode);
				printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
				printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			}
			break;
		case 1:
			printf("MR32/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
}

void pinc_load(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("LQI/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", VI%02d", (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("SQI/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			printf(", VI%02d", (opcode >> 16) & 0x1f);
			break;
		case 2:
			printf("LQD/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", VI%02d", (opcode >> 11) & 0x1f);
			break;
		case 3:
			printf("SQD/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 11) & 0x1f); p_dest(opcode);
			printf(", VI%02d", (opcode >> 16) & 0x1f);
			break;
	}
}

void p_ftf(u_long ftf)
{
	switch(ftf){
		case 0:
			putchar('x');
			break;
		case 1:
			putchar('y');
			break;
		case 2:
			putchar('z');
			break;
		case 3:
			putchar('w');
			break;
	}
}

void mftir(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("MTIR/"); p_dest(opcode);
			printf(" VI%02d, VF%02d", 
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			p_dest(opcode);
			break;
		case 1:
			printf("MFIR/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", VI%02d", (opcode >> 11) & 0x1f);
			break;
		case 2:
			printf("ILWR/"); p_dest(opcode);
			printf(" VI%02d, VI%02d",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
		case 3:
			printf("ISWR/"); p_dest(opcode);
			printf(" VI%02d, VI%02d",
				(opcode >> 16) & 0x1f, (opcode >> 11) & 0x1f);
			break;
	}
}

void div(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("DIV Q, VF%02d", (opcode >> 11) & 0x1f);
			p_ftf((opcode >> 21) & 0x3);
			printf(", VF%02d", (opcode >> 16) & 0x1f);
			p_ftf((opcode >> 23) & 0x3);
			break;
		case 1:
			printf("SQRT Q, VF%02d", (opcode >> 16) & 0x1f);
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 2:
			printf("RSQRT Q, VF%02d", (opcode >> 11) & 0x1f);
			p_ftf((opcode >> 21) & 0x3);
			printf(", VF%02d", (opcode >> 16) & 0x1f);
			p_ftf((opcode >> 23) & 0x3);
			break;
		case 3:
			printf("WAITQ");
			break;
	}
}

void esadd_ersadd(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ESADD");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("ERSADD");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
		case 2:
			printf("ELENG");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
		case 3:
			printf("ERLENG");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
	}
}

void esrsqrt(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ESQRT");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 1:
			printf("ERSQRT");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 2:
			printf("ERCPR");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 3:
			printf("WAITP");break;
	}
}

void eatanxyz(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("EATANxy");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
		case 1:
			printf("EATANxz");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
		case 2:
			printf("ESUM");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f);
			break;
	}
}

void esinatan(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("ESIN");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 1:
			printf("EATAN");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 2:
			printf("EEXP");
			printf(" P, VF%02d", (opcode >> 11) & 0x1f); 
			p_ftf((opcode >> 21) & 0x3);
			break;
	}
}

void mfp(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("MFP/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", P");
			break;
	}
}

void xgkick(u_long opcode)
{
	printf("XGKICK VI%02d", (opcode >> 11) &0x1f);
}

void xitop(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("XTOP VI%02d", (opcode >> 16) & 0x1f);
			break;
		case 1:
			printf("XITOP VI%02d", (opcode >> 16) & 0x1f);
			break;
	}
}

void rand(u_long opcode)
{
	switch(opcode & 0x3){
		case 0:
			printf("RNEXT/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", R");
			break;
		case 1:
			printf("RGET/"); p_dest(opcode);
			printf(" VF%02d", (opcode >> 16) & 0x1f); p_dest(opcode);
			printf(", R");
			break;
		case 2:
			printf("RINIT R, VF%02d", (opcode >> 11) & 0x1f);
			p_ftf((opcode >> 21) & 0x3);
			break;
		case 3:
			printf("RXOR R, VF%02d", (opcode >> 11) & 0x1f);
			p_ftf((opcode >> 21) & 0x3);
			break;
	}
}

void Special_Low(u_long opcode)
{
	switch((opcode >> 6) & 0x1f){
		case 12:
			move(opcode); break;
		case 13:
			pinc_load(opcode); break;
		case 14:
			div(opcode); break;
		case 15:
			mftir(opcode); break;
		case 16:
			rand(opcode); break;
		case 25:
			mfp(opcode); break;
		case 26:
			xitop(opcode); break;
		case 27:
			xgkick(opcode); break;
		case 28:
			esadd_ersadd(opcode); break;
		case 29:
			eatanxyz(opcode); break;
		case 30:
			esrsqrt(opcode); break;
		case 31:
			esinatan(opcode); break;
		default:
			printf("Undefined opcode(%08lx)", opcode);
			break;
	}
			
}

void maj_spcial(u_long opcode)
{
	if(((opcode >> 2) & 0xf) == 12){
		iadd(opcode);
	}else
	if(((opcode >> 2) & 0xf) == 13){
		iand(opcode);
	}else
	if(((opcode >> 2) & 0xf) == 15){
		Special_Low(opcode);
	}else{
		printf("Undefined opcode(%08lx)", opcode);
	}
}

void opcode_analyze(u_long *opcode)
{
	/* UpperOp */
	switch((opcode[0] >> 2) & 0xf){ /* bit37-34 */
		case 0:	
			printf("ADD"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 1: 
			printf("SUB"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 2:
			printf("MADD"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 3:
			printf("MSUB"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 4:
			printf("MAX"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 5:
			printf("MINI"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 6:
			printf("MUL"); p_bc(opcode[0]);
			bc_reg(opcode[0]); break;
		case 7:
			mul_q(opcode[0]); break;
		case 8:
			add_q(opcode[0]); break;
		case 9:
			sub_q(opcode[0]); break;
		case 10:
			add(opcode[0]); break;
		case 11:
			sub(opcode[0]); break;
		case 15:
			special(opcode[0]); break;
		default:
			printf("Undefined opcode(%08lx)", opcode[0]);
			break;
	}

	printf(" | ");

	/* LowerOp */
	if(opcode[0]&0x80000000){
		printf("LOI %f", *(float *)&opcode[1]);
	}else
	if(opcode[0]&0x40000000){
		printf("END");
	}else
	if(opcode[0]&0x04000000){
		printf("PRT %08lx", opcode[1]);
	}else{
		switch((opcode[1] >> 27) & 0x1f){
			case 0:
				quad_load(opcode[1]); break;
			case 1:
				int_load(opcode[1]); break;
			case 2:
				uint_add(opcode[1]); break;
			case 4:
				fceq(opcode[1]); break;
			case 5:
				fseq(opcode[1]); break;
			case 6:
				fmand(opcode[1]); break;
			case 7:
				fcget(opcode[1]); break;
			case 8:
				br(opcode[1]); break;
			case 9:
				jr(opcode[1]); break;
			case 10:
				int_br(opcode[1]); break;
			case 11:
				cond_int_br(opcode[1]); break;
			case 16:
				maj_spcial(opcode[1]); break;
			default:
				printf("Undefined opcode(%08lx)", opcode[1]);
				break;
		}
	}

}

#if 0
void main(int argc, char **argv)
{
	FILE	*fpr;
	char	rfilename[64];
	u_long	opcode[2];
	u_long	elop[2];
	int		count = 0;

	if(argc < 2){
		fprintf(stderr, "Usage: dpppasm filename\n");
		exit(1);
	}
	strcpy(rfilename, argv[1]);
	if((fpr = fopen(rfilename, "r")) == NULL){
		fprintf(stderr, "can not open %s\n", rfilename);
		exit(1);
	}

	while(fread(&elop[0], 4, 2, fpr) != 0){
		opcode[0] = (elop[1] << 24 ) | ((elop[1] << 8) & 0xff0000) |
			((elop[1] >> 8) & 0xff00) | ((elop[1] >> 24) & 0xff);
		opcode[1] = (elop[0] << 24 ) | ((elop[0] << 8) & 0xff0000) |
			((elop[0] >> 8) & 0xff00) | ((elop[0] >> 24) & 0xff);
		printf("%04d: ", count++);
		opcode_analyze(opcode);
		putchar('\n');
	}

	fclose(fpr);
}
#endif

/****************************************************************************/
/*                                                                          */
/*             Sony Computer Entertainment CONFIDENTIAL                     */
/*      (C) 1997 Sony Computer Entertainment Inc. All Rights Reserved       */
/*                                                                          */
/*      Psuedo GPUIF simulator (handle only PATH1)                          */
/*                                                                          */
/****************************************************************************/

#define PRIM	0x00
#define RGBAQ	0x01
#define ST		0x02
#define UV		0x03
#define XYZF2	0x04
#define XYZ2	0x05
#define TEX0_1	0x06
#define TEX0_2	0x07
#define CLAMP_1	0x08
#define CLAMP_2	0x09
#define XYZF	0x0a
#define XYZF3	0x0c
#define XYZ3	0x0d
#define A_D		0x0e
#define GPUNOP	0x0f

#define NOOUT	0
#define PRTGPUIF	1
#define PRTGPU	2
#define PRTGPI	3

extern int	_GIF_SIM_OFF;
extern int _GIF_BUSY;
extern int _GIF_VUCALL;
extern int _GIF_VUADDR;

static	u_long	stQ;
extern int _pgpuif;
extern FILE *_fp_gpu;

void GPU2_Put_gpu(int addr, u_long reg1, u_long reg0)
{
	float	xx, yy;

	fprintf(_fp_gpu,"%02x %08lx %08lx\n", addr, reg1, reg0);
}

void GPU2_Put_gpi(int addr, u_long reg1, u_long reg0)
{
	float	xx, yy;
	int ctxt, fst, aa1, abe, fge, tme, iip, prim;

	switch(addr){
		case PRIM:
			fprintf(_fp_gpu, "PRIM PRIM=%d ", (reg0 & 0x7));
			if(reg0 & 0x200)
				fprintf(_fp_gpu, "CTXT=1 ");
			if(reg0 & 0x100)
				fprintf(_fp_gpu, "FST=1 ");
			if(reg0 & 0x80)
				fprintf(_fp_gpu, "AA1=1 ");
			if(reg0 & 0x40)
				fprintf(_fp_gpu, "ABE=1 ");
			if(reg0 & 0x20)
				fprintf(_fp_gpu, "FGE=1 ");
			if(reg0 & 0x10)
				fprintf(_fp_gpu, "TME=1 ");
			if(reg0 & 0x8)
				fprintf(_fp_gpu, "IIP=1 ");
			fprintf(_fp_gpu, "\n");
			break;
		case RGBAQ:
			xx = *(float *)&reg1;
			fprintf(_fp_gpu, "RGBAQ %d %d %d %d %e\n", reg0&0xff, (reg0>>8)&0xff,
				(reg0>>16)&0xff, (reg0>>24)&0xff, xx);
			break;
		case ST:
			xx = *(float *)&reg0;
			yy = *(float *)&reg1;
			fprintf(_fp_gpu, "ST %e %e\n", xx, yy);
			break;
		case UV:
			xx = (float)(reg0 & 0xffff)/16.0;
			yy = (float)((reg0 >> 16) & 0xffff)/16.0;
			fprintf(_fp_gpu, "UV %f %f\n", xx, yy);
			break;
		case XYZF2:
			xx = (float)(reg0&0xffff)/16.0;
			yy = (float)((reg0 >> 16)&0xffff)/16.0;
			fprintf(_fp_gpu, "XYZF2 %f %f %d %d\n", xx, yy,
				reg1&0xffffff, (reg1>>24)&0xff);
			break;
		case XYZ2:
			xx = (float)(reg0&0xffff)/16.0;
			yy = (float)((reg0 >> 16)&0xffff)/16.0;
			fprintf(_fp_gpu, "XYZ2 %f %f %d \n", xx, yy, reg1);
			break;
		case XYZF:
			xx = (float)(reg0&0xffff)/16.0;
			yy = (float)((reg0 >> 16)&0xffff)/16.0;
			fprintf(_fp_gpu, "XYZF %f %f %d %d\n", xx, yy,
				reg1&0xffffff, (reg1>>24)&0xff);
			break;
		case XYZF3:
			xx = (float)(reg0&0xffff)/16.0;
			yy = (float)((reg0 >> 16)&0xffff)/16.0;
			fprintf(_fp_gpu, "XYZF3 %f %f %d %d\n", xx, yy,
				reg1&0xffffff, (reg1>>24)&0xff);
			break;
		case XYZ3:
			xx = (float)(reg0&0xffff)/16.0;
			yy = (float)((reg0 >> 16)&0xffff)/16.0;
			fprintf(_fp_gpu, "XYZ3 %f %f %d\n", xx, yy, reg1);
			break;
		case A_D:
			fprintf(_fp_gpu, "!%02x %08lx %08lx\n", addr, reg1, reg0);
			break;
		case GPUNOP:
			break;
		default:
			fprintf(_fp_gpu, "!%02x %08lx %08lx\n", addr, reg1, reg0);
			break;
	}
}

void	to_gpu(VectorUnitState *state, int regno, int addr)
{
	u_long reg0, reg1;

	switch(regno){
		case RGBAQ:
			reg0 = (state->MEM[addr][0] & 0xff) | ((state->MEM[addr][1] & 0xff) << 8) |
				((state->MEM[addr][2] & 0xff) << 16) | ((state->MEM[addr][3] & 0xff) << 24);
			switch(_pgpuif){
				case NOOUT:
					break;
				case PRTGPUIF:
					break;
				case PRTGPU:
					GPU2_Put_gpu(RGBAQ, stQ, reg0);
					break;
				case PRTGPI:
					GPU2_Put_gpi(RGBAQ, stQ, reg0);
					break;
			}
			break;
		case UV:
			reg0 = (state->MEM[addr][0] & 0xffff) | ((state->MEM[addr][1] & 0xffff) << 16);
			switch(_pgpuif){
				case NOOUT:
					break;
				case PRTGPUIF:
					break;
				case PRTGPU:
					GPU2_Put_gpu(UV, 0, reg0);
					break;
				case PRTGPI:
					GPU2_Put_gpi(UV, 0, reg0);
					break;
			}
			break;
		case XYZF2:
		case XYZF3:
			reg0 = (state->MEM[addr][0] & 0xffff) | ((state->MEM[addr][1] & 0xffff) << 16); 
			reg1 = ((state->MEM[addr][2]>>4) & 0xffffff) |
				(((state->MEM[addr][3]>>4) & 0xff) << 24); 
			switch(_pgpuif){
				case NOOUT:
					break;
				case PRTGPUIF:
					break;
				case PRTGPU:
					if(state->MEM[addr][3] & 0x8000)
						GPU2_Put_gpu(XYZF3, reg1, reg0);
					else
						GPU2_Put_gpu(XYZF2, reg1, reg0);
					break;
				case PRTGPI:
					if(state->MEM[addr][3] & 0x8000)
						GPU2_Put_gpi(XYZF3, reg1, reg0);
					else
						GPU2_Put_gpi(XYZF2, reg1, reg0);
					break;
			}
			break;
		case ST:
			stQ = state->MEM[addr][2];
		case PRIM:
		case TEX0_1:
		case TEX0_2:
		case CLAMP_1:
		case CLAMP_2:
			switch(_pgpuif){
				case NOOUT:
					break;
				case PRTGPUIF:
					break;
				case PRTGPU:
					GPU2_Put_gpu(regno, state->MEM[addr][1], state->MEM[addr][0]);
					break;
				case PRTGPI:
					GPU2_Put_gpi(regno, state->MEM[addr][1], state->MEM[addr][0]);
					break;
			}
			break;
		case A_D:
			switch(_pgpuif){
				case NOOUT:
					break;
				case PRTGPUIF:
					break;
				case PRTGPU:
					GPU2_Put_gpu(state->MEM[addr][2] & 0xff, state->MEM[addr][1], state->MEM[addr][0]);
					break;
				case PRTGPI:
					GPU2_Put_gpi(state->MEM[addr][2] & 0xff, state->MEM[addr][1], state->MEM[addr][0]);
					break;
			}
			break;
	}
}

void GpuIfKick(VectorUnitState *state, int addr)
{
	int nreg, flg, prim, pre, nloop;
	int regs[16], i, j;
	u_long tag0;

	if(_GIF_SIM_OFF){
		if(_GIF_BUSY){
			sflag = 1;
			hc_count++;
			pipe[0][0].flag = 0;
			apipe.flag = 0;
		}else{
			_GIF_VUCALL = 1;
			_GIF_VUADDR = addr;
			_GIF_BUSY = 1;
			sflag = 0;
		}
	}else{
		tag0 = state->MEM[addr][0];
		nreg  = (state->MEM[addr][1] >> 28) & 0xf;
		flg   = (state->MEM[addr][1] >> 26) & 0x3;
		prim  = (state->MEM[addr][1] >> 15) & 0x7ff;
		pre   = (state->MEM[addr][1] >> 14) & 0x1;
		nloop =  state->MEM[addr][0] & 0x7fff;
	do {
		tag0 = state->MEM[addr][0];
		nreg  = (state->MEM[addr][1] >> 28) & 0xf;
		flg   = (state->MEM[addr][1] >> 26) & 0x3;
		prim  = (state->MEM[addr][1] >> 15) & 0x7ff;
		pre   = (state->MEM[addr][1] >> 14) & 0x1;
		nloop =  state->MEM[addr][0] & 0x7fff;

		if(nreg == 0) nreg = 16;
		if(flg != 0){
			fprintf(stderr, "GIF: not support FLG(%d)<%d>\n", flg, addr);
			return;
		}
		for(i = 0; i < 8; i++)
			regs[i] = (state->MEM[addr][2] >> (i * 4)) & 0xf;

		for(i = 8; i < 16; i++)
			regs[i] = (state->MEM[addr][3] >> ((i - 8) * 4)) & 0xf;

		if(pre){
			switch(_pgpuif){
				case NOOUT:
				case PRTGPUIF:
					break;
				case PRTGPU:
					GPU2_Put_gpu(PRIM, 0, (u_long)prim);
					break;
				case PRTGPI:
					GPU2_Put_gpi(PRIM, 0, (u_long)prim);
					break;
			}
		}

		if(_pgpuif == PRTGPUIF)
			fprintf(_fp_gpu, "%08lx %08lx %08lx %08lx\n",
				state->MEM[addr][3], state->MEM[addr][2], state->MEM[addr][1], state->MEM[addr][0]);

		addr++;
		for(i = 0; i < nloop; i++){
			for(j = 0; j < nreg; j++){
				if(_pgpuif == PRTGPUIF){
					fprintf(_fp_gpu, "%08lx %08lx %08lx %08lx\n",
						state->MEM[addr][3], state->MEM[addr][2], state->MEM[addr][1], state->MEM[addr][0]);
				}else{
					to_gpu(state, regs[j], addr);
				}
				addr++;
			}
		}
	}while((tag0 & 0x8000) == 0);
	}

}
