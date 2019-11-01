from enum import IntEnum


class f_filts(IntEnum):
    """
    This class defines filter mappings.

    @author: sdmay18-31
    """
    __order__ = 'LP1 LP5 HP1 HP5 BP2 BP6 NOTCH PASS'
    LP1 = 0
    LP5 = 1
    HP1 = 2
    HP5 = 3
    BP2 = 4
    BP6 = 5
    NOTCH = 6
    PASS = 7

    def __str__(self):
        """
        Returns the stringified version of this enum.

        Args:
            None

        Returns:
            String representing the enum data.
        """
        ret = "No Data"
        if(self == self.NOTCH):
            ret = "60 Hz Notch"
        elif(self == self.BP6):
            ret = "Sixth-Order Bandpass"
        elif(self == self.LP1):
            ret = "First-Order Lowpass"
        elif(self == self.LP5):
            ret = "Fifth-Order Lowpass"
        elif(self == self.BP2):
            ret = "Second-Order Bandpass"
        elif(self == self.HP5):
            ret = "Fifth-Order Highpass"
        elif(self == self.HP1):
            ret = "First-Order Highpass"
        elif(self == self.PASS):
            ret = "No Filter"

        return ret


class f_sensor_type(IntEnum):
    """
    This class defines types for sensors:
    Analog, I2C, or SPI.

    @author: sdmay18-31
    """
    ANALOG = 0
    I2C = 1
    SPI = 2
    NONE = 255


class f_sensors(IntEnum):
    """
    This class defines sensors. Add to this class when
    defining a new digital sensor. ADC0-2 can be used
    to identify analog channels.

    @author: sdmay18-31
    """
    ADC0 = 0        # Reserved for ADC_route
    ADC1 = 1        # Reserved for ADC_route
    ADC2 = 2        # Reserved for ADC_route
    IMU_ACCEL = 3
    IMU_GYRO = 4
    IMU_TEMP = 5
    NONE = 255

    def __str__(self):
        """
        Returns the stringified version of this enum.

        Args:
            None

        Returns:
            String representing the enum data.
        """
        ret = "NONE"

        if(self == self.ADC0):
            ret = "ADC0"
        elif(self == self.ADC1):
            ret = "ADC1"
        elif(self == self.ADC2):
            ret = "ADC2"
        elif(self == self.IMU_ACCEL):
            ret = "IMU_ACCEL"
        elif(self == self.IMU_GYRO):
            ret = "IMU_GYRO"
        elif(self == self.IMU_TEMP):
            ret = "IMU_TEMP"

        return ret

    def get_srate(self):
        """
        Returns sampling rate associated with the sensor associated
        with the enum.

        Args:
            None

        Returns:
            Integer representing the sampling rate.
        """
        ret = 0
        if(self == self.ADC0):
            ret = mdl_fw_vals().adc0_s_rate
        elif(self == self.ADC1):
            ret = mdl_fw_vals().adc1_s_rate
        elif(self == self.ADC2):
            ret = mdl_fw_vals().adc2_s_rate
        elif(self == self.IMU_ACCEL):
            ret = 100
        elif(self == self.IMU_GYRO):
            ret = 100
        elif(self == self.IMU_TEMP):
            ret = 100

        return ret

    def get_type(self):
        """
        Returns type of sensor the enum value represents.

        Args:
            None

        Returns:
            f_sensor_type, representing the type of sensor associated with
            the enum value.
        """
        ret = f_sensor_type.NONE
        if(self == self.ADC0):
            ret = f_sensor_type.ANALOG
        elif(self == self.ADC1):
            ret = f_sensor_type.ANALOG
        elif(self == self.ADC2):
            ret = f_sensor_type.ANALOG
        elif(self == self.IMU_ACCEL):
            ret = f_sensor_type.I2C
        elif(self == self.IMU_GYRO):
            ret = f_sensor_type.I2C
        elif(self == self.IMU_TEMP):
            ret = f_sensor_type.I2C

        return ret


