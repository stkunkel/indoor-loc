webtalk_init -webtalk_dir /home/ruppi/Masterarbeit/Zynq/indoor-loc/indoor-loc/indoor-loc.sdk/webtalk
webtalk_register_client -client project
webtalk_add_data -client project -key date_generated -value "Mon Nov 2 11:14:07 2015" -context "software_version_and_target_device"
webtalk_add_data -client project -key product_version -value "SDK v2015.3" -context "software_version_and_target_device"
webtalk_add_data -client project -key build_version -value "2015.3" -context "software_version_and_target_device"
webtalk_add_data -client project -key os_platform -value "amd64" -context "software_version_and_target_device"
webtalk_add_data -client project -key registration_id -value "" -context "software_version_and_target_device"
webtalk_add_data -client project -key tool_flow -value "SDK" -context "software_version_and_target_device"
webtalk_add_data -client project -key beta -value "false" -context "software_version_and_target_device"
webtalk_add_data -client project -key route_design -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_family -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_device -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_package -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key target_speed -value "NA" -context "software_version_and_target_device"
webtalk_add_data -client project -key random_id -value "rejsruq3smt6ihof882h6bijtn" -context "software_version_and_target_device"
webtalk_add_data -client project -key project_id -value "2015.3_2" -context "software_version_and_target_device"
webtalk_add_data -client project -key project_iteration -value "2" -context "software_version_and_target_device"
webtalk_add_data -client project -key os_name -value "" -context "user_environment"
webtalk_add_data -client project -key os_release -value "" -context "user_environment"
webtalk_add_data -client project -key cpu_name -value "" -context "user_environment"
webtalk_add_data -client project -key cpu_speed -value "" -context "user_environment"
webtalk_add_data -client project -key total_processors -value "" -context "user_environment"
webtalk_add_data -client project -key system_ram -value "" -context "user_environment"
webtalk_register_client -client sdk
webtalk_add_data -client sdk -key uid -value "1446459219000" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key isZynq -value "true" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key Processors -value "2" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key VivadoVersion -value "2015.3" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key Arch -value "zynq" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key Device -value "7z020" -context "sdk\\\\hardware/1446459219000"
webtalk_add_data -client sdk -key IsHandoff -value "true" -context "sdk\\\\hardware/1446459219000"
webtalk_transmit -clientid 2374985232 -regid "" -xml /home/ruppi/Masterarbeit/Zynq/indoor-loc/indoor-loc/indoor-loc.sdk/webtalk/usage_statistics_ext_sdk.xml -html /home/ruppi/Masterarbeit/Zynq/indoor-loc/indoor-loc/indoor-loc.sdk/webtalk/usage_statistics_ext_sdk.html -wdm /home/ruppi/Masterarbeit/Zynq/indoor-loc/indoor-loc/indoor-loc.sdk/webtalk/sdk_webtalk.wdm -intro "<H3>SDK Usage Report</H3><BR>"
webtalk_terminate
