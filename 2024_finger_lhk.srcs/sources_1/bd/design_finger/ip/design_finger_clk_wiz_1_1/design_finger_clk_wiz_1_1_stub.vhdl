-- Copyright 1986-2018 Xilinx, Inc. All Rights Reserved.
-- --------------------------------------------------------------------------------
-- Tool Version: Vivado v.2018.3 (win64) Build 2405991 Thu Dec  6 23:38:27 MST 2018
-- Date        : Fri May 17 10:42:38 2024
-- Host        : LEGION-R7000P-lhk running 64-bit major release  (build 9200)
-- Command     : write_vhdl -force -mode synth_stub
--               G:/XilinxPrograms/2024_finger/2024_finger.srcs/sources_1/bd/design_finger/ip/design_finger_clk_wiz_1_1/design_finger_clk_wiz_1_1_stub.vhdl
-- Design      : design_finger_clk_wiz_1_1
-- Purpose     : Stub declaration of top-level module interface
-- Device      : xc7a100tcsg324-1
-- --------------------------------------------------------------------------------
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity design_finger_clk_wiz_1_1 is
  Port ( 
    clk_out1 : out STD_LOGIC;
    resetn : in STD_LOGIC;
    locked : out STD_LOGIC;
    clk_in1 : in STD_LOGIC
  );

end design_finger_clk_wiz_1_1;

architecture stub of design_finger_clk_wiz_1_1 is
attribute syn_black_box : boolean;
attribute black_box_pad_pin : string;
attribute syn_black_box of stub : architecture is true;
attribute black_box_pad_pin of stub : architecture is "clk_out1,resetn,locked,clk_in1";
begin
end;