class mdl_fw_vals:
    """
    This class defines a structure of configuration settings that shall
    be interpreted and sent to the firmware.

    This class is a singleton. Only one instance will be returned on a
    call to the constructor. Do not build classes that inherit from this
    class.

    @author: sdmay18-31
    """

    # Concrete instance of mdl_const
    __inst = None
    __init_complete = False

    def clear_filter(self, ach):
        """
        Used to clear filter settings associated with an analog channel

        Args:
            filt: Type of f_filts, indicating filter to clear

        Returns:
            None
        """
        if(self.lp_1st_route == ach):
            self.lp_1st_enabled = False
            self.lp_1st_route = f_sensors.NONE
            self.lp_1st_3db = 0

        if(self.lp_5th_route == ach):
            self.lp_5th_enabled = False
            self.lp_5th_route = f_sensors.NONE
            self.lp_5th_3db = 0

        if(self.hp_1st_route == ach):
            self.hp_1st_enabled = False
            self.hp_1st_route = f_sensors.NONE
            self.hp_1st_3db = 0
            self.hp_1st_dc_offset = 0

        if(self.hp_5th_route == ach):
            self.hp_5th_enabled = False
            self.hp_5th_route = f_sensors.NONE
            self.hp_5th_3db = 0
            self.hp_5th_dc_offset = 0

        if(self.bp_2nd_route == ach):
            self.bp_2nd_enabled = False
            self.bp_2nd_route = f_sensors.NONE
            self.bp_2nd_center = 0
            self.bp_2nd_dc_offset = 0

        if(self.bp_6th_route == ach):
            self.bp_6th_enabled = False
            self.bp_6th_route = f_sensors.NONE
            self.bp_6th_center = 0
            self.bp_6th_dc_offset = 0

        if(self.bp_60_route == ach):
            self.bp_60_enabled = False
            self.bp_60_route = f_sensors.NONE

        if(self.pass_thru_route == ach):
            self.pass_thru_enabled = False
            self.pass_thru_route = f_sensors.NONE

    def get_ach_data(self, ach):
        """
        Used to return the filter data associated with a given analog channel.

        Args:
            ach: Analog filter fetch filter data for

        Returns:
            Arg0: f_filts, indicates filter set for given channel
            Arg0: int, indicates filter frequency
            Arg0: int, indicates filter dc offset
        """
        if(self.lp_1st_enabled is True and self.lp_1st_route == ach):
            return f_filts.LP1, self.lp_1st_3db, 0

        if(self.lp_5th_enabled is True and self.lp_5th_route == ach):
            return f_filts.LP5, self.lp_5th_3db, 0

        if(self.hp_1st_enabled is True and self.hp_1st_route == ach):
            return f_filts.HP1, self.hp_1st_3db, self.hp_1st_dc_offset

        if(self.hp_5th_enabled is True and self.hp_5th_route == ach):
            return f_filts.HP5, self.hp_5th_3db, self.hp_5th_dc_offset

        if(self.bp_2nd_enabled is True and self.bp_2nd_route == ach):
            return f_filts.BP2, self.bp_2nd_center, self.bp_2nd_dc_offset

        if(self.bp_6th_enabled is True and self.bp_6th_route == ach):
            return f_filts.BP6, self.bp_6th_center, self.bp_6th_dc_offset

        if(self.bp_60_enabled is True and self.bp_60_route == ach):
            return f_filts.NOTCH, 0, 0

        if(self.pass_thru_enabled is True and self.pass_thru_route == ach):
            return f_filts.PASS, 0, 0

        return None, None, None

    def print(self):
        """
        Prints out current firmware settings. Used for debugging.
        """
        print("adc0_enabled: " + str(self.adc0_enabled))
        print("adc0_s_rate: " + str(self.adc0_s_rate))
        print("adc1_enabled: " + str(self.adc1_enabled))
        print("adc1_s_rate: " + str(self.adc1_s_rate))
        print("adc2_enabled: " + str(self.adc2_enabled))
        print("adc2_s_rate: " + str(self.adc2_s_rate))

        # Filter Settings
        # 1st Order Low Pass
        print("lp_1st_enabled: " + str(self.lp_1st_enabled))
        print("lp_1st_route: " + str(self.lp_1st_route))
        print("lp_1st_3db: " + str(self.lp_1st_3db))

        # 5th Order Low Pass
        print("lp_5th_enabled: " + str(self.lp_5th_enabled))
        print("lp_5th_route: " + str(self.lp_5th_route))
        print("lp_5th_3db: " + str(self.lp_5th_3db))

        # 1st Order High Pass
        print("hp_1st_enabled: " + str(self.hp_1st_enabled))
        print("hp_1st_route: " + str(self.hp_1st_route))
        print("hp_1st_3db: " + str(self.hp_1st_3db))
        print("hp_1st_dc_offset: " + str(self.hp_1st_dc_offset))

        # 5th Order High Pass
        print("hp_5th_enabled: " + str(self.hp_5th_enabled))
        print("hp_5th_route: " + str(self.hp_5th_route))
        print("hp_5th_3db: " + str(self.hp_5th_3db))
        print("hp_5th_dc_offset: " + str(self.hp_5th_dc_offset))

        # 2nd Order Band Pass
        print("bp_2nd_enabled: " + str(self.bp_2nd_enabled))
        print("bp_2nd_route: " + str(self.bp_2nd_route))
        print("bp_2nd_center: " + str(self.bp_2nd_center))
        print("bp_2nd_dc_offset: " + str(self.bp_2nd_dc_offset))

        # 6th Order Band Pass
        print("bp_6th_enabled: " + str(self.bp_6th_enabled))
        print("bp_6th_route: " + str(self.bp_6th_route))
        print("bp_6th_center: " + str(self.bp_6th_center))
        print("bp_6th_dc_offset: " + str(self.bp_6th_dc_offset))

        # 60 Hz Notch
        print("bp_60_enabled: " + str(self.bp_60_enabled))
        print("bp_60_route: " + str(self.bp_60_route))

        # Pass Through
        print("pass_thru_enabled: " + str(self.pass_thru_enabled))
        print("pass_thru_route: " + str(self.pass_thru_route))

        # Digital Sensors
        print("spi_enabled: " + str(self.spi_enabled))
        print("spi_sensor: " + str(self.spi_sensor))
        print("i2c_enabled: " + str(self.i2c_enabled))
        print("i2c_sensor: " + str(self.i2c_sensor))

    def reset_vals(self):
        """
        Resets firmware settings to default values

        Args:
            None

        Returns:
            None
        """
        # ADC Channel Settings
        # Note ensure that the total sampling rate is constrained before
        # passing to sig_logger()
        self.adc0_enabled = False
        self.adc0_s_rate = 0
        self.adc1_enabled = False
        self.adc1_s_rate = 0
        self.adc2_enabled = False
        self.adc2_s_rate = 0

        # Filter Settings
        # 1st Order Low Pass
        self.lp_1st_enabled = False
        self.lp_1st_route = f_sensors.NONE
        self.lp_1st_3db = 0

        # 5th Order Low Pass
        self.lp_5th_enabled = False
        self.lp_5th_route = f_sensors.NONE
        self.lp_5th_3db = 0

        # 1st Order High Pass
        self.hp_1st_enabled = False
        self.hp_1st_route = f_sensors.NONE
        self.hp_1st_3db = 0
        self.hp_1st_dc_offset = 0

        # 5th Order High Pass
        self.hp_5th_enabled = False
        self.hp_5th_route = f_sensors.NONE
        self.hp_5th_3db = 0
        self.hp_5th_dc_offset = 0

        # 2nd Order Band Pass
        self.bp_2nd_enabled = False
        self.bp_2nd_route = f_sensors.NONE
        self.bp_2nd_center = 0
        self.bp_2nd_dc_offset = 0

        # 6th Order Band Pass
        self.bp_6th_enabled = False
        self.bp_6th_route = f_sensors.NONE
        self.bp_6th_center = 0
        self.bp_6th_dc_offset = 0

        # 60 Hz Notch
        self.bp_60_enabled = False
        self.bp_60_route = f_sensors.NONE

        # Pass Through
        self.pass_thru_enabled = False
        self.pass_thru_route = f_sensors.NONE

        # Digital Sensors
        self.spi_enabled = False
        self.spi_sensor = f_sensors.NONE
        self.i2c_enabled = False
        self.i2c_sensor = f_sensors.NONE

    def validate(self):
        """
        Confirms that current firmware settings are OK to send
        to the CyDAQ box.

        Args:
            None

        Returns:
            True if firmware settings are OK, False otherwise
        """
        if((self.adc0_enabled is False) and
           (self.adc1_enabled is False) and
           (self.adc2_enabled is False) and
           (self.spi_enabled is False) and
           (self.i2c_enabled is False)):
            return False

        if((self.spi_enabled is True) and
           (self.spi_sensor == f_sensors.NONE)):
            return False

        if((self.i2c_enabled is True) and
           (self.i2c_sensor == f_sensors.NONE)):
            return False

        filt, freq, offset = self.get_ach_data(f_sensors.ADC0)
        if((self.adc0_enabled is True) and
           (filt is None)):
            return False

        filt, freq, offset = self.get_ach_data(f_sensors.ADC1)
        if((self.adc1_enabled is True) and
           (filt is None)):
            return False

        filt, freq, offset = self.get_ach_data(f_sensors.ADC2)
        if((self.adc2_enabled is True) and
           (filt is None)):
            return False

        return True

    def copy(self, other):
        """
        Copy ctor
        """
        mdl_fw_vals.__inst = other

    def __init__(self):
        if mdl_fw_vals.__init_complete is True:
            return

        # Use reset_vals() to declare and initialize firmware values
        self.reset_vals()

        mdl_fw_vals.__init_complete = True

    def __new__(cls):

        if mdl_fw_vals.__inst is None:
            mdl_fw_vals.__inst = object.__new__(cls)

        return mdl_fw_vals.__inst
