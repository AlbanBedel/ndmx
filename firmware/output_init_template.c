#if OUTPUT_ID < OUTPUT_COUNT
case OUTPUT_ID:
{
    pin_direction_output(OUTPUT_PORT, OUTPUT_PIN, OUTPUT_IS_LOW_ACTIVE);
    return 0;
}
#endif
