#ifndef NDMX_H
#define NDMX_H

#include <stdint.h>

struct ndmx_config {
    const uint8_t		*entry[OUTPUT_COUNT];
};

#define FUNC_GET_ID		0
#define FUNC_GET_INPUT		1
#define FUNC_GET_OUTPUT		2
#define FUNC_GET_FRAME		3

extern const struct ndmx_config ndmx_config
__attribute__((section(".ndmx_config")));

extern const uint8_t ndmx_default_prog[]
__attribute__((section(".ndmx_prog")));

int16_t ndmx_vm_run(uint8_t vector);

uint8_t ndmx_get_input(uint8_t i);

void ndmx_init_outputs(void);

uint8_t ndmx_get_output(uint8_t id);

void ndmx_update_outputs(void);

#endif /* NDMX_H */
