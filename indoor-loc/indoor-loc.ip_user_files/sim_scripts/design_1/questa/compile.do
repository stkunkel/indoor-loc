vlib work
vlib msim

vlib msim/xil_defaultlib
vlib msim/lib_cdc_v1_0_2
vlib msim/proc_sys_reset_v5_0_8
vlib msim/generic_baseblocks_v2_1_0
vlib msim/axi_infrastructure_v1_1_0
vlib msim/axi_register_slice_v2_1_6
vlib msim/fifo_generator_v13_0_0
vlib msim/axi_data_fifo_v2_1_5
vlib msim/axi_crossbar_v2_1_7
vlib msim/axi_lite_ipif_v3_0_3
vlib msim/interrupt_control_v3_1_2
vlib msim/axi_gpio_v2_0_8
vlib msim/axi_protocol_converter_v2_1_6

vmap xil_defaultlib msim/xil_defaultlib
vmap lib_cdc_v1_0_2 msim/lib_cdc_v1_0_2
vmap proc_sys_reset_v5_0_8 msim/proc_sys_reset_v5_0_8
vmap generic_baseblocks_v2_1_0 msim/generic_baseblocks_v2_1_0
vmap axi_infrastructure_v1_1_0 msim/axi_infrastructure_v1_1_0
vmap axi_register_slice_v2_1_6 msim/axi_register_slice_v2_1_6
vmap fifo_generator_v13_0_0 msim/fifo_generator_v13_0_0
vmap axi_data_fifo_v2_1_5 msim/axi_data_fifo_v2_1_5
vmap axi_crossbar_v2_1_7 msim/axi_crossbar_v2_1_7
vmap axi_lite_ipif_v3_0_3 msim/axi_lite_ipif_v3_0_3
vmap interrupt_control_v3_1_2 msim/interrupt_control_v3_1_2
vmap axi_gpio_v2_0_8 msim/axi_gpio_v2_0_8
vmap axi_protocol_converter_v2_1_6 msim/axi_protocol_converter_v2_1_6

vlog -work xil_defaultlib -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_wr.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_rd.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_wr_4.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_rd_4.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_hp2_3.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_arb_hp0_1.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_ssw_hp.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_sparse_mem.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_reg_map.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_ocm_mem.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_intr_wr_mem.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_intr_rd_mem.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_fmsw_gp.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_regc.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_ocmc.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_interconnect_model.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_gen_reset.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_gen_clock.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_ddrc.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_axi_slave.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_axi_master.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_afi_slave.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl/processing_system7_bfm_v2_0_processing_system7_bfm.v" \
"./../../../bd/design_1/ip/design_1_processing_system7_0_0/sim/design_1_processing_system7_0_0.v" \

vcom -work lib_cdc_v1_0_2 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/lib_cdc_v1_0/hdl/src/vhdl/cdc_sync.vhd" \

vcom -work proc_sys_reset_v5_0_8 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/proc_sys_reset_v5_0/hdl/src/vhdl/upcnt_n.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/proc_sys_reset_v5_0/hdl/src/vhdl/sequence_psr.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/proc_sys_reset_v5_0/hdl/src/vhdl/lpf.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/proc_sys_reset_v5_0/hdl/src/vhdl/proc_sys_reset.vhd" \

vcom -work xil_defaultlib -64 \
"./../../../bd/design_1/ip/design_1_rst_processing_system7_0_100M_0/sim/design_1_rst_processing_system7_0_100M_0.vhd" \

vlog -work generic_baseblocks_v2_1_0 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_carry_and.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_carry_latch_and.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_carry_latch_or.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_carry_or.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_carry.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_command_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_mask_static.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_mask.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_sel_mask_static.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_sel_mask.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_sel_static.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_sel.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator_static.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_comparator.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_mux_enc.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_mux.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/generic_baseblocks_v2_1/hdl/verilog/generic_baseblocks_v2_1_nto1_mux.v" \

vlog -work axi_infrastructure_v1_1_0 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog/axi_infrastructure_v1_1_axi2vector.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog/axi_infrastructure_v1_1_axic_srl_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog/axi_infrastructure_v1_1_vector2axi.v" \

vlog -work axi_register_slice_v2_1_6 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_register_slice_v2_1/hdl/verilog/axi_register_slice_v2_1_axic_register_slice.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_register_slice_v2_1/hdl/verilog/axi_register_slice_v2_1_axi_register_slice.v" \

