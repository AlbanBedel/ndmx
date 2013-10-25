#ifndef COMPUTE_H
#define COMPUTE_H

#include <stdint.h>
#include <stdarg.h>

#include "utils.h"

#ifndef NANO_VM_MAX_STACK_SIZE
#define NANO_VM_MAX_STACK_SIZE 8
#endif

typedef uint16_t nano_vm_ptr_t;
typedef int16_t  nano_vm_off_t;

typedef int16_t  nano_vm_int_t;
typedef uint16_t nano_vm_uint_t;

struct nano_vm {
    uint8_t sp;
    nano_vm_ptr_t pc;
    nano_vm_int_t stack[NANO_VM_MAX_STACK_SIZE];
};

// Calls use the low 7 bits for ID
#define OP_TYPE_CALL_MASK	0x80
// Extended ops use the low 4 bits for ID
#define OP_TYPE_EXTENDED_MASK	0xF0
// Short ops use the low 3 bits for args
#define OP_TYPE_MASK		0xF8
#define OP_ARGS_MASK		0x07

#define OP_TYPE_ID(n_)		( (n_) << 3)

// Call an external function
#define OP_TYPE_CALL		OP_TYPE_ID(0x10)

// Call an extended op, the low 4 bits are the extended op id
#define OP_TYPE_EXTENDED	OP_TYPE_ID(0xE)

// Jump use the low 3 bit for flags
#define OP_JMP_LONG		BIT(0)
#define OP_JMP_NOT		BIT(1)
#define OP_JMP_COND		BIT(2)
#define OP_TYPE_JMP		OP_TYPE_ID(0xD)

// Pop use the low 3 bits for count-1
#define OP_TYPE_POP		OP_TYPE_ID(0xC)

// All the other ops use the low 3 bits for data type:
// stack, const low 8 bits, const high 8 bits, const 16 bits,
// or immediate value from -1 to 2
#define OP_ARG_IMM(x)		((x) & 3)
#define OP_ARG_STACK		4
#define OP_ARG_CONST_8L		5
#define OP_ARG_CONST_8H		6
#define OP_ARG_CONST_16		7

#define OP_TYPE_PUSH		OP_TYPE_ID(0xB)
#define OP_TYPE_SAR		OP_TYPE_ID(0xA)
#define OP_TYPE_SHR		OP_TYPE_ID(0x9)
#define OP_TYPE_SHL		OP_TYPE_ID(0x8)
#define OP_TYPE_GTE		OP_TYPE_ID(0x7)
#define OP_TYPE_GT		OP_TYPE_ID(0x6)
#define OP_TYPE_EQ		OP_TYPE_ID(0x5)
#define OP_TYPE_DIV		OP_TYPE_ID(0x4)
#define OP_TYPE_MUL		OP_TYPE_ID(0x3)
#define OP_TYPE_SUB		OP_TYPE_ID(0x2)
#define OP_TYPE_ADD		OP_TYPE_ID(0x1)
#define OP_TYPE_RETURN		OP_TYPE_ID(0x0)

// The extended ops have no argument
#define OP_TYPE_EXTENDED_ID(x)	(OP_TYPE_EXTENDED | ((x) & 0xF))
#define OP_TYPE_XCHG		OP_TYPE_EXTENDED_ID(0)
#define OP_TYPE_NEG		OP_TYPE_EXTENDED_ID(1)
#define OP_TYPE_NOT		OP_TYPE_EXTENDED_ID(2)

/*
 * Macros to read the opcodes
 */

#define OP_IS_CALL(op)		((op) & OP_TYPE_CALL)
#define OP_CALL_ID(op)		((op) & ~OP_TYPE_CALL)

#define OP_IS_EXTENDED(op)	\
    (((op) & OP_TYPE_EXTENDED_MASK) == OP_TYPE_EXTENDED)

#define OP_TYPE(x)		((x) & OP_TYPE_MASK)
#define OP_ARGS(x)		((x) & OP_ARGS_MASK)

