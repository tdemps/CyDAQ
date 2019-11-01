"""
Install script for resolving dependencies for CyDAQ.

@author: sdmay18-31
"""
import pip
import sys


def install(package):
    pip.main(['install', package])


if(sys.version_info < (3, 6)):
    print('ERROR: Python version >3.6 is required!')
    exit()

install('pyserial')
install('scipy')
install('numpy')
install('pycodestyle')
install('jsonpickle')
install('doxypy')
install('matplotlib')
install('pylama')
