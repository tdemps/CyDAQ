import numpy
import re
import csv

from scipy import io
from tkinter import filedialog

from mdl_global import mdl_const


class ctrl_fileio:
    """
    This class defines control logic for file output and input functions.
    of the front end.

    @author: sdmay18-31
    """

    def parse_data_file():
        """
        Generate a data list and sampling rate based on a opened MAT or
        CSV file. Prompts the user for a file to open.

        Args:
            None

        Returns:
            data_list: List of data samples
            data_srate: Sampling rate of data file
        """
        cnst = mdl_const()
        ret_list = []
        ret_srate = 0

        filetype_arg = [("CSV/MAT (*.csv, *.mat)",
                         "*.csv *.mat")]

        f_name = filedialog.askopenfilename(initialdir=cnst.fout_dflt_dir,
                                            title="Open data file: ",
                                            filetypes=filetype_arg)

        csv_pat = re.compile("^(.)+(.csv)$")
        mat_pat = re.compile("^(.)+(.mat)$")

        if(csv_pat.match(f_name)):
            with open(str(f_name), 'r') as f_ptr:
                reader = csv.reader(f_ptr,
                                    delimiter=',')
                c_time = 0
                i = 0

                for row in reader:
                    # Skip first row with the column labels
                    if(i > 0):
                        c_time = float(row[0])
                        ret_list.append(int(row[1]))
                    i = i + 1

                ret_srate = len(ret_list) / c_time

        elif(mat_pat.match(f_name)):
                mat = io.loadmat(f_name)

                for elem in mat["data"][0]:
                    ret_list.append(int(elem))

                ret_srate = len(ret_list) / float(mat["time"][0][-1:])

        return ret_list, ret_srate

    def produce_output_file(input_list, freq, sensor_name):
        """
        Generate an output based on an input list. Promnts the user to
        select an output file and calls the appropriate output function.

        Args:
            input_list: input data list
            freq: sampling rate of the input data

        Returns:
            None
        """
        fname = ctrl_fileio.promnt_get_out_file(sensor_name)

        input_list = [int(i) for i in input_list]

        # Regex for checking if we are outputing a MAT or a CSV
        csv_pat = re.compile("^(.)+(.csv)$")
        mat_pat = re.compile("^(.)+(.mat)$")

        if(csv_pat.match(fname)):
            ctrl_fileio.csv_gen(input_list, fname, freq)
        elif(mat_pat.match(fname)):
            ctrl_fileio.mat_gen(input_list, fname, freq)

    def promnt_get_out_file(title_str):
        """
        Prompts the user to select a CSV or MAT file for output.

        Args:
            None

        Returns:
            Full path to output file
        """
        cnst = mdl_const()

        f_name = filedialog.asksaveasfilename(initialdir=cnst.fout_dflt_dir,
                                              title="Save File for: " +
                                                    str(title_str),
                                              defaultextension=".csv",
                                              filetypes=[("CSV (*.csv)",
                                                          "*.csv"),
                                                         ("MAT (*.mat)",
                                                          "*.mat")])
        return f_name

    def csv_gen(input_list, output_file, freq):
        """
        Generate a formatted CSV file from a raw input CSV.

        Args:
            input_list: input data list
            output_file: full path to output file
            freq: sampling frequency of the data

        Returns:
            None
        """

        csv_output = numpy.array(input_list)
        numSamples = numpy.arange(0, csv_output.size)

        with open(str(output_file), 'w+') as f2:

            f2.write("{}, {}\n".format("TIME", "DATA"))

            for x in zip(numSamples/freq,
                         csv_output):
                f2.write("{},{}\n".format(x[0], x[1]))

        f2.close()

    def mat_gen(input_list, output_file, freq):
        """
        Generate a formatted MAT file from a raw input CSV.

        Args:
            input_list: input data list
            output_file: full path to output file
            freq: sampling frequency of the data

        Returns:
            None
        """

        mat_output = numpy.array(input_list)
        numSamples = numpy.arange(0, mat_output.size)

        io.savemat(file_name=str(output_file),
                   appendmat=True,
                   mdict=dict(data=mat_output,
                              time=numSamples/freq))
