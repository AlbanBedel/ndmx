#include <stdlib.h>

#include "errno.h"
#include "utils.h"

#include "ndmx.h"
#include "pwm.h"
#include "pin.h"
#include "board_config.h"

struct output {
    uint8_t	value;
};

static struct output outputs[OUTPUT_COUNT];

/* Defines for the code templates */
#define OUTPUT_IS_LOW_ACTIVE	CAT3(OUTPUT_,OUTPUT_ID,_IS_LOW_ACTIVE)
#define OUTPUT_PORT		CAT3(OUTPUT_,OUTPUT_ID,_PORT)
#define OUTPUT_PIN		CAT3(OUTPUT_,OUTPUT_ID,_PIN)
#define OUTPUT_HAS_PWM		CAT3(OUTPUT_,OUTPUT_ID,_HAS_PWM)
#define OUTPUT_TIMER		CAT3(OUTPUT_,OUTPUT_ID,_TIMER)
#define OUTPUT_CHANNEL		CAT3(OUTPUT_,OUTPUT_ID,_CHANNEL)

static int8_t ndmx_set_output(uint8_t id, uint8_t val)
{
    uint16_t old_value;

    if (id >= ARRAY_SIZE(outputs))
        return -EINVAL;

    old_value = outputs[id].value;
    if (old_value == val)
        return 0;

    outputs[id].value = val;
    switch (id) {
#   undef  OUTPUT_ID
#   define OUTPUT_ID	0
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	1
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	2
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	3
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	4
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	5
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	6
#       include "output_set_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	7
#       include "output_set_template.c"
#   if OUTPUT_COUNT > OUTPUT_ID
#       warning "Not all outputs supported yet, extend the list!"
#   endif
#   undef OUTPUT_ID
    default:
        return -ENODEV;
    }
}

static int8_t ndmx_update_output(uint8_t id)
{
    ndmx_set_output(id, ndmx_get_input(id));
    return 0;
}

void ndmx_update_outputs(void)
{
    uint8_t i;

    for (i = 0 ; i < ARRAY_SIZE(outputs) ; i += 1)
        ndmx_update_output(i);
}

static int8_t ndmx_init_output(uint8_t id)
{
    if (id >= ARRAY_SIZE(outputs))
        return -EINVAL;

    switch (id) {
#   undef  OUTPUT_ID
#   define OUTPUT_ID	0
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	1
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	2
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	3
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	4
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	5
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	6
#       include "output_init_template.c"
#   undef  OUTPUT_ID
#   define OUTPUT_ID	7
#       include "output_init_template.c"
#   if OUTPUT_COUNT > OUTPUT_ID
#       warning "Not all outputs supported yet, extend the list!"
#   endif
#   undef OUTPUT_ID
    default:
        return -ENODEV;
    }
}

void ndmx_init_outputs(void)
{
    uint8_t i;

    for (i = 0 ; i < ARRAY_SIZE(outputs) ; i += 1)
        ndmx_init_output(i);
}
