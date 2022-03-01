#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

char buf[128];
char *s;

float prim_exp(),add_exp(),mul_exp(),num_exp();

void skip()
{
  while(isspace((unsigned char)*s))
    s++;
}

float num_exp()
{
  float x;
  x=strtod(s,&s);
  return x;
}

float mul_exp()
{
  float x,y;
  char c;
  x=prim_exp();
  skip();
  while(*s=='*'||*s=='/'){
    c=*s++;
    skip();
    y=prim_exp();
    if(c=='*') x*=y; else x/=y;
    skip();
  }
  return x;
}

float add_exp()
{
  float x,y;
  char c;
  x=mul_exp();
  skip();
  while(*s=='+'||*s=='-'){
    c=*s++;
    skip();
    y=mul_exp();
    if(c=='+') x+=y; else x-=y;
    skip();
  }
  return x;
}

float prim_exp()
{
  if(*s=='('){
    float x;
    s++;
    skip();
    x=add_exp();
    skip();
    if(*s==')')
      s++;
    skip();
    return x;
  }else
    return num_exp();
}

int main()
{
  while(1){
    printf("> ");
    fflush(stdout);
    fgets(buf,sizeof(buf)-1,stdin);
    s=buf;
    printf("\nresult: %g\n",add_exp());
  }
  
}

