#ifndef PWM_H
#define PWM_H

#include <stdint.h>
#include <avr/io.h>

#define TIMER_TYPE(tmr)	CAT3(TIMER_,tmr,_TYPE)
#define TIMER(tmr)	((volatile uint8_t *)_SFR_ADDR(CAT3(TCCR,tmr,A)))
#define TIMER_OCR(tmr)	((volatile uint8_t *)_SFR_ADDR(CAT3(OCR,tmr,A)))

// Helper macros to access the PWMs with having to worry about the
// type of the underlying hardware. Note that the timer must be a
// compile time constant.

#define pwm_enable(tmr, chan, invert) \
  CAT3(pwm_,TIMER_TYPE(tmr),_enable)(TIMER(tmr), chan, invert)

#define pwm_disable(tmr, chan) \
  CAT3(pwm_,TIMER_TYPE(tmr),_disable)(TIMER(tmr), chan)

#define pwm_set_duty(tmr, chan, duty) \
  CAT3(pwm_,TIMER_TYPE(tmr),_set_duty)(TIMER_OCR(tmr), chan, duty)

// Note: The code here doesn't use any symbolic names for the register
// access. This is because the symbolic names would just be A, B, C, D
// not improving things much.
static inline void
pwm_8bit_set_com(volatile uint8_t *tccr,
                 uint8_t channel, uint8_t com)
{
    uint8_t sh = 6 - (channel<<1);
    tccr[0] = (tccr[0] & ~(3 << sh)) | ((com & 3) << sh);
}

static inline void
pwm_8bit_set_cs(volatile uint8_t *tccr, uint8_t cs)
{
    if ((tccr[1] & 7) != cs)
        tccr[1] = (tccr[1] & ~7) | cs;
}

static inline void
pwm_8bit_enable(volatile uint8_t *tccr, uint8_t channel,
                uint8_t inverted)
{
    // Set WGM
    if ((tccr[0] & 3) != 3)
        tccr[0] |= 0x3;
    if (tccr[1] & BIT(3))
        tccr[1] &= ~BIT(3);
    // Set COM & CS
    pwm_8bit_set_com(tccr, channel, inverted ? 3 : 2);
    pwm_8bit_set_cs(tccr, 1);
}

static inline void
pwm_8bit_disable(volatile uint8_t *tccr, uint8_t channel)
{
    pwm_8bit_set_com(tccr, channel, 0);
}

static inline void
pwm_8bit_set_duty(volatile uint8_t *ocr, uint8_t channel,
                  uint8_t val)
{
    ocr[channel] = val;
}


static inline void
pwm_16bit_enable(volatile uint8_t *tccr, uint8_t channel,
                 uint8_t inverted)
{
    // Set WGM
    if ((tccr[0] & 3) != 1)
        tccr[0] = (tccr[0] & ~3) | 1;
    if ((tccr[1] & 0x18) != 0x8)
        tccr[1] = (tccr[1] & ~0x18) | 0x8;
    // Set COM & CS
    pwm_8bit_set_com(tccr, channel, inverted ? 3 : 2);
    pwm_8bit_set_cs(tccr, 1);
}

static inline void
pwm_16bit_disable(volatile uint8_t *tccr, uint8_t channel)
{
    pwm_8bit_set_com(tccr, channel, 0);
}

static inline void
pwm_16bit_set_duty(volatile uint8_t *ocr, uint8_t channel,
                   uint8_t val)
{
    ((volatile uint16_t *)ocr)[channel] = val;
}

static inline void
pwm_10bit_set_com(volatile uint8_t *tccr,
                  uint8_t channel, uint8_t com)
{
    if (channel < 2) {
        pwm_8bit_set_com(tccr, channel, com);
        if (com)
            tccr[0] |= (2 >> channel);
        else
            tccr[0] &= ~(2 >> channel);
    } /* Channel 2 has no output */
    else if (channel == 3) {
        tccr[2] = (tccr[2] & ~0xC) | (com << 2);
        if (com)
            tccr[2] |= 1;
        else
            tccr[2] &= ~1;
    }
}

