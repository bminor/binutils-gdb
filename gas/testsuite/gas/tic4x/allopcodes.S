;;; 
;;;  Test all opcodes and argument permuation
;;;  To make our job a lot simpler, we define a couple of
;;;  insn classes, that we use to generate the proper
;;;  test output.
;;;  
;;;  To rebuild this file you must use
;;;    ./rebuild.sh
;;; 
#include "opclasses.h"
        
            .text
start:      B_CLASS( absf )
            P_CLASS( absf, stf )
            A_CLASS( absi )
            P_CLASS( absi, sti )
            A_CLASS( addc )
            TC_CLASS( addc )
            B_CLASS( addf )
            SC_CLASS( addf )
            QC_CLASS( addf, stf )
            A_CLASS( addi )
            TC_CLASS( addi )
            QC_CLASS( addi, sti )
            AU_CLASS( and )
            TC_CLASS( and )
            QC_CLASS( and, sti )
            AU_CLASS( andn )
            T_CLASS( andn )
            A_CLASS( ash )
            T_CLASS( ash )
            Q_CLASS( ash, sti )
            J_CLASS( bC, b )
            J_CLASS( bCd, bd )
br_I:       br      start
brd_I:      brd     start
call_I:     call    start
call_JS:    callc   R0
            callc   start
            B_CLASS( cmpf )
            S2_CLASS( cmpf )
            A_CLASS( cmpi )
            T2_CLASS( cmpi )
            D_CLASS( dbC, db )
            D_CLASS( dbCd, dbd )
            AF_CLASS( fix )
            P_CLASS( fix, sti )
            BI_CLASS( float )
            P_CLASS( float, stf )
iack_Z:     iack    @start
            iack    *+AR0(1)
idle_Z:     idle
            .ifdef TEST_IDLE2
idle2_Z:    idle2
            .endif
            B_CLASS( lde )
            B_CLASS( ldf )
            LL_CLASS( ldf )
            P_CLASS( ldf, stf )
            BB_CLASS( ldfC )
            B6_CLASS( ldfi )
            A_CLASS( ldi )
            LL_CLASS( ldi )
            P_CLASS( ldi, sti )
            AB_CLASS( ldiC )
            A6_CLASS( ldii )
ldp_Z:      ldp     start
            B_CLASS( ldm )
            .ifdef TEST_LPWR
lopower_Z:  lopower
            .endif
            A_CLASS( lsh )
            T_CLASS( lsh )
            Q_CLASS( lsh, sti )
            .ifdef TEST_LPWR
maxspeed_Z: maxspeed
            .endif
            B_CLASS( mpyf )
            SC_CLASS( mpyf )
            M_CLASS( mpyf, addf )
            QC_CLASS( mpyf, stf )
            M_CLASS( mpyf, subf )
            A_CLASS( mpyi )
            TC_CLASS( mpyi )
            M_CLASS( mpyi, addi )
            QC_CLASS( mpyi, sti )
            M_CLASS( mpyi, subi )
            A_CLASS( negb )
            B_CLASS( negf )
            P_CLASS( negf, stf )
            A_CLASS( negi )
            P_CLASS( negi, sti )
            A2_CLASS( nop )
            B_CLASS( norm )
            AU_CLASS( not )
            P_CLASS( not, sti )
            AU_CLASS( or )
            TC_CLASS( or )
            QC_CLASS( or, sti )
            R_CLASS( pop )
            RF_CLASS( popf )
            R_CLASS( push )
            RF_CLASS( pushf )
reti_Z:     retiC
            reti
rets_Z:     retsC
            rets
            B_CLASS( rnd )
            R_CLASS( rol )
            R_CLASS( rolc )
            R_CLASS( ror )
            R_CLASS( rorc )
rptb_I2:    rptb    start
            A3_CLASS( rpts )
sigi_Z:     sigi
            B7_CLASS( stf )
            LS_CLASS( stf )
            B7_CLASS( stfi )
            A7_CLASS( sti )
            LS_CLASS( sti )
            A7_CLASS( stii )
            A_CLASS( subb )
            T_CLASS( subb )
            A_CLASS( subc )
            B_CLASS( subf )
            S_CLASS( subf )
            Q_CLASS( subf, stf )
            A_CLASS( subi )
            T_CLASS( subi )
            Q_CLASS( subi, sti )
            A_CLASS( subrb )
            B_CLASS( subrf )
            A_CLASS( subri )
swi_Z:      swi
trap_Z:     trapC   10
            trap    10
            AU_CLASS( tstb )
            T2C_CLASS( tstb )
            AU_CLASS( xor )
            TC_CLASS( xor )
            QC_CLASS( xor, sti )
    
            .ifdef  TEST_C4X
            J_CLASS( bCaf, baf )
            J_CLASS( bCat, bat )
            B6_CLASS( frieee )
            P_CLASS( frieee, stf )
laj_I:      laj     start
laj_JS:     lajc    R0
            lajc    start
lat_Z:      latC    10
            A_CLASS( lb0 )
            A_CLASS( lb1 )
            A_CLASS( lb2 )
            A_CLASS( lb3 )
            AU_CLASS( lbu0 )
            AU_CLASS( lbu1 )
            AU_CLASS( lbu2 )
            AU_CLASS( lbu3 )
            AY_CLASS( lda )
ldep_Z:     ldep    IVTP, AR0
ldhi_Z:     ldhi    35, R0
            ldhi    start, R0
ldpe_Z:     ldpe    AR0, IVTP
ldpk_Z:     ldpk    start
            A_CLASS( lh0 )
            A_CLASS( lh1 )
            AU_CLASS( lhu0 )
            AU_CLASS( lhu1 )
            A_CLASS( lwl0 )
            A_CLASS( lwl1 )
            A_CLASS( lwl2 )
            A_CLASS( lwl3 )
            A_CLASS( lwr0 )
            A_CLASS( lwr1 )
            A_CLASS( lwr2 )
            A_CLASS( lwr3 )
            A_CLASS( mb0 )
            A_CLASS( mb1 )
            A_CLASS( mb2 )
            A_CLASS( mb3 )
            A_CLASS( mh0 )
            A_CLASS( mh1 )
            A_CLASS( mh2 )
            A_CLASS( mh3 )
            A_CLASS( mpyshi )
            TC_CLASS( mpyshi )
            A_CLASS( mpyuhi )
            TC_CLASS( mpyuhi )
            BA_CLASS( rcpf )
retid_Z:    retiCd
            retid
rptb2_I2:   rptb    AR0
rptbd_I2:   rptbd  start
            rptbd   AR0
            B_CLASS( rsqrf )
            A6_CLASS( sigi )
sti2_A7:    sti     -5, @start
            sti     -5, *+AR0(5)
stik_Z:     stik    -5, @start
            stik    -5, *+AR0(5)
            B_CLASS( toieee )
            P_CLASS( toieee, stf )
            .endif
            .end
        
