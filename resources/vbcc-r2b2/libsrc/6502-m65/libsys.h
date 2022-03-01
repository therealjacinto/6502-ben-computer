
unsigned int __write(int,const char *,unsigned int);
unsigned int __read(int,char *,unsigned int);

void __chrout(unsigned char x)="\tlda\t#0\n"
                               "\ttay\n"
                               "\tldx\t#%11110000\n"
                               "\tldz\t#%10000011\n"
                               "\tmap\n"
                               "\tlda\tr0\n"
                               "\tjsr\t0xffd2\n"
                               "\tldy\t#1\n"
                               "\tjsr\t___bankswitch\n"
                               "\tldz\t#0";

char __chrin (void)="\tlda\t#0\n"
                    "\ttay\n"
                    "\tldx\t#%11110000\n"
                    "\tldz\t#%10110000\n"
                    "\tmap\n"
                    "\tldz\t#0\n"
                    ".l:\n"
                    "\tlda\t54800\n"
                    "\tbeq\t.l\n"
                    "\tstz\t54800\n"
                    "\tpha\n"
                    "\tldy\t#1\n"
                    "\tjsr\t___bankswitch\n"
                    "\tpla\n"
                    "\tldz\t#0\n"
                    "\tstz\t54800";

long __rdtim(void)="\tlda\t#0\n"
                    "\ttay\n"
                    "\tldx\t#%11110000\n"
                    "\tldz\t#%10000011\n"
                    "\tmap\n"
                    "\tjsr\t0xffde\n"
                    "\tstz\tbtmp0+0\n"
                    "\tsta\tbtmp0+1\n"
                    "\tstx\tbtmp0+2\n"
                    "\tsty\tbtmp0+3\n"
                    "\tldy\t#1\n"
                    "\tjsr\t___bankswitch\n"
                    "\tldz\t#0\n";
