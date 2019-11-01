from tkinter import (Frame, Label, Listbox, Button)

from mdl_global import mdl_const
from mdl_firmware import f_sensors, f_sensor_type, mdl_fw_vals
from gui_collect import fr_collect


class fr_d_sen_sel(Frame):
    """
    Frame used to select a digital sensor for data collection.

    @author: sdmay18-31
    """

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="Select Digital Sensors",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Build list of digital sensors
        self.digi_sensor_list = list()
        for sen in f_sensors:
            if((sen.get_type() == f_sensor_type.I2C) or
               (sen.get_type() == f_sensor_type.SPI)):
                self.digi_sensor_list.append(str(sen))

        # Create Listbox
        self.digi_sensor_lbox = Listbox(self,
                                        height=len(self.digi_sensor_list),
                                        font=cnst.font_small)
        self.digi_sensor_lbox.configure(exportselection=False)

        # Add sensor names to listbox
        for i in range(len(self.digi_sensor_list)):
            self.digi_sensor_lbox.insert(i, self.digi_sensor_list[i])
        self.digi_sensor_lbox.select_set(0)
        self.digi_sensor_lbox.pack()

        # Create IMU Buttons and Text
        btn_sel_sen = Button(self,
                             text="Select",
                             width=20,
                             font=cnst.font_small,
                             bd=3,
                             command=lambda: self.set_digi_sen())
        btn_sel_sen.pack(pady=5, padx=10)

    def set_digi_sen(self):
        """
        Handles selecting a digital sensor

        Args:
            None

        Returns:
            None
        """
        cfg = mdl_fw_vals()
        cnst = mdl_const()

        # Fetch selected sensor
        curs_sel = self.digi_sensor_lbox.curselection()
        sen_name = self.digi_sensor_lbox.get(curs_sel)

        # Search for matching sensor and apply to mdl_fw_vals
        for sen in f_sensors:
            if(str(sen) == sen_name):
                if(sen.get_type() == f_sensor_type.I2C):
                    cfg.i2c_enabled = True
                    cfg.i2c_sensor = sen
                elif(sen.get_type() == f_sensor_type.SPI):
                    cfg.spi_enabled = True
                    cfg.spi_sensor = sen

        cnst.top_wndw.show_frame(fr_collect)