#define OP_HAS_INT_ARG(op)	(OP_TYPE(op) < OP_TYPE_POP)

#define OP_ARG_IS_IMM(op)	(((op) & 0x04) == 0)

/*
 * Macros to generate code
 */

#define DECL_OP(n, arg)		(CAT2(OP_TYPE_,n) | (arg))
#define DECL_OP_ARG(n,arg)	CAT2(OP_TYPE_,n) | CAT2(DECL_ARG_,arg)

#define DECL_ARG_IMM(x)		OP_ARG_IMM(x)
#define DECL_ARG_STACK		OP_ARG_STACK
#define DECL_ARG_CONST_8L(x)	OP_ARG_CONST_8L, ((x) & 0xFF)
#define DECL_ARG_CONST_8H(x)	OP_ARG_CONST_8H, ((x) & 0xFF)
#define DECL_ARG_CONST_16(x)	OP_ARG_CONST_16, \
        (((x) >> 8) & 0xFF), ((x) & 0xFF)

#define OP_CALL(arg)		DECL_OP(CALL, (arg) & 0x7F)

#define OP_JMP			DECL_OP(JMP, 0)
#define OP_JMP_IF		DECL_OP(JMP, OP_JMP_COND)
#define OP_JMP_IF_NOT		DECL_OP(JMP, OP_JMP_COND | OP_JMP_NOT)

#define OP_JMPL			(OP_JMP | OP_JMP_LONG)
#define OP_JMPL_IF		(OP_JMP_IF | OP_JMP_LONG)
#define OP_JMPL_IF_NOT		(OP_JMP_IF_NOT | OP_JMP_LONG)

#define OP_POPN(arg)		DECL_OP(POP, (arg) & 0x03)
#define OP_POP			OP_POPN(0)
#define OP_PUSH(arg)		DECL_OP_ARG(PUSH, arg)

#define OP_RETURN(arg)		DECL_OP_ARG(RETURN, arg)

#define OP_ADD(arg)		DECL_OP_ARG(ADD, arg)
#define OP_SUB(arg)		DECL_OP_ARG(SUB, arg)
#define OP_MUL(arg)		DECL_OP_ARG(MUL, arg)
#define OP_DIV(arg)		DECL_OP_ARG(DIV, arg)

#define OP_EQ(arg)		DECL_OP_ARG(EQ, arg)
#define OP_GT(arg)		DECL_OP_ARG(GT, arg)
#define OP_GTE(arg)		DECL_OP_ARG(GTE, arg)

#define OP_SHL(arg)		DECL_OP_ARG(SHL, arg)
#define OP_SHR(arg)		DECL_OP_ARG(SHR, arg)
#define OP_SAR(arg)		DECL_OP_ARG(SAR, arg)

#define OP_XCHG			DECL_OP(XCHG, 0)
#define OP_NEG			DECL_OP(NEG, 0)
#define OP_NOT			DECL_OP(NOT, 0)

// op with out immediate args

#define OP_RETURN_ERROR		OP_RETURN(IMM(-1))

/* Read program data and move the PC.
 * The application must provide this funtion to allow the vm to
 * read its code. */
uint8_t nano_vm_r8(struct nano_vm *vm);
/* Read a 16 bit value from the code.
 * The default implementation use nano_vm_r8(), but the application
 * may provide its own optimized implementation. */
int16_t nano_vm_r16(struct nano_vm *vm);

/* Stack manipulation */
void nano_vm_push(struct nano_vm *vm, nano_vm_int_t val);
nano_vm_int_t nano_vm_pop(struct nano_vm *vm);

/* Run a single cycle */
int8_t nano_vm_run_once(struct nano_vm *vm);
/* Run a whole program */
int8_t nano_vm_run(struct nano_vm *vm);

/* Reset the VM to run another program */
void nano_vm_reset(struct nano_vm *vm);

/* The application can implement this function to extend the VM
 * with custom functions. */
int8_t nano_vm_call(struct nano_vm *vm, uint8_t id);

#endif /* COMPUTE_H */
