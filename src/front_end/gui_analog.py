from tkinter import (Frame, Label, Listbox, Button, Scale, END, messagebox)

from mdl_global import mdl_const
from mdl_firmware import f_sensors
from mdl_firmware import f_filts
from mdl_firmware import mdl_fw_vals
from gui_collect import fr_collect
from ctrl_utils import utils


class fr_a_ch_sel(Frame):
    """
    Frame used to manage the selection and setting of sampling frequencies
    for ADC Channels.

    @author: sdmay18-31
    """
    curr_cfg_ADC = None

    def get_curr_cfg_ADC():
        """
        Accessor for class scoped variable curr_cfg_ADC.
        Can be used to determine the ADC channel under
        configuration.

        Args:
            None

        Returns:
            Value of curr_cfg_ADC
        """
        return fr_a_ch_sel.curr_cfg_ADC

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="ADC Channels Select",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Subtitle Text
        label_sub_title = Label(self,
                                text="Set ADC sampling rates",
                                font=cnst.font_medium,
                                height=1)
        label_sub_title.pack()

        # Create ADC0 Buttons and Text
        self.sfreq_slider_0 = Scale(self,
                                    from_=cnst.adc_min,
                                    to=cnst.adc_max,
                                    resolution=25,
                                    length=320,
                                    orient="horizontal")
        self.sfreq_slider_0.pack()

        conf_chan_button0 = Button(self,
                                   text="Config Channel 0",
                                   width=20,
                                   font=cnst.font_small,
                                   bd=3,
                                   command=lambda: self.set_adc0())
        conf_chan_button0.pack(pady=5, padx=10)

        # Create ADC1 Buttons and Text
        self.sfreq_slider_1 = Scale(self,
                                    from_=cnst.adc_min,
                                    to=cnst.adc_max,
                                    resolution=25,
                                    length=320,
                                    orient="horizontal")
        self.sfreq_slider_1.pack()

        conf_chan_button1 = Button(self,
                                   text="Config Channel 1",
                                   width=20,
                                   font=cnst.font_small,
                                   bd=3,
                                   command=lambda: self.set_adc1())
        conf_chan_button1.pack(pady=5, padx=10)

        # Create ADC2 Buttons and Text
        self.sfreq_slider_2 = Scale(self,
                                    from_=cnst.adc_min,
                                    to=cnst.adc_max,
                                    resolution=25,
                                    length=320,
                                    orient="horizontal")
        self.sfreq_slider_2.pack()

        conf_chan_button2 = Button(self,
                                   text="Config Channel 2",
                                   width=20,
                                   font=cnst.font_small,
                                   bd=3,
                                   command=lambda: self.set_adc2())
        conf_chan_button2.pack(pady=5, padx=10)

    def reset(self):
        """
        Refresh callback
        Resets the slider maximums based on the number of tokens, samples
        per second, currently in use.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        cfg = mdl_fw_vals()

        toks_used = cfg.adc0_s_rate + cfg.adc1_s_rate + cfg.adc2_s_rate

        self.sfreq_slider_0.config(to=cnst.adc_max -
                                      toks_used +
                                      cfg.adc0_s_rate)
        self.sfreq_slider_0.set(cfg.adc0_s_rate)

        self.sfreq_slider_1.config(to=cnst.adc_max -
                                      toks_used +
                                      cfg.adc1_s_rate)
        self.sfreq_slider_1.set(cfg.adc1_s_rate)

        self.sfreq_slider_2.config(to=cnst.adc_max -
                                      toks_used +
                                      cfg.adc2_s_rate)
        self.sfreq_slider_2.set(cfg.adc2_s_rate)

    def set_adc0(self):
        """
        Sets the sampling rate of ADC channel 0, launches the filter
        selection frame.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        cfg = mdl_fw_vals()

        fr_a_ch_sel.curr_cfg_ADC = f_sensors.ADC0

        if ((utils.val_str_as_int(self.sfreq_slider_0.get(),
                                  cnst.adc_min,
                                  cnst.adc_max)) is True):
            cfg.adc0_enabled = True
            cfg.adc0_s_rate = int(self.sfreq_slider_0.get())
            cnst.top_wndw.show_frame(fr_a_f_sel)
        else:
            messagebox.showerror("Setup Error",
                                 "Sampling Rate of 0 is set!")

    def set_adc1(self):
        """
        Sets the sampling rate of ADC channel 1, launches the filter
        selection frame.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        cfg = mdl_fw_vals()

        fr_a_ch_sel.curr_cfg_ADC = f_sensors.ADC1

        if ((utils.val_str_as_int(self.sfreq_slider_1.get(),
                                  cnst.adc_min,
                                  cnst.adc_max)) is True):
            cfg.adc1_enabled = True
            cfg.adc1_s_rate = int(self.sfreq_slider_1.get())
            cnst.top_wndw.show_frame(fr_a_f_sel)
        else:
            messagebox.showerror("Setup Error",
                                 "Sampling Rate of 0 is set!")

    def set_adc2(self):
        """
        Sets the sampling rate of ADC channel 2, launches the filter
        selection frame.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        cfg = mdl_fw_vals()

        fr_a_ch_sel.curr_cfg_ADC = f_sensors.ADC2

        if ((utils.val_str_as_int(self.sfreq_slider_2.get(),
                                  cnst.adc_min,
                                  cnst.adc_max)) is True):
            cfg.adc2_enabled = True
            cfg.adc2_s_rate = int(self.sfreq_slider_2.get())
            cnst.top_wndw.show_frame(fr_a_f_sel)

        else:
            messagebox.showerror("Setup Error",
                                 "Sampling Rate of 0 is set!")


