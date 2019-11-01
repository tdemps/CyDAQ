from tkinter import (Frame, Label, Button, StringVar, END, Listbox, messagebox)
from threading import (Thread, Event, Timer)

from ctrl_fileio import ctrl_fileio
from mdl_global import mdl_const
from mdl_firmware import mdl_fw_vals
from mdl_firmware import f_sensors
from ctrl_comm import ctrl_comm


class fr_collect(Frame):
    """
    This class creates a simple frame for collecting data from a serial
    device. The collected data is printed in an output window and saved in a
    csv or MAT file

    @author: sdmay18-31
    """

    # Helper Thread for data collection
    class data_thread(Thread):
        """
        This class defines a thread for handling serial IO outside
        the UI thread

        @author: sdmay18-31
        """
        def __init__(self, serial, buffer, btn_txt):
            super(fr_collect.data_thread, self).__init__()
            self.__stop_event = Event()
            self.__serial = serial
            self.__buffer = buffer
            self.__btn_txt = btn_txt

        def stop(self):
            """
            Stop thread
            """

            self.__stop_event.set()

        def run(self):
            """
            Continuously collect samples and output them until the stop()
            function is called.
            """

            try:
                while not self.__stop_event.isSet():
                    samples = self.__serial.get_samples()

                    if not self.__stop_event.isSet():
                        self.__buffer.extend(samples)
            except Exception as e:
                self.__serial.close()
                messagebox.showerror("Communication Error",
                                     "Data read timeout occurred!")
                self.__btn_txt.set("START")

    def on_close(self):
        """
        Ensures the comm device is closed before the window is destroyed

        Args:
            None

        Returns:
            None
        """

        self.__comm.close()

    def collect_data(self):
        """
        Handles data collection. Spawns a child thread to pull data from the
        serial device until the STOP button clicked. Will output data to a
        timestamped csv file.

        Args:
            None

        Returns:
            None
        """

        cursor = self.__list_com_ports.curselection()

        if(len(cursor) == 0):
            messagebox.showerror("Setup Error",
                                 "No COM Port selected!")
        else:
            self.__port_selection = self.__list_com_ports.get(cursor)

            if self.__btn_start_text.get() == "START":
                cfg = mdl_fw_vals()
                if(cfg.validate()):
                    # Change button text
                    self.__btn_start_text.set("STOP")

                    # Start comm device
                    try:
                        self.__data.clear()
                        self.__comm.open(self.__port_selection)

                        # Setup worker thread
                        self.worker = self.data_thread(self.__comm,
                                                       self.__data,
                                                       self.__btn_start_text)
                        self.worker.start()
                    except Exception as e:
                        self.__comm.close()
                        self.__btn_start_text.set("START")
                        messagebox.showerror("Communication Error",
                                             "Timeout occurred on setup!")
                else:
                    messagebox.showerror("Invalid Configuration",
                                         "Check and clear your settings.")
            else:
                # Change button text
                self.__btn_start_text.set("START")

                # End worker thread
                self.worker.stop()
                self.worker.join()

                # Close comm connection
                self.__comm.close()

                cnst = mdl_const()
                cfg = mdl_fw_vals()

                # Parse the samples from the unified data list
                if(cfg.adc0_enabled is True):
                    raw_data = self.__comm.parse_data_list(self.__data,
                                                           f_sensors.ADC0)

                    ctrl_fileio.produce_output_file(raw_data,
                                                    cfg.adc0_s_rate,
                                                    str(f_sensors.ADC0))

                if(cfg.adc1_enabled is True):
                    raw_data = self.__comm.parse_data_list(self.__data,
                                                           f_sensors.ADC1)

                    ctrl_fileio.produce_output_file(raw_data,
                                                    cfg.adc1_s_rate,
                                                    str(f_sensors.ADC1))

                if(cfg.adc2_enabled is True):
                    raw_data = self.__comm.parse_data_list(self.__data,
                                                           f_sensors.ADC2)

                    ctrl_fileio.produce_output_file(raw_data,
                                                    cfg.adc2_s_rate,
                                                    str(f_sensors.ADC2))

                if(cfg.spi_enabled is True):
                    raw_data = self.__comm.parse_data_list(self.__data,
                                                           cfg.spi_sensor)

                    ctrl_fileio.produce_output_file(raw_data,
                                                    cfg.spi_sensor.get_srate(),
                                                    str(cfg.spi_sensor))

                if(cfg.i2c_enabled is True):
                    raw_data = self.__comm.parse_data_list(self.__data,
                                                           cfg.i2c_sensor)

                    ctrl_fileio.produce_output_file(raw_data,
                                                    cfg.i2c_sensor.get_srate(),
                                                    str(cfg.i2c_sensor))

                # Display number of collected samples
                # print("Num Samples: " + str(len(self.__data)))

                self.__data.clear()
                cnst.port_timer = Timer(1.0, self.__update_com_list)
                cnst.port_timer.start()

    def __update_com_list(self):
        """
        Queries the available device comm list and updates the page's
        list of devices.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        if self.__btn_start_text.get() == "START":
            port_list = self.__comm.get_ports()

            cursor = self.__list_com_ports.curselection()
            if len(cursor) != 0:
                port_selection = self.__list_com_ports.get(cursor)
            else:
                port_selection = ""

            self.__list_com_ports.delete(0, END)
            for element in port_list:
                self.__list_com_ports.insert(END, element)
                if element == port_selection:
                    elem_num = self.__list_com_ports.size() - 1
                    self.__list_com_ports.select_set(elem_num)

            cnst.port_timer = Timer(1.0, self.__update_com_list)
            cnst.port_timer.start()

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Create communication object
        self.__comm = ctrl_comm()
        self.__port_selection = "None"
        self.__data = list()

        # Create frame title
        label_title = Label(self,
                            text="CyDAQ Data Collection",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Declare list of ports, set button, and labels
        label_ports = Label(self,
                            text="Available COM Ports",
                            font=cnst.font_small,
                            height=1)

        label_ports.pack()
        self.__list_com_ports = Listbox(self,
                                        height=5,
                                        font=cnst.font_small)

        self.__list_com_ports.pack(pady=5, padx=5)

        self.__list_com_ports.configure(exportselection=False)

        # Query available ports
        port_list = self.__comm.get_ports()
        for element in port_list:
            self.__list_com_ports.insert(END, element)

        self.__list_com_ports.select_set(0)
        self.__list_com_ports.event_generate("<<ListboxSelect>>")
        self.__list_com_ports.pack(pady=5, padx=5)


        # Start Button Setup
        self.__btn_start_text = StringVar()
        self.__btn_start_text.set("START")
        self.__btn_start = Button(self,
                                  textvariable=self.__btn_start_text,
                                  width=10,
                                  font=cnst.font_small,
                                  bd=3,
                                  command=lambda: self.collect_data())
        self.__btn_start.pack(pady=5, padx=5)

        # Setup timer to refresh list of devices every 1 second
        cnst.port_timer = Timer(1.0, self.__update_com_list)
        cnst.port_timer.start()
