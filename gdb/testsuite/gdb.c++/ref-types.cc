void marker1 (void)
{
    
}



int main(void)
{
    short s;
    short &rs = s;
    short *ps;
    short *&rps = ps;
    short as[4];
    short (&ras)[4] = as;
    s = -1;
    ps = &s;
    as[0] = 0;
    as[1] = 1;
    as[2] = 2;
    as[3] = 3;

   #ifdef usestubs
       set_debug_traps();
       breakpoint();
    #endif
    marker1();
 
    return 0;
}
