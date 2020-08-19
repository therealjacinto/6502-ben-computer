#include "lcdio/lcdio.h"

#pragma section data
char message[6]; // 6 bytes in 0x0200X section
#pragma section default

// Add the character in the A register to the beginning of the 
// null-terminated string `message`
void push_char(char character, char *message)
{
    char temp = character;
    char *y = message;
    char x;
    
    while (temp != 0)
    {
        x = *y;
        *y = temp;
        //print_char(temp);
        y++;
        temp = x;
    }
    *y = temp;
}

void divide()
{
    // Initialize value to be the number to convert
    unsigned int value = 1729;
    unsigned char overflow_val = 0;
    unsigned char overflow_mod = 0;

    // if value != 0, then continue dividing
    while (value != 0)
    {
        // Initialize remainder to be zero
        unsigned int mod10 = 0;

        char i = 16;

        while (i > 0)
        {
            // Rotate quotient and remainder
            if (value >= 32768)
            {
                // Top bit is a 1
                overflow_val = 1;
            }
            value = value << 1;
            value = value | overflow_mod;
            mod10 = mod10 << 1;
            mod10 = mod10 | overflow_val;
            overflow_val = 0;
            overflow_mod = 0;
            
            if (mod10 >= 10)
            {
                mod10 = mod10 - 10;
                overflow_mod = 1;
            }
            i--;
        }

        value = value << 1; // shift in the last bit of the quotient
        value = value | overflow_mod;
        overflow_mod = 0;

        unsigned char lowermod10 = (unsigned char)mod10;
        lowermod10 = lowermod10 + '0';
        push_char(lowermod10, message);
    }
}

int main(int argc, char **argv)
{
    *message = 0;

    divide();
    
    print_to_lcd(message);
}
