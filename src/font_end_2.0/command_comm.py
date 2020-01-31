from serial_comm import ctrl_comm
from master_enum import enum_filter, enum_commands, enum_input, enum_output,\
    sig_serial
import struct
import time as t

ctrl_comm_obj = ctrl_comm()


class cmd:
    def recieve_acknowlege_zybo(self, port_select):
        """
               receives ACK! from the ZYBO

               Args:
                   port_select: Zybo comm port

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
                    elif buffer == 'ERR':
                        print('CyDAQ encountered error during configuration, contact ETG')
                        return False
                    else:
                        # self.__throw_exception('ack was not received')
                        print("'ack' was not received")
                        return False
                else:
                    return False
        else:
            return False

    def send_parameters(self, port_select, input_set, sample_rate, filter_select, corner_freq_upper, corner_freq_lower, corner_freq):
        """
        sends all parameters to the Zybo

        Args:
            port_select: Zybo comm port
            input_set: input selection enum value
            sample_rate: sample rate value
            filter_select: filter select enum value
            corner_freq_lower: lower corner frequency value
            corner_freq_upper: upper corner frequency value


        Returns: True if cydaq was programmed successfully, false otherwise

        """
        try:
            ctrl_comm_obj.open(port_select)
        except ValueError as e:
            return False
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
                self.send_corner_freq(port_select, corner_freq_upper, corner_freq_lower, corner_freq, filter_select)
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

        return True

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
            if wait == 1 and self.recieve_acknowlege_zybo(port_select):
                print("Sampling stop ACK received")
                cursor += 1
                wait = 0
            else:
                pass
        ctrl_comm_obj.close()

    def send_fetch(self, port_select):
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', enum_commands.fetch_samples.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sample transfer init failed')
            return False

    def send_input(self, port_select, input_set):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port
                    input_set: input select enum value

                Returns:
                    True
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', enum_commands.input_select.value))
            ctrl_comm_obj.write(struct.pack('!B', int(input_set)))
            print("Input set Enum = " + str(input_set))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending Input Failed')
            return False

    def send_sample_rate(self, port_select, sample_rate):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port
                    sample_rate: sample rate enum value

                Returns:
                    True
                """

        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!BI', enum_commands.sample_rate.value, int(sample_rate)))
            print("Sample Rate = " + sample_rate)
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending Sample Rate Failed')
            return False

    def send_filter(self, port_select, filter_select):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port
                    filter_select: filter selection enum

                Returns:
                    True or False
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!BB', enum_commands.filter.value, int(filter_select)))
            print("Filter Enum = " + str(filter_select))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending filter failed')
            return False

    def send_corner_freq(self, port_select, u_corner_freq, l_corner_freq, corner_freq, filter):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port
                    u_corner_freq: upper corner frequency value
                    l_corner_freq: lower corner frequency value


                Returns:
                    True or false
                """
        val_to_write = None
        ctrl_comm_obj.open(port_select)

        if ctrl_comm_obj.isOpen() is True:
            if filter == enum_filter.BP2.value or filter == enum_filter.BP6.value:
                val_to_write = struct.pack('!BHH', enum_commands.corner_freq.value, int(l_corner_freq), int(u_corner_freq))
                print("Corner Frequency = " + l_corner_freq + " / " + u_corner_freq)
            elif filter == enum_filter.LP1.value or filter == enum_filter.LP6.value:
                val_to_write = struct.pack('!BHH', enum_commands.corner_freq.value, int(corner_freq), 0)
                print("Corner Frequency = " + corner_freq + " / " + str(0))
            elif filter == enum_filter.HP1.value or filter == enum_filter.HP6.value:
                val_to_write = struct.pack('!BHH', enum_commands.corner_freq.value, int(corner_freq), 0)
                print("Corner Frequency = " + corner_freq + " / " + str(0))
            else:
                val_to_write = struct.pack('!BHH', enum_commands.corner_freq.value, int(l_corner_freq), int(u_corner_freq))
                print("Corner Frequency = " + l_corner_freq + " / " + u_corner_freq)

            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(val_to_write)
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())
            return True
        else:
            self.__throw_exception('Sending corner freq failed')
            return False

    def send_start(self, port_select):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port

                Returns:
                    True or false
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', enum_commands.START.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode())

        else:
            self.__throw_exception('Sending start failed')
            return False

    def send_stop(self, port_select):
        """
                Sends the Input.

                Args:
                    port_select: Zybo comm port

                Returns:
                    True or false
                """
        ctrl_comm_obj.open(port_select)
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode())
            ctrl_comm_obj.write(struct.pack('!B', enum_commands.STOP.value))
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
                    port_select: Zybo comm port


                Returns:
                    None
                """
        try:
            ctrl_comm_obj.open(port_select)
        except ValueError as e:
            return False
        if ctrl_comm_obj.isOpen() is True:
            ctrl_comm_obj.write(sig_serial.START_BYTE.value.encode('ascii'))
            ctrl_comm_obj.write(struct.pack('!B', enum_commands.ping.value))
            ctrl_comm_obj.write(sig_serial.END_BYTE.value.encode('ascii'))
            cnt = 0
            while True:
                if self.recieve_acknowlege_zybo(port_select):
                    return True
                elif cnt > 10:
                    return False
                else:
                    t.sleep(0.1)
                    cnt += 1
                    # if ctrl_comm_obj.read_byte() == sig_serial.START_BYTE.value:
                    #     buffer = ""
                    #     byte_value = ""
                    #     if len(buffer) < 20:
                    #         while byte_value != sig_serial.END_BYTE.value:
                    #             byte_value = ctrl_comm_obj.read_byte()
                    #             if byte_value != sig_serial.END_BYTE.value:
                    #                 buffer += byte_value
                    #     else:
                    #         print("Acknowledge was incorrect")
                    #         return False
                    #     if buffer == 'ACK':
                    #         print(buffer)
                    #         return True
                    #     else:
                    #         print("Acknowledge was incorrect")
                    #         return False
                    # else:
                    #     pass
        else:
            return False

    #not needed since struct library takes care of byte convertions for us
    # def decimal_to_binary(self, number):
    #     bin_num = bin(int(number))
    #     return bin_num
    #
    # def binary_to_hex(self, number):
    #     hex_num = hex(int(number, 2))
    #     return hex_num
    #
    # def hex_to_dec(self, hex_number):
    #     dec_num = str(int(hex_number, 16))
    #     return dec_num