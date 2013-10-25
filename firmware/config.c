#include "ndmx.h"
#include "nano_vm.h"

const uint8_t
__attribute__((weak))
ndmx_default_prog[] = {
    OP_CALL(FUNC_GET_ID),
    OP_CALL(FUNC_GET_INPUT),
    OP_RETURN(STACK),
};

const struct ndmx_config
__attribute__((weak))
ndmx_config = {
    .entry = {
#if OUTPUT_COUNT > 0
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 1
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 2
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 3
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 4
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 5
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 6
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 7
        ndmx_default_prog,
#endif
#if OUTPUT_COUNT > 8
        ndmx_default_prog,
#endif
    },
};
