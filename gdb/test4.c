main()
{
  int i,j,k;

  i = 100;
  j = 101;
  k = foo(0,1,2,3,4,5,6,7,8);
}

int f3 (x)
{
  return x;
}

int f2 (x)
{
  return f3 (x+1);
}

int f1 (x)
{
  return f2 (x+1);
}

foo(a,b,c,d,e,f,g,h,i)
     int a,b,c,d,e,f,g,h,i;
{
  int r;
  r = a + b + c + d + e + f + g + h + i;
  if (f1 (11))
    return r;
  else
    return 0;
}

bar(ind)
     int ind;
{
  printf("this is a test\n");
}
