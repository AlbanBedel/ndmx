#include <stdlib.h>
#include <stdint.h>

#include "utils.h"
#include "errno.h"
#include "nano_vm.h"

uint8_t __attribute__((weak)) nano_vm_r8(struct nano_vm *vm)
{
    return OP_RETURN_ERROR;
}

int16_t __attribute__((weak)) nano_vm_r16(struct nano_vm *vm)
{
    return (((int16_t)nano_vm_r8(vm)) << 8) | nano_vm_r8(vm);
}

void nano_vm_push(struct nano_vm *vm, nano_vm_int_t val)
{
    if (vm->sp < ARRAY_SIZE(vm->stack))
        vm->stack[vm->sp++] = val;
}

nano_vm_int_t nano_vm_pop(struct nano_vm *vm)
{
    return (vm->sp > 0) ? vm->stack[--vm->sp] : (nano_vm_int_t)0;
}

int8_t __attribute__((weak))
nano_vm_call(struct nano_vm *vm, uint8_t id)
{
    return -ENOENT;
}

static int8_t nano_vm_run_op_extended(struct nano_vm *vm, uint8_t op)
{
    nano_vm_int_t a, b;

    switch (op) {
    case OP_TYPE_XCHG:
        b = nano_vm_pop(vm);
        a = nano_vm_pop(vm);
        nano_vm_push(vm, b);
        nano_vm_push(vm, a);
        return 1;

    case OP_TYPE_NEG:
        nano_vm_push(vm, -nano_vm_pop(vm));
        return 1;
    case OP_TYPE_NOT:
        nano_vm_push(vm, !nano_vm_pop(vm));
        return 1;

    default:
        return -EINVAL;
    }
}

static int8_t nano_vm_run_op(struct nano_vm *vm, uint8_t op, uint8_t op_arg)
{
    nano_vm_int_t a, b, ret;

    // Handle the OPs with specific argument types

    // Jump encode various types in the argument
    if (op == OP_TYPE_JMP) {
        nano_vm_off_t off;
        if (op_arg & OP_JMP_LONG)
            off = nano_vm_r16(vm);
        else
            off = nano_vm_r8(vm);
        if (op_arg & OP_JMP_COND) {
            nano_vm_int_t v = nano_vm_pop(vm);
            if (op_arg & OP_JMP_NOT)
                v = !v;
            if (!v)
                return 1;
        }
        vm->pc += off;
        return 1;
    }

    // Pop encode the number of elements to pop, minus 1
    if (op == OP_TYPE_POP) {
        do
            nano_vm_pop(vm);
        while (op_arg-- > 0);
        return 1;
    }

    // Handle the OPs using the extended argument type

    // Read the argument
    if (op_arg == OP_ARG_STACK) {
        b = nano_vm_pop(vm);
        // Pushing from the stack should duplicate the element
        if (op == OP_TYPE_PUSH)
            vm->sp += 1;
    } else if (op_arg == OP_ARG_CONST_8L)
        b = nano_vm_r8(vm);
    else if (op_arg == OP_ARG_CONST_8H)
        b = ((uint16_t)nano_vm_r8(vm)) << 8;
    else if (op_arg == OP_ARG_CONST_16)
        b = nano_vm_r16(vm);
    else {
        // immediate value from -1 to 2
        if (op_arg == 3)
            b = -1;
        else
            b = OP_ARG_IMM(op_arg);
    }

    // Push and return both push 'b' on the stack
    if (op == OP_TYPE_RETURN || op == OP_TYPE_PUSH) {
        nano_vm_push(vm, b);
        return op != OP_TYPE_RETURN;
    }

    // Handle the binary operators
    a = nano_vm_pop(vm);
    switch(op) {
    case OP_TYPE_ADD:
        ret = a + b;
        break;
    case OP_TYPE_SUB:
        ret = a - b;
        break;

    case OP_TYPE_MUL:
        ret = a * b;
        break;
    case OP_TYPE_DIV:
        ret = a / b;
        break;

    case OP_TYPE_EQ:
        ret = (a == b);
        break;
    case OP_TYPE_GT:
        ret = (a > b);
        break;
    case OP_TYPE_GTE:
        ret = (a >= b);
        break;

    case OP_TYPE_SHL:
        ret = (nano_vm_uint_t)a << (uint8_t)b;
        break;
    case OP_TYPE_SHR:
        ret = (nano_vm_uint_t)a >> (uint8_t)b;
        break;
    case OP_TYPE_SAR:
        ret = a >> (uint8_t)b;
        break;

    default:
        // Shouldn't happen
        return -EINVAL;
    }

    nano_vm_push(vm, ret);
    return 1;
}

int8_t nano_vm_run_once(struct nano_vm *vm)
{
    uint8_t op = nano_vm_r8(vm);

    if (OP_IS_CALL(op))
        return nano_vm_call(vm, OP_CALL_ID(op));

    if (OP_IS_EXTENDED(op))
        return nano_vm_run_op_extended(vm, op);

    return nano_vm_run_op(vm, OP_TYPE(op), OP_ARGS(op));
}

int8_t nano_vm_run(struct nano_vm *vm)
{
    int run = 1;
    while (run)
        run = nano_vm_run_once(vm);
    return run;
}

void nano_vm_reset(struct nano_vm *vm)
{
    vm->sp = 0;
    vm->pc = 0;
}
