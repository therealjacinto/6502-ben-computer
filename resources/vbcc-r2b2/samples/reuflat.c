#include <stdio.h>

int main()
{
  unsigned long p,s=0;

  /* calculate sum the simple way */
  for(p=0;p<100000;p+=4) s+=p;
  printf("sum1=%lu\n",s);

  /* fill array */
  for(p=0;p<100000;p+=4) *(__far long *)p=p;
  /* calculate sum using REU */
  s=0;
  for(p=0;p<100000;p+=4) s+=*(__far long *)p;
  printf("sum2=%lu\n",s);
}
