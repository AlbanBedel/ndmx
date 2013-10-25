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

struct input {
    uint8_t		value;
};

static struct input inputs[INPUT_COUNT];

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

void __attribute__((weak))
ndmx_board_pre_init(void)
{}

void __attribute__((weak))
ndmx_board_post_init(void)
{}

int ndmx_init(void)
{
    ndmx_board_pre_init();
    dmx_init(1, DMX_CHANNEL_COUNT);
    ndmx_init_outputs();
    ndmx_board_post_init();
    return 0;
}

int main(void)
{
    ndmx_init();

    sei();
    while (1) {
        ndmx_update_outputs();
    }

    return 0;
}
