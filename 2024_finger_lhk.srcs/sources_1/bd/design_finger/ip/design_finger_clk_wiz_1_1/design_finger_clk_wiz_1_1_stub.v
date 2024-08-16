// Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
// --------------------------------------------------------------------------------
// Tool Version: Vivado v.2018.3 (win64) Build 2405991 Thu Dec  6 23:38:27 MST 2018
// Date        : Fri May 17 10:42:38 2024
// Host        : LEGION-R7000P-lhk running 64-bit major release  (build 9200)
// Command     : write_verilog -force -mode synth_stub
//               G:/XilinxPrograms/2024_finger/2024_finger.srcs/sources_1/bd/design_finger/ip/design_finger_clk_wiz_1_1/design_finger_clk_wiz_1_1_stub.v
// Design      : design_finger_clk_wiz_1_1
// Purpose     : Stub declaration of top-level module interface
// Device      : xc7a100tcsg324-1
// --------------------------------------------------------------------------------

// This empty module with port declaration file causes synthesis tools to infer a black box for IP.
// The synthesis directives are for Synopsys Synplify support to prevent IO buffer insertion.
// Please paste the declaration into a Verilog source file or add the file as an additional source.
module design_finger_clk_wiz_1_1(clk_out1, resetn, locked, clk_in1)
/* synthesis syn_black_box black_box_pad_pin="clk_out1,resetn,locked,clk_in1" */;
  output clk_out1;
  input resetn;
  output locked;
  input clk_in1;
endmodule
