CyDAQ-01: 1st order LP, F_c = 2kHz
CyDAQ-02: 1st order HP, F_c = 8kHz
CyDAQ-03: 2nd order BP, F_cH = 5kHz and F_cL = 15kHz
CyDAQ-04: 2nd order BP, F_cH = 1kHz and F_cL = 10kHz
CyDAQ-05: 1st order HP, F_c = 22kHz
CyDAQ-06: 2nd order BP, F_cH = 20kHz and F_cL = 30kHz
CyDAQ-07: 1st order LP, F_c = 11kHz

These config files have the above configurations. To use, place on top level on sd card (with a BOOT.BIN)
file, and rename to 'config.json'. It will not work with any other file name.

Currently only corners, filter, and input selection. If using a one corner filter, make second value
in corner array 0. All config files must have same object hierarchy, i.e., strings for keys
and primitives for values. Corners key MUST have array as value. Other values should follow
enums in shared_definitions.h of firmware. Sample rate could be supported if sd_config library
was updated with support.