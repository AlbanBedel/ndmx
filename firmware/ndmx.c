/*
 * Nano DMX - A generic DMX device for dimmers and relays
 *
 */

#include <stdlib.h>

#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>

#include "errno.h"
#include "utils.h"

#include "ndmx.h"
#include "dmx.h"
#include "nano_vm.h"

struct input {
    uint8_t		value;
};

static struct input inputs[INPUT_COUNT];

struct vm {
    struct nano_vm vm;
    uint8_t vector;
};

static uint16_t frame;

void ndmx_set_input(uint8_t i, uint8_t val)
{
    if (i < ARRAY_SIZE(inputs))
        inputs[i].value = val;
}

uint8_t ndmx_get_input(uint8_t i)
{
    return i < ARRAY_SIZE(inputs) ? inputs[i].value : 0;
}

uint8_t nano_vm_r8(struct nano_vm *vm)
{
    return (vm->pc < E2END) ?
        eeprom_read_byte((uint8_t*)(vm->pc++)) : OP_RETURN_ERROR;
}

int8_t nano_vm_call(struct nano_vm *vm, uint8_t func)
{
    struct vm *ndmx_vm = (struct vm *)vm;
    switch (func) {
    case FUNC_GET_ID:
        nano_vm_push(vm, ndmx_vm->vector);
        return 1;
    case FUNC_GET_INPUT:
        nano_vm_push(vm, ndmx_get_input(nano_vm_pop(vm)));
        return 1;
    case FUNC_GET_OUTPUT:
        nano_vm_push(vm, ndmx_get_output(nano_vm_pop(vm)));
        return 1;
    case FUNC_GET_FRAME:
        nano_vm_push(vm, frame);
        return 1;
    default:
        return -ENOENT;
    }
}

int16_t ndmx_vm_run(uint8_t vector)
{
    struct vm ndmx_vm = {};

    if (vector >= ARRAY_SIZE(ndmx_config.entry))
        return 0;

    ndmx_vm.vector = vector;
    ndmx_vm.vm.pc =
        (nano_vm_ptr_t)eeprom_read_word(
            (const uint16_t *)&ndmx_config.entry[vector]);

    return nano_vm_run(&ndmx_vm.vm) ? 0 : (int16_t)nano_vm_pop(&ndmx_vm.vm);
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

void ndmx_init(void)
{
    ndmx_board_pre_init();
    dmx_init(1, DMX_CHANNEL_COUNT);
    ndmx_init_outputs();
    ndmx_board_post_init();
}

int  __attribute__((OS_main)) main(void)
{
    ndmx_init();

    sei();
    while (1) {
        ndmx_update_outputs();
        frame += 1;
    }

    return 0;
}
