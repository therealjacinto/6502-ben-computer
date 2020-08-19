#include "iochip/iochip.h"

void lcdbusy()
{
    while (1)
    {
        *PORTA = RW;
        *PORTA = RW | E;
        if ((*PORTB & 128) == 0)
            break;
    }
    *PORTA = RW;
    *DDRB = 255; /* Port B is output */
}

void lcd_wait()
{
    *DDRB = 0; /* Port B is input */
    lcdbusy();
}

void lcd_instruction(char command)
{   
    lcd_wait();
    *PORTB = command;
    *PORTA = 0; /* Clear RS/RW/E bits */
    *PORTA = E; /* Set E bit to send instruction */
    *PORTA = 0; /* Clear RS/RW/E bits */
}

void print_char(char character) {
    lcd_wait();
    *PORTB = character;
    *PORTA = RS; /* Set RS; Clear RW/E bits */
    *PORTA = (RS | E); /* Set E bit to send instruction */
    *PORTA = RS; /* Clear E bit */
}

void lcd_reset()
{
    *DDRB = 255; /* Set all pins on port B to output */
    *DDRA = 224; /* Set top 3 pins on port A to output */
    lcd_instruction(56); /* Set 8-bit mode; 2-line display; 5x8 font */
    lcd_instruction(14); /* Display on; cursor on; blink off */
    lcd_instruction(6); /* Increment and shift cursor; don't shift display */
    lcd_instruction(1); /* Clear display */
}

void print_to_lcd(char *message) {
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i]);
        i++;
    }
}
