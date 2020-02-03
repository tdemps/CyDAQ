import binascii
import serial
import threading
import serial.tools.list_ports
import struct
import ctrl_crc

from enum import IntEnum, Enum

from mdl_firmware import mdl_fw_vals
from mdl_firmware import f_sensors


class sig_serial(Enum):
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end.

    @author: sdmay18-31



    """
    END_BYTE = "!"       # decimal for '!'
    START_BYTE = "@"    # decimal for '@'

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
    END_BYTE = 33       # decimal for '!'
    START_BYTE = 64     # decimal for '@'
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


class parameter_options(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """
    NO_FILTER = 10
    NOTCH = 11
    HP1 = 12
    LP2 = 13
    BP2 = 14
    HP6 = 15
    LP6 = 16
    BP6 = 17

    AUDIO = 20

class ctrl_comm:
    """
       This class creates an abstraction for a connection to a device over
       a Serial connection.

       @author: sdmay18-31
       """

    def __init__(self):
        self.__s_comm = serial.Serial()
        self.__s_comm.port = self.serial_info()
        self.__s_comm.baudrate = 115200
        self.__s_comm.bytesize = serial.EIGHTBITS
        self.__s_comm.stopbits = serial.STOPBITS_ONE
        self.__s_comm.xonxoff = True
        self.__s_comm.rtscts = False
        self.__s_comm.dsrdtr = False
        self.__s_comm.parity = serial.PARITY_NONE
        self.__s_comm.timeout = 2
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

    ****CHANGE: Only the COM port that says 'USB Serial Port' (this is what the zybo
                shows up as) will appear
        """

        all_ports = serial.tools.list_ports.comports()
        open_ports = []

        # zybo_port = list(serial.tools.list_ports.grep("USB Serial Port"))
        #print(zybo_port)
        for element in all_ports:
            if ("USB Serial Port" in element.description):
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

    def write(self,data):
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

    def recieve_acknowlege_zybo(self, port_select):
        """
               receives ACK! from the ZYBO

               Args:
                   port_select = port that is selected i.e. the zybo port

               Returns:
                   None
               """

        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            while True:
                if self.read_byte() == sig_serial.START_BYTE.value:
                    buffer = ""
                    byte_value = ""
                    while byte_value != sig_serial.END_BYTE.value:
                        byte_value = self.read_byte()
                        if byte_value != sig_serial.END_BYTE.value:
                            buffer += byte_value

                    if len(buffer) != 3:
                        self.__throw_exception('SerialReadTimeout')
                        return False
                    # buffer = buffer.decode('ascii')
                    if buffer == "ack":
                        print("Data Received")
                        return True
                    else:
                        self.__throw_exception('SerailReadTimeout')
                        print("'ack' was not recieved")
                        return False
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

    def read_byte(self):
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

            buffer = buffer.decode('ascii')
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

    def __read_acknowledge(self):
        """
        Read a 3 bytes from the serial device

        Args:
            None

        Returns:
            a 1 if true and false if device is not open or string doesnt match "ack".
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(4)
            print(buffer)
            if 4 != len(buffer):
                self.__throw_exception('SerialReadTimeout')
            if buffer:
                buffer = "ack!"
                return True
            return False
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

    def serial_info(self):
        """
            Prints all data from the Zybo serial port

        Args:
            None

        Returns:
             None
        """
        all_ports = serial.tools.list_ports.comports()
        open_ports = []
        for element in all_ports:
            if ("USB Serial Port" in element.description):
                open_ports.append(element.device)
        zybo_port = open_ports[0]
        port = str(zybo_port)

        return port

    def serial_test(self):

        serial_port = serial.Serial(self.serial_info(), 115200, timeout=2)

        def read_from_port(self):
            while True:
                reading = serial_port.readline().decode('ASCII')
                data = reading
                print(str(data))

        zybo_info = threading.Thread(target=self.read(self), args=(serial_port,))
        zybo_info.start()


    """
        This class defines a table of commands that are sent to the ZYBO specifying the settings for
        the Zybo

        """
    """
          4 bytes
    cmd = command (srst = sample rate set, srgt = sample rate get, fbst =  set active filter, fbgt = get active filter,
                    inst = input set, ingt = input get, strt = start sampling, stop = stop sampling, fbtn = filter board tune)

    inst is the input select (audio, analog input)
    end of command is an an "!"
    if the command has multiple values in command, seperate them with with commas
    if zybo doesnt need to send anything, it will send 'ack!'

    srst = '8 digits in samples per second'!, expected value will be 'ack!'
    srgt = srgt!, expected value to receive will be srgt='8 bit number'!
    fbst = enum, 2 character! expected value will be 'ack!'
    fbgt = fbgt!, expected value to receive will be fbgt='2 characters'!
    inst = enum, 2 character! expected value will be 'ack!'
    ingt = fbgt!, expected value to receive will be ingt='2 characters'!
    strt = strt!, expected value will be 'ack!'
    stop = stop!, expected value will be 'ack!'
    fbtn = '2 digit filter','8 digits (for frequency)','8 digits (for second frequency)(if only one frequency is needed set to 0)'
            expected value will be 'ack!'


    raw data will come in 2 bytes in hexidecimal
    """

    def parameters(self):
        self.sample_rate_set = ""
        self.sample_rate_get = ""
        self.filter_set = ""
        self.filter_get = ""
        self.input_set = ""
        self.input_get = ""
        self.start = "START"
        self.stop = "STOP"
        self.lower_corner_freq_set = "0"
        self.lower_corner_freq_get = ""
        self.upper_corner_freq_set = "0"
        self.upper_corner_freq_get = ""

    def send_parameters(self, port_select):
        """
        sends all parameters to the Zybo

        Args:
        port_select = port that is selected i.e. the zybo port
         command = commands being sent

        Returns: None

        """

        self.open(port_select)
        self.write("@" + self.input_set + "," + self.filter_set + "," + self.sample_rate_set + "," + self.lower_corner_freq_set +
                   self.upper_corner_freq_set + "!")
        while not self.recieve_acknowlege_zybo(port_select):
            self.write("@" + self.input_set + self.filter_set + binascii.a2b_hex(self.sample_rate_set) + self.lower_corner_freq_set +
                 self.upper_corner_freq_set + "!")
        self.close()

    def compare_parameters(self, port_select):
        """
                Compares parameter settings from the Zybo with what is supposed to be set

                Args:
                    None

                Returns:
                    None
                """
        def __get_parameters(self):
            self.open(port_select)
            while True:
                if self.read_byte() == sig_serial.START_BYTE.value:
                    buffer = ""
                    byte_value = ""
                    while byte_value != sig_serial.END_BYTE.value:
                        byte_value = self.read_byte()
                        if byte_value != sig_serial.END_BYTE.value:
                            buffer += byte_value

                    if len(buffer) != 13:
                        self.__throw_exception('SerialReadTimeout')
                        print("parameters received in incorrect format")
                        return False

                    self.input_get = buffer[0] + buffer[1]
                    self.filter_get = buffer[2] + buffer[3]
                    sample_rate_hex = buffer[4] + buffer[5] + buffer[6] + buffer[7] + buffer [8]

                    self.lower_corner_freq_set = buffer[9] + buffer[10]
                    self.upper_corner_freq_get = buffer[11] + buffer[12]

        __get_parameters(port_select)
        compare = False
        while not compare:
            if ((self.input_get == self.input_set) & (self.filter_get == self.filter_set) &
                   (self.sample_rate_get == self.sample_rate_set) & (self.lower_corner_freq_get ==
                   self.lower_corner_freq_set) & (self.upper_corner_freq_get == self.upper_corner_freq_set)):
                self.close()
                compare = True
            else:
                self.send_parameters(port_select)
                __get_parameters(port_select)
                compare = False


