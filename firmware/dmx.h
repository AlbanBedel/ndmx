#ifndef DMX_H
#define DMX_H

#include <stdint.h>

void dmx_init(uint16_t address, uint8_t count);

void dmx_received_channel(uint16_t channel, uint8_t data);

void dmx_receive_start(void);

#endif /* DMX_H */
