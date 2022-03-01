#include <stdio.h>
#include <ctype.h>
#include <string.h>

extern volatile unsigned char __nmiJOYPAD1;

#define OAMBUF ((unsigned char*)0x0200)

static unsigned char buf[33];
static unsigned char idx;
static void input();

int __read(int f,char *p,int n)
{
  unsigned char x=0;
  if(buf[idx]==0){
    input();
    idx=0;
  }
  while(n!=0&&buf[idx]){
    *p++=buf[idx];
    n--;x++;idx++;
  }
  return x;
}

static void input()
{
  unsigned char i,j,l=0;
  unsigned int tm,delay=0;
  i=0;
  buf[0]='a';buf[1]=0;
  while(j=__nmiJOYPAD1);
  while(1){
    __write(0,"\r",1);
    __write(0,buf,strlen(buf));
    if(j!=l) delay=0x7000;
    l=j;
    if((j&1)&&i<30) i++;
    if((j&2)&&i!=0) i--;
    if(buf[i]==0) {buf[i]=buf[i-1];buf[i+1]=0;}
    if(j&8) do{buf[i]++;}while(!isprint(buf[i]));
    if(j&4) do{buf[i]--;}while(!isprint(buf[i]));
    if(j&128) break;
    if((j&64)&&i<31){
      for(j=i+1;j<=31;j++)
        if(buf[j]) buf[j]=' ';
      __write(0,"\r",1);
      __write(0,buf,strlen(buf));

      buf[i+1]=0;
    }
    OAMBUF[255]=i<<3;
    for(tm=0;tm<delay;tm++) if((j=__nmiJOYPAD1)!=l) break;
    delay=0x500;
  }
  __write(0,"\n",1);
  buf[i+1]='\n';
  buf[i+2]=0;
}

