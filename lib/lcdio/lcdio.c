/*
 * Desctiption: Functions that control the HD44780U LCD
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

#include "iochip/iochip.h"

#define E   128 /* 10000000 Enable bit via PORTA */
#define RW   64 /* 01000000 Read/Write bit via PORTA */
#define RS   32 /* 00100000 Register Select bit via PORTA */

/*
 * Function:  wait_for_lcd 
 * -----------------------
 * Queries LCD infinitely until the busy flag is cleared.
 */
void wait_for_lcd()
{
    unsigned char busy = 1;
    set_data_direction_port_b(0);
    set_data_direction_port_a(224);
    do
    {
        set_data_port_a(RW);             /* Set read bit */
        set_data_port_a(RW | E);         /* Enable read */
        busy = get_data_port_b() & 128;  /* Mask busy flag */
    } while (busy != 0);
    set_data_port_a(RW);                 /* Clear enable bit */
}

/*
 * Function:  send_lcd_instruction 
 * -------------------------------
 * Send an instruction code to the LCD.
 *
 *  instruction: instruction code
 */
void send_lcd_instruction(unsigned char instruction)
{
    /* Verify busy flag is clear */
    wait_for_lcd();

    /* Push instruction on data bus */
    set_data_direction_port_b(255);
    set_data_port_b(instruction);

    /* Enable instruction */
    set_data_direction_port_a(224);
    set_data_port_a(0);  /* Clear RS/RW/E bits */
    set_data_port_a(E);  /* Set E bit to send instruction */
    set_data_port_a(0);  /* Clear RS/RW/E bits */
}

/*
 * Function:  reset_lcd 
 * --------------------
 * Initialize LCD and clear display.
 */
void reset_lcd()
{
    send_lcd_instruction(56); /* Set 8-bit mode; 2-line display; 5x8 font */
    send_lcd_instruction(14); /* Display on; cursor on; blink off */
    send_lcd_instruction(6);  /* Increment and shift cursor; don't shift */
                              /* display */
    send_lcd_instruction(1);  /* Clear display */
}

/*
 * Function:  print_char_to_lcd 
 * ----------------------------
 * Print single character to LCD. Assumes LCD has been initialized.
 * 
 *  character: char to print
 */
void print_char_to_lcd(unsigned char character) {
    /* Verify busy flag is clear */
    wait_for_lcd();

    /* Push character on data bus */
    set_data_direction_port_b(255);
    set_data_port_b(character);

    /* Enable character */
    set_data_direction_port_a(224);
    set_data_port_a(RS);      /* Set RS; Clear RW/E bits */
    set_data_port_a(RS | E);  /* Set E bit to send instruction */
    set_data_port_a(RS);      /* Clear E bit */
}

/*
 * Function:  print_message_to_lcd 
 * -------------------------------
 * Print string to LCD. Assumes the LCD has been initialized.
 * 
 *  character: char to print
 */
void print_message_to_lcd(char *message) {
    int i = 0;
    while (message[i] != 0)
    {
        print_char_to_lcd(message[i]);
        i++;
    }
}
