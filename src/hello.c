#include "lcdio/lcdio.h"

void print_to_lcd(char *message) {
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i]);
        i++;
    }
}

int main(int argc, char **argv)
{
    lcd_reset();
    print_to_lcd("Hello world!");
}
