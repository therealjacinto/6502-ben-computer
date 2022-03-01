#define PORTB ((volatile unsigned char*)0x6000)
#define PORTA ((volatile unsigned char*)0x6001)
#define DDRB  ((volatile unsigned char*)0x6002)
#define DDRA  ((volatile unsigned char*)0x6003)
#define E   128 // 10000000
#define RW   64 // 00110100
#define RS   32 // 00011010

void set_PORTA(unsigned char val)
{
    *PORTA = val;
}

void set_PORTB(unsigned char val)
{
    *PORTB = val;
}

void set_DDRA(unsigned char val)
{
    *DDRA = val;
}

void set_DDRB(unsigned char val)
{
    *DDRB = val;
}

unsigned char get_PORTB()
{
    return *PORTB;
}
