-makelib xcelium_lib/xilinx_vip -sv \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_axi4streampc.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_axi4pc.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/xil_common_vip_pkg.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_pkg.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_pkg.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi4stream_vip_if.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/axi_vip_if.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/clk_vip_if.sv" \
  "C:/Xilinx/Vivado/2019.1/data/xilinx_vip/hdl/rst_vip_if.sv" \
-endlib
-makelib xcelium_lib/xil_defaultlib -sv \
  "C:/Xilinx/Vivado/2019.1/data/ip/xpm/xpm_cdc/hdl/xpm_cdc.sv" \
  "C:/Xilinx/Vivado/2019.1/data/ip/xpm/xpm_memory/hdl/xpm_memory.sv" \
-endlib
-makelib xcelium_lib/xpm \
  "C:/Xilinx/Vivado/2019.1/data/ip/xpm/xpm_VCOMP.vhd" \
-endlib
-makelib xcelium_lib/axi_infrastructure_v1_1_0 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/ec67/hdl/axi_infrastructure_v1_1_vl_rfs.v" \
-endlib
-makelib xcelium_lib/axi_vip_v1_1_5 -sv \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/d4a8/hdl/axi_vip_v1_1_vl_rfs.sv" \
-endlib
-makelib xcelium_lib/processing_system7_vip_v1_0_7 -sv \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/8c62/hdl/processing_system7_vip_v1_0_vl_rfs.sv" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/ip/block_design_processing_system7_0_0/sim/block_design_processing_system7_0_0.v" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_conv_funs_pkg.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_proc_common_pkg.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_ipif_pkg.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_family_support.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_family.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_soft_reset.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/proc_common_v3_30_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_pselect_f.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/axi_lite_ipif_v1_01_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_address_decoder.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/axi_lite_ipif_v1_01_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_slave_attachment.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/interrupt_control_v2_01_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_interrupt_control.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/axi_lite_ipif_v1_01_a/hdl/src/vhdl/block_design_xadc_wiz_0_1_axi_lite_ipif.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/block_design_xadc_wiz_0_1_xadc_core_drp.vhd" \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/block_design_xadc_wiz_0_1_axi_xadc.vhd" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/ip/block_design_xadc_wiz_0_1/block_design_xadc_wiz_0_1.v" \
-endlib
-makelib xcelium_lib/lib_cdc_v1_0_2 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/ef1e/hdl/lib_cdc_v1_0_rfs.vhd" \
-endlib
-makelib xcelium_lib/proc_sys_reset_v5_0_13 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/8842/hdl/proc_sys_reset_v5_0_vh_rfs.vhd" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/ip/block_design_rst_ps7_0_50M_0/sim/block_design_rst_ps7_0_50M_0.vhd" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/sim/block_design.v" \
-endlib
-makelib xcelium_lib/generic_baseblocks_v2_1_0 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/b752/hdl/generic_baseblocks_v2_1_vl_rfs.v" \
-endlib
-makelib xcelium_lib/fifo_generator_v13_2_4 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/1f5a/simulation/fifo_generator_vlog_beh.v" \
-endlib
-makelib xcelium_lib/fifo_generator_v13_2_4 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/1f5a/hdl/fifo_generator_v13_2_rfs.vhd" \
-endlib
-makelib xcelium_lib/fifo_generator_v13_2_4 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/1f5a/hdl/fifo_generator_v13_2_rfs.v" \
-endlib
-makelib xcelium_lib/axi_data_fifo_v2_1_18 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/5b9c/hdl/axi_data_fifo_v2_1_vl_rfs.v" \
-endlib
-makelib xcelium_lib/axi_register_slice_v2_1_19 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/4d88/hdl/axi_register_slice_v2_1_vl_rfs.v" \
-endlib
-makelib xcelium_lib/axi_protocol_converter_v2_1_19 \
  "../../../../XADC_Standalone.srcs/sources_1/bd/block_design/ipshared/c83a/hdl/axi_protocol_converter_v2_1_vl_rfs.v" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  "../../../bd/block_design/ip/block_design_auto_pc_0/sim/block_design_auto_pc_0.v" \
-endlib
-makelib xcelium_lib/xil_defaultlib \
  glbl.v
-endlib

