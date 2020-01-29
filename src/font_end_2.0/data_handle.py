from serial_comm import ctrl_comm
from master_enum import enum_output, enum_filter, enum_input, enum_commands, sig_serial
import numpy as np
from tkinter import filedialog, messagebox
import csv
import subprocess
import struct
from os import path
import scipy.io as io

serial_obj = ctrl_comm()

def open_instruction():
    chrome_address = r'C:\Program Files (x86)\Google\Chrome\Application\chrome.exe'
    file_path = path.join(path.dirname(path.abspath(__file__)),r'This is a test.pdf')
    subprocess.Popen([chrome_address, file_path], shell=True)

class raw_data:
    def __adc_data(self, sampling_rate, zport):
        """
        gets the adc data from the zybo, parses the data, puts it into a numpy array

        :param sampling_rate: sampling rate, used to determine the time of each sample
        :param zport: zybo port
        :return: numpy array containing all the data and time
        """
        #stores incoming bytes from serial
        data_array = bytearray()
        #used to assign timestamps to each sample
        time = 0
        #increment added to give samples timestamps. Initialized as variable to save clocks
        sec_per_sample = 1 / int(sampling_rate)
        #end sequence sent by zybo saying sample transfer is complete
        end_seq = '00000000'.encode('ascii')

        serial_obj.open(zport)

        #wait for start character from zybo before reading data
        while serial_obj.read_byte() != sig_serial.START_BYTE.value:
            pass

        print("Reading Samples...", end='')
        try:
            while(1):
                pending = serial_obj.getSerialObj().inWaiting()
                if pending:
                    read = serial_obj.getSerialObj().read(pending)
                    data_array.extend(read)
                elif( len(data_array) > 8 and data_array[-len("00000000"):] == end_seq):
                    break

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
            f = filedialog.asksaveasfile(mode='w', defaultextension=".csv", filetypes=[("CSV File", ".*csv"),("MAT File", ".mat"),
                                                                                       ("All Files", "*.*")])
            if f is None:  # asksaveasfile return `None` if dialog closed with "cancel".
                return
            print("Writing to:", f.name, '...', end='')

            if f.name[-4:] == '.csv':
                with open(f.name, 'w', newline='') as output:
                    w = csv.writer(output)
                    w.writerow(['Time (s)','Value (Decimal)', 'Voltage (V)'])
                    w.writerows(data_array)

            elif f.name[-4:] == '.mat':
                tmp = dict(data=data_array[:,1].tolist(), time=data_array[:,0].tolist())
                io.savemat(file_name=str(f.name),appendmat=False, mdict=tmp)

            f.close()
            print("Done.")
        except Exception as e:
            print("Error saving file, do you have that file open in another program?")




