import tkinter as tk
from tkinter import *
from tkinter.ttk import Combobox
from data_handle import raw_data, open_instruction
from master_enum import enum_input, enum_filter, enum_output
from command_comm import cmd
from serial_comm import get_port

# https://www.daniweb.com/programming/software-development/code/484591/a-tooltip-class-for-tkinter



upperCorner = 20000
lowerCorner = 10000
lowerCornerMin = 100
lowerCornerMax = 20000
upperCornerMin = 2000
upperCornerMax = 40000
sampleRateMin = 100
sampleRateMax = 50000
corner = 100
inputSel = ""
filterSel = "6th Order Band Pass"
outSel = ""
sampleRate = "44100"
comm_port = ""
#serial_obj = ctrl_comm()
raw_adc_data = raw_data()
cmd_obj = cmd()

title = "CyDAQ 2.0 by ETG"

filterlabelName = "Select Filter"
sampleRateLabelName = "Select Sample Rate (Hz)"

filterNames = [
    "All Pass",
    "60 hz Notch",
    "1st Order High Pass",
    "1st Order Low Pass",
    "2nd Order Band Pass",
    "6th Order Band Pass",
    "6th Order High Pass",
    "6th Order Low Pass",
    "NA"
]

inputLabelName = "Select Input"

inputNames = [
    "5 volt",
    "3.3 volt",
    "1.8 volt",
    "Analog In",
    "Audio In",
    "Digital I2C Bus",
    "Digital SPI Bus",
    "Digital UART"
]

outLabelName = "Select Output"

outputNames = [
    "XADC",
    "Digital"
]

################ Master Class that Controls All Subclasses ########################
class CyDAQ_GUI(tk.Tk):
    def __init__(self):
        tk.Tk.__init__(self)
        self._frame = None
        self.switch_frame(CyDAQ_Config)
        global comm_port
        comm_port = get_port()

        # For debugging only
        #global filterSel
       #print("The Zybo is on " + comm_port)
       # print("The current filter is " + filterSel)

    ############## Function to Move Between Frames #############
    def switch_frame(self, frame_class):
        new_frame = frame_class(self)
        self._frame = new_frame
        self._frame.place(rely=0, relx=0)


############## The Frame containing CyDAQ Configuration ################



