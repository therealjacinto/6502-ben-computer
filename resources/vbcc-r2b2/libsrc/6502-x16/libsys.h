#define BUFSIZ 16

unsigned int __write(int,const char *,unsigned int);
unsigned int __read(int,char *,unsigned int);

void __chrout(__reg("a") unsigned char x)="\tjsr\t0xffd2";
char __chrin(void)="\tjsr\t0xffcf";

long __rdtim(void)="\tjsr\t0xffde\n"
                   "\tsta\tbtmp0\n"
                   "\tstx\tbtmp0+1\n"
                   "\tsty\tbtmp0+2\n"
                   "\tlda\t#0\n"
                   "\tsta\tbtmp0+3";
