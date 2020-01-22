import time
import threading
import serial
import serial.tools.list_ports
from tkinter import *

class ctrl_comm:
    """
       This class creates an abstraction for a connection to a device over
       a Serial connection.

       """

    def __init__(self):
        self.__s_comm = serial.Serial()
        self.__s_comm.port = None
        while self.__s_comm.port is None:
            try:
                self.__s_comm.port = self.get_port()
            except:
                not_connected()
                pass
        self.__s_comm.baudrate = 921600
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
            if "USB Serial Port" in element.description:
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
            # print(self.__s_comm.read(self.__s_comm.in_waiting()))
        else:
            self.__s_comm.flushInput()

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

            if len(buffer) >= 2:
                self.__throw_exception('SerialReadTimeout')
            elif not len(buffer):
                print("Nothing Received over Comm Port")
                return False
            buffer1 = buffer.decode('ascii')
            return buffer1
        else:
            return False

    def read_uint8(self):
        """
        Read a uint16 from the serial device

        Args:
            None

        Returns:
            A uint16 from the serial device. False if the device is not open
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(1)

            if 1 != len(buffer):
                self.__throw_exception('SerialReadTimeout')

            return int.from_bytes(buffer, byteorder=self.__order, signed=False)
        else:
            return False
    def read_uint16(self):
        """
        Read a uint16 from the serial device

        Args:
            None

        Returns:
            A uint16 from the serial device. False if the device is not open
        """

        if self.__s_comm.isOpen() is True:
            buffer = self.__s_comm.read(2)

            if len(buffer) != 2:
                self.__throw_exception('SerialReadTimeout')

            return int.from_bytes(buffer, byteorder=self.__order, signed=False)
        else:
            return False
    def __throw_exception(self, text):
        """
        Throws an exception for a read timeout
        """
        raise Exception(text)

    def isOpen(self):
        return self.__s_comm.is_open

    def getSerialObj(self):
        return self.__s_comm


class not_connected:
    def __init__(self):
        root = Tk()
        root.geometry("700x300")
        rootTitle = Label(root, text="Zybo Connection", height=2, width=len("Zybo Connection"), font=44)
        rootTitle.pack()
        global not_connected_label
        not_connected_labelName = "The Zybo is not connected or turned on."
        not_connected_label = Label(root, text=not_connected_labelName, height=1, width=len(not_connected_labelName),
                                    relief=RAISED, font=60)
        not_connected_label.place(relx=.5, rely=.5, anchor=CENTER)
        root.mainloop()

    def quit(self):
        exit()

    def check_connection(self):

        all_ports = serial.tools.list_ports.comports()
        open_ports = []
        for element in all_ports:
            if "USB Serial Port" in element.description:
                open_ports.append(element.device)
        try:
            zybo_port = open_ports[0]
            port = str(zybo_port)
            return port
        except:
            return False
