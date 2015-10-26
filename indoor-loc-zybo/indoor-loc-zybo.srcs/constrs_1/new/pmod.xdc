#Pmod Header JE
#IO_L4P_T0_34
set_property PACKAGE_PIN V12 [get_ports {pwmPulse[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[0]}]

#IO_L18N_T2_34
set_property PACKAGE_PIN W16 [get_ports {pwmPulse[1]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[1]}]

#IO_25_35
set_property PACKAGE_PIN J15 [get_ports {pwmPulse[2]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[2]}]

#IO_L19P_T3_35
set_property PACKAGE_PIN H15 [get_ports {pwmPulse[3]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[3]}]

#IO_L3N_T0_DQS_34
set_property PACKAGE_PIN V13 [get_ports {pwmPulse[4]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[4]}]

#IO_L9N_T1_DQS_34
set_property PACKAGE_PIN U17 [get_ports {pwmPulse[5]}]
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[5]}]

##IO_L20P_T3_34
#set_property PACKAGE_PIN T17 [get_ports {je[6]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {je[6]}]

##IO_L7N_T1_34
#set_property PACKAGE_PIN Y17 [get_ports {je[7]}]
#set_property IOSTANDARD LVCMOS33 [get_ports {je[7]}]

create_debug_core u_ila_0_0 ila
set_property ALL_PROBE_SAME_MU true [get_debug_cores u_ila_0_0]
set_property ALL_PROBE_SAME_MU_CNT 2 [get_debug_cores u_ila_0_0]
set_property C_ADV_TRIGGER false [get_debug_cores u_ila_0_0]
set_property C_DATA_DEPTH 131072 [get_debug_cores u_ila_0_0]
set_property C_EN_STRG_QUAL true [get_debug_cores u_ila_0_0]
set_property C_INPUT_PIPE_STAGES 0 [get_debug_cores u_ila_0_0]
set_property C_TRIGIN_EN false [get_debug_cores u_ila_0_0]
set_property C_TRIGOUT_EN false [get_debug_cores u_ila_0_0]
set_property port_width 1 [get_debug_ports u_ila_0_0/clk]
connect_debug_port u_ila_0_0/clk [get_nets [list design_1_i/processing_system7_0/inst/FCLK_CLK0]]
set_property PROBE_TYPE DATA_AND_TRIGGER [get_debug_ports u_ila_0_0/probe0]
set_property port_width 1 [get_debug_ports u_ila_0_0/probe0]
connect_debug_port u_ila_0_0/probe0 [get_nets [list {design_1_i/pwm4zybo_0/pwmPulse[0]}]]
set_property C_CLK_INPUT_FREQ_HZ 300000000 [get_debug_cores dbg_hub]
set_property C_ENABLE_CLK_DIVIDER false [get_debug_cores dbg_hub]
set_property C_USER_SCAN_CHAIN 1 [get_debug_cores dbg_hub]
connect_debug_port dbg_hub/clk [get_nets u_ila_0_FCLK_CLK0]
