#include <stdio.h>

#pragma bank 0
void f1()
{
 puts("f1");
}

#pragma bank 1
void f2()
{
 puts("f2");
}

#pragma bank 2
void f3()
{
 puts("f3");
}

#pragma bank -1
typedef __far void (* fptr)();

fptr farray[] = {f1,f2,f3};

int main()
{
 unsigned char i;
 for(i=0;i<sizeof(farray)/sizeof(farray[0]);i++)
   farray[i]();
}