class CyDAQ_Config(tk.Frame):
    ############## Initializing the frame ##################
    def __init__(self, master):
        # Include the globals
        global filterSel
        global inputSel
        global outSel
        global corner
        global upperCorner
        global lowerCorner
        global sampleRate

        tk.Frame.__init__(self, master)
        root = master
        #self.master = master
        self.master.title(title)
        # Initialize the menu
        menu = Menu(self)
        self.master.config(menu=menu)


        ########################## Initialize the Menu ########################

        file = Menu(menu, tearoff=0)
        menu.add_cascade(label="File", menu=file)

        # Add drop down commands to file menu
        # file.add_command(label="Save Settings")
        #file.add_command(label="Save") # no longer needed according to Matt
        # file.add_cascade(label="Save As")
        file.add_command(label="Exit", command=self.client_exit)

        # Initialize CyDAQ Configurtion Menu
        ConfigureCydaq = Menu(menu, tearoff=0)
        menu.add_cascade(label="Configure CyDAQ", menu=ConfigureCydaq)

        # Set up Cascade for CyDAQ Configuration Menu
        ConfigureCydaq.add_command(label="Setup Instructions", command=open_instruction)
        ConfigureCydaq.add_command(label="CyDAQ Setup")

        # Setup Sampling Menu
        SampleData = Menu(menu, tearoff=0)
        menu.add_cascade(label="Sample Data", menu=SampleData)

        # Configure Sampling Menu
        # Might have to reinitialize this everytime COM port is plugged in
        COMport = Menu(SampleData, tearoff=0)
        SampleData.add_cascade(label="Test Connection", menu=COMport)
        COMport.add_command(label="Ping", command=PingCyDAQ)
        SampleData.add_command(label="Start Sampling", command=self.killAndSwitchToSample)

        ################################## Initialize System Input Select ####################################
        self.inputLabel = Label(root, text=inputLabelName, height=1, width=len(inputLabelName))
        self.inputLabel.place(relx=.25, rely=.17, anchor=CENTER)

        self.inputSelectComboBox = Combobox(root, values=inputNames)
        self.inputSelectComboBox.place(relx=0.25, rely=.25, anchor=CENTER)
        self.inputSelectComboBox.current(0)
        self.inputSelectComboBox.bind("<<ComboboxSelected>>", self.changeInput)

        ################################ Initialize Filter Select ##########################################
        self.filterLabel = Label(root, text=filterlabelName, height=1, width=len(filterlabelName))
        self.filterLabel.place(relx=.5, rely=.17, anchor=CENTER)

        self.filterSelectComboBox = Combobox(root, values=filterNames, textvariable=filterSel)
        self.filterSelectComboBox.place(relx=0.5, rely=0.25, anchor=CENTER)
        self.filterSelectComboBox.current(5)
        self.filterSelectComboBox.bind("<<ComboboxSelected>>", self.changeFilter)

        ############################# Initialize Output Select ##############################################
        self.outSelecLabel = Label(root, text=outLabelName, height=1, width=len(outLabelName))
        self.outSelecLabel.place(relx=.75, rely=.17, anchor=CENTER)

        self.outSelectComboBox = Combobox(root, values=outputNames)
        self.outSelectComboBox.place(relx=.75, rely=.25, anchor=CENTER)
        self.outSelectComboBox.current(0)
        self.outSelectComboBox.bind("<<ComboboxSelected>>", self.outChange)

        ############################## Initialize Sample Rate Select ###################################
        self.sampleRateLabel = Label(root, text=sampleRateLabelName, height=1, width=len(sampleRateLabelName))
        self.sampleRateLabel.place(relx=.5, rely=.75, anchor=CENTER)

        #sampleRate = StringVar()
        self.sampleRateEntry = Entry(root, textvariable=sampleRate, justify='center')
        self.sampleRateEntry.place(relx=.5, rely=.85, anchor=CENTER)
        self.sampleRateEntry.delete(END, 0)
        #self.sampleRateEntry.insert(END, sampleRate)
        self.sampleRateEntry.bind("<Leave>", self.check_sample)
        self.sampleRateEntry.bind("<Return>", self.check_sample)

        ################################ Initialize Lower Corner Select ################################
        self.lowerCornerLabel = Label(root, text="Lower Corner", height=1, width=len("Lower Corner"))
        self.lowerCornerSelect = Entry(root, textvariable=lowerCorner, justify='center')
        self.lowerCornerSelect.bind("<Leave>", self.changeLowerCorner)
        self.lowerCornerSelect.bind("<Return>", self.changeLowerCorner)

        ############################## Initialize Upper Corner Select ##################################
        self.upperCornerLabel = Label(root, text="Upper Corner", height=1, width=len("Upper Corner"))
        self.upperCornerSelect = Entry(root, textvariable=upperCorner, justify='center')
        self.upperCornerSelect.bind("<Leave>", self.changeUpperCorner)
        self.upperCornerSelect.bind("<Return>", self.changeUpperCorner)

        ################################# Iniialize Single Corner Select #################################
        self.cornerLabel = Label(root, text="Corner", height=1, width=len("Corner"))
        self.cornerSelect = Entry(root, textvariable=corner, justify='center')
        self.cornerSelect.bind("<Leave>", self.changeCorner)
        self.cornerSelect.bind("<Return>", self.changeCorner)

        ##################################################################################################
        ################################ Reload Previous Settings Between Frammes #########################
        ###################################################################################################

        ############################### Place the correct corner inputs ###################################
        # Place the corner for HP or LP filters
        if filterSel == filterNames[2] or filterSel == filterNames[3] or filterSel == filterNames[6] or filterSel == filterNames[7]:
            self.cornerLabel.place(relx=.5, rely=.44, anchor=CENTER)
            self.cornerSelect.place(relx=.5, rely=.55, anchor=CENTER)
        # Place upper and lower corners for bandpass filters
        if filterSel == filterNames[4] or filterSel == filterNames[5]:
            self.upperCornerSelect.place(relx=.75, rely=.55, anchor=CENTER)
            self.upperCornerLabel.place(relx=.75, rely=.44, anchor=CENTER)
            self.lowerCornerSelect.place(relx=.25, rely=.55, anchor=CENTER)
            self.lowerCornerLabel.place(relx=.25, rely=.44, anchor=CENTER)

        ############################# Make sure the comboboxes are set correctly ############################
        for x in range(0, len(filterNames)):
            if filterSel == filterNames[x]:
                self.filterSelectComboBox.current(x)

        for x in range(0, len(inputNames)):
            if inputSel == inputNames[x]:
                self.inputSelectComboBox.current(x)

        for x in range(0, len(outputNames)):
            if outSel == outputNames[x]:
                self.outSelectComboBox.current(x)

        ############################# Make sure corner selects are loaded in #################################
        self.cornerSelect.delete(0, END)
        self.cornerSelect.insert(END, corner)
        self.upperCornerSelect.delete(0, END)
        self.upperCornerSelect.insert(END, upperCorner)
        self.lowerCornerSelect.delete(0, END)
        self.lowerCornerSelect.insert(END, lowerCorner)

        ########################### Load in the sample rate ##########################
        self.sampleRateEntry.delete(0, END)
        self.sampleRateEntry.insert(END, sampleRate)
        self.updateConfig()


    # On the event that the sample rate is changed
    # Make sure the sample rate is in bounds
    def check_sample(self, event):
        checkWidget = event.widget
        checkString = checkWidget.get()
        if checkString.isnumeric():
            if (int(checkString) < sampleRateMin) or (int(checkString) > sampleRateMax):
                event.widget.delete(0, END)
                event.widget.insert(END, "44100")
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Sample rate must be between 100 and 50000 samples/s", height=5,
                                  width=len("Sample rate must be between 100 and 50000 samples/s"))
                rootTitle.pack()

        else:
            event.widget.delete(0, END)
            event.widget.insert(END, "44100")
            root = Tk()
            root.geometry("500x200")
            root.title("OUT OF BOUNDS!!!")
            rootTitle = Label(root, text="Sampler rate must be a number", height=5,
                              width=len("Sampler rate must be a number"))
            rootTitle.pack()
        self.updateConfig()

    # On the event that the filter is changed
    # make sure the inputs/outputs and corners
    # available to configure are correct
    def changeFilter(self, event):
        widget = event.widget
        self.forgetCorners()
        self.outSelectComboBox.current(0)
        if (widget.get() == filterNames[2]
                or widget.get() == filterNames[3]
                or widget.get() == filterNames[6]
                or widget.get() == filterNames[7]):
            self.cornerLabel.place(relx=.5, rely=.44, anchor=CENTER)
            self.cornerSelect.place(relx=.5, rely=.55, anchor=CENTER)
        elif (widget.get() == filterNames[4]
              or widget.get() == filterNames[5]):
            self.upperCornerSelect.place(relx=.75, rely=.55, anchor=CENTER)
            self.upperCornerLabel.place(relx=.75, rely=.44, anchor=CENTER)
            self.lowerCornerSelect.place(relx=.25, rely=.55, anchor=CENTER)
            self.lowerCornerLabel.place(relx=.25, rely=.44, anchor=CENTER)
        elif widget.get() == filterNames[8]:
            self.outSelectComboBox.current(1)
            if (not (
                    self.inputSelectComboBox.get() == inputNames[7] or
                    self.inputSelectComboBox.get() == inputNames[6] or
                    self.inputSelectComboBox.get() == inputNames[5]
            )):
                self.inputSelectComboBox.current(5)
        if not widget.get() == filterNames[8]:
            if (self.inputSelectComboBox.get() == inputNames[7] or
                    self.inputSelectComboBox.get() == inputNames[6] or
                    self.inputSelectComboBox.get() == inputNames[5]):
                self.inputSelectComboBox.current(0)
        self.updateConfig()
        self.master.filterSel = event.widget.get()


    # On the Event that an input is changed
    # Make sure all other configuration options
    # are viable
    def changeInput(self, event):
        widget = event.widget
        if (widget.get() == inputNames[6] or
                widget.get() == inputNames[7] or
                widget.get() == inputNames[5]):
            self.filterSelectComboBox.current(8)
            self.outSelectComboBox.current(1)
            self.cornerSelect.place_forget()
            self.cornerLabel.place_forget()
            self.upperCornerSelect.place_forget()
            self.upperCornerLabel.place_forget()
            self.lowerCornerSelect.place_forget()
            self.lowerCornerLabel.place_forget()

        else:
            if self.filterSelectComboBox.get() == filterNames[8]:
                self.filterSelectComboBox.current(0)
                self.forgetCorners()
        if (self.filterSelectComboBox.get() == filterNames[2]
                or self.filterSelectComboBox.get() == filterNames[3]
                or self.filterSelectComboBox.get() == filterNames[6]
                or self.filterSelectComboBox.get() == filterNames[7]):
            self.cornerLabel.place(relx=.5, rely=.44, anchor=CENTER)
            self.cornerSelect.place(relx=.5, rely=.55, anchor=CENTER)
        elif (self.filterSelectComboBox.get() == filterNames[4]
              or self.filterSelectComboBox.get() == filterNames[5]):
            self.upperCornerSelect.place(relx=.75, rely=.55, anchor=CENTER)
            self.upperCornerLabel.place(relx=.75, rely=.44, anchor=CENTER)
            self.lowerCornerSelect.place(relx=.25, rely=.55, anchor=CENTER)
            self.lowerCornerLabel.place(relx=.25, rely=.44, anchor=CENTER)
        self.updateConfig()

    # TODO: Make an error function that pops up when things are out of bounds
    # On the event that a corner is changed,
    # make sure that the corner is in bounds
    def changeUpperCorner(self, event):
        checkWidget = event.widget
        checkString = checkWidget.get()
        # Make sure the upper corner is in range
        if checkString.isnumeric():
            if int(checkString) < upperCornerMin:
                event.widget.delete(0, END)
                event.widget.insert(END, upperCornerMin)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Upper Corner must be greater than " + str(lowerCornerMin), height=5,
                                  width=len("Upper Corner must be greater than " + str(lowerCornerMin)))
                rootTitle.pack()
            if int(checkString) > upperCornerMax:
                event.widget.delete(0, END)
                event.widget.insert(END, upperCornerMax)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Upper Corner must be less than " + str(upperCornerMax), height=5,
                                  width=len("Upper Corner must be less than " + str(upperCornerMax)))
                rootTitle.pack()
            if (int(checkString) - 100) < int(self.lowerCornerSelect.get()):
                newCorner = int(self.lowerCornerSelect.get()) + 100
                event.widget.delete(0, END)
                event.widget.insert(END, str(newCorner))
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Upper Corner must be 100hz more than Lower Corner", height=5,
                                  width=len("Upper Corner must be 100hz more than Lower Corner"))
                rootTitle.pack()
        else:
            event.widget.delete(0, END)
            event.widget.insert(END, upperCornerMax)
            root = Tk()
            root.geometry("500x200")
            root.title("OUT OF BOUNDS!!!")
            rootTitle = Label(root, text="Upper Corner must be a number", height=5,
                              width=len("UpperCorner must be a number"))
            rootTitle.pack()
        self.updateConfig()

    # TODO: Use error function instead
    # On the event that a corner is changed,
    # make sure that the corner is in bounds
    def changeLowerCorner(self, event):
        checkWidget = event.widget
        checkString = checkWidget.get()
        if checkString.isnumeric():
            if int(checkString) < lowerCornerMin:
                event.widget.delete(0, END)
                event.widget.insert(END, lowerCornerMin)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Lower Corner must be greater than " + str(lowerCornerMin), height=5,
                                  width=len("Lower Corner must be greater than " + str(lowerCornerMin)))
                rootTitle.pack()
            if int(checkString) > lowerCornerMax:
                event.widget.delete(0, END)
                event.widget.insert(END, lowerCornerMax)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Lower Corner must be less than " + str(lowerCornerMax), height=5,
                                  width=len("Lower Corner must be less than " + str(lowerCornerMax)))
                rootTitle.pack()
            if int(checkString) > (int(self.upperCornerSelect.get()) - 100):
                newCorner = int(self.upperCornerSelect.get()) - 100
                event.widget.delete(0, END)
                event.widget.insert(END, str(newCorner))
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Lower Corner must be 100hz less than Upper Corner", height=5,
                                  width=len("Lower Corner must be 100hz less than Upper Corner"))
                rootTitle.pack()
        else:
            event.widget.delete(0, END)
            event.widget.insert(END, lowerCornerMin)
            root = Tk()
            root.geometry("500x200")
            root.title("OUT OF BOUNDS!!!")
            rootTitle = Label(root, text="Lower Corner must be a number", height=5,
                              width=len("Lower Corner must be a number"))
            rootTitle.pack()
        self.updateConfig()

    # TODO: Use error function instead
    # Make sure the corner is in bounds
    def changeCorner(self, event):
        checkWidget = event.widget
        checkString = checkWidget.get()
        if checkString.isnumeric():
            if int(checkString) < lowerCornerMin:
                event.widget.delete(0, END)
                event.widget.insert(END, lowerCornerMin)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Lower Corner must be greater than " + str(lowerCornerMin), height=5,
                                  width=len("Lower Corner must be greater than " + str(lowerCornerMin)))
                rootTitle.pack()
            if int(checkString) > upperCornerMax:
                event.widget.delete(0, END)
                event.widget.insert(END, upperCornerMax)
                root = Tk()
                root.geometry("500x200")
                root.title("OUT OF BOUNDS!!!")
                rootTitle = Label(root, text="Upper Corner must be less than " + str(upperCornerMax), height=5,
                                  width=len("Upper Corner must be less than " + str(upperCornerMax)))
                rootTitle.pack()
        else:
            event.widget.delete(0, END)
            event.widget.insert(END, lowerCornerMin)
            root = Tk()
            root.geometry("500x200")
            root.title("OUT OF BOUNDS!!!")
            rootTitle = Label(root, text="Lower Corner must be a number", height=5,
                              width=len("Lower Corner must be a number"))
            rootTitle.pack()
        self.updateConfig()

    def outChange(self, event):
        widget = event.widget
        if widget.get() == outputNames[0]:
            if self.filterSelectComboBox.get() == filterNames[8]:
                self.filterSelectComboBox.current(0)
                self.inputSelectComboBox.current(0)
                self.forgetCorners()

        elif widget.get() == outputNames[1]:
            if not (self.filterSelectComboBox.get() == filterNames[8]):
                self.filterSelectComboBox.current(8)
                self.inputSelectComboBox.current(5)
                self.forgetCorners()

        self.updateConfig()

    def forgetCorners(self):
        self.upperCornerLabel.place_forget()
        self.upperCornerSelect.place_forget()
        self.lowerCornerSelect.place_forget()
        self.lowerCornerLabel.place_forget()
        self.cornerLabel.place_forget()
        self.cornerSelect.place_forget()

    def killAndSwitchToSample(self):
        self.upperCornerLabel.destroy()
        self.upperCornerSelect.destroy()
        self.lowerCornerSelect.destroy()
        self.lowerCornerLabel.destroy()
        self.cornerLabel.destroy()
        self.cornerSelect.destroy()
        self.inputLabel.destroy()
        self.inputSelectComboBox.destroy()
        self.filterLabel.destroy()
        self.filterSelectComboBox.destroy()
        self.filterLabel.destroy()
        self.outSelecLabel.destroy()
        self.outSelectComboBox.destroy()
        self.sampleRateLabel.destroy()
        self.sampleRateEntry.destroy()
        self.master.switch_frame(SamplePage)

    def client_exit(self):
        exit()

    def updateConfig(self):
        ##### Taylor does not know what this block is for, ask Josh ####
        ##### Taylor thinks it would be better to stick with globals ###
        ##### because all locals are lost inbetween frames #############
        # TODO: please fix for Taylor's sanity

        ##### This is what is used to update the global variables to save state inbetween frames ########
        #### This is all the code that should be included in this function ######
        #### The stuff above should be done elsewhere, talk to Josh ###########
        global filterSel
        global outSel
        global inputSel
        global corner
        global upperCorner
        global lowerCorner
        global sampleRate
        filterSel = self.filterSelectComboBox.get()
        inputSel = self.inputSelectComboBox.get()
        outSel = self.outSelectComboBox.get()
        corner = self.cornerSelect.get()
        upperCorner = self.upperCornerSelect.get()
        lowerCorner = self.lowerCornerSelect.get()
        sampleRate = self.sampleRateEntry.get()




