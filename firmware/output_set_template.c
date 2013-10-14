#if OUTPUT_ID < OUTPUT_COUNT
case OUTPUT_ID:
{
#if OUTPUT_HAS_PWM
    if (val > 0) {
        pwm_set_duty(OUTPUT_TIMER, OUTPUT_CHANNEL, val);
        if (old_value == 0)
            pwm_enable(OUTPUT_TIMER, OUTPUT_CHANNEL, OUTPUT_IS_LOW_ACTIVE);
    } else
        pwm_disable(OUTPUT_TIMER, OUTPUT_CHANNEL);
#else
    val = (val >= 0x80);
    if (OUTPUT_IS_LOW_ACTIVE)
        val = !val;
    pin_set_value(OUTPUT_PORT, OUTPUT_PIN, val);
#endif
    return 0;
}
#endif
