#include <avr/io.h>

void ndmx_board_pre_init(void)
{
    // The arduino micro bootloader leave some USB interrupts enabled
    UDIEN = 0;
}
