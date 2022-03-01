#include "iochip/iochip.h"

void lcdbusy() {
    unsigned char temp = 1;
    do
    {
        set_PORTA(RW);
        set_PORTA(RW | E);
        temp = get_PORTB() & 128;
    } while (temp != 0);
    set_PORTA(RW);
    set_DDRB(255); /* Port B is output */
}

void lcd_wait()
{
    set_DDRB(0); /* Port B is input */ // NO
    lcdbusy();
}

void lcd_instruction(unsigned char command)
{   
    lcd_wait();
    set_PORTB(command);
    set_PORTA(0); /* Clear RS/RW/E bits */
    set_PORTA(E); /* Set E bit to send instruction */
    set_PORTA(0); /* Clear RS/RW/E bits */
}

void lcd_reset()
{
    set_DDRB(255); /* Set all pins on port B to output */
    set_DDRA(224); /* Set top 3 pins on port A to output */
    lcd_instruction(56); /* Set 8-bit mode; 2-line display; 5x8 font */
    lcd_instruction(14); /* Display on; cursor on; blink off */
    lcd_instruction(6); /* Increment and shift cursor; don't shift display */
    lcd_instruction(1); /* Clear display */
}

void print_char(unsigned char character) {
    lcd_wait();
    set_PORTB(character);
    set_PORTA(RS); /* Set RS; Clear RW/E bits */
    set_PORTA(RS | E); /* Set E bit to send instruction */
    set_PORTA(RS); /* Clear E bit */
}

void print_message_to_lcd(char *message) {
    int i = 0;
    while (message[i] != 0)
    {
        print_char(message[i]);
        i++;
    }
}
