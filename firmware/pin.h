#ifndef PIN_H
#define PIN_H

#include <stdint.h>
#include <avr/io.h>
#include "utils.h"

#define PORT(port)	((volatile uint8_t *)_SFR_ADDR(CAT2(PIN,port)))

#define pin_set_value(p,b,e)                    \
    port_pin_set_value(PORT(p), b, e)

#define pin_get_value(p,b)                      \
    port_pin_get_value(PORT(p), b)

#define pin_direction_input(p,b,pup)            \
    port_pin_direction_input(PORT(p), b, pup)

#define pin_direction_output(p,b,en)            \
    port_pin_direction_output(PORT(p), b, en)

#define PORT_REG_PIN		0
#define PORT_REG_DDR		1
#define PORT_REG_PORT		2

static inline void
port_pin_set_value(volatile uint8_t *port, uint8_t pin, uint8_t  enable)
{
    if (enable)
        port[PORT_REG_PORT] |= BIT(pin);
    else
        port[PORT_REG_PORT] &= ~BIT(pin);
}

static inline void
port_pin_direction_input(volatile uint8_t *port, uint8_t pin, uint8_t  pull_up)
{
    port[PORT_REG_DDR] &= ~BIT(pin);
    port_pin_set_value(port, pin, pull_up);
}

static inline void
port_pin_direction_output(volatile uint8_t *port, uint8_t pin, uint8_t  enable)
{
    port_pin_set_value(port, pin, enable);
    port[PORT_REG_DDR] |= BIT(pin);
}

static inline uint8_t
port_pin_get_value(volatile uint8_t *port, uint8_t pin)
{
    return (port[PORT_REG_PIN] >> pin) & 1;
}

#endif /* PIN_H */
