import time
from enum import IntEnum, Enum
import threading
import serial
import serial.tools.list_ports


# from mdl_firmware import mdl_fw_vals

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

class sig_serial(Enum):
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end.

    @author: sdmay18-31



    """
    END_BYTE = "!"       # decimal for '!'
    START_BYTE = "@"    # decimal for '@'


class parameter_options(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """
    input_select = 0
    sample_rate = 1
    filter = 2
    corner_freq = 3
    START = 8
    STOP = 9

class parameter_options_filter(IntEnum):
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

class parameter_options_input(IntEnum):
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

class parameter_options_output(IntEnum):
    """
           This class defines a table of commands that are sent to the ZYBO specifying the settings for
           the Zybo

          """

    xadc = 0
    digital = 1

class ctrl_comm:
    """
       This class creates an abstraction for a connection to a device over
       a Serial connection.

       @author: sdmay18-31
       """

    def __init__(self):
        self.__s_comm = serial.Serial()
        self.__s_comm.port = self.get_port()            #todo 
        self.__s_comm.baudrate = 921600
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
            self.__s_comm.close()

    def get_port(self):
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
        for element in all_ports:
            if ("USB Serial Port" in element.description):
                open_ports.append(element.device)
        try:
            zybo_port = open_ports[0]
            port = str(zybo_port)
            return port
        except:
            print("Zybo not connected")
            return False

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

    def write(self, data):
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

        if self.__s_comm.isOpen() is True:
            while True:

                if self.read_byte() == sig_serial.START_BYTE.value:
                    buffer = ""
                    byte_value = ""
                    if len(buffer) < 6:
                        while byte_value != sig_serial.END_BYTE.value:
                            byte_value = self.read_byte()
                            if byte_value != sig_serial.END_BYTE.value:
                                buffer += byte_value

                    if len(buffer) != 3:
                        # self.__throw_exception('SerialReadTimeout')
                        return False
                    # buffer = buffer.decode('ascii')
                    if buffer == "ack":
                        print("Data Received")
                        return True
                    else:
                        self.__throw_exception('ack was not received')
                        print("'ack' was not received")
                        return False
                else:
                    return False
        else:
            return False

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




    def __throw_exception(self, text):
        """
        Throws an exception for a read timeout
        """
        raise Exception(text)




    def send_parameters(self, port_select, input_set, sample_rate, filter_select, corner_freq_upper, corner_freq_lower):
        """
        sends all parameters to the Zybo

        Args:
        port_select = port that is selected i.e. the zybo port
         command = commands being sent

        Returns: None

        """

        self.open(port_select)
        while True:
            self.send_input(port_select, input_set)
            if self.recieve_acknowlege_zybo(port_select):
                break
        while True:
            self.send_sample_rate(port_select, sample_rate)
            if self.recieve_acknowlege_zybo(port_select):
                break
        while True:
            self.send_filter(port_select, filter_select)
            if self.recieve_acknowlege_zybo(port_select):
                break
        while True:
            self.send_corner_freq(port_select, corner_freq_upper, corner_freq_lower)
            if self.recieve_acknowlege_zybo(port_select):
                break
        while True:
            self.send_start(port_select)
            if self.recieve_acknowlege_zybo(port_select):
                break
        self.close()

    def send_stop_cmd(self, port_select):

        self.open(port_select)
        while True:
            self.send_stop(port_select)
            if self.recieve_acknowlege_zybo(port_select):
                break
        self.close()




    def send_input(self, port_select , input_set):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.input_select.value)
            self.write(input_set)
            print("Input set Enum= " + input_set)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending Input Failed')
            return False


    def send_sample_rate(self, port_select, sample_rate):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        sample_rate_bin = self.decimal_to_binary(sample_rate)
        sample_rate_hex = self.binary_to_hex(sample_rate_bin)

        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.sample_rate.value)
            self.write(sample_rate_hex)
            print("Sample Rate = " + sample_rate)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending Input Failed')
            return False

    def send_filter(self, port_select , filter_select):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.filter.value)
            self.write(filter_select)
            print("Filter Enum = " + filter_select)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending filter failed')
            return False

    def send_corner_freq(self, port_select , u_corner_freq, l_corner_freq):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        u_bin = self.decimal_to_binary(u_corner_freq)
        u_hex = self.binary_to_hex(u_bin)
        l_bin = self.decimal_to_binary(l_corner_freq)
        l_hex = self.binary_to_hex(l_bin)

        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.corner_freq.value)
            self.write(u_hex + l_hex)
            print("Corner Frequency = " + u_corner_freq + " / " + l_corner_freq)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending corner freq failed')
            return False

    def send_start(self, port_select):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.START.value)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending start failed')
            return False

    def send_stop(self, port_select):
        """
                Sends the Input.

                Args:
                    None

                Returns:
                    True
                """
        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.__s_comm.write(sig_serial.START_BYTE.value.encode())
            self.write(parameter_options.STOP.value)
            self.__s_comm.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending stop failed')
            return False

    def ping_zybo(self, port_select):
        """
                Handshake between zybo and the cydaq

                Args:
                    None

                Returns:
                    None
                """
        self.open(port_select)
        if self.__s_comm.isOpen() is True:
            self.write("Hello Mr.Zybo".encode())
            while True:
                if self.recieve_acknowlege_zybo(port_select):
                    if self.read_byte() == sig_serial.START_BYTE.value:
                        buffer = ""
                        byte_value = ""
                        if len(buffer) < 20:
                            while byte_value != sig_serial.END_BYTE.value:
                                byte_value = self.read_byte()
                                if byte_value != sig_serial.END_BYTE.value:
                                    buffer += byte_value
                        else:
                            self.__throw_exception('ack was not received')
                            return False
                        if buffer == 'Hello Mr.CyDAQ':
                            print(buffer)
                            return True
                        else:
                            self.__throw_exception('test not received')
                            return False
                    else:
                        pass
                else:
                    return False
        else:
            return False

    def decimal_to_binary(self, number):
        bin_num = bin(number)
        return bin_num

    def binary_to_hex(self, number):
        hex_num = hex(int(number, 2))
        return hex_num



