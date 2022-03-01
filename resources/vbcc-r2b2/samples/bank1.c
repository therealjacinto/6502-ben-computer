#include <stdio.h>

__bank(1) const int t1[4000]={1,2,3};

__bank(2) const int t2[4000]={4,5,6};

__bank(0) __far const int * const p[]={t1,t2};

__bank(1) const struct t {int x,y[2];} s1 = {7,8,9};

__bank(0) __far const struct t *const sp=&s1;

__bank(2) void f()
{
  unsigned char i;
  for(i=0;i<sizeof(p)/sizeof(p[0]);i++)
    printf("%d %d %d\n",p[i][0],p[i][1],p[i][2]);
  printf("%d",s1.x);
  for(i=0;i<sizeof(s1.y)/sizeof(s1.y[0]);i++)
    printf(" %d",s1.y[i]);
  printf("\n");
  printf("%d",sp->x);
  for(i=0;i<sizeof(s1.y)/sizeof(s1.y[0]);i++)
    printf(" %d",sp->y[i]);
  printf("\n");

}

int main()
{
  f();
}

