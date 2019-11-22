# master_EE_224
This is the repository for the Iowa State University teaching lab's CyDAQ hardware. The CyDAQ is a data acquisition system designed around the Zybo Z7 FPGA. It features analog inputs to a filter block feature 7 adjustable filters, power breakout, and Zybo PMOD breakouts. 

## Filters:
The board features the following 7 filters:
    - 60 Hz Notch 
    - 1st Order High Pass
    - 1st Order Low Pass
    - 2nd Order Bandpass
    - 6th Order How Pass
    - 6th Order Low pass
    - 6th Order Bandpass

Each of these filters can be selected to process any of the analog inputs, and their outputs are sent to the XADC on the Zybo and the Vout breakout header. 

## Power
The board has three user accessible power rails: 1.8V, 3.3V, and 5.0V. Each rail can source up to 1A. 

## Digital Communication Protocols
The board breaks out I2C and UART busses accessible through the Zybo.