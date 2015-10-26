#!/bin/bash -f
xv_path="/opt/Xilinx/Vivado/2015.3"
ExecStep()
{
"$@"
RETVAL=$?
if [ $RETVAL -ne 0 ]
then
exit $RETVAL
fi
}
ExecStep $xv_path/bin/xelab -wto 619a5751f4c24640bc3b90557ffbfd3f -m64 --debug typical --relax --mt 8 -L xil_defaultlib -L axi_lite_ipif_v3_0_3 -L lib_cdc_v1_0_2 -L interrupt_control_v3_1_2 -L axi_gpio_v2_0_8 -L proc_sys_reset_v5_0_8 -L generic_baseblocks_v2_1_0 -L axi_infrastructure_v1_1_0 -L axi_register_slice_v2_1_6 -L fifo_generator_v13_0_0 -L axi_data_fifo_v2_1_5 -L axi_crossbar_v2_1_7 -L axi_protocol_converter_v2_1_6 -L unisims_ver -L unimacro_ver -L secureip --snapshot design_1_wrapper_behav xil_defaultlib.design_1_wrapper xil_defaultlib.glbl -log elaborate.log
