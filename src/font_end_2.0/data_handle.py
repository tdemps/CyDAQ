from serial_comm import ctrl_comm
from master_enum import parameter_options_output, parameter_options_filter, parameter_options_input, parameter_options, sig_serial
import numpy as np
from tkinter import filedialog, messagebox
import csv
import subprocess

serial_obj = ctrl_comm()
port = ""


class raw_data:
    def __adc_data(self, sampling_rate, zport):
        """
        gets the adc data from the zybo, parses the data, puts it into a numpy array

        :param sampling_rate: sampling rate, used to determine the time of each sample
        :param zport: zybo port
        :return: numpy array containing all the data and time
        """
        try:
            data_array = []
            new_data_array = []
            time_array = []
            time = 0
            serial_obj.open(zport)
            last = ""

            while serial_obj.read_byte() != sig_serial.START_BYTE.value:
                pass

            try:
                while True:
                    sample = serial_obj.read_uint16()
                    data_array.append(sample)
            except Exception as e:
                messagebox.showerror("Communication Message",
                                     "Sample Transfer Complete")
            # while byte_char != sig_serial.END_BYTE.value or last != sig_serial.END_BYTE.value:
            #     byte_value = serial_obj.read_uint8()
            #     last = byte_char
            #     byte_char = chr(byte_value)
            #     if byte_char != sig_serial.END_BYTE.value:
            #         data_array.append(byte_value)
            for i in range(0, len(data_array)):
                # sum1 = (256 * data_array[i+1]) + data_array[i] #zybo sends MSB first
                new_data_array.append(data_array[i] & 4095)
                time += 1 / int(sampling_rate)
                time_array.append(round(time, 6))

            np_data_array = np.array(new_data_array)
            np_time_array = np.array(time_array)
            final_data = np.vstack((np_time_array, np_data_array))
            serial_obj.close()
            return final_data
        except AttributeError as e:
            print("No data was recorded (timeout error)")
            serial_obj.close()

    def collect_data(self, sampling_rate, zport):
        """
        collects data that is being sent from the zybo

        :param sampling_rate: sampling rate, used to determine the time of each sample
        :param zport: zybo port
        :return: True or False depending on if there is a problem or not
        """
        data_array = self.__adc_data(sampling_rate, zport)
        try:
            data_array_T = data_array.T
        except AttributeError:
            return False
        try:
            self.__file_save(data_array_T)
        except AttributeError:
            print("No data was recorded (timeout error)")
        # print(data_array)
        return True

    def __file_save(self, data_array):
        """
        saves 'data_array' to a csv to whatever location the user defines in the pop up

        :param data_array: the numpy array containing all the data
        :return:

        """
        f = filedialog.asksaveasfile(mode='w', defaultextension=".csv", filetypes=(("CSV file", ".*csv"),
                                                                                   ("All Files", "*.*")))
        if f is None:  # asksaveasfile return `None` if dialog closed with "cancel".
            return
        try:
            with open(f.name, 'w', newline='') as output:
                w = csv.writer(output)
                w.writerow(['Time'] + ['Value'])
                w.writerows(data_array)
        except Exception as e:
            print("Error saving csv, do you have that file open currently?")


class open_instruction:
    def open_pdf(self):
        chrome_address = r'C:\Program Files (x86)\Google\Chrome\Application\chrome.exe'
        file_path = r'U:\ETG\EE_224\src\font_end_2.0\This is a test.pdf'
        subprocess.Popen([chrome_address, file_path], shell=True)