class fr_a_f_sel(Frame):
    """
    Frame used to manage the selection of analog filter paths and tuning
    for the current ADC channel under configuration, as specified by the
    fr_a_ch_sel.curr_cfg_ADC variable.

    @author: sdmay18-31
    """

    def listbox_select_handler(self, event):
        """
        Callback for selecting a listbox item. Handles hiding the filtering
        scale for the passthrough filter and the 60Hz Notch filter.

        Args:
            event: Event that evoked the callback

        Returns:
            None
        """

        index = int(event.widget.curselection()[0])
        value = event.widget.get(index)

        if (value == str(f_filts.NOTCH) or
                value == str(f_filts.PASS)):
            self.label_freq.pack_forget()
            self.freq_slider.pack_forget()
        else:
            new_min = 0
            new_max = 8000
            if value == str(f_filts.LP1):
                new_min = 435;
                new_max = 8000;
            elif value == str(f_filts.LP5):
                new_min = 45;
                new_max = 1422;
            elif value == str(f_filts.HP1):
                new_min = 246;
                new_max = 2478;
            elif value == str(f_filts.HP5):
                new_min = 1682;
                new_max = 8000;
            elif value == str(f_filts.BP2):
                new_min = 558;
                new_max = 6767;
            elif value == str(f_filts.BP6):
                new_min = 800;
                new_max = 8000;

            self.freq_slider.configure(from_=new_min,
                                       to=new_max)
            self.btn_set_filter.pack_forget()
            self.label_freq.pack(pady=0, padx=10)
            self.freq_slider.pack()
            self.btn_set_filter.pack(pady=5, padx=10)

    def set_filter(self):
        """
        Callback for setting filter tuning for an analog channel.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        cfg = mdl_fw_vals()

        # Grab the currently selected filter
        curs_sel = self.analog_filterTypeList.curselection()
        afiltName = self.analog_filterTypeList.get(curs_sel)

        # Grab the filter tuning parameter and channel we are configuring
        freq = self.freq_slider.get()
        active_route = fr_a_ch_sel.get_curr_cfg_ADC()

        # Clear the old filter associated with this channel
        cfg.clear_filter(active_route)

        if afiltName == str(f_filts.LP1):
            cfg.lp_1st_enabled = True
            cfg.lp_1st_route = active_route
            cfg.lp_1st_3db = freq

        if afiltName == str(f_filts.LP5):
            cfg.lp_5th_enabled = True
            cfg.lp_5th_route = active_route
            cfg.lp_5th_3db = freq

        if afiltName == str(f_filts.HP1):
            cfg.hp_1st_enabled = True
            cfg.hp_1st_route = active_route
            cfg.hp_1st_3db = freq

        if afiltName == str(f_filts.HP5):
            cfg.hp_5th_enabled = True
            cfg.hp_5th_route = active_route
            cfg.hp_5th_3db = freq

        if afiltName == str(f_filts.BP2):
            cfg.bp_2nd_enabled = True
            cfg.bp_2nd_route = active_route
            cfg.bp_2nd_center = freq

        if afiltName == str(f_filts.BP6):
            cfg.bp_6th_enabled = True
            cfg.bp_6th_route = active_route
            cfg.bp_6th_center = freq

        if afiltName == str(f_filts.NOTCH):
            cfg.bp_60_enabled = True
            cfg.bp_60_route = active_route

        if afiltName == str(f_filts.PASS):
            cfg.pass_thru_enabled = True
            cfg.pass_thru_route = active_route

        # Change frame to collect frame
        cnst.top_wndw.show_frame(fr_collect)

    def print_list_box(self):
        """
        Dynamically Creating List Box
        Args:
            None

        Returns:
            None
        """
        cfg = mdl_fw_vals()

        filt0, freq, offset = cfg.get_ach_data(f_sensors.ADC0)
        filt1, freq, offset = cfg.get_ach_data(f_sensors.ADC1)
        filt2, freq, offset = cfg.get_ach_data(f_sensors.ADC2)

        # Reset filt value to None to allow associated filter to be set
        if (fr_a_ch_sel.curr_cfg_ADC == f_sensors.ADC0):
            filt0 = None

        if (fr_a_ch_sel.curr_cfg_ADC == f_sensors.ADC1):
            filt1 = None

        if (fr_a_ch_sel.curr_cfg_ADC == f_sensors.ADC2):
            filt2 = None

        self.analog_filterTypeList.delete(0, END)

        for i in range(len(self.analogFilterNames)):
            if (filt0 != self.analogFilterNames[i] and
                    filt1 != self.analogFilterNames[i] and
                    filt2 != self.analogFilterNames[i]):
                self.analog_filterTypeList.insert(i, self.analogFilterNames[i])

        self.analog_filterTypeList.select_set(0)
        self.analog_filterTypeList.event_generate("<<ListboxSelect>>")

    def reset(self):
        """
        Refresh callback
        Resets the listbox based on selected values, refresh operation.

        Args:
            None

        Returns:
            None
        """
        self.print_list_box()
        self.freq_slider.set(0)

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Create Frame title
        label_title = Label(self,
                            text="Analog Filter Settings",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Build list of filter names
        self.analogFilterNames = list()
        for n in f_filts:
            self.analogFilterNames.append(n)

        self.filtNum = len(self.analogFilterNames)

        # Create filter selection box and title label
        label_filter_box = Label(self,
                                 text="Filter Type:",
                                 font=cnst.font_medium,
                                 height=1)
        label_filter_box.pack()

        self.analog_filterTypeList = Listbox(self,
                                             height=self.filtNum,
                                             font=cnst.font_small)
        self.analog_filterTypeList.pack(pady=10, padx=10)
        self.analog_filterTypeList.configure(exportselection=False)
        self.print_list_box()
        self.analog_filterTypeList.bind('<<ListboxSelect>>',
                                        self.listbox_select_handler)
        self.analog_filterTypeList.pack()

        # Create frequency slider with title
        self.label_freq = Label(self,
                                text="Frequency",
                                font=cnst.font_medium)
        self.label_freq.pack(pady=0, padx=10)

        self.freq_slider = Scale(self,
                                 from_=cnst.freq_min,
                                 to=cnst.freq_max,
                                 resolution=25,
                                 length=320,
                                 orient="horizontal")
        self.freq_slider.pack(pady=0, padx=10)

        # Create set filter button
        self.btn_set_filter = Button(self,
                                     text="Set Filter",
                                     width=14,
                                     font=cnst.font_medium,
                                     bd=3,
                                     command=lambda: self.set_filter())
        self.btn_set_filter.pack(pady=5, padx=10)
