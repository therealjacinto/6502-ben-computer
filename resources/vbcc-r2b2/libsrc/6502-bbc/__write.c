
void OSNEWL()="\tjsr\t0xffe7";
void OSWRCHR(__reg("a") char)="\tjsr\t0xffee";

unsigned int __write(int f,const char *p,unsigned int len)
{
  unsigned int n=len;
  while(n){
   char c=*p++;
   if(c=='\n')
     OSNEWL();
   else
     OSWRCHR(c);
   n--;
  }
  return len;
}
