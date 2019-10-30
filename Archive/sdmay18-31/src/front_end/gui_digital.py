import re

from tkinter import (Frame, Label, Entry, StringVar, IntVar, Button,
                     messagebox, Scale, Radiobutton, LabelFrame)

from mdl_global import mdl_const
from mdl_digital import (d_ftype, d_fproto)
from ctrl_digital import digi_filt
from ctrl_fileio import ctrl_fileio
import scipy.signal as signal


class fr_d_filter_gen(Frame):
    """
    Frame used to allow a user to generate a digital filter.

    @author: sdmay18-31
    """
    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="Digital Filtering",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Subtitle Text
        label_fir_title = Label(self,
                                text="FIR Filter",
                                font=cnst.font_medium)
        label_fir_title.pack(pady=10, padx=10)

        # Create set digital filter button
        btn_filter_d = Button(self,
                              text="Design FIR Filter",
                              width=20,
                              font=cnst.font_small,
                              bd=3,
                              command=lambda: self.set_fir_filter_d_())
        btn_filter_d.pack(pady=10, padx=10)

        # Subtitle IIR Filter
        label_iir_title = Label(self,
                                text="IIR Filter",
                                font=cnst.font_medium)
        label_iir_title.pack(pady=10, padx=10)

        self.iir_label_frame = LabelFrame(self,
                                          text="IIR Filter",
                                          padx=5,
                                          pady=5)
        self.iir_label_frame.pack()

        # Design IIR Filter button
        btn_iir_dsn = Button(self,
                             text="Design IIR Filter",
                             width=20,
                             font=cnst.font_small,
                             bd=3,
                             command=lambda: self.iir_dsn())
        btn_iir_dsn.pack(pady=10, padx=10)

        # Clear Digital Filters
        btn_clear_d_filt = Button(self,
                                  text="Clear Both",
                                  width=20,
                                  font=cnst.font_small,
                                  bd=3,
                                  command=lambda: self.clear_d_filt())
        btn_clear_d_filt.pack(pady=10, padx=10)

    def iir_dsn(self):
        """
        Button callback, used to launch the IIR filter design frame.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        data, sampleFreq = ctrl_fileio.parse_data_file()
        if(len(data) != 0):
            cnst.df_data = data
            cnst.df_sampFreq = int(sampleFreq)
            cnst.top_wndw.show_frame(fr_iir_choose_f_type)
        else:
            messagebox.showerror("Setup Error",
                                 "No load data found!")

    def set_fir_filter_d_(self):
        """
        Button callback, used to launch the FIR filter design frame.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        data, sampleFreq = ctrl_fileio.parse_data_file()
        if len(data) != 0:
            cnst.df_data = data
            cnst.df_sampFreq = sampleFreq
            cnst.top_wndw.show_frame(fr_fir_save)
        else:
            messagebox.showerror("Setup Error",
                                 "No load data found!")

    def clear_d_filt(self):
        """
        Button callback, used to reset digital filter settings.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        # Clear FIR
        cnst.fir_filt = []

        # Clear IIR Filter
        cnst.iir_filt = []
        cnst.df_sampFreq = []
        cnst.filt_type = []
        cnst.a_pass_Freq = []
        cnst.butter_cheb_type = []


class fr_fir_save(Frame):
    """
    Frame used to allow a user to plot a filtered FIR filter and
    save the filtered signal.

    @author: sdmay18-31
    """
    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        self.filt_data = []

        # Title Text
        label_title = Label(self,
                            text="Design FIR Filter",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # FIR Filter Vector label
        label_filter_v = Label(self,
                               text="FIR Filter Vector",
                               font=cnst.font_small)
        label_filter_v.pack(pady=10, padx=10)

        self.fir_h_n = Entry(self)
        self.fir_h_n.pack(pady=0, padx=0)

        btn_iir_create = Button(self,
                                text="Create FIR",
                                width=20,
                                font=cnst.font_small,
                                bd=3,
                                command=lambda: self.firfiltcreate())
        btn_iir_create.pack(pady=10, padx=10)

        btn_iir_create = Button(self,
                                text="Save Filtered Data",
                                width=20,
                                font=cnst.font_small,
                                bd=3,
                                command=lambda: self.save_filtered_data())
        btn_iir_create.pack(pady=10, padx=10)

    def firfiltcreate(self):
        """
        Button callback, validate, accept, and create a FIR filter
        using user input and plot the outputted signal.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        input_str = self.fir_h_n.get()

        re_str = "^(((\d)+(.(\d)+)*)((\s)*,(\s)*))*(((\d)+(.(\d)+)*))+$"
        comma_num_pat = re.compile(re_str)

        if(comma_num_pat.match(input_str)):
            filt_check = digi_filt.save_imp_resp(input_str)

            if filt_check is True:
                self.filt_data = signal.convolve(cnst.fir_filt,
                                                 cnst.df_data)
                digi_filt.plt_filt_dft(self, cnst.fir_filt, self.filt_data)
            else:
                # Spawn Error Window
                messagebox.showerror("Setup Error",
                                     "FIR Filter Not Valid")
        else:
            messagebox.showerror("Setup Error",
                                 "Invalid FIR coefficients")

    def save_filtered_data(self):
        """
        Button callback, used to save outputed data.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()
        if len(self.filt_data) != 0:
            ctrl_fileio.produce_output_file(self.filt_data,
                                            cnst.df_sampFreq,
                                            "Filtered Data")
        else:
            messagebox.showerror("Setup Error",
                                 "No filtered data found!")


class fr_iir_choose_f_type(Frame):
    """
    Frame used to allow a user to select a type of IIR filter,
    eg LPF, HPF, BPF, BSF.

    @author: sdmay18-31
    """
    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="IIR Filter Type",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        F_TYPE = [("LPF", int(d_ftype.LPF)),
                  ("HPF", int(d_ftype.HPF)),
                  ("BPF", int(d_ftype.BPF)),
                  ("BSF", int(d_ftype.BSF))]

        f_type_var = IntVar()
        f_type_var.set(0)

        for text, f_type in F_TYPE:
            f_type_rad_button = Radiobutton(self,
                                            text=text,
                                            variable=f_type_var,
                                            value=f_type)
            f_type_rad_button.pack(pady=10, padx=10)

        btn_butter = Button(self,
                            text="Butterworth",
                            width=20,
                            font=cnst.font_small,
                            bd=3,
                            command=lambda:
                                self.dsn_cheb_butter(d_fproto.BUTTER,
                                                     f_type_var))
        btn_butter.pack(pady=10, padx=10)

        btn_cheb1 = Button(self,
                           text="Chebychev I",
                           width=20,
                           font=cnst.font_small,
                           bd=3,
                           command=lambda:
                                self.dsn_cheb_butter(d_fproto.CHEBY1,
                                                     f_type_var))
        btn_cheb1.pack(pady=10, padx=10)

        btn_cheb2 = Button(self,
                           text="Chebychev II",
                           width=20,
                           font=cnst.font_small,
                           bd=3,
                           command=lambda:
                                self.dsn_cheb_butter(d_fproto.CHEBY2,
                                                     f_type_var))
        btn_cheb2.pack(pady=10, padx=10)

    def dsn_cheb_butter(self, prototype, f_type):
        """
        Button callback, used to launch a frame to design a filter with a
        given characteristic and type.
        Ex. Chebychev Low-pass

        Args:
            prototype, classification of filter, eg Chebychev, Butterworth
            f_type, filter characteristic, eg, Low-pass, High-pass, etc.

        Returns:
            None
        """
        cnst = mdl_const()

        cnst.filt_type = f_type.get()

        if prototype == d_fproto.BUTTER:

            cnst.butter_cheb_type = d_fproto.BUTTER
            cnst.top_wndw.show_frame(fr_butter_params)

        elif prototype == d_fproto.CHEBY1:

            cnst.butter_cheb_type = d_fproto.CHEBY1
            cnst.top_wndw.show_frame(fr_butter_params)

        elif prototype == d_fproto.CHEBY2:
            cnst.butter_cheb_type = d_fproto.CHEBY2
            cnst.top_wndw.show_frame(fr_butter_params)


class fr_butter_params(Frame):
    """
    Frame used to allow a user to configure a IIR filter with a given
    frequency.

    @author: sdmay18-31
    """
    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        self.filt_data = []

        # Title Text
        label_title = Label(self,
                            text="Design Butterworth Filter",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Analog Pass Omega
        self.label_pass_Freq_text = StringVar()
        self.label_pass_Freq_text.set("Analog Cutoff Frequency")
        label_pass_Freq = Label(self,
                                textvariable=self.label_pass_Freq_text,
                                font=cnst.font_small)
        label_pass_Freq.pack(pady=10, padx=10)

        self.freq_slider = Scale(self,
                                 from_=cnst.adc_min,
                                 to=cnst.adc_max,
                                 resolution=5,
                                 orient="horizontal")
        self.freq_slider.pack()

        btn_iir_create = Button(self,
                                text="Create IIR",
                                width=20,
                                font=cnst.font_small,
                                bd=3,
                                command=lambda: self.iirfiltcreate())
        btn_iir_create.pack(pady=10, padx=10)

        btn_iir_create = Button(self,
                                text="Save Filtered Data",
                                width=20,
                                font=cnst.font_small,
                                bd=3,
                                command=lambda: self.save_filtered_data())
        btn_iir_create.pack(pady=10, padx=10)

    def iirfiltcreate(self):
        """
        Button callback, used to pull IIR frequency data from user input
        slider, and plot filter and plot filtered signal.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        df = digi_filt()

        cnst.a_pass_Freq = int(self.freq_slider.get())

        cnst.iir_filt = df.f_order()

        if len(cnst.iir_filt) != 0:
            self.filt_data = signal.convolve(cnst.iir_filt.real, cnst.df_data)
            digi_filt.plt_filt_dft(self, cnst.iir_filt, self.filt_data)

    def reset(self):
        """
        Refresh callback
        Updates this frame before being lifted up to the
        top context. Updates label text.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        if cnst.df_sampFreq <= cnst.adc_min:
            self.freq_slider.config(from_=cnst.df_sampFreq,
                                    to=cnst.df_sampFreq)
            self.freq_slider.set(cnst.df_sampFreq)
        else:
            self.freq_slider.config(from_=cnst.adc_min,
                                    to=cnst.df_sampFreq)
            self.freq_slider.set(cnst.df_sampFreq)

        if cnst.filt_type == d_ftype.BPF or cnst.filt_type == d_ftype.BSF:
            self.label_pass_Freq_text.set("Analog Center Frequency")
        elif cnst.filt_type == d_ftype.LPF or cnst.filt_type == d_ftype.HPF:
            self.label_pass_Freq_text.set("Analog Cutoff Frequency")

    def save_filtered_data(self):
        """
        Button callback, allows the user to output the filtered signal.

        Args:
            None

        Returns:
            None
        """
        cnst = mdl_const()

        if len(self.filt_data) != 0:
            ctrl_fileio.produce_output_file(self.filt_data,
                                            cnst.df_sampFreq,
                                            "Filtered Data")
        else:
            messagebox.showerror("Setup Error",
                                 "No filtered data found!")
