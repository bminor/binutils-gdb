void marker1 ()
{
    
}


int main()
{
    char C, *pC, **ppC, ***pppC, ****ppppC, *****pppppC, ******ppppppC;
    unsigned char UC, *pUC;
    short S, *pS;
    unsigned short US, *pUS;
    int I, *pI;
    unsigned int UI, *pUI;
    long L, *pL;
    unsigned long UL, *pUL;
    float F, *pF;
    double D, *pD;
    C = 'A';
    UC = 21;
    S = -14;
    US = 7;
    I = 102;
    UI = 1002;
    L = -234;
    UL = 234;
    F = 1.25E10;
    D = -1.375E-123;
    pC = &C;
    ppC = &pC;
    pppC = &ppC;
    ppppC = &pppC;
    pppppC = &ppppC;
    ppppppC = &pppppC;
    pUC = &UC;
    pS = &S;
    pUS = &US;
    pI = &I;
    pUI = &UI;
    pL = &L;
    pUL = &UL;
    pF = &F;
    pD = &D;
    
    #ifdef usestubs
       set_debug_traps();
       breakpoint();
    #endif
    marker1();
    return 0;
}
