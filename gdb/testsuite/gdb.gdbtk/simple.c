void
main(int argc, char * argv[])
{
   int i;
   char *a;
   char *b = "abc";
   long  foo;

   a = (char *) malloc (300);

   for (i=0; i < 50; i++)
     {
       int j = i % 3;
       int k = 3 - j;
       strncpy (a[i], b[k], j);
       foo = (long) j * k / i + 2 * k * k * k;
     }
}

