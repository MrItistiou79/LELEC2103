# TCL File Generated by Component Editor 18.1
# Wed Jun 23 14:46:07 CEST 2021
# DO NOT MODIFY


# 
# FIFO2AVS "FIFO2AVS" v1.0
#  2021.06.23.14:46:07
# 
# 

# 
# request TCL package from ACDS 16.1
# 
package require -exact qsys 16.1


# 
# module FIFO2AVS
# 
set_module_property DESCRIPTION ""
set_module_property NAME FIFO2AVS
set_module_property VERSION 1.0
set_module_property INTERNAL false
set_module_property OPAQUE_ADDRESS_MAP true
set_module_property GROUP LELEC2102_IPs
set_module_property AUTHOR ""
set_module_property DISPLAY_NAME FIFO2AVS
set_module_property INSTANTIATE_IN_SYSTEM_MODULE true
set_module_property EDITABLE true
set_module_property REPORT_TO_TALKBACK false
set_module_property ALLOW_GREYBOX_GENERATION false
set_module_property REPORT_HIERARCHY false


# 
# file sets
# 
add_fileset QUARTUS_SYNTH QUARTUS_SYNTH "" ""
set_fileset_property QUARTUS_SYNTH TOP_LEVEL fifo2avs
set_fileset_property QUARTUS_SYNTH ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property QUARTUS_SYNTH ENABLE_FILE_OVERWRITE_MODE false
add_fileset_file fifo2avs.v VERILOG PATH ip/fifo2avs/fifo2avs.v TOP_LEVEL_FILE

add_fileset SIM_VERILOG SIM_VERILOG "" ""
set_fileset_property SIM_VERILOG TOP_LEVEL fifo2avs
set_fileset_property SIM_VERILOG ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VERILOG ENABLE_FILE_OVERWRITE_MODE true
add_fileset_file fifo2avs.v VERILOG PATH ip/fifo2avs/fifo2avs.v

add_fileset SIM_VHDL SIM_VHDL "" ""
set_fileset_property SIM_VHDL TOP_LEVEL fifo2avs
set_fileset_property SIM_VHDL ENABLE_RELATIVE_INCLUDE_PATHS false
set_fileset_property SIM_VHDL ENABLE_FILE_OVERWRITE_MODE true
add_fileset_file fifo2avs.v VERILOG PATH ip/fifo2avs/fifo2avs.v


# 
# parameters
# 
add_parameter datawidth INTEGER 48 ""
set_parameter_property datawidth DEFAULT_VALUE 48
set_parameter_property datawidth DISPLAY_NAME datawidth
set_parameter_property datawidth TYPE INTEGER
set_parameter_property datawidth UNITS None
set_parameter_property datawidth ALLOWED_RANGES -2147483648:2147483647
set_parameter_property datawidth DESCRIPTION ""
set_parameter_property datawidth HDL_PARAMETER true


# 
# display items
# 


# 
# connection point avalon_streaming_source
# 
add_interface avalon_streaming_source avalon_streaming start
set_interface_property avalon_streaming_source associatedClock clock_sink
set_interface_property avalon_streaming_source associatedReset reset_sink
set_interface_property avalon_streaming_source dataBitsPerSymbol 12
set_interface_property avalon_streaming_source errorDescriptor ""
set_interface_property avalon_streaming_source firstSymbolInHighOrderBits true
set_interface_property avalon_streaming_source maxChannel 0
set_interface_property avalon_streaming_source readyLatency 0
set_interface_property avalon_streaming_source ENABLED true
set_interface_property avalon_streaming_source EXPORT_OF ""
set_interface_property avalon_streaming_source PORT_NAME_MAP ""
set_interface_property avalon_streaming_source CMSIS_SVD_VARIABLES ""
set_interface_property avalon_streaming_source SVD_ADDRESS_GROUP ""

add_interface_port avalon_streaming_source avalon_streaming_source_data data Output datawidth
add_interface_port avalon_streaming_source avalon_streaming_source_valid valid Output 1


# 
# connection point clock_sink
# 
add_interface clock_sink clock end
set_interface_property clock_sink clockRate 0
set_interface_property clock_sink ENABLED true
set_interface_property clock_sink EXPORT_OF ""
set_interface_property clock_sink PORT_NAME_MAP ""
set_interface_property clock_sink CMSIS_SVD_VARIABLES ""
set_interface_property clock_sink SVD_ADDRESS_GROUP ""

add_interface_port clock_sink clock_sink_clk clk Input 1


# 
# connection point reset_sink
# 
add_interface reset_sink reset end
set_interface_property reset_sink associatedClock clock_sink
set_interface_property reset_sink synchronousEdges DEASSERT
set_interface_property reset_sink ENABLED true
set_interface_property reset_sink EXPORT_OF ""
set_interface_property reset_sink PORT_NAME_MAP ""
set_interface_property reset_sink CMSIS_SVD_VARIABLES ""
set_interface_property reset_sink SVD_ADDRESS_GROUP ""

add_interface_port reset_sink reset_sink_reset reset Input 1


# 
# connection point conduit_in
# 
add_interface conduit_in conduit end
set_interface_property conduit_in associatedClock clock_sink
set_interface_property conduit_in associatedReset reset_sink
set_interface_property conduit_in ENABLED true
set_interface_property conduit_in EXPORT_OF ""
set_interface_property conduit_in PORT_NAME_MAP ""
set_interface_property conduit_in CMSIS_SVD_VARIABLES ""
set_interface_property conduit_in SVD_ADDRESS_GROUP ""

add_interface_port conduit_in fifo_wdata wdata Input datawidth
add_interface_port conduit_in fifo_wrreq wrreq Input 1

