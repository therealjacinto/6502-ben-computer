#include "libsys.h"

unsigned int __read(int f,char *p,unsigned int len)
{
  unsigned int n=0;
  char c;
  do{
   *p++=c=__chrin();
    __chrout(c);
    n++;
  }while(n<len&&c!='\n');
  return n;
}
