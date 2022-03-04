/*
 * Desctiption: Functions that control the HD44780U LCD
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

/* Initialize LCD hardware and clear screen */
void reset_lcd();

/* Print string to LCD */
void print_message_to_lcd(unsigned char *message);
