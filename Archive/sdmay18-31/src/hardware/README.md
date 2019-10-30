# Hardware

This directory contains the hardware component for Senior Design Team SDMay18-31. Pspice simulations are used for hardware analysis. Eagle is used for PCB design.

## Hardware Simulations
### Initial Filter Simulations
Seven different filters were designed and simulated in PSpice. These filters are a 60 Hz Notch filter, a 1st and 5th order Low Pass filter, a 1st and 5th order High Pass filter, and a 2nd and 6th order Band Pass Filter. These filters were simulated by performing a frequency analysis (AC Sweep) of the filters. The results of this analysis are detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/74).

### Filter Simulations With Unity Gain Stage
The seven filters were then simulated with the addition of a unity gain stage. This addition included a new op-amp component that would not simulate correctly in an AC Sweep. In order to still obtain a valuable analysis of this new system, the system was analyzed at several relevant input frequencies. The results of this analysis are detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/123).

### Band-Pass Additional Gain Simulations
After the previous simulations, it was understood that the band pass filters would require additional gain. This was achieved by addin a gain stage in between the unity gain stage and the filter. The results of this analysis are detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/128).

### Twin-Tee 60-Hz Notch Filter Simulations
After several hardware [tests](https://git.ece.iastate.edu/sd/sdmay18-31/issues/139) it was determined that the 60-Hz Notch filter design was not performing properly. After some research a twin-tee design was proposed. Unfortunately this design does not appear to function adequately as detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/173).

### Tuned Twin-Tee 60-Hz Notch Filter Simulations
After some more research, it appears that the twin-tee is a well known design for Notch filters, and after some tuning of the component values a better performing design was achieved. The results of this analysis are detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/175).

## Hardware Schematics
### Initial Circuit Design
This PCB design includes changes to the circuit design up to the added gain stage of the band-pass filter. This diagram is documented [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/73)
### Rev 2 PSpice Design
[This](https://git.ece.iastate.edu/sd/sdmay18-31/issues/191) issue documents the schematics that are being used in our PCB Rev. 2.

## Hardware Testing
### Initial Breadboard Testing
The filters were prototyped on a breadboard and tested with a function generator and oscilloscope at the same frequencies as those specified for the simulations. Testing of designs up to the added gain has occured. In addition the demultiplexer was tested for crosstalk between each of the channels. The results of this testing is detailed [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/139).

## Hardware Specifications
The electrical specifications of the hardware is a work in progress, and will not be finalized until a working design is complete. However, the current specifications can be found [here](https://git.ece.iastate.edu/sd/sdmay18-31/issues/36).