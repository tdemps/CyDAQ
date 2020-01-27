from serial_comm import ctrl_comm
from master_enum import parameter_options_output, parameter_options_filter, parameter_options_input, parameter_options, sig_serial
import numpy as np
from tkinter import filedialog, messagebox
import csv
import subprocess
import struct
import time as t
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

        data_array = bytearray()
        time = 0
        sec_per_sample = 1 / int(sampling_rate)
        end_seq = '00000000'.encode('ascii')
        serial_obj.open(zport)

        while serial_obj.read_byte() != sig_serial.START_BYTE.value:
            pass

        print("Reading Samples...", end='')
        try:
            # data_array.append(serial_obj.getSerialObj().read_until())
            while(1):
                pending = serial_obj.getSerialObj().inWaiting()
                if pending:
                    read = serial_obj.getSerialObj().read(pending)
                    data_array.extend(read)
                elif( len(data_array) > 8 and data_array[-len("00000000"):] == end_seq):
                    break
                    # pending = serial_obj.getSerialObj().inWaiting()
                    # if pending:
                    #     data_array.append(serial_obj.getSerialObj().read(pending))
                    # elif errcnt > 5:
                    #     break
                    # else:
                    #     t.sleep(0.2)
                    #     errcnt += 1
                    # while(cnt < sample_burst_size):
                    #     sample = serial_obj.read_uint16()
                    #     data_array.append(sample)
                    #     cnt += 1
            print('done.')
            serial_obj.close()
            data_array = data_array.rstrip(end_seq)
            messagebox.showinfo("Communication Message",
                                 "Sample transfer complete, " + str(len(data_array)/2) + " samples received.")
            print('Processing Samples...', end='')

            test = struct.iter_unpack("<H", data_array)
            np_data_array = np.empty(int(len(data_array) / 2), dtype=np.uint16)
            np_time_array = np.empty(int(len(data_array) / 2), dtype=np.float64)
            np_voltage_array = np.empty(int(len(data_array) / 2), dtype=np.float64)
            i = 0
            for sample in test:  # in range(0, len(data_array), 2):
                np_data_array[i] = (sample[0] & 4095)  #((data_array[i+1] << 8) | data_array[i]) & 4095
                np_voltage_array[i] = (sample[0] / 4096.0)
                time += sec_per_sample
                np_time_array[i] = (round(time, 6))
                i += 1

            #np_voltage_array = np.array(np_voltage_array)
            final_data = np.vstack((np_time_array, np_data_array, np_voltage_array))
            print('done')
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


        try:
            f = filedialog.asksaveasfile(mode='w', defaultextension=".csv", filetypes=(("CSV file", ".*csv"),
                                                                                       ("All Files", "*.*")))
            if f is None:  # asksaveasfile return `None` if dialog closed with "cancel".
                return
            with open(f.name, 'w', newline='') as output:
                print("Writing to:",f.name)
                w = csv.writer(output)
                w.writerow(['Time (s)','Value (Decimal)', 'Voltage (V)'])
                w.writerows(data_array)

            f.close()
            print("File Write Complete.")
        except Exception as e:
            print("Error saving csv, do you have that file open currently?")


class open_instruction:
    def open_pdf(self):
        chrome_address = r'C:\Program Files (x86)\Google\Chrome\Application\chrome.exe'
        file_path = r'U:\ETG\EE_224\src\font_end_2.0\This is a test.pdf'
        subprocess.Popen([chrome_address, file_path], shell=True)
