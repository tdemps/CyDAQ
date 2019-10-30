from enum import IntEnum


class d_ftype(IntEnum):
    """
    This class defines enumerations that represent digital filter types.

    @author: sdmay18-31
    """

    LPF = 0
    HPF = 1
    BPF = 2
    BSF = 3
    NONE = 255


class d_fproto(IntEnum):
    """
    This Class defines enumerations that represent digital filter prototypes.

    @author: sdmay18-31
    """

    BUTTER = 0
    CHEBY1 = 1
    CHEBY2 = 2


class d_fir_iir(IntEnum):
    """
    This Class defines the enumerations that determine which digital filter
    variable is being used.

    @author: sdmay18-31
    """

    NONE = 0
    FIR = 1


class dat_id_enum(IntEnum):
    """
    This Class defines the enumerations that determine which data variable
    is being filtered.
    """
    __order__ = 'NONE ADC0 ADC1 ADC2 I2C SPI'
    NONE = 0
    ADC0 = 1
    ADC1 = 2
    ADC2 = 3
    I2C = 4
    SPI = 5
