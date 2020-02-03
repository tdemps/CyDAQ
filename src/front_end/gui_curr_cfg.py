from tkinter import (Frame, Label, StringVar, LEFT, Button)

from gui_welcome import fr_welcome

from mdl_global import mdl_const
from mdl_firmware import (mdl_fw_vals, f_sensors)


class fr_curr_cfg(Frame):
    """
    This class creates a frame for displaying the current CyDAQ configuration.

    @author: sdmay18-31
    """

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="Current Configuration",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Dynamic Body Text
        self.text = StringVar()

        label_text = Label(self,
                           font=cnst.font_small,
                           textvariable=self.text,
                           justify=LEFT)
        label_text.pack(pady=10, padx=10)

        # Clear value button
        self.btn_clr_vals = Button(self,
                                   text="Clear All",
                                   width=20,
                                   font=cnst.font_small,
                                   bd=3,
                                   command=lambda: self.clear_cfg())
        self.btn_clr_vals.pack(pady=5, padx=10)

    def clear_cfg(self):
        """
        Clears the current firmware configuration.

        Args:
            None

        Returns:
            None
        """
        cfg = mdl_fw_vals()
        cnst = mdl_const()
        cfg.reset_vals()

        cnst.top_wndw.show_frame(fr_welcome)

    def update_text(self):
        """
        Updates the frame's text field based on the current global state.

        Args:
            None

        Returns:
            None
        """
        cfg = mdl_fw_vals()

        new_str = ""

        # ADC Channel 0 display
        if(cfg.adc0_enabled is True):
            new_str += ("ADC0 Enabled: " +
                        str(cfg.adc0_s_rate) +
                        " samples/sec" +
                        "\n")

            ach, freq, offset = cfg.get_ach_data(f_sensors.ADC0)

            if(ach is not None):
                new_str += ("  Route: " +
                            str(ach))
                if(freq is not None and freq != 0):
                    new_str += (" at " +
                                str(freq) +
                                " Hz")
                new_str += "\n"

        # ADC Channel 1 display
        if(cfg.adc1_enabled is True):
            new_str += ("ADC1 Enabled: " +
                        str(cfg.adc1_s_rate) +
                        " samples/sec" +
                        "\n")

            ach, freq, offset = cfg.get_ach_data(f_sensors.ADC1)

            if(ach is not None):
                new_str += ("  Route: " +
                            str(ach))
                if(freq is not None and freq != 0):
                    new_str += (" at " +
                                str(freq) +
                                " Hz")
                new_str += "\n"

        # ADC Channel 2 display
        if(cfg.adc2_enabled is True):
            new_str += ("ADC2 Enabled: " +
                        str(cfg.adc2_s_rate) +
                        " samples/sec" +
                        "\n")

            ach, freq, offset = cfg.get_ach_data(f_sensors.ADC2)

            if(ach is not None):
                new_str += ("  Route: " +
                            str(ach))
                if(freq is not None and freq != 0):
                    new_str += (" at " +
                                str(freq) +
                                " Hz")
                new_str += "\n"

        # SPI display
        if(cfg.spi_enabled is True):
            new_str += ("SPI: " +
                        str(cfg.spi_sensor) +
                        " Enabled: " +
                        str(cfg.spi_sensor.get_srate()) +
                        " samples/sec" +
                        "\n")

        # I2C display
        if(cfg.i2c_enabled is True):
            new_str += ("I2C: " +
                        str(cfg.i2c_sensor) +
                        " Enabled: " +
                        str(cfg.i2c_sensor.get_srate()) +
                        " samples/sec" +
                        "\n")

        # Set string if no settings are valid
        if(new_str == ""):
            new_str = "No Current Configured Data"
            self.btn_clr_vals.pack_forget()
        else:
            self.btn_clr_vals.pack(pady=5, padx=10)

        self.text.set(new_str)
