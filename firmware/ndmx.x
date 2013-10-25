SECTIONS
{
  .eeprom :
  {
    *(.ndmx_config)
    *(.ndmx_prog)
  } > eeprom
}
