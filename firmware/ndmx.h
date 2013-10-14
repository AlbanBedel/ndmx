#ifndef NDMX_H
#define NDMX_H

#include <stdint.h>

uint8_t ndmx_get_input(uint8_t i);

void ndmx_init_outputs(void);

void ndmx_update_outputs(void);

#endif /* NDMX_H */
