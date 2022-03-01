#include "libsys.h"

unsigned int __write(int f,const char *p,unsigned int len)
{
  unsigned int n=len;
  while(n){
   __chrout(*p++);
   n--;
  }
  return len;
}
