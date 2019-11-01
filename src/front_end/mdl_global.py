from pathlib import Path
from mdl_firmware import mdl_fw_vals


class mdl_const:
    """
    Stores the global state of the CyDAQ application.

    This class is a singleton. Only one instance will be returned on a
    call to the constructor. Do not build classes that inherit from this
    class.

    @author: sdmay18-31
    """
    # Concrete instance of mdl_const
    __inst = None
    __init_complete = False

    def __init__(self):

        if mdl_const.__init_complete is True:
            return

        # Application Name
        self.app_name = "CyDAQ"

        # Fonts
        font_type = 'Arial'
        font_lrg_size = 20
        font_med_size = 16
        font_reg_size = 12
        self.font_large = (font_type, font_lrg_size)
        self.font_medium = (font_type, font_med_size)
        self.font_small = (font_type, font_reg_size)

        # Window Size
        self.wndw_width = 750
        self.wndw_height = 500

        # By default, we will output to the user's home directory
        self.fout_dflt_dir = str(Path.home())

        # Limits for filter frequencies
        self.freq_min = 0
        self.freq_max = 8000

        # Limits for ADC Sampling Rates
        self.adc_min = 20
        self.adc_max = 8000

        # 5 Variables for Collected Data
        # TODO Deprecate these lists
        self.dat_adc0 = []
        self.dat_adc1 = []
        self.dat_adc2 = []
        self.dat_i2c = []
        self.dat_spi = []

        # 5 Variables for Digitally Filtered Data
        self.df_adc0 = []
        self.df_adc1 = []
        self.df_adc2 = []
        self.df_i2c = []
        self.df_spi = []

        # Digital Filter Data
        self.df_data = []
        self.df_sampFreq = 0

        # FIR Filter Vector
        self.fir_filt = []

        # IIR Filter Vector
        self.iir_filt = []

        self.butter_cheb_type = []
        self.filt_type = []
        self.a_pass_Freq = []

        # Instantiate global instance of settings
        self.fws = mdl_fw_vals()

        # Reference to top window used for changing frames -TVT
        self.top_wndw = None

        # Timer for refreshing ports
        self.port_timer = None

        mdl_const.__init_complete = True

    def __new__(cls):

        if mdl_const.__inst is None:
            mdl_const.__inst = object.__new__(cls)

        return mdl_const.__inst
