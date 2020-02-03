connect -url tcp:127.0.0.1:3121
source C:/ETG-Git-Repos/cydaq_2.0_firmware/src/zybo_z7_firmware/Z7-I2C-Test/Z7-I2C-Test.sdk/design_1_wrapper_hw_platform_0/ps7_init.tcl
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351AB730EA"} -index 0
loadhw -hw C:/ETG-Git-Repos/cydaq_2.0_firmware/src/zybo_z7_firmware/Z7-I2C-Test/Z7-I2C-Test.sdk/design_1_wrapper_hw_platform_0/system.hdf -mem-ranges [list {0x40000000 0xbfffffff}]
configparams force-mem-access 1
targets -set -nocase -filter {name =~"APU*" && jtag_cable_name =~ "Digilent Zybo Z7 210351AB730EA"} -index 0
stop
ps7_init
ps7_post_config
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351AB730EA"} -index 0
rst -processor
targets -set -nocase -filter {name =~ "ARM*#0" && jtag_cable_name =~ "Digilent Zybo Z7 210351AB730EA"} -index 0
dow C:/ETG-Git-Repos/cydaq_2.0_firmware/src/zybo_z7_firmware/Z7-I2C-Test/Z7-I2C-Test.sdk/I2C_Test_SW/Debug/I2C_Test_SW.elf
configparams force-mem-access 0
bpadd -addr &main