####################### The Page to Initialize Sampling on the CyDAQ #######################
class SamplePage(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self, master)

        ######################### Initialize the Start and Stop Buttons #######################
        root = self.master
        global startButton
        global stopButton

        stopButton = Button(root, text="Stop Sampling", font=('Helvetica', 24, "bold"), bg="red",
                            command=self.createStartButton)

        startButton = Button(root, text="Start Sampling", font=('Helvetica', 24, "bold"), bg="green",
                             command=self.createStopButton)
        startButton.place(relx=.5, rely=.5, anchor=CENTER)

        ######################## Initialize the Menu ########################
        menu = Menu(self)
        self.master.config(menu=menu)
        # Initialize the file menu
        file = Menu(menu, tearoff=0)
        menu.add_cascade(label="File", menu=file)

        # Add drop down commands to file menu
        # file.add_command(label="Save Settings")
        #file.add_command(label="Save") # Not needed according to Matt
        # file.add_cascade(label="Save As")
        file.add_command(label="Exit", command=self.client_exit)

        # Initialize CyDAQ Configurtion Menu
        ConfigureCydaq = Menu(menu, tearoff=0)
        menu.add_cascade(label="Configure CyDAQ", menu=ConfigureCydaq)

        # Set up Cascade for CyDAQ Configuration Menu
        ConfigureCydaq.add_command(label="Setup Instructions", command=open_instruction)
        ConfigureCydaq.add_command(label="CyDAQ Setup", command=self.killSwitchConfig)

        # Setup Sampling Menu
        SampleData = Menu(menu, tearoff=0)
        menu.add_cascade(label="Sample Data", menu=SampleData)

        # Configure Sampling Menu
        # Might have to reinitialize this every time COM port is plugged in
        COMport = Menu(SampleData, tearoff=0)
        SampleData.add_cascade(label="Test Connection", menu=COMport)
        COMport.add_command(label="Ping", command=PingCyDAQ)
        SampleData.add_command(label="Start Sampling")
        self.inputEnum = None
        self.outputEnum = None
        self.sampleRate = sampleRate
        self.filterEnum = None
        self.lowerCorner = lowerCorner
        self.upperCorner = upperCorner
        self.midCorner = corner

    def createStartButton(self):
        stopButton.place_forget()
        startButton.place(relx=.5, rely=.5, anchor=CENTER)
        print("Stopping")
        cmd_obj.send_stop_cmd(comm_port)
        if not raw_adc_data.collect_data(sampleRate, comm_port):
            print("Error occurred getting data")

    def createStopButton(self):
        self.nameToEnum()
        global comm_port
        if(comm_port == "" or comm_port is None):
            comm_port = get_port()
        result = cmd_obj.send_parameters(comm_port, self.inputEnum, self.sampleRate, self.filterEnum, self.upperCorner, self.lowerCorner, self.midCorner)
        if result is False:
            print("Error connecting to cyDaq, try ping test to confirm")
            return
        startButton.place_forget()
        stopButton.place(relx=.5, rely=.5, anchor=CENTER)
        print("Starting")

    def nameToEnum(self):
        global inputSel, filterSel, outSel
        if inputSel == "5 volt":
            self.inputEnum = enum_input.volt_5.value
        elif inputSel == "3.3 volt":
            self.inputEnum = enum_input.volt_3.value
        elif inputSel == "1.8 volt":
            self.inputEnum = enum_input.volt_1.value
        elif inputSel == "Analog In":
            self.inputEnum = enum_input.analog_in.value
        elif inputSel == "Audio In":
            self.inputEnum = enum_input.audio_in.value
        elif inputSel == "Digital I2C Bus":
            self.inputEnum = enum_input.digital_i2c.value
        elif inputSel == "Digital SPI Bus":
            self.inputEnum = enum_input.digital_spi_bus.value
        elif inputSel == "Digital UART":
            self.inputEnum = enum_input.digital_uart.value
        else:
            self.inputEnum = enum_input.analog_in.value


        if filterSel == "All Pass":
            self.filterEnum = enum_filter.NO_FILTER.value
        elif filterSel == "60 hz Notch":
            self.filterEnum = enum_filter.NOTCH.value
        elif filterSel == "1st Order High Pass":
            self.filterEnum = enum_filter.HP1.value
        elif filterSel == "1st Order Low Pass":
            self.filterEnum = enum_filter.LP1.value
        elif filterSel == "2nd Order Band Pass":
            self.filterEnum = enum_filter.BP2.value
        elif filterSel == "6th Order High Pass":
            self.filterEnum = enum_filter.HP6.value
        elif filterSel == "6th Order Low Pass":
            self.filterEnum = enum_filter.LP6.value
        elif filterSel == "6th Order Band Pass":
            self.filterEnum = enum_filter.BP6.value
        else:
            self.filterEnum = enum_filter.NO_FILTER.value

        if outSel == "XADC":
            output = enum_output.xadc.value
        elif outSel == "Digital":
            output = enum_output.digital.value

    def client_exit(self):
        exit()

    def killSwitchConfig(self):
        stopButton.destroy()
        startButton.destroy()
        self.master.switch_frame(CyDAQ_Config)


