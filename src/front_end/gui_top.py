from tkinter import (Frame, Menu, Tk)

from gui_analog import fr_a_ch_sel
from gui_analog import fr_a_f_sel
from gui_digital import (fr_d_filter_gen, fr_butter_params,
                         fr_iir_choose_f_type, fr_fir_save)
from gui_curr_cfg import fr_curr_cfg
from gui_collect import fr_collect
from gui_welcome import fr_welcome
from gui_digital_sen import fr_d_sen_sel

from mdl_global import mdl_const

from ctrl_settings import ctrl_settings


class main_wndw(Tk):
    """
    Top level window containing frames for the CyDAQ application.

    @author: sdmay18-31
    """

    def cleanup():
        """
        Handles cleaning up on a window close

        Args:
            None

        Returns:
            None
        """

        cnst = mdl_const()

        if cnst.port_timer.isAlive():
            cnst.port_timer.cancel()

        cnst.__tk.destroy()

    def show_frame(self, new_frame):
        """
        Updates the right frame and the text on the left frame.

        Args:
            new_frame: reference to frame that will populate the
                right frame. Must be a member of self.frames.

        Returns:
            None
        """
        frame = self.frames[new_frame]

        # Call reset methods for frames that support the feature
        if hasattr(frame, 'reset') and callable(getattr(frame, 'reset')):
            frame.reset()

        frame.tkraise()
        self.left_frameFrame.update_text()

    def __init__(self, *args, **kwargs):
        Tk.__init__(self, *args, **kwargs)

        # Initialize state data
        cnst = mdl_const()
        cnst.top_wndw = self
        self.winfo_toplevel().title(cnst.app_name)

        # Save toplevel
        cnst.__tk = self

        # Set window at a fixed size
        self.minsize(cnst.wndw_width, cnst.wndw_height)
        self.maxsize(cnst.wndw_width, cnst.wndw_height)

        # Initialize Menu bar
        self.__menuBar = Menu(self)

        menubar = self.__menuBar
        menubar.add_command(label="Home",
                            command=lambda: self.show_frame(fr_welcome))
        menubar.add_command(label="Load Settings",
                            command=ctrl_settings.promnt_load)
        menubar.add_command(label="Save Settings",
                            command=ctrl_settings.promnt_save)
        menubar.add_command(label="ADC Channel Select",
                            command=lambda: self.show_frame(fr_a_ch_sel))
        menubar.add_command(label="Collect Data",
                            command=lambda: self.show_frame(fr_collect))
        menubar.add_command(label="Digital Sensor Select",
                            command=lambda: self.show_frame(fr_d_sen_sel))
        menubar.add_command(label="Digital Filter Gen",
                            command=lambda: self.show_frame(fr_d_filter_gen))

        self.config(menu=menubar)

        # Initialize Left Frame
        self.left_frame = Frame(self,
                                width=(cnst.wndw_width/2),
                                height=cnst.wndw_height,
                                pady=10,
                                padx=10)
        self.left_frame.pack(side="left", anchor='w')
        self.left_frame.grid_rowconfigure(0, weight=1)
        self.left_frame.grid_columnconfigure(0, weight=1)

        self.left_frameFrame = fr_curr_cfg(self.left_frame)
        self.left_frameFrame.grid(row=0, column=0, sticky="nsew")

        # Initialize Right Frame
        self.right_frame = Frame(self,
                                 width=(cnst.wndw_width/2),
                                 height=cnst.wndw_height,
                                 pady=10,
                                 padx=25)
        self.right_frame.pack(side="right", anchor='e')
        self.right_frame.grid_rowconfigure(0, weight=1)
        self.right_frame.grid_columnconfigure(0, weight=1)

        self.frames = {}

        # Setup list of items that can be used in the right frame
        for F in (fr_welcome,
                  fr_d_sen_sel,
                  fr_d_filter_gen,
                  fr_butter_params,
                  fr_iir_choose_f_type,
                  fr_a_ch_sel,
                  fr_a_f_sel,
                  fr_collect,
                  fr_fir_save):
            frame = F(self.right_frame)
            self.frames[F] = frame
            frame.grid(row=0, column=0, sticky="nsew")

        # Start by displaying the initial frame
        self.show_frame(fr_welcome)

        # Bind cleanup handler
        cnst.__tk.protocol("WM_DELETE_WINDOW", main_wndw.cleanup)


