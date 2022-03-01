#include <stdio.h>
#include <string.h>

typedef struct {int a[10];} styp;

__bank(0) const styp s0={0,1,2,3,4,5,6,7,8,9};
__bank(1) const styp s1={1,2,3,4,5,6,7,8,9,0};
__bank(2) const styp s2={2,3,4,5,6,7,8,9,0,1};

styp sram;

__far const styp *p2=&s2,*p1=&s1,*p0=&s0;
__far styp *pram=&sram;

__bank(1) void g(styp x)
{
 unsigned char i;
 puts("enter g");
 for(i=0;i<10;i++) printf("%d ",x.a[i]);
 puts("\nexit g");
}


__bank(1) void f()
{
 unsigned char i;

 puts("enter f");
 for(i=0;i<10;i++) printf("%d ",s0.a[i]);
 printf("\n");
 for(i=0;i<10;i++) printf("%d ",s1.a[i]);
 printf("\n");
 for(i=0;i<10;i++) printf("%d ",s2.a[i]);
 printf("\n");
 for(i=0;i<10;i++) printf("%d ",sram.a[i]);
 printf("\n");
 sram=s0;
 for(i=0;i<10;i++) printf("%d ",sram.a[i]);
 printf("\n");
 *pram=*p2;
 for(i=0;i<10;i++) printf("%d ",sram.a[i]);
 printf("\n");
 _fmemcpy(&sram,&s1,sizeof(sram));
 for(i=0;i<10;i++) printf("%d ",sram.a[i]);
 printf("\n");
 _fmemcpy(pram,p0,sizeof(sram));
 for(i=0;i<10;i++) printf("%d ",sram.a[i]);
 printf("\n");

 g(s0);
 g(s1);
 g(s2);

 puts("exit f");
}


int main()
{
 f();
 puts("done");
}
