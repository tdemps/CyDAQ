from tkinter import (Frame, Label, Button)

from mdl_global import mdl_const
from gui_analog import fr_a_ch_sel


class fr_welcome(Frame):
    """
    This class creates a "welcome" frame that presents the user with
    basic information about the program and what they can do with it.

    @author: sdmay18-31
    """

    def __init__(self, parent):
        Frame.__init__(self, parent)

        cnst = mdl_const()

        # Title Text
        label_title = Label(self,
                            text="Welcome to CyDAQ",
                            font=cnst.font_large)
        label_title.pack(pady=10, padx=10)

        # Body Text
        str_info = ('This application will allow you to\n' +
                    'collect data from sensors attached to\n' +
                    'the CyDAQ collection box. Please start\n' +
                    'by pressing the button below.')

        label_info = Label(self,
                           text=str_info,
                           font=cnst.font_medium)
        label_info.pack(pady=10, padx=10)

        # Getting Started button
        top_wndw = cnst.top_wndw
        btn_start = Button(self,
                           text="Get Started",
                           command=lambda: top_wndw.show_frame(fr_a_ch_sel),
                           font=cnst.font_small)
        btn_start.pack()
