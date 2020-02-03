from enum import IntEnum, Enum

class sig_serial(Enum):
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end.

    """
    END_BYTE = "!"       # decimal for '!'
    START_BYTE = "@"    # decimal for '@'


class enum_commands(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """
    input_select = 0
    sample_rate = 1
    filter = 2
    corner_freq = 3
    fetch_samples = 4
    ping = 7
    START = 8
    STOP = 9


class enum_filter(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """
    LP1 = 0
    HP1 = 1
    BP2 = 2
    BP6 = 3
    HP6 = 4
    LP6 = 5
    NOTCH = 6
    NO_FILTER = 7


class enum_input(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """

    audio_in = 0
    analog_in = 1
    volt_1 = 2
    volt_3 = 3
    volt_5 = 4
    digital_i2c = 5
    digital_spi_bus = 6
    digital_uart = 7


class enum_output(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """

    xadc = 0
    digital = 1
