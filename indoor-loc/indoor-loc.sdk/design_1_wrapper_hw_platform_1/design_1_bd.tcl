
################################################################
# This is a generated script based on design: design_1
#
# Though there are limitations about the generated script,
# the main purpose of this utility is to make learning
# IP Integrator Tcl commands easier.
################################################################

################################################################
# Check if script is running in correct Vivado version.
################################################################
set scripts_vivado_version 2015.3
set current_vivado_version [version -short]

if { [string first $scripts_vivado_version $current_vivado_version] == -1 } {
   puts ""
   puts "ERROR: This script was generated using Vivado <$scripts_vivado_version> and is being run in <$current_vivado_version> of Vivado. Please run the script in Vivado <$scripts_vivado_version> then open the design in Vivado <$current_vivado_version>. Upgrade the design by running \"Tools => Report => Report IP Status...\", then run write_bd_tcl to create an updated script."

   return 1
}

################################################################
# START
################################################################

# To test this script, run the following commands from Vivado Tcl console:
# source design_1_script.tcl

# If you do not already have a project created,
# you can create a project using the following command:
#    create_project project_1 myproj -part xc7z020clg484-1
#    set_property BOARD_PART em.avnet.com:zed:part0:1.3 [current_project]

# CHECKING IF PROJECT EXISTS
if { [get_projects -quiet] eq "" } {
   puts "ERROR: Please open or create a project!"
   return 1
}



# CHANGE DESIGN NAME HERE
set design_name design_1

# If you do not already have an existing IP Integrator design open,
# you can create a design using the following command:
#    create_bd_design $design_name

# Creating design if needed
set errMsg ""
set nRet 0

set cur_design [current_bd_design -quiet]
set list_cells [get_bd_cells -quiet]

if { ${design_name} eq "" } {
   # USE CASES:
   #    1) Design_name not set

   set errMsg "ERROR: Please set the variable <design_name> to a non-empty value."
   set nRet 1

} elseif { ${cur_design} ne "" && ${list_cells} eq "" } {
   # USE CASES:
   #    2): Current design opened AND is empty AND names same.
   #    3): Current design opened AND is empty AND names diff; design_name NOT in project.
   #    4): Current design opened AND is empty AND names diff; design_name exists in project.

   if { $cur_design ne $design_name } {
      puts "INFO: Changing value of <design_name> from <$design_name> to <$cur_design> since current design is empty."
      set design_name [get_property NAME $cur_design]
   }
   puts "INFO: Constructing design in IPI design <$cur_design>..."

} elseif { ${cur_design} ne "" && $list_cells ne "" && $cur_design eq $design_name } {
   # USE CASES:
   #    5) Current design opened AND has components AND same names.

   set errMsg "ERROR: Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 1
} elseif { [get_files -quiet ${design_name}.bd] ne "" } {
   # USE CASES: 
   #    6) Current opened design, has components, but diff names, design_name exists in project.
   #    7) No opened design, design_name exists in project.

   set errMsg "ERROR: Design <$design_name> already exists in your project, please set the variable <design_name> to another value."
   set nRet 2

} else {
   # USE CASES:
   #    8) No opened design, design_name not in project.
   #    9) Current opened design, has components, but diff names, design_name not in project.

   puts "INFO: Currently there is no design <$design_name> in project, so creating one..."

   create_bd_design $design_name

   puts "INFO: Making design <$design_name> as current_bd_design."
   current_bd_design $design_name

}

puts "INFO: Currently the variable <design_name> is equal to \"$design_name\"."

if { $nRet != 0 } {
   puts $errMsg
   return $nRet
}

##################################################################
# DESIGN PROCs
##################################################################



