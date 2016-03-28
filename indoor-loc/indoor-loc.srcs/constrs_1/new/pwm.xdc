###################################################################
# PWM on JB
###################################################################
#JB1 - pwmPulse(0)
set_property PACKAGE_PIN W12 [get_ports {pwmPulse[0]}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[0]}]
#JB2 - pwmPulse(1)
set_property PACKAGE_PIN W11 [get_ports {pwmPulse[1]}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[1]}]
#JB3 - pwmPulse(3)
set_property PACKAGE_PIN V10 [get_ports {pwmPulse[3]}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[3]}]
#JB4 - pwmPulse(5)
set_property PACKAGE_PIN W8 [get_ports {pwmPulse[5]}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[5]}]
#JB7 - pwmDir
set_property PACKAGE_PIN V12 [get_ports {pwmDir}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmDir}]
#JB8 - pwmOe
set_property PACKAGE_PIN W10 [get_ports {pwmOe}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmOe}]
#JB9 - pwmPulse(2)
set_property PACKAGE_PIN V9 [get_ports {pwmPulse[2]}]	
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[2]}]
#JB10 - pwmPulse(4)
set_property PACKAGE_PIN V8 [get_ports {pwmPulse[4]}]		
set_property IOSTANDARD LVCMOS33 [get_ports {pwmPulse[4]}]

###################################################################
# UWB on JC
###################################################################
# JC1P - uwb_spi_sel_n
set_property PACKAGE_PIN AB7 [get_ports {uwb_spi_sel_n[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_spi_sel_n[0]}]

#JC1N - uwb_spi_mosi
set_property PACKAGE_PIN AB6 [get_ports {uwb_spi_mosi}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_spi_mosi}]

#JC2P - uwb_spi_miso
set_property PACKAGE_PIN Y4 [get_ports {uwb_spi_miso}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_spi_miso}]

#JC2N - uwb_spi_sck[0]
set_property PACKAGE_PIN AA4 [get_ports {uwb_spi_sck[0]}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_spi_sck[0]}]

#JC3P - uwb_gpio_in[0]
set_property PACKAGE_PIN R6 [get_ports {uwb_gpio_in}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_gpio_in}]

#JC3N - uwb_gpio_io
set_property PACKAGE_PIN T6 [get_ports {uwb_gpio_io}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_gpio_io}]
set_property PULLDOWN true [get_ports {uwb_gpio_io}]

#JC4P - uwb_rst_n
set_property PACKAGE_PIN T4 [get_ports {uwb_rst_n}]
set_property IOSTANDARD LVCMOS33 [get_ports {uwb_rst_n}]