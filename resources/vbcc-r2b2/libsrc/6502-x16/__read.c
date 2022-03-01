#include "libsys.h"

unsigned int __read(int f,char *p,unsigned int len)
{
  unsigned int n=0;
  char c;
  do{
   *p++=c=__chrin();
    n++;
  }while(n<len&&c!='\n');
  if(c=='\n') __chrout('\n');
  return n;
}
