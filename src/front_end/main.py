"""
Program entry point for CyDAQ.

@author: sdmay18-31
"""
from gui_top import main_wndw

app = main_wndw()
app.mainloop()

"""
      4 bytes
cmd = command (srst = sample rate set, srgt = sample rate get, fbst =  set active filter, fbgt = get active filter,
                inst = input set, ingt = input get, strt = start sampling, stop = stop sampling, fbtn = filter board tune)

end of command is an an "!"
if the command has multiple values in command, seperate them with with commas
if zybo doesnt need to send anything, it will send 'ack!'

srst = '8 digits in samples per second'!, expected value will be 'ack!'
srgt = srgt!, expected value to receive will be srgt='8 bit number'!
fbst = enum, 2 character! expected value will be 'ack!'
fbgt = fbgt!, expected value to receive will be fbgt='2 characters'!
inst = enum, 2 character! expected value will be 'ack!'
ingt = fbgt!, expected value to receive will be ingt='2 characters'!
strt = strt!, expected value will be 'ack!'
stop = stop!, expected value will be 'ack!'
fbtn = '2 digit filter','8 digits (for frequency)','8 digits (for second frequency)(if only one frequency is needed set to 0)'
        expected value will be 'ack!'

__send_data(cmd)
    

"""