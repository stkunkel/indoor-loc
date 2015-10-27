#-------------------------------------------------------------
# Generated Example Tcl script for IP 'ila' (xilinx.com:ip:ila:6.0)
#-------------------------------------------------------------

# Declare source IP directory
set srcIpDir "/home/rupprich/Zynq/indoor-loc/.Xil/ila/tmp_ila.srcs/sources_1/ip/ila"

# Create project
create_project -name ila_example -force
set_property part xc7z020clg484-1 [current_project]
set_property target_language vhdl [current_project]
set_property simulator_language VHDL [current_project]
set_property coreContainer.enable false [current_project]
set_property board_part em.avnet.com:zed:part0:0.9 [current_project]
set returnCode 0

# Set the repo paths from the original project
set_property ip_repo_paths [list [list /home/rupprich/Zynq/ip_repo/pwm_1.0]] [current_fileset]
update_ip_catalog

# Import the original IP (excluding example files)
import_ip -files [list [file join $srcIpDir ila.xci]] -name ila
reset_target {open_example} [get_ips ila]

# Generate the IP
proc _filter_supported_targets {targets ip} {
  set res {}
  set all [get_property SUPPORTED_TARGETS $ip]
  foreach target $targets {
    lappend res {*}[lsearch -all -inline -nocase $all $target]
  }
  return $res
}
generate_target [_filter_supported_targets {instantiation_template synthesis simulation implementation shared_logic} [get_ips ila]] [get_ips ila]

# Add example synthesis HDL files
add_files -scan_for_includes -quiet -fileset [current_fileset] \
  [list [file join $srcIpDir example_ila.v]]

# Add example miscellaneous synthesis files
add_files -quiet -fileset [current_fileset] \
  [list [file join $srcIpDir example_ila.txt]] \
  [list [file join $srcIpDir xdc_ila.txt]]

# Add example XDC files
add_files -quiet -fileset [current_fileset -constrset] \
  [list [file join $srcIpDir example_ila.xdc]]


# Add example simulation HDL files
if { [catch {current_fileset -simset} exc] } { create_fileset -simset sim_1 }
add_files -quiet -scan_for_includes -fileset [current_fileset -simset] \
  [list [file join $srcIpDir example_tb_ila.v]]
set_property USED_IN_SYNTHESIS false [get_files [list [file join $srcIpDir example_tb_ila.v]]]

# Add example miscellaneous simulation files
if { [catch {current_fileset -simset} exc] } { create_fileset -simset sim_1 }
add_files -quiet -fileset [current_fileset -simset] \
  [list [file join $srcIpDir example_tb_top.txt]]

# Import all files while preserving hierarchy
import_files

# Set top
set_property TOP [lindex [find_top] 0] [current_fileset]

# Update compile order
update_compile_order -fileset [current_fileset]
update_compile_order -fileset [current_fileset -simset]
generate_target -quiet all [concat [ get_ips -quiet -filter scope=={} ] [get_files -quiet *bd ] ]
export_ip_user_files -force

set dfile "/home/rupprich/Zynq/indoor-loc/.Xil/ila/tmp_ila.srcs/sources_1/ip/ila/oepdone.txt"
set doneFile [open $dfile w]
puts $doneFile "Open Example Project DONE"
close $doneFile
if { $returnCode != 0 } {
  error "Problems were encountered while executing the example design script, please review the log files."
}
