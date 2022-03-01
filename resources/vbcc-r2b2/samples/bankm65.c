#include <stdio.h>

/* Simple demo for the preliminary banking support for the MEGA65

   The upper 32KB are used as banking window. The following
   addresses wi

   Bank 0: 0x000000..0x007FFF
   Bank 1: 0x008000..0x00FFFF
   Bank 2: 0x010000..0x017FFF
   Bank 3: 0x018000..0x01FFFF
   ...
*/

#define SIZE 100

__bank(2) long b[SIZE];
__bank(3) long c[SIZE];

main()
{
  __far char *p;
  int i;
  long sum;

  printf("Displaying the upper part of the C65 ROM bank (bank 7):\n");

  p=(__far char *)0x7FFC0;
  for(i=0;i<=64;i++)
    printf("%02x  ",*p++);
  printf("\n");

  printf("Displaying the upper part of the C64 ROM bank (bank 5):\n");

  p=(__far char *)0x5FFC0;
  for(i=0;i<=64;i++)
    printf("%02x  ",*p++);
  printf("\n");


  for(i=0;i<SIZE;i++) b[i]=i;
  for(i=0;i<SIZE;i++) c[i]=i;


  for(i=0,sum=0;i<SIZE;i++) sum+=b[i]+c[i];

  printf("sum = %ld\n",sum);

  for(i=0,sum=0;i<SIZE;i++) sum+=i+i;

  printf("comparison sum = %ld\n",sum);

}

