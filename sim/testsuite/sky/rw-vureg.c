#include <stdio.h>
#include <string.h>

extern void* memcpy( void*, const void*, size_t n );

/* Register offsets within memory-mapped window */
enum 
{
  VU_REG_VF	= 0,		/* Start of VF registers */
  VU_REG_VI	= 0x200,	/* Start of VI registers */
  VU_REG_MST	= 0x300,	/* Start of misc registers */
  VU_REG_MMC	= 0x310,
  VU_REG_MCP	= 0x320,
  VU_REG_MR	= 0x330,
  VU_REG_MI	= 0x340,
  VU_REG_MQ	= 0x350,
  VU_REG_MP	= 0x360,
  VU_REG_MTPC	= 0x3a0,
  VU_REG_STAT	= 0x3d0,	/* Start of special registers */
  VU_REG_CIA	= 0x3e0,
  VU_REG_END	= 0x3f0
};

#define VU0_REGISTER_WINDOW_START	0x10000C00
#define VU1_REGISTER_WINDOW_START	0x11007000

struct sreg {
  short reg;
  char filler[14];
};

struct wreg {
  int reg;
  char filler[12];
};

struct vuregs {
  float f[32][4];
  struct sreg i[16];
  struct wreg m[16];
} vu[2];

struct vuregs buf;

int main() {
    volatile char *vu0, *vu1;
    int i, j;
    float f;
    short s;
    int v;
    long long int lli;
    int error = 0;

    vu0 = (char *) VU0_REGISTER_WINDOW_START;
    vu1 = (char *) VU1_REGISTER_WINDOW_START;

   /* Write VU1 registers using friendly alignments and sizes (common case?).
    * At the same time, initialize local (reference) copy of VU0 registers.
    */
    printf( "Initialize VU1 registers...\n" );
    for( i=0; i<32; i++ )
      for( j=0; j<4; j++ ) {
	vu[0].f[i][j] = i + j/10.0;

	vu[1].f[i][j] = 100.0 + i + j/10.0;
	((struct vuregs *) vu1)->f[i][j] = vu[1].f[i][j];
      }
	
    for( i=0; i<16; i++ ) {
      vu[0].i[i].reg = i;

      vu[1].i[i].reg = 100 + i;
      ((struct vuregs *) vu1)->i[i].reg = 100 + i;
    }

    for( i=0; i<12; i++ ) {
      vu[0].m[i].reg = i;

      vu[1].m[i].reg = 100 + i;
      ((struct vuregs *) vu1)->m[i].reg = 100 + i;
    }

    /* Blast VU0 registers across using memcpy */
    printf( "Initialize VU0 registers...\n" );
    memcpy( vu0, &(vu[0]), VU_REG_END );

    /* Check VU0 registers using friendly alignment and sizes */
    printf( "Check VU0 initial register values...\n" );
    for( i=0; i<32; i++ )
      for( j=0; j<4; j++ ) {
	f = ((struct vuregs *) vu0)->f[i][j];
	if( f != vu[0].f[i][j] ) {
	  printf( "vu0_vf%02d%d: got %f (%x) != %f (%x)\n", i, j,
		  f, *((int *) &f), vu[0].f[i][j], *((int *) &(vu[0].f[i][j])));
	  error++;
	}
      }
	
    for( i=0; i<16; i++ ) {
      s = ((struct vuregs *) vu0)->i[i].reg;
      if( s != vu[0].i[i].reg ) {
	printf( "vu0_vi%02d: got %d (%x) != %d (%x)\n", i,
		s, s, vu[0].i[i].reg, vu[0].i[i].reg );
	error++;
      }
    }

    for( i=0; i<7; i++ ) {
      v = ((struct vuregs *) vu0)->m[i].reg;
      if( v != vu[0].m[i].reg ) {
	printf( "vu0_misc %2d: got %d (%x) != %d (%x)\n", i,
		v, v, vu[0].m[i].reg, vu[0].m[i].reg );
	error++;
      }
    }

    /* Load buf from VU1 reg window a double word at a time */
    for( i=0; i<VU_REG_END; i+=8 )
      *((long long int *) (((char *) &buf) + i)) 
	= *(( long long int *) (vu1 + i));

    printf( "Check VU1 initial register values...\n" );
    for( i=0; i<32; i++ )
      for( j=0; j<4; j++ ) {
	if( buf.f[i][j] != vu[1].f[i][j] ) {
	  printf( "vu1_vf%02d%d: got %f (%x) != %f (%x)\n", i, j,
		  buf.f[i][j], *((int *) &(buf.f[i][j])), 
		  vu[1].f[i][j], *((int *) &(vu[1].f[i][j])) );
	  error++;
	}
      }
	
    for( i=0; i<16; i++ ) {
      if( buf.i[i].reg != vu[1].i[i].reg ) {
	printf( "vu1_vi%02d: got %d (%x) != %d (%x)\n", i,
		buf.i[i].reg, buf.i[i].reg, vu[1].i[i].reg, vu[1].i[i].reg );
	error++;
      }
    }

    for( i=0; i<7; i++ ) {
      if( buf.m[i].reg != vu[1].m[i].reg ) {
	printf( "vu1_misc %d: got %d (%x) != %d (%x)\n", i,
		buf.m[i].reg, buf.m[i].reg, vu[1].m[i].reg, vu[1].m[i].reg );
	error++;
      }
    }

    if( error ) {
      printf( "%d errors detected\n", error );
      error = 1;
    }

    exit( error );
}