vcom -work fifo_generator_v13_0_0 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/fifo_generator_v13_0/simulation/fifo_generator_vhdl_beh.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/fifo_generator_v13_0/hdl/fifo_generator_v13_0_rfs.vhd" \

vlog -work axi_data_fifo_v2_1_5 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_axic_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_fifo_gen.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_axic_srl_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_axic_reg_srl_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_ndeep_srl.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_data_fifo_v2_1/hdl/verilog/axi_data_fifo_v2_1_axi_data_fifo.v" \

vlog -work axi_crossbar_v2_1_7 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_addr_arbiter_sasd.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_addr_arbiter.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_addr_decoder.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_arbiter_resp.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_crossbar_sasd.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_crossbar.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_decerr_slave.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_si_transactor.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_splitter.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_wdata_mux.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_wdata_router.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_crossbar_v2_1/hdl/verilog/axi_crossbar_v2_1_axi_crossbar.v" \

vlog -work xil_defaultlib -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../bd/design_1/ip/design_1_xbar_0/sim/design_1_xbar_0.v" \

vcom -work xil_defaultlib -64 \
"./../../../bd/design_1/ipshared/user.org/pwm_v1_0/hdl/pwm.vhd" \
"./../../../bd/design_1/ipshared/user.org/pwm_v1_0/hdl/pwm_v1_0_S00_AXI.vhd" \
"./../../../bd/design_1/ipshared/user.org/pwm_v1_0/hdl/pwm_v1_0.vhd" \
"./../../../bd/design_1/ip/design_1_pwm_0_1/sim/design_1_pwm_0_1.vhd" \
"./../../../bd/design_1/hdl/design_1.vhd" \
"./../../../bd/design_1/ipshared/xilinx.com/xlconstant_v1_1/xlconstant.vhd" \
"./../../../bd/design_1/ip/design_1_xlconstant_0_0/sim/design_1_xlconstant_0_0.vhd" \
"./../../../bd/design_1/ip/design_1_xlconstant_0_1/sim/design_1_xlconstant_0_1.vhd" \

vcom -work axi_lite_ipif_v3_0_3 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_lite_ipif_v3_0/hdl/src/vhdl/ipif_pkg.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_lite_ipif_v3_0/hdl/src/vhdl/pselect_f.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_lite_ipif_v3_0/hdl/src/vhdl/address_decoder.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_lite_ipif_v3_0/hdl/src/vhdl/slave_attachment.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_lite_ipif_v3_0/hdl/src/vhdl/axi_lite_ipif.vhd" \

vcom -work interrupt_control_v3_1_2 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/interrupt_control_v3_1/hdl/src/vhdl/interrupt_control.vhd" \

vcom -work axi_gpio_v2_0_8 -64 \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_gpio_v2_0/hdl/src/vhdl/gpio_core.vhd" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_gpio_v2_0/hdl/src/vhdl/axi_gpio.vhd" \

vcom -work xil_defaultlib -64 \
"./../../../bd/design_1/ip/design_1_axi_gpio_0_0/sim/design_1_axi_gpio_0_0.vhd" \

vlog -work axi_protocol_converter_v2_1_6 -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_a_axi3_conv.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_axi3_conv.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_axilite_conv.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_r_axi3_conv.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_w_axi3_conv.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b_downsizer.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_decerr_slave.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_simple_fifo.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_wrap_cmd.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_incr_cmd.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_wr_cmd_fsm.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_rd_cmd_fsm.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_cmd_translator.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_b_channel.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_r_channel.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_aw_channel.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s_ar_channel.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_b2s.v" \
"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_protocol_converter_v2_1/hdl/verilog/axi_protocol_converter_v2_1_axi_protocol_converter.v" \

vlog -work xil_defaultlib -64 +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/axi_infrastructure_v1_1/hdl/verilog" +incdir+"../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/xilinx.com/processing_system7_bfm_v2_0/hdl" +incdir+"../../../bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_processing_system7_0_0" +incdir+"./../../../../indoor-loc.srcs/sources_1/bd/design_1/ipshared/user.org/pwm_v1_0/drivers/pwm_v1_0/src" +incdir+"./../../../ipstatic/axi_infrastructure_v1_1/hdl/verilog" +incdir+"./../../../ipstatic/processing_system7_bfm_v2_0/hdl" \
"./../../../bd/design_1/ip/design_1_auto_pc_0/sim/design_1_auto_pc_0.v" \

vlog -work xil_defaultlib "glbl.v"