static inline void
pwm_10bit_set_cs(volatile uint8_t *tccr, uint8_t cs)
{
    if ((tccr[1] & 0xF) != cs)
        tccr[1] = (tccr[1] & ~0xF) | cs;
}

static inline void
pwm_10bit_enable(volatile uint8_t *tccr, uint8_t channel,
                 uint8_t inverted)
{
    // Set WGM
    if ((tccr[4] & 3) != 0)
        tccr[4] = (tccr[0] & ~3) | 1;
    // Set COM & CS
    pwm_10bit_set_com(tccr, channel, inverted ? 3 : 2);
    pwm_10bit_set_cs(tccr, 1);
}

static inline void
pwm_10bit_disable(volatile uint8_t *tccr, uint8_t channel)
{
    pwm_10bit_set_com(tccr, channel, 0);
}

static inline void
pwm_10bit_set_duty(volatile uint8_t *ocr, uint8_t channel,
                   uint8_t val)
{
    ((volatile uint16_t *)ocr)[channel] = val;
}

/*
 * Timer types definitions
 */

#define IS_10BIT_TIMER(n)	CAT3(CS, n, 3)
#define IS_16BIT_TIMER(n)	CAT3(WGM, n, 3)
#define IS_8BIT_TIMER(n)	CAT3(WGM, n, 1)

#if IS_10BIT_TIMER(0)
#    define TIMER_0_TYPE		10bit
#elif IS_16BIT_TIMER(0)
#    define TIMER_0_TYPE		16bit
#elif IS_8BIT_TIMER(0)
#    define TIMER_0_TYPE		8bit
#endif

#if IS_10BIT_TIMER(1)
#    define TIMER_1_TYPE		10bit
#elif IS_16BIT_TIMER(1)
#    define TIMER_1_TYPE		16bit
#elif IS_8BIT_TIMER(1)
#    define TIMER_1_TYPE		8bit
#endif

#if IS_10BIT_TIMER(2)
#    define TIMER_2_TYPE		10bit
#elif IS_16BIT_TIMER(2)
#    define TIMER_2_TYPE		16bit
#elif IS_8BIT_TIMER(2)
#    define TIMER_2_TYPE		8bit
#endif

#if IS_10BIT_TIMER(3)
#    define TIMER_3_TYPE		10bit
#elif IS_16BIT_TIMER(3)
#    define TIMER_3_TYPE		16bit
#elif IS_8BIT_TIMER(3)
#    define TIMER_3_TYPE		8bit
#endif

#if IS_10BIT_TIMER(4)
#    define TIMER_4_TYPE		10bit
#elif IS_16BIT_TIMER(4)
#    define TIMER_4_TYPE		16bit
#elif IS_8BIT_TIMER(4)
#    define TIMER_4_TYPE		8bit
#endif

#if IS_10BIT_TIMER(5)
#    define TIMER_5_TYPE		10bit
#elif IS_16BIT_TIMER(5)
#    define TIMER_5_TYPE		16bit
#elif IS_8BIT_TIMER(5)
#    define TIMER_5_TYPE		8bit
#endif

#if IS_10BIT_TIMER(6)
#    define TIMER_6_TYPE		10bit
#elif IS_16BIT_TIMER(6)
#    define TIMER_6_TYPE		16bit
#elif IS_8BIT_TIMER(6)
#    define TIMER_6_TYPE		8bit
#endif

#if IS_10BIT_TIMER(7)
#    define TIMER_7_TYPE		10bit
#elif IS_16BIT_TIMER(7)
#    define TIMER_7_TYPE		16bit
#elif IS_8BIT_TIMER(7)
#    define TIMER_7_TYPE		8bit
#endif

#endif /* PWM_H */
