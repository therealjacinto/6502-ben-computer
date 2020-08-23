#define PORTB ((volatile unsigned char*)0x6000)
#define PORTA ((volatile unsigned char*)0x6001)
#define DDRB ((volatile unsigned char*)0x6002)
#define DDRA ((volatile unsigned char*)0x6003)
#define E 128
#define RW 64
#define RS 32

void set_PORTA(char val)
{
    *PORTA = val;
}

void set_PORTB(char val)
{
    *PORTB = val;
}

void set_DDRA(char val)
{
    *DDRA = val;
}

void set_DDRB(char val)
{
    *DDRB = val;
}

char get_PORTB()
{
    return *PORTB;
}
