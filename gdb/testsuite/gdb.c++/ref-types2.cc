int f()
{
    int f1;
    f1 = 1;
    return f1;
}

int main(void)
{
    char C;
    unsigned char UC;
    short S;
    unsigned short US;
    int I;
    unsigned int UI;
    long L;
    unsigned long UL;
    float F;
    double D;
    char &rC = C;
    unsigned char &rUC = UC;
    short &rS = S;
    unsigned short &rUS = US;
    int &rI = I;
    unsigned int &rUI = UI;
    long &rL = L;
    unsigned long &rUL = UL;
    float &rF = F;
    double &rD = D;
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
    I = f();
    #ifdef usestubs
       set_debug_traps();
       breakpoint();
    #endif
    return 0;
    
}
