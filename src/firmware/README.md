########## CYDAQ 2.0 FIRMWARE README ##########

## Firmware Project Information
--/src/firmware/zybo_z7_firmware/full_firmware_build contains the HW and SW files of the final firmware build.
--All other folders in /zybo_z7_firmware/ are test/example projects.
--Projects are built in Vivado 2019.1
--Software for firmware is in /src/firmware/zybo_z7_firmware/full_firmware_build/full_firmware_build.sdk/full_firmware_sw/
--The fsbl sw project is used to generate a bootloader, which must be used when creating a BOOT.BIN file of the sw.
--BOOT.BIN file contains hardware and software of project, used to run firmware from sd card.



## Creating the BOOT.BIN file
--Highlight the firmware software project in Xilinx SDK
--Navigate to Tools>Create Boot Image
--For data files, add the following in order:

/full_firmware_build.sdk/fsbl/Debug/fsbl.elf	Data type: bootloader
/full_firmware_build.sdk/design_1_wrapper_hw_platform_X/design_1_wrapper.bit	Data type: Datafile
/full_firmware_build.sdk/*firmware sw project name*/Debug/*firmware sw proj name*.elf	Data type: Datafile

--Replace X with hw platform revision number
--Click Create Image button at bottom
--SDK console should say "Build Finished" when image was successfully created
--The BOOT.BIN will be in the firmware sw project folder in /bootimage