# Procedure to create entire design; Provide argument to make
# procedure reusable. If parentCell is "", will use root.
proc create_root_design { parentCell } {

  if { $parentCell eq "" } {
     set parentCell [get_bd_cells /]
  }

  # Get object for parentCell
  set parentObj [get_bd_cells $parentCell]
  if { $parentObj == "" } {
     puts "ERROR: Unable to find parent cell <$parentCell>!"
     return
  }

  # Make sure parentObj is hier blk
  set parentType [get_property TYPE $parentObj]
  if { $parentType ne "hier" } {
     puts "ERROR: Parent <$parentObj> has TYPE = <$parentType>. Expected to be <hier>."
     return
  }

  # Save current instance; Restore later
  set oldCurInst [current_bd_instance .]

  # Set parent object as current
  current_bd_instance $parentObj


  # Create interface ports
  set DDR [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:ddrx_rtl:1.0 DDR ]
  set FIXED_IO [ create_bd_intf_port -mode Master -vlnv xilinx.com:display_processing_system7:fixedio_rtl:1.0 FIXED_IO ]
  set btns_5bits [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 btns_5bits ]
  set leds_8bits [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 leds_8bits ]
  set sws_8bits [ create_bd_intf_port -mode Master -vlnv xilinx.com:interface:gpio_rtl:1.0 sws_8bits ]

  # Create ports
  set pwmDir [ create_bd_port -dir O -from 0 -to 0 -type data pwmDir ]
  set pwmOe [ create_bd_port -dir O -from 0 -to 0 -type data pwmOe ]
  set pwmPulse [ create_bd_port -dir O -from 5 -to 0 pwmPulse ]
  set uwb_gpio_in [ create_bd_port -dir I uwb_gpio_in ]
  set uwb_gpio_io [ create_bd_port -dir IO uwb_gpio_io ]
  set uwb_rst_n [ create_bd_port -dir IO uwb_rst_n ]
  set uwb_spi_miso [ create_bd_port -dir I uwb_spi_miso ]
  set uwb_spi_mosi [ create_bd_port -dir O uwb_spi_mosi ]
  set uwb_spi_sck [ create_bd_port -dir O -from 0 -to 0 uwb_spi_sck ]
  set uwb_spi_sel_n [ create_bd_port -dir O -from 0 -to 0 uwb_spi_sel_n ]

  # Create instance: axi_gpio_0, and set properties
  set axi_gpio_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_0 ]
  set_property -dict [ list \
CONFIG.C_ALL_INPUTS {0} \
CONFIG.C_ALL_OUTPUTS {1} \
CONFIG.C_ALL_OUTPUTS_2 {1} \
CONFIG.C_GPIO2_WIDTH {8} \
CONFIG.C_GPIO_WIDTH {8} \
CONFIG.C_INTERRUPT_PRESENT {0} \
CONFIG.C_IS_DUAL {0} \
CONFIG.GPIO2_BOARD_INTERFACE {Custom} \
CONFIG.GPIO_BOARD_INTERFACE {leds_8bits} \
CONFIG.USE_BOARD_FLOW {true} \
 ] $axi_gpio_0

  # Create instance: axi_gpio_1, and set properties
  set axi_gpio_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_1 ]
  set_property -dict [ list \
CONFIG.C_ALL_INPUTS {1} \
CONFIG.C_ALL_INPUTS_2 {1} \
CONFIG.C_GPIO2_WIDTH {5} \
CONFIG.C_GPIO_WIDTH {8} \
CONFIG.C_INTERRUPT_PRESENT {1} \
CONFIG.C_IS_DUAL {1} \
CONFIG.GPIO2_BOARD_INTERFACE {btns_5bits} \
CONFIG.GPIO_BOARD_INTERFACE {sws_8bits} \
CONFIG.USE_BOARD_FLOW {true} \
 ] $axi_gpio_1

  # Create instance: axi_gpio_uwb, and set properties
  set axi_gpio_uwb [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_gpio:2.0 axi_gpio_uwb ]
  set_property -dict [ list \
CONFIG.C_ALL_INPUTS {0} \
CONFIG.C_ALL_INPUTS_2 {1} \
CONFIG.C_ALL_OUTPUTS {1} \
CONFIG.C_DOUT_DEFAULT {0xFFFFFFFF} \
CONFIG.C_GPIO2_WIDTH {3} \
CONFIG.C_GPIO_WIDTH {2} \
CONFIG.C_IS_DUAL {1} \
CONFIG.GPIO2_BOARD_INTERFACE {Custom} \
CONFIG.GPIO_BOARD_INTERFACE {Custom} \
CONFIG.USE_BOARD_FLOW {true} \
 ] $axi_gpio_uwb

  # Create instance: axi_quad_spi_uwb, and set properties
  set axi_quad_spi_uwb [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_quad_spi:3.2 axi_quad_spi_uwb ]
  set_property -dict [ list \
CONFIG.C_FIFO_DEPTH {0} \
CONFIG.C_USE_STARTUP {0} \
CONFIG.C_USE_STARTUP_INT {0} \
CONFIG.FIFO_INCLUDED {0} \
CONFIG.Multiples16 {4} \
 ] $axi_quad_spi_uwb

  # Create instance: axi_timer_0, and set properties
  set axi_timer_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_timer:2.0 axi_timer_0 ]
  set_property -dict [ list \
CONFIG.enable_timer2 {0} \
 ] $axi_timer_0

  # Create instance: iobuf_ak_2, and set properties
  set iobuf_ak_2 [ create_bd_cell -type ip -vlnv aku:user:iobuf_ak:1.3 iobuf_ak_2 ]

  # Create instance: iobuf_ak_3, and set properties
  set iobuf_ak_3 [ create_bd_cell -type ip -vlnv aku:user:iobuf_ak:1.3 iobuf_ak_3 ]

  # Create instance: processing_system7_0, and set properties
  set processing_system7_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:processing_system7:5.5 processing_system7_0 ]
  set_property -dict [ list \
CONFIG.PCW_ENET0_PERIPHERAL_ENABLE {0} \
CONFIG.PCW_I2C0_I2C0_IO {MIO 10 .. 11} \
CONFIG.PCW_I2C0_PERIPHERAL_ENABLE {1} \
CONFIG.PCW_IRQ_F2P_INTR {1} \
CONFIG.PCW_MIO_10_PULLUP {enabled} \
CONFIG.PCW_MIO_10_SLEW {slow} \
CONFIG.PCW_MIO_11_PULLUP {enabled} \
CONFIG.PCW_MIO_11_SLEW {slow} \
CONFIG.PCW_QSPI_GRP_SINGLE_SS_ENABLE {1} \
CONFIG.PCW_SD0_PERIPHERAL_ENABLE {0} \
CONFIG.PCW_USB0_PERIPHERAL_ENABLE {0} \
CONFIG.PCW_USE_FABRIC_INTERRUPT {1} \
CONFIG.preset {ZedBoard} \
 ] $processing_system7_0

  # Create instance: processing_system7_0_axi_periph, and set properties
  set processing_system7_0_axi_periph [ create_bd_cell -type ip -vlnv xilinx.com:ip:axi_interconnect:2.1 processing_system7_0_axi_periph ]
  set_property -dict [ list \
CONFIG.NUM_MI {6} \
 ] $processing_system7_0_axi_periph

  # Create instance: pwm_0, and set properties
  set pwm_0 [ create_bd_cell -type ip -vlnv user.org:user:pwm:1.0 pwm_0 ]

  # Create instance: pwm_dir_1, and set properties
  set pwm_dir_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 pwm_dir_1 ]

  # Create instance: pwm_oe_0, and set properties
  set pwm_oe_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 pwm_oe_0 ]
  set_property -dict [ list \
CONFIG.CONST_VAL {0} \
 ] $pwm_oe_0

  # Create instance: rst_processing_system7_0_100M, and set properties
  set rst_processing_system7_0_100M [ create_bd_cell -type ip -vlnv xilinx.com:ip:proc_sys_reset:5.0 rst_processing_system7_0_100M ]

  # Create instance: util_vector_logic_0, and set properties
  set util_vector_logic_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_0 ]
  set_property -dict [ list \
CONFIG.C_OPERATION {not} \
CONFIG.C_SIZE {1} \
 ] $util_vector_logic_0

  # Create instance: util_vector_logic_1, and set properties
  set util_vector_logic_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:util_vector_logic:2.0 util_vector_logic_1 ]
  set_property -dict [ list \
CONFIG.C_OPERATION {not} \
CONFIG.C_SIZE {1} \
 ] $util_vector_logic_1

  # Create instance: xlconcat_0, and set properties
  set xlconcat_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_0 ]
  set_property -dict [ list \
CONFIG.NUM_PORTS {3} \
 ] $xlconcat_0

  # Create instance: xlconcat_1, and set properties
  set xlconcat_1 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconcat:2.1 xlconcat_1 ]
  set_property -dict [ list \
CONFIG.NUM_PORTS {3} \
 ] $xlconcat_1

  # Create instance: xlconstant_0, and set properties
  set xlconstant_0 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlconstant:1.1 xlconstant_0 ]
  set_property -dict [ list \
CONFIG.CONST_VAL {0} \
 ] $xlconstant_0

  # Create instance: xlslice_3, and set properties
  set xlslice_3 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_3 ]
  set_property -dict [ list \
CONFIG.DIN_FROM {0} \
CONFIG.DIN_TO {0} \
CONFIG.DIN_WIDTH {2} \
 ] $xlslice_3

  # Create instance: xlslice_4, and set properties
  set xlslice_4 [ create_bd_cell -type ip -vlnv xilinx.com:ip:xlslice:1.0 xlslice_4 ]
  set_property -dict [ list \
CONFIG.DIN_FROM {0} \
CONFIG.DIN_TO {0} \
CONFIG.DIN_WIDTH {2} \
 ] $xlslice_4

  # Create interface connections
  connect_bd_intf_net -intf_net axi_gpio_0_GPIO [get_bd_intf_ports leds_8bits] [get_bd_intf_pins axi_gpio_0/GPIO]
  connect_bd_intf_net -intf_net axi_gpio_1_GPIO [get_bd_intf_ports sws_8bits] [get_bd_intf_pins axi_gpio_1/GPIO]
  connect_bd_intf_net -intf_net axi_gpio_1_GPIO2 [get_bd_intf_ports btns_5bits] [get_bd_intf_pins axi_gpio_1/GPIO2]
  connect_bd_intf_net -intf_net processing_system7_0_DDR [get_bd_intf_ports DDR] [get_bd_intf_pins processing_system7_0/DDR]
  connect_bd_intf_net -intf_net processing_system7_0_FIXED_IO [get_bd_intf_ports FIXED_IO] [get_bd_intf_pins processing_system7_0/FIXED_IO]
  connect_bd_intf_net -intf_net processing_system7_0_M_AXI_GP0 [get_bd_intf_pins processing_system7_0/M_AXI_GP0] [get_bd_intf_pins processing_system7_0_axi_periph/S00_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M00_AXI [get_bd_intf_pins axi_gpio_0/S_AXI] [get_bd_intf_pins processing_system7_0_axi_periph/M00_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M01_AXI [get_bd_intf_pins axi_timer_0/S_AXI] [get_bd_intf_pins processing_system7_0_axi_periph/M01_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M02_AXI [get_bd_intf_pins processing_system7_0_axi_periph/M02_AXI] [get_bd_intf_pins pwm_0/S00_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M03_AXI [get_bd_intf_pins axi_gpio_uwb/S_AXI] [get_bd_intf_pins processing_system7_0_axi_periph/M03_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M04_AXI [get_bd_intf_pins axi_quad_spi_uwb/AXI_LITE] [get_bd_intf_pins processing_system7_0_axi_periph/M04_AXI]
  connect_bd_intf_net -intf_net processing_system7_0_axi_periph_M05_AXI [get_bd_intf_pins axi_gpio_1/S_AXI] [get_bd_intf_pins processing_system7_0_axi_periph/M05_AXI]

  # Create port connections
  connect_bd_net -net Net [get_bd_ports uwb_gpio_io] [get_bd_pins iobuf_ak_3/pin]
  connect_bd_net -net Net1 [get_bd_ports uwb_rst_n] [get_bd_pins iobuf_ak_2/pin]
  connect_bd_net -net axi_gpio_1_ip2intc_irpt [get_bd_pins axi_gpio_1/ip2intc_irpt] [get_bd_pins xlconcat_0/In0]
  connect_bd_net -net axi_gpio_uwb_gpio_io_o [get_bd_pins axi_gpio_uwb/gpio_io_o] [get_bd_pins xlslice_3/Din] [get_bd_pins xlslice_4/Din]
  connect_bd_net -net axi_quad_spi_uwb_io0_o [get_bd_ports uwb_spi_mosi] [get_bd_pins axi_quad_spi_uwb/io0_o]
  connect_bd_net -net axi_quad_spi_uwb_ip2intc_irpt [get_bd_pins axi_quad_spi_uwb/ip2intc_irpt] [get_bd_pins xlconcat_0/In2]
  connect_bd_net -net axi_quad_spi_uwb_sck_o [get_bd_pins axi_quad_spi_uwb/sck_o] [get_bd_pins util_vector_logic_0/Op1]
  connect_bd_net -net axi_quad_spi_uwb_ss_o [get_bd_ports uwb_spi_sel_n] [get_bd_pins axi_quad_spi_uwb/ss_o]
  connect_bd_net -net axi_timer_0_interrupt [get_bd_pins axi_timer_0/interrupt] [get_bd_pins xlconcat_0/In1]
  connect_bd_net -net iobuf_ak_2_dout [get_bd_pins iobuf_ak_2/dout] [get_bd_pins xlconcat_1/In2]
  connect_bd_net -net iobuf_ak_3_dout [get_bd_pins iobuf_ak_3/dout] [get_bd_pins xlconcat_1/In1]
  connect_bd_net -net processing_system7_0_FCLK_CLK0 [get_bd_pins axi_gpio_0/s_axi_aclk] [get_bd_pins axi_gpio_1/s_axi_aclk] [get_bd_pins axi_gpio_uwb/s_axi_aclk] [get_bd_pins axi_quad_spi_uwb/ext_spi_clk] [get_bd_pins axi_quad_spi_uwb/s_axi_aclk] [get_bd_pins axi_timer_0/s_axi_aclk] [get_bd_pins processing_system7_0/FCLK_CLK0] [get_bd_pins processing_system7_0/M_AXI_GP0_ACLK] [get_bd_pins processing_system7_0_axi_periph/ACLK] [get_bd_pins processing_system7_0_axi_periph/M00_ACLK] [get_bd_pins processing_system7_0_axi_periph/M01_ACLK] [get_bd_pins processing_system7_0_axi_periph/M02_ACLK] [get_bd_pins processing_system7_0_axi_periph/M03_ACLK] [get_bd_pins processing_system7_0_axi_periph/M04_ACLK] [get_bd_pins processing_system7_0_axi_periph/M05_ACLK] [get_bd_pins processing_system7_0_axi_periph/S00_ACLK] [get_bd_pins pwm_0/s00_axi_aclk] [get_bd_pins rst_processing_system7_0_100M/slowest_sync_clk]
  connect_bd_net -net processing_system7_0_FCLK_RESET0_N [get_bd_pins processing_system7_0/FCLK_RESET0_N] [get_bd_pins rst_processing_system7_0_100M/ext_reset_in]
  connect_bd_net -net pwm_0_pwmPulse [get_bd_ports pwmPulse] [get_bd_pins pwm_0/pwmPulse]
  connect_bd_net -net pwm_dir_1_dout [get_bd_ports pwmDir] [get_bd_pins pwm_dir_1/dout]
  connect_bd_net -net pwm_oe_0_dout [get_bd_ports pwmOe] [get_bd_pins pwm_oe_0/dout]
  connect_bd_net -net rst_processing_system7_0_100M_interconnect_aresetn [get_bd_pins processing_system7_0_axi_periph/ARESETN] [get_bd_pins rst_processing_system7_0_100M/interconnect_aresetn]
  connect_bd_net -net rst_processing_system7_0_100M_peripheral_aresetn [get_bd_pins axi_gpio_0/s_axi_aresetn] [get_bd_pins axi_gpio_1/s_axi_aresetn] [get_bd_pins axi_gpio_uwb/s_axi_aresetn] [get_bd_pins axi_quad_spi_uwb/s_axi_aresetn] [get_bd_pins axi_timer_0/s_axi_aresetn] [get_bd_pins processing_system7_0_axi_periph/M00_ARESETN] [get_bd_pins processing_system7_0_axi_periph/M01_ARESETN] [get_bd_pins processing_system7_0_axi_periph/M02_ARESETN] [get_bd_pins processing_system7_0_axi_periph/M03_ARESETN] [get_bd_pins processing_system7_0_axi_periph/M04_ARESETN] [get_bd_pins processing_system7_0_axi_periph/M05_ARESETN] [get_bd_pins processing_system7_0_axi_periph/S00_ARESETN] [get_bd_pins pwm_0/s00_axi_aresetn] [get_bd_pins rst_processing_system7_0_100M/peripheral_aresetn]
  connect_bd_net -net util_vector_logic_0_Res [get_bd_pins util_vector_logic_0/Res] [get_bd_pins util_vector_logic_1/Op1]
  connect_bd_net -net util_vector_logic_1_Res [get_bd_ports uwb_spi_sck] [get_bd_pins util_vector_logic_1/Res]
  connect_bd_net -net uwb_gpio_in_1 [get_bd_ports uwb_gpio_in] [get_bd_pins xlconcat_1/In0]
  connect_bd_net -net uwb_spi_miso_1 [get_bd_ports uwb_spi_miso] [get_bd_pins axi_quad_spi_uwb/io1_i]
  connect_bd_net -net xlconcat_0_dout [get_bd_pins processing_system7_0/IRQ_F2P] [get_bd_pins xlconcat_0/dout]
  connect_bd_net -net xlconcat_1_dout [get_bd_pins axi_gpio_uwb/gpio2_io_i] [get_bd_pins xlconcat_1/dout]
  connect_bd_net -net xlconstant_0_dout [get_bd_pins iobuf_ak_2/din] [get_bd_pins iobuf_ak_3/din] [get_bd_pins xlconstant_0/dout]
  connect_bd_net -net xlslice_0_Dout [get_bd_pins iobuf_ak_3/tin] [get_bd_pins xlslice_4/Dout]
  connect_bd_net -net xlslice_3_Dout [get_bd_pins iobuf_ak_2/tin] [get_bd_pins xlslice_3/Dout]

  # Create address segments
  create_bd_addr_seg -range 0x10000 -offset 0x41200000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_gpio_0/S_AXI/Reg] SEG_axi_gpio_0_Reg
  create_bd_addr_seg -range 0x10000 -offset 0x41220000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_gpio_1/S_AXI/Reg] SEG_axi_gpio_1_Reg
  create_bd_addr_seg -range 0x10000 -offset 0x41210000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_gpio_uwb/S_AXI/Reg] SEG_axi_gpio_uwb_Reg
  create_bd_addr_seg -range 0x10000 -offset 0x41E00000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_quad_spi_uwb/AXI_LITE/Reg] SEG_axi_quad_spi_uwb_Reg
  create_bd_addr_seg -range 0x10000 -offset 0x42800000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs axi_timer_0/S_AXI/Reg] SEG_axi_timer_0_Reg
  create_bd_addr_seg -range 0x10000 -offset 0x43C00000 [get_bd_addr_spaces processing_system7_0/Data] [get_bd_addr_segs pwm_0/S00_AXI/S00_AXI_reg] SEG_pwm_0_S00_AXI_reg

  # Perform GUI Layout
  regenerate_bd_layout -layout_string {
   guistr: "# # String gsaved with Nlview 6.5.5  2015-06-26 bk=1.3371 VDI=38 GEI=35 GUI=JA:1.8
#  -string -flagsOSRD
preplace port DDR -pg 1 -y 570 -defaultsOSRD
preplace port sws_8bits -pg 1 -y 530 -defaultsOSRD
preplace port leds_8bits -pg 1 -y 390 -defaultsOSRD
preplace port uwb_spi_mosi -pg 1 -y 850 -defaultsOSRD
preplace port btns_5bits -pg 1 -y 550 -defaultsOSRD
preplace port uwb_gpio_in -pg 1 -y 980 -defaultsOSRD
preplace port uwb_rst_n -pg 1 -y 890 -defaultsOSRD
preplace port uwb_gpio_io -pg 1 -y 740 -defaultsOSRD
preplace port uwb_spi_miso -pg 1 -y 1090 -defaultsOSRD
preplace port FIXED_IO -pg 1 -y 610 -defaultsOSRD
preplace portBus pwmOe -pg 1 -y 640 -defaultsOSRD
preplace portBus pwmPulse -pg 1 -y 260 -defaultsOSRD
preplace portBus uwb_spi_sel_n -pg 1 -y 1030 -defaultsOSRD
preplace portBus uwb_spi_sck -pg 1 -y 1000 -defaultsOSRD
preplace portBus pwmDir -pg 1 -y 510 -defaultsOSRD
preplace inst iobuf_ak_3 -pg 1 -lvl 5 -y 730 -defaultsOSRD
preplace inst axi_quad_spi_uwb -pg 1 -lvl 3 -y 860 -defaultsOSRD
preplace inst xlconstant_0 -pg 1 -lvl 4 -y 700 -defaultsOSRD
preplace inst pwm_dir_1 -pg 1 -lvl 5 -y 430 -defaultsOSRD
preplace inst rst_processing_system7_0_100M -pg 1 -lvl 2 -y 370 -defaultsOSRD
preplace inst xlslice_3 -pg 1 -lvl 4 -y 890 -defaultsOSRD
preplace inst util_vector_logic_0 -pg 1 -lvl 4 -y 1000 -defaultsOSRD
preplace inst xlslice_4 -pg 1 -lvl 4 -y 790 -defaultsOSRD
preplace inst xlconcat_0 -pg 1 -lvl 1 -y 520 -defaultsOSRD
preplace inst axi_timer_0 -pg 1 -lvl 5 -y 30 -defaultsOSRD
preplace inst axi_gpio_0 -pg 1 -lvl 5 -y 320 -defaultsOSRD
preplace inst pwm_oe_0 -pg 1 -lvl 5 -y 550 -defaultsOSRD
preplace inst axi_gpio_1 -pg 1 -lvl 3 -y 510 -defaultsOSRD
preplace inst util_vector_logic_1 -pg 1 -lvl 5 -y 1000 -defaultsOSRD
preplace inst xlconcat_1 -pg 1 -lvl 1 -y 1000 -defaultsOSRD
preplace inst axi_gpio_uwb -pg 1 -lvl 3 -y 660 -defaultsOSRD
preplace inst pwm_0 -pg 1 -lvl 5 -y 190 -defaultsOSRD
preplace inst iobuf_ak_2 -pg 1 -lvl 5 -y 880 -defaultsOSRD
preplace inst processing_system7_0 -pg 1 -lvl 2 -y 680 -defaultsOSRD
preplace inst processing_system7_0_axi_periph -pg 1 -lvl 3 -y 210 -defaultsOSRD
preplace netloc processing_system7_0_DDR 1 2 4 NJ 740 NJ 600 NJ 600 NJ
preplace netloc pwm_0_pwmPulse 1 5 1 NJ
preplace netloc axi_quad_spi_uwb_ss_o 1 3 3 1300 840 NJ 940 NJ
preplace netloc xlslice_3_Dout 1 4 1 N
preplace netloc processing_system7_0_axi_periph_M03_AXI 1 2 2 740 -10 1300
preplace netloc processing_system7_0_axi_periph_M00_AXI 1 3 2 N 160 1560
preplace netloc iobuf_ak_2_dout 1 0 6 -20 1070 NJ 1070 NJ 1070 NJ 1070 NJ 1070 1860
preplace netloc iobuf_ak_3_dout 1 0 6 -20 800 NJ 800 NJ 760 NJ 650 NJ 650 1860
preplace netloc processing_system7_0_M_AXI_GP0 1 2 1 690
preplace netloc processing_system7_0_axi_periph_M05_AXI 1 2 2 750 430 1290
preplace netloc util_vector_logic_0_Res 1 4 1 N
preplace netloc xlconcat_1_dout 1 1 3 N 1000 NJ 1000 1290
preplace netloc processing_system7_0_FCLK_RESET0_N 1 1 2 220 280 670
preplace netloc axi_gpio_1_GPIO2 1 3 3 NJ 500 NJ 500 NJ
preplace netloc processing_system7_0_axi_periph_M02_AXI 1 3 2 1350 170 N
preplace netloc uwb_gpio_in_1 1 0 1 N
preplace netloc axi_quad_spi_uwb_sck_o 1 3 1 1310
preplace netloc rst_processing_system7_0_100M_peripheral_aresetn 1 2 3 720 -20 1330 -10 1550
preplace netloc xlconcat_0_dout 1 1 1 200
preplace netloc xlconstant_0_dout 1 4 1 1560
preplace netloc processing_system7_0_FIXED_IO 1 2 4 NJ 580 NJ 580 NJ 610 NJ
preplace netloc axi_gpio_1_ip2intc_irpt 1 0 4 -30 -50 NJ -50 NJ -50 1320
preplace netloc uwb_spi_miso_1 1 0 4 N 1090 NJ 1090 NJ 1090 1280
preplace netloc axi_gpio_0_GPIO 1 5 1 1910
preplace netloc util_vector_logic_1_Res 1 5 1 N
preplace netloc Net1 1 5 1 N
preplace netloc Net 1 5 1 N
preplace netloc pwm_dir_1_dout 1 5 1 NJ
preplace netloc rst_processing_system7_0_100M_interconnect_aresetn 1 2 1 680
preplace netloc processing_system7_0_FCLK_CLK0 1 1 4 210 270 710 -30 N -30 1570
preplace netloc axi_gpio_1_GPIO 1 3 3 NJ 490 NJ 490 NJ
preplace netloc axi_gpio_uwb_gpio_io_o 1 3 1 1320
preplace netloc processing_system7_0_axi_periph_M04_AXI 1 2 2 750 750 1300
preplace netloc processing_system7_0_axi_periph_M01_AXI 1 3 2 1340 -20 N
preplace netloc axi_quad_spi_uwb_io0_o 1 3 3 1340 630 NJ 630 NJ
preplace netloc xlslice_0_Dout 1 4 1 1570
preplace netloc axi_quad_spi_uwb_ip2intc_irpt 1 0 4 -30 910 NJ 910 NJ 960 1270
preplace netloc axi_timer_0_interrupt 1 0 6 -20 450 NJ 460 NJ 440 NJ 440 1560 480 1900
preplace netloc pwm_oe_0_dout 1 5 1 NJ
levelinfo -pg 1 -50 110 470 1120 1460 1730 1930 -top -70 -bot 1150
",
}

  # Restore current instance
  current_bd_instance $oldCurInst

  save_bd_design
}
# End of create_root_design()


##################################################################
# MAIN FLOW
##################################################################

create_root_design ""


