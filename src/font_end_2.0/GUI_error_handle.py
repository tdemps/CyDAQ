import serial
import serial.tools.list_ports
import time
import threading
from tkinter import *
from tkinter import messagebox

finish = True
initial = True
count = 0


class App:
    def __init__(self, master):
        global finish
        master.geometry("400x200")
        master.title("Zybo Connection Error")
        text_1 = Label(master, text="Zybo Not Connected",
                          font=("Helvetica", 16)).place(x=100, y=25)
        text_2 = Label(master, text="Please make sure the Zybo is turned on and the",
                          font=("Helvetica", 12)).place(x=30, y=50)
        text_2 = Label(master, text="cable is plugged in",
                          font=("Helvetica", 12)).place(x=135, y=75)
        finish = False


def InfiniteProcess():
    while not check_connection():
        pass


def check_connection():
    global finish
    global count
    all_ports = serial.tools.list_ports.comports()
    open_ports = []
    for element in all_ports:
        count += 1
        print("Times port has been checked: " + str(count))
        if "USB Serial Port" in element.description:
            open_ports.append(element.device)
    try:
        zybo_port = open_ports[0]
        port = str(zybo_port)
        print("connected (Initially)")
        finish = True
        if initial:
            return True
        else:
            mainWindow.destroy()
            Process.cancel()
            return True
    except:
        return False


if not check_connection():
    initial = False
    print("not connected (Initially)")
    Process = threading.Timer(.1, InfiniteProcess)
    Process.daemon = True
    Process.start()
    mainWindow = Tk()
    app = App(mainWindow)
    mainWindow.mainloop()


