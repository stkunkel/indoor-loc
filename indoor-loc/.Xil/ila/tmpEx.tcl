# Create project
create_project -name tmp_ila -force
set_property part xc7z020clg484-1 [current_project]
set_property target_language vhdl [current_project]
set_property simulator_language VHDL [current_project]
set_property coreContainer.enable false [current_project]
set_property board_part em.avnet.com:zed:part0:0.9 [current_project]
# Set the repo paths from the original project
set_property ip_repo_paths [list /home/rupprich/Zynq/ip_repo/pwm_1.0] [current_fileset]
update_ip_catalog

# Import the original IP, renamed to ila
import_ip -files /home/rupprich/Zynq/indoor-loc/indoor-loc.srcs/sources_1/bd/design_1/ip/design_1_ila_0_1/design_1_ila_0_1.xci -name ila
generate_target open_example [get_ips ila]
close_project
exit 0
