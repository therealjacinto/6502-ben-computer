#define PORTB ((volatile unsigned char*)0x6000)
#define PORTA ((volatile unsigned char*)0x6001)
#define DDRB ((volatile unsigned char*)0x6002)
#define DDRA ((volatile unsigned char*)0x6003)
#define E 128
#define RW 64
#define RS 32

void set_PORTA(__reg("a") char val) =
  "\tsta $6001";

void set_PORTB(__reg("a") char val) =
  "\tsta $6000";

void set_DDRA(__reg("a") char val) =
  "\tsta $6003";

void set_DDRB(__reg("a") char val) =
  "\tsta $6002";

__regsused("a") char get_PORTB() = 
  "\tlda $6000";
