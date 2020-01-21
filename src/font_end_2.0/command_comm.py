from serial_comm import ctrl_comm
from master_enum import parameter_options_filter, parameter_options, parameter_options_input, parameter_options_output,\
    sig_serial
import struct
import serial
import serial.tools.list_ports

ctrl_comm_obj = ctrl_comm()


class cmd:
    def recieve_acknowlege_zybo(self, port_select):
        """
               receives ACK! from the ZYBO

               Args:
                   port_select = port that is selected i.e. the zybo port

               Returns:
                   None
               """

        if ctrl_comm_obj.isOpen() is True:
            while True:
                cnt = 0
                if ctrl_comm_obj.read_byte() == sig_serial.START_BYTE.value:
                    buffer = ""
                    byte_value = ""
                    if len(buffer) < 6:
                        while byte_value != sig_serial.END_BYTE.value:
                            byte_value = ctrl_comm_obj.read_byte()
                            if byte_value != sig_serial.END_BYTE.value:
                                buffer += byte_value

                    if len(buffer) != 3:
                        # self.__throw_exception('SerialReadTimeout')
                        print("read cycle count: " + str(cnt))
                        return False
                    # buffer = buffer.decode('ascii')
                    if buffer == "ACK":
                        print("Data Received")
                        return True
                    else:
                        # self.__throw_exception('ack was not received')
                        print("'ack' was not received")
                        return False
                else:
                    return False
        else:
            return False

    def send_parameters(self, port_select, input_set, sample_rate, filter_select, corner_freq_upper, corner_freq_lower):
        """
        sends all parameters to the Zybo

        Args:
        port_select = port that is selected i.e. the zybo port


        Returns: None

        """

        ctrl_comm_obj.open(port_select)
        cnt = 0
        cursor = 0
        wait = 0
        while cnt < 3 and cursor < 5:
            if cursor == 0 and wait == 0:
                self.send_input(port_select, input_set)
                wait = 1
            elif cursor == 1 and wait == 0:
                self.send_sample_rate(port_select, sample_rate)
                wait = 1
            elif cursor == 2 and wait == 0:
                self.send_filter(port_select, filter_select)
                wait = 1
            elif cursor == 3 and wait == 0:
                self.send_corner_freq(port_select, corner_freq_upper, corner_freq_lower)
                wait = 1
            elif cursor == 4 and wait == 0:
                self.send_start(port_select)
                wait = 1
            if wait == 1:
                if self.recieve_acknowlege_zybo(port_select):
                    print("Ack received")
                    cursor += 1
                    cnt = 0
                    wait = 0
                else:
                    pass
            else:
                cnt += 1
        if cnt < 3:
            print("Commands Sent/Received")
        else:
            print("Commands Not Sent/Received")
        ctrl_comm_obj.close()

    def send_stop_cmd(self, port_select):
        cursor = 0
        wait = 0
        ctrl_comm_obj.open(port_select)
        while cursor < 2:
            if cursor == 0:
                self.send_stop(port_select)
                wait = 1
            elif cursor == 1:
                self.send_fetch(port_select)
                wait = 1
            if wait == 1:
                if self.recieve_acknowlege_zybo(port_select):
                    print("Ack received")
                    cursor += 1
                    wait = 0
                else:
                    pass
        ctrl_comm_obj.close()

    def send_fetch(self, port_select):
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', parameter_options.fetch_samples.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending fetch failed')
            return False
    def send_input(self, port_select, input_set):
        """
                Sends the Input.

                Args:
                    port selection
                    input selection

                Returns:
                    True
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', parameter_options.input_select.value))
            ctrl_comm_obj.write(struct.pack('!B', input_set))
            print("Input set Enum = " + str(input_set))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

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

        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!Bi', parameter_options.sample_rate.value, int(sample_rate)))
            print("Sample Rate = " + sample_rate)
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending Input Failed')
            return False

    def send_filter(self, port_select, filter_select):
        """
                Sends the Input.

                Args:
                    port selection
                    filter selection

                Returns:
                    True
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!BB', parameter_options.filter.value, filter_select))
            print("Filter Enum = " + str(filter_select))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending filter failed')
            return False

    def send_corner_freq(self, port_select, u_corner_freq, l_corner_freq):
        """
                Sends the Input.

                Args:
                    port selection
                    upper corner freq
                    lower corner freq


                Returns:
                    True
                """

        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!BHH', parameter_options.corner_freq.value,  l_corner_freq, u_corner_freq))
            print("Corner Frequency = " + str(u_corner_freq) + " / " + str(l_corner_freq))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

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
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', parameter_options.START.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending start failed')
            return False

    def send_stop(self, port_select):
        """
                Sends the Input.

                Args:
                    port selection


                Returns:
                    True
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', parameter_options.STOP.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())


        else:
            self.__throw_exception('Sending stop failed')
            return False

    def __throw_exception(self, text):
        """
        Throws an exception for a read timeout
        """
        raise Exception(text)

    def ping_zybo(self, port_select):
        """
                Handshake between zybo and the cydaq

                Args:
                    port selection


                Returns:
                    None
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode('ascii'))
            ctrl_comm_obj.write(struct.pack('!B', parameter_options.ping.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode('ascii'))
            while True:
                if self.recieve_acknowlege_zybo(port_select):
                    if ctrl_comm_obj.read_byte() == sig_serial.START_BYTE.value:
                        buffer = ""
                        byte_value = ""
                        if len(buffer) < 20:
                            while byte_value != sig_serial.END_BYTE.value:
                                byte_value = ctrl_comm_obj.read_byte()
                                if byte_value != sig_serial.END_BYTE.value:
                                    buffer += byte_value
                        else:
                            print("Acknowledge was incorrect")
                            return False
                        if buffer == 'ACK':
                            print(buffer)
                            return True
                        else:
                            print("Acknowledge was incorrect")
                            return False
                    else:
                        pass
                else:
                    pass
        else:
            return False

    def decimal_to_binary(self, number):
        bin_num = bin(int(number))
        return bin_num

    def binary_to_hex(self, number):
        hex_num = hex(int(number, 2))
        return hex_num

    def hex_to_dec(self, hex_number):
        dec_num = str(int(hex_number, 16))
        return dec_num