#include "lcdio/lcdio.h"

#pragma section data
char message[6];
#pragma section default

// Add the character in the A register to the beginning of the 
// null-terminated string `message`
void push_char(char character, char *message)
{
    // Push new first char onto temp
    char temp = character;
    char *y = message;
    char x;
    
    while (temp != 0)
    {
        // Get char on string and put into x
        x = *y;
        // Pull char off temp and add it to the string
        *y = temp;
        y++;
        temp = x;
    }
    // Pull the null off temp and add to the end of the string
    *y = temp;
}

void divide(unsigned int value)
{
    // Initialize value to be the number to convert
    unsigned char overflow_val = 0;
    unsigned char overflow_mod = 0;

    // if value != 0, then continue dividing
    while (value != 0)
    {
        // Initialize remainder to be zero
        unsigned int mod10 = 0;

        // Number of bits in value
        char i = 16;

        while (i > 0)
        {
            // Check if value will overflow
            if (value >= 32768)
            {
                overflow_val = 1;
            }

            // Rotate quotient and remainder
            value = value << 1;
            value = value | overflow_mod;
            mod10 = mod10 << 1;
            mod10 = mod10 | overflow_val;

            // Reset overflows
            overflow_val = 0;
            overflow_mod = 0;
            
            // sec; sub 10; bcc;
            // ignore if dividend < divisor
            if (mod10 >= 10)
            {
                // dividend - divisor
                mod10 = mod10 - 10;
                overflow_mod = 1;
            }
            i--;
        }

        value = value << 1; // shift in the last bit of the quotient
        value = value | overflow_mod;
        overflow_mod = 0;

        push_char((unsigned char)mod10 + '0', message);
    }
}

int main(int argc, char **argv)
{
    // Initialize message so we know where the end is
    *message = 0;

    divide(1729);
    
    print_message_to_lcd(message);
}
