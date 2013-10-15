/* MCU */
#define MCU			atmega32u4
#define F_CPU			16000000

/* Inputs */
#define DMX_CHANNEL_COUNT	4
#define DMX_RECV_PORT		D
#define DMX_RECV_PIN		2

#define BUTTON_COUNT		0

#define INPUT_COUNT		(DMX_CHANNEL_COUNT+BUTTON_COUNT)

/* Outputs */
#define OUTPUT_0_IS_LOW_ACTIVE	0
#define OUTPUT_0_PORT		C
#define OUTPUT_0_PIN		7

#define OUTPUT_0_HAS_PWM	1
#define OUTPUT_0_TIMER		4
#define OUTPUT_0_CHANNEL	0

#define OUTPUT_1_IS_LOW_ACTIVE	0
#define OUTPUT_1_PORT		D
#define OUTPUT_1_PIN		5

#define OUTPUT_COUNT		2
