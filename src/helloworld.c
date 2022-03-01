#include "lcdio/lcdio.h"

int main() {
    lcd_reset();
    print_message_to_lcd("Hello, world!");
}