################# a page to be created later so ignore for now #######################
class PageTwo(tk.Frame):
    def __init__(self, master):
        tk.Frame.__init__(self, master)
        tk.Frame.configure(self, bg='red')
        tk.Label(self, text="Page two", font=('Helvetica', 18, "bold")).pack(side="top", fill="x", pady=5)
        tk.Button(self, text="Go back to start page",
                  command=lambda: master.switch_frame(CyDAQ_Config)).place(rely=.5, relx=.5)


class PingCyDAQ:
    def __init__(self):
        root = Tk()
        root.geometry("200x100")
        root.title("ETG")
        rootTitle = Label(root, text="Connection Test", height=1, width=len("Connection Test"))
        rootTitle.pack()
        if cmd_obj.ping_zybo(comm_port):
            print("Test Passed")
            global ping_success_label
            ping_success_labelName = "Ping Successful"
            ping_success_label = Label(root, text=ping_success_labelName, height=1, width=len(ping_success_labelName),
                                    relief=RAISED)
            ping_success_label.place(relx=.5, rely=.5, anchor=CENTER)
        else:
            global ping_fail_label
            ping_fail_labelName = "Ping Failed"
            ping_fail_label = Label(root, text=ping_fail_labelName, height=1, width=len(ping_fail_labelName),
                                       relief=RAISED)
            ping_fail_label.place(relx=.5, rely=.5, anchor=CENTER)
        root.mainloop()


################### initialize and run ####################
app = CyDAQ_GUI()
app.geometry("700x300")
app.mainloop()