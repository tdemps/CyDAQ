from mdl_global import mdl_const
from mdl_digital import (d_ftype, d_fproto)
import math
import numpy
import scipy.signal as signal
import matplotlib
matplotlib.use("TkAgg")
import matplotlib.pyplot as plt  # noqa


class digi_filt:
    """
    This class defines a table of commands that are sent between the firmware
    and the front-end for adc configuration.

    @author: sdmay18-31
    """

    def c_freq(freq, period):
        return (2 * math.atan((2 * math.pi) *
                              (freq * period) /
                              (2)) / math.pi)

    def f_order(self):
        """
        This function takes in an angular cut-off frequency,
        an angular min attenuation freq (M.A.F), and the M.A. dB value
        and outputs the filter order.

        This function is assuming the frequencies have already
        been prewarped.

        This order calculation is based on the fact that the
        30 dB attenuation for M.A.F. This has been generalized.

        Args:
            None

        Returns:
            The one-dimensional inverse discrete Fourier Transform of
            the current signal.
        @author: sdmay18-31
        """
        cnst = mdl_const()
        period = 1 / cnst.df_sampFreq

        # Convert analog frequencies into digital frequencies
        if cnst.filt_type == d_ftype.BPF or cnst.filt_type == d_ftype.BSF:
            # Calculating bandwidth for BPF and BSF
            a_pass_Freq1 = cnst.a_pass_Freq*7/8

            if cnst.a_pass_Freq*9/8 <= cnst.df_sampFreq:
                a_pass_Freq2 = cnst.a_pass_Freq*9/8
            else:
                a_pass_Freq2 = cnst.df_sampFreq

            d_pass_Freq1 = digi_filt.c_freq(a_pass_Freq1, period)
            d_pass_Freq2 = digi_filt.c_freq(a_pass_Freq2, period)
            d_stop_Freq1 = digi_filt.c_freq(1, period)
            d_stop_Freq2 = digi_filt.c_freq(cnst.df_sampFreq+1500, period)
            d_pass_Freq = [d_pass_Freq1, d_pass_Freq2]
            d_stop_Freq = [d_stop_Freq1, d_stop_Freq2]
        elif cnst.filt_type == d_ftype.LPF or cnst.filt_type == d_ftype.HPF:
            d_pass_Freq = digi_filt.c_freq(cnst.a_pass_Freq, period)
            d_stop_Freq = digi_filt.c_freq(cnst.df_sampFreq+1500, period)

        # Determining order and frequencies for filters
        if cnst.butter_cheb_type == d_fproto.BUTTER:
            order, crit_freqs = signal.buttord(d_pass_Freq,
                                               d_stop_Freq,
                                               3,
                                               40,
                                               analog=False)
        elif cnst.butter_cheb_type == d_fproto.CHEBY1:
            order, crit_freqs = signal.cheb1ord(d_pass_Freq,
                                                d_stop_Freq,
                                                3,
                                                40,
                                                analog=False)
        elif cnst.butter_cheb_type == d_fproto.CHEBY2:
            order, crit_freqs = signal.cheb2ord(d_pass_Freq,
                                                d_stop_Freq,
                                                3,
                                                40,
                                                analog=False)

        # Set the filter-type string:
        b_type = 'highpass'
        if cnst.filt_type == d_ftype.BPF:
            b_type = 'bandpass'
        elif cnst.filt_type == d_ftype.BSF:
            b_type = 'bandstop'
        elif cnst.filt_type == d_ftype.HPF:
            b_type = 'highpass'
        elif cnst.filt_type == d_ftype.LPF:
            b_type = 'lowpass'

        # Calculate filter numerator and denominator
        if cnst.butter_cheb_type == d_fproto.BUTTER:
            num, denom = signal.butter(order,
                                       crit_freqs,
                                       b_type,
                                       analog=False,
                                       output='ba')

        elif cnst.butter_cheb_type == d_fproto.CHEBY1:
            num, denom = signal.cheby1(order,
                                       1,
                                       crit_freqs,
                                       b_type,
                                       analog=False,
                                       output='ba')

        elif cnst.butter_cheb_type == d_fproto.CHEBY2:
            num, denom = signal.cheby2(order,
                                       40,
                                       crit_freqs,
                                       b_type,
                                       analog=False,
                                       output='ba')
        # Construct filter response
        freq_w, resp_w = signal.freqz(num, denom)
        # Calculate impulse response
        you_t = numpy.fft.ifft(resp_w, len(resp_w))

        return you_t

    def plt_filt_dft(self, filt_data, signal_data):
        """
        This function takes the FFT of any signal (collected, filtered, etc.)
        and plots it.

        Args:
            filt_data, list of filter coefficients
            signal_data, list of signal data points

        Returns:
            None
        """
        cnst = mdl_const()
        s_period = 1/cnst.df_sampFreq

        filt_resp = numpy.fft.fft(filt_data, len(filt_data))

        # Calculate analog frequency vector
        freq = numpy.arange(0, math.pi, math.pi/len(filt_data))
        w_freqs_a = 2*numpy.tan(freq/2)/(2*math.pi*s_period)
        for i in range(0, len(w_freqs_a)):
            if((w_freqs_a[i] >= cnst.df_sampFreq) or
               (w_freqs_a[i] >= (cnst.df_sampFreq + 100))):
                w_freqs_a = w_freqs_a[:(i+1)]
                break

        # Calculate magnitude in decibels
        filt_magnitude = numpy.absolute(filt_resp)

        where_zero = []

        for i in range(0, len(filt_magnitude)):
            where_zero.append(filt_magnitude[i] != 0)

        filt_magnitude_dB = 20*numpy.log10(filt_magnitude,
                                           where=where_zero)

        filt_magnitude_dB = filt_magnitude_dB[:len(w_freqs_a)]

        signal_resp = numpy.fft.fft(signal_data, len(signal_data))

        signal_magnitude = numpy.absolute(signal_resp)

        where_zero = []

        for i in range(0, len(signal_magnitude)):
            where_zero.append(signal_magnitude[i] != 0)

        signal_magnitude_dB = 20*numpy.log10(signal_magnitude,
                                             where=where_zero)
        signal_magnitude_dB = signal_magnitude_dB[:len(w_freqs_a)]

        # Plot frequency response
        plt.figure(1)
        plt.plot(w_freqs_a, filt_magnitude_dB)
        plt.ylabel('Magnitude (dB)')
        plt.xlabel('Frequency (Hz)')
        plt.title('Frequency Response of Filter')
        plt.grid()

        # Plot filtered signal
        time2 = numpy.arange(len(signal_data))*1/cnst.df_sampFreq
        plt.figure(2)
        plt.plot(time2, signal_data)
        plt.ylabel('Amplitude')
        plt.xlabel('Time')
        plt.title('Filtered Signal')
        plt.grid()
        plt.show()

    def save_imp_resp(h_n_str):
        """
        This function takes input string from GUI,
        converts it to vector, and saves it to fir_filter
        or iir_filter in mdl_global, based on fir_or_iir bool.

        save_imp_resp casts each element of h_n_str into a float.
        h_n_str must be a comma separated list:

            ex.
            input:  '1, 2, 3, 4, 5'
            output: [1. 2. 3. 4. 5.]

        Args:
            h_n_str: comma separated string of doubles

        Returns:
            True if the filter could be created, false otherwise.
        """

        const = mdl_const()
        if len(h_n_str) == 0:
            filt_valid = False
        else:
            filt_valid = True
            const.fir_filt = numpy.fromstring(h_n_str, dtype=float, sep=',')

        return filt_valid
