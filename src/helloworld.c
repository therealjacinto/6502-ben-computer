/*
 * Desctiption: Prints "Hello, World!" to LCD screen.
 * 
 * Author: therealjacinto
 * Date:   2022-03-03
 */

#include "lcdio/lcdio.h"

int main() {
    reset_lcd();
    print_message_to_lcd("Hello, world!");
}
