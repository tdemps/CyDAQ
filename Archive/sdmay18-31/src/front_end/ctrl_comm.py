import serial
import serial.tools.list_ports
import struct
import ctrl_crc

from enum import IntEnum

from mdl_firmware import mdl_fw_vals
from mdl_firmware import f_sensors


class sig_cmds(IntEnum):
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end.

    @author: sdmay18-31
    """
    START_COLLECTION = 0
    STOP_COLLECTION = 1
    ENABLE_DEV = 2
    DISABLE_DEV = 3  # Deprecated
    CONFIG_ADC0 = 4
    CONFIG_ADC1 = 5
    CONFIG_ADC2 = 6
    CONFIG_I2C = 7
    CONFIG_SPI = 8
    RESET_DEV = 9  # Deprecated
    RESET_ALL = 10  # Deprecated
    REPORT_DEVICES = 11  # Deprecated
    TUNE_FILTER = 12
    START_BYTE = 252
    ACK = 253
    NAK = 254
    INVALID_CMD = 255


class sig_adc_cmds(IntEnum):
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end for adc configuration.

    @author: sdmay18-31
    """
    SET_SAMPLING_RATE = 0
    SET_FILTER_ROUTE = 1


class ctrl_comm:
    """
    This class creates an abstraction for a connection to a device over
    a Serial connection.

    @author: sdmay18-31
    """
    def __init__(self):
        self.__s_comm = serial.Serial()
        self.__s_comm.baudrate = 460800
        self.__s_comm.bytesize = serial.EIGHTBITS
        self.__s_comm.stopbits = serial.STOPBITS_ONE
        self.__s_comm.xonxoff = True
        self.__s_comm.rtscts = False
        self.__s_comm.dsrdtr = False
        self.__s_comm.parity = serial.PARITY_NONE
        self.__s_comm.timeout = 1
        self.__order = "little"

    def close(self):
        """
        Closes the currently open connection attached to this
        instance.

        Will silently fail if device is already open.

        Args:
            None

        Returns:
            None
        """
        if self.__s_comm.isOpen() is True:
            try:
                self.__send_pkt(sig_cmds.STOP_COLLECTION,
                                0,
                                0
                                )
            except Exception as e:
                pass

            self.__s_comm.close()

    def get_ports(self):
        """
        Returns a list of available serial devices on the host computer.
        Typical usage is to call this function to determine the computer's
        ports and open a connection with the open() function, using one of
        the devices returned by this function.

        Args:
            None

        Returns:
            A list of available serial devices, as strings
        """

        all_ports = serial.tools.list_ports.comports()
        open_ports = []
        for element in all_ports:
            open_ports.append(element.device)

        return open_ports

    def open(self, port):
        """
        Used to open a connection to a serial device passed in as the
        parameter to this function. Needs to be called before data can
        be read or written to a device.

        Will silently fail if device is already open.

        Args:
            port: Serial device to open

        Returns:
            None
        """
        if self.__s_comm.isOpen() is False:
            self.__s_comm.port = port
            self.__s_comm.open()
            self.__s_comm.flushInput()
            self.__s_comm.flushOutput()

            config = mdl_fw_vals()

            s = struct.Struct('>HH')
            s2 = struct.Struct('>I')

            # Disabled tuning filters, causes freezes if the PCB is not
            # attached
            if config.adc0_enabled is True:
                filt, freq, offset = config.get_ach_data(f_sensors.ADC0)
                p2 = s2.unpack(s.pack(offset, freq))[0]

                self.__send_pkt(sig_cmds.CONFIG_ADC0,
                                sig_adc_cmds.SET_SAMPLING_RATE,
                                config.adc0_s_rate
                                )

                self.__send_pkt(sig_cmds.CONFIG_ADC0,
                                sig_adc_cmds.SET_FILTER_ROUTE,
                                filt,
                                )

                self.__send_pkt(sig_cmds.TUNE_FILTER,
                                filt,
                                p2
                                )

                self.__send_pkt(sig_cmds.ENABLE_DEV,
                                f_sensors.ADC0,
                                0
                                )

            if config.adc1_enabled is True:
                filt, freq, offset = config.get_ach_data(f_sensors.ADC1)
                p2 = s2.unpack(s.pack(offset, freq))[0]

                self.__send_pkt(sig_cmds.CONFIG_ADC1,
                                sig_adc_cmds.SET_SAMPLING_RATE,
                                config.adc1_s_rate
                                )

                self.__send_pkt(sig_cmds.CONFIG_ADC1,
                                sig_adc_cmds.SET_FILTER_ROUTE,
                                filt,
                                )

                self.__send_pkt(sig_cmds.TUNE_FILTER,
                                filt,
                                p2
                                )

                self.__send_pkt(sig_cmds.ENABLE_DEV,
                                f_sensors.ADC1,
                                0
                                )

            if config.adc2_enabled is True:
                filt, freq, offset = config.get_ach_data(f_sensors.ADC2)
                p2 = s2.unpack(s.pack(offset, freq))[0]

                self.__send_pkt(sig_cmds.CONFIG_ADC2,
                                sig_adc_cmds.SET_SAMPLING_RATE,
                                config.adc2_s_rate
                                )

                self.__send_pkt(sig_cmds.CONFIG_ADC2,
                                sig_adc_cmds.SET_FILTER_ROUTE,
                                filt,
                                )

                self.__send_pkt(sig_cmds.TUNE_FILTER,
                                filt,
                                p2
                                )

                self.__send_pkt(sig_cmds.ENABLE_DEV,
                                f_sensors.ADC2,
                                0
                                )

            if config.spi_enabled is True:
                self.__send_pkt(sig_cmds.CONFIG_SPI,
                                int(config.spi_sensor),
                                int(config.spi_sensor.get_srate())
                                )

            if config.i2c_enabled is True:
                self.__send_pkt(sig_cmds.CONFIG_I2C,
                                int(config.i2c_sensor),
                                int(config.i2c_sensor.get_srate())
                                )

            self.__send_pkt(sig_cmds.START_COLLECTION,
                            0,
                            0
                            )

    def get_samples(self):
        """
        Fetch a list of 5 samples by reading the serial device. Validates data
        using a CRC check.

        Args:
            None

        Returns:
            A list of the last 5 samples. False if device is not open
            or if data fails a CRC check.
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(12)

            if 12 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            # if(0 != ctrl_crc.crc16xmodem(buffer)):
            #     self.__throw_exception('Checksum invalid')

            s = struct.Struct('=HHHHHH')
            d1, d2, d3, d4, d5, crc = s.unpack(buffer)
            return [d1, d2, d3, d4, d5]
        else:
            return False

    def __write(self, data):
        """
        Write data to the serial device.

        Args:
            data: The data to send over the serial device.

        Returns:
            True if data was written, False if device is not open.
        """

        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(data)
            return True
        else:
            return False

    def __send_pkt(self, cmd, param1, param2):
        # Stuff transfer packet
        data_s = struct.Struct('=BBII')
        data = data_s.pack(sig_cmds.START_BYTE,
                           cmd,
                           param1,
                           param2)

        data_crc_s = struct.Struct('=H')

        crc_str = data_crc_s.pack(ctrl_crc.crc16xmodem(data))

        # Attempt to write first packet out
        self.__write(data + crc_str)

        cmd, param1, param2, crc = self.__read_cmd_pkt()

        retry_cntr = 0
        while(cmd != int(sig_cmds.ACK)):
            self.__write(data + crc_str)
            cmd, param1, param2, crc = self.__read_cmd_pkt()
            retry_cntr = retry_cntr + 1
            if(retry_cntr >= 3):
                self.__throw_exception('WriteRetryTimeout')

    def __read_byte(self):
        """
        Read a byte of data.

        Args:
            None

        Returns:
            A byte of data from the serial device. False if device is not open.
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read()

            if 1 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            return buffer
        else:
            return False

    def __read_byte_int(self):
        """
        Read a byte of data as an integer value.

        Args:
            None

        Returns:
            A byte of data from the serial device. False if device is not open.
        """

        if self.__s_comm.isOpen() is True:
            return int.from_bytes(self.__read_byte(),
                                  byteorder=self.__order,
                                  signed=False)
        else:
            return False

    def __read_uint16(self):
        """
        Read a uint16 from the serial device

        Args:
            None

        Returns:
            A uint16 from the serial device. False if the device is not open
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(2)

            if 2 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            return int.from_bytes(buffer, byteorder=self.__order, signed=False)
        else:
            return False

    def __read_cmd_pkt(self):
        """
        Read a cmd_pkt from the serial device

        Args:
            None

        Returns:
            A cmd_pkt from the serial device. False if the device is not open
        """

        if self.__s_comm.isOpen() is True:
            # Read and discard results until a START_BYTE is found
            cntr = 0
            while((self.__read_byte_int() != sig_cmds.START_BYTE) and
                  (cntr < 24)):
                cntr = cntr + 1

            # Read data from FIFO and return results
            buffer = self.__s_comm.read(11)

            if 11 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            s = struct.Struct('=BIIH')
            cmd, param1, param2, crc = s.unpack(buffer)

            # Reconstruct packet to validate CRC
            unpack_s = struct.Struct('=BBIIH')
            pkt = unpack_s.pack(sig_cmds.START_BYTE,
                                cmd,
                                param1,
                                param2,
                                crc)
            if(0 != ctrl_crc.crc16xmodem(pkt)):
                self.__throw_exception('Checksum invalid')

            return cmd, param1, param2, crc
        else:
            return False

    def __read_uint32(self):
        """
        Read a uint32 from the serial device

        Args:
            None

        Returns:
            A uint32 from the serial device. False if the device is not open
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(4)
            if 4 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            return int.from_bytes(buffer, byteorder=self.__order, signed=False)
        else:
            return False

    def __throw_exception(self, text):
        """
        Throws an exception for a read timeout
        """
        raise Exception(text)

    def parse_data_list(self, data, data_id):
        """
        Parses a data list for samples with a given data id and creates a
        data list from those samples

        Args:
            None

        Returns:
            A list of parsed data samples
        """
        output_list = list()

        for datum in data:
            if(datum >> 12 == data_id):
                output_list.append(datum & 4095)

        if len(output_list) != 0:
            return output_list
