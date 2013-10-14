/*
 * Nano DMX - A generic DMX device for dimmers and relays
 *
 */

#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>

#include "errno.h"
#include "utils.h"

#include "ndmx.h"
#include "dmx.h"
#include "board_config.h"

struct input {
    uint8_t		value;
};

static struct input inputs[INPUT_COUNT];
static uint8_t update;

void ndmx_set_input(uint8_t i, uint8_t val)
{
    if (i < ARRAY_SIZE(inputs))
        inputs[i].value = val;
}

uint8_t ndmx_get_input(uint8_t i)
{
    return i < ARRAY_SIZE(inputs) ? inputs[i].value : 0;
}

void dmx_received_channel(uint16_t channel, uint8_t data)
{
    if (channel < ARRAY_SIZE(inputs)) {
        ndmx_set_input(channel-1, data);
    }
}

void dmx_receive_start(void)
{
    update = 1;
}

int ndmx_init(void)
{
    // On the arduino micro the bootloader leave some USB interrupts enabled
    // TODO: Move this to some board fixup code
#ifdef UDIEN
    UDIEN = 0;
#endif

    dmx_init(1, DMX_CHANNEL_COUNT);
    ndmx_init_outputs();
    update = 1;
    return 0;
}

int main(void)
{
    ndmx_init();

    sei();
    while (1) {
        if (update) {
            update = 0;
            ndmx_update_outputs();
        }
        sleep_mode();
    }

    return 0;
}
