#include <stdio.h>

class math_stuff
{
   char name[40];
   int value;

public:

   void bla();

   math_stuff(char*);

   ~math_stuff();

   void add(int);

   void print_value(char*);
};

void math_stuff::bla()
{
  printf("bla\n");
}

math_stuff::math_stuff(char* new_name)
{
  sprintf((char *)this->name,(char *)"MATH_STUFF_%s",new_name);
  value = 0;
}

math_stuff::~math_stuff()
{
  printf((char *)"Deleting MATH_STUFF instance '%s'\n",this->name);
}

void math_stuff::add(int x)
{
  this->value += x;
}

void math_stuff::print_value(char *where)
{
  printf((char *)"current value of '%s' at '%s' = %d\n", this->name, where, this->value);
}

class derived : public math_stuff
{
  char *dname;
  int val;

public:
  math_stuff stuff;
  derived(char*);
  ~derived();
  void print_all(char*);
};

derived::derived(char *der_name)
     : ((char *)"hello, world!"), stuff("derived stuff"), val(10)
{
  printf((char *)"derived name = %s\n", der_name);
  dname = der_name;
  stuff.add(6);
  stuff.print_value ((char *)"grind");
}

derived::~derived()
{
  printf ((char *)"Good bye! \n");
}

derived::print_all(char *msg)
{
  printf((char *)"Here's the lowdown:\n");
  printf((char *)"My name is %s\n", dname);
  printf((char *)"My value = %d\n", val);
  stuff.print_value ((char *)"low");
  this->print_value((char *)"down");
  printf((char *)"That's all you get tonight.\n\n");
}

main()
{
  int pid;
  int i;
 
  pid = vfork();
  if (pid > 0)
    printf(" the inferior pid of the child is %d\n",pid);
  if (pid == 0)
    {
/*      execl ("/bin/sh", "sh", "-c",argv[1],0);*/

      math_stuff* a;
      derived* d;
      math_stuff b $ (" no gnus are good gnus ");
      int child;
      child = getpid();
      printf( " the child from parent.c is %d\n",child);
      ptrace (0);   
     
      a = new math_stuff((char *)"a");
      
      d = new derived((char *)"two words");
      
      a->add(2);
      a->add(4);
      a->add(5);
      a->print_value((char *)"here");
      a->add(7);
      a->print_value((char *)"there");
      a->add(3);
      a->add(1);
      a->print_value((char *)"end");
      
      d->print_all ((char *)"all");
      
      delete a;
      delete d;
      exit(0);
    }
  i = 5;
  i = 6;
  i = 7;
}
