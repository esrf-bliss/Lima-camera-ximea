Ximea Tango device
==================

This is the reference documentation of the Ximea Tango device.

You can also find some useful information about the camera models/prerequisite/installation/configuration/compilation in the :ref:`Ximea camera plugin <camera-ximea>` section.


Properties
----------

========================= ========== ========================================== ====================================================
Property name             Mandatory  Default value                              Description
========================= ========== ========================================== ====================================================
camera_id                 Yes        N/A                                        Camera ID
trigger_gpi_port          No         PORT_2	                                    GPI port used by default for trigger input
timeout                   No         200                                        Timeout for internal loop (on top of exposure time)
startup_temp_control_mode No         AUTO                                       Startup temperature control mode
startup_target_temp	      No         25.0                                       Startup target temperature
========================= ========== ========================================== ====================================================


Attributes
----------

============================= ==== ========== ================================================================
Attribute name                RW   Type       Description
============================= ==== ========== ================================================================
trigger_polarity              rw   DevString  Select trigger polarity
software_trigger               w   DevBoolean Software trigger; write to generate trigger, reads always false
gpi_selector                  rw   DevString  Select GPI to configure
gpi_mode                      rw   DevString  Select GPI mode
gpi_level                     r    DevLong    Read GPI level
gpi_level_at_exp_start        r    DevLong    Read GPI level at exposure start
gpi_level_at_exp_end          r    DevLong    Read GPI level at exposure end
gpi_debounce                  rw   DevBoolean Enable GPI debounce
gpo_selector                  rw   DevString  Select GPO to configure
gpo_mode                      rw   DevString  Select GPO mode
led_selector                  rw   DevString  Select LED to configure
led_mode                      rw   DevString  Select LED mode
mode                          rw   DevString  Select configuration preset
gain_selector                 rw   DevString  Select gain type
gain                          rw   DevLong    Gain value
is_cooled                     r    DevLong    Is the camera cooled
temp_control_mode             rw   DevString  Temperature control mode
temp_target                   rw   DevDouble  Target temperature
thermometer                   rw   DevString  Select thermometer
temperature                   r    DevDouble  Thermometer temperature
temp_chip                     r    DevDouble  Camera sensor temperature
temp_housing                  r    DevDouble  Camera housing temperature
temp_back                     r    DevDouble  Camera housing back side temperature
temp_sensor                   r    DevDouble  Sensor board temperature
thermal_element               rw   DevString  Thermal control element
thermal_element_value         rw   DevDouble  Thermal element control value
exposure_selector             rw   DevString  Exposure mode selector
burst_count                   rw   DevLong    Burst count
downsampling                  rw   DevString  Downsampling value
downsampling_type             rw   DevString  Downsampling type
test_pattern_generator        rw   DevString  Test pattern generator
test_pattern                  rw   DevString  Test pattern
image_format                  rw   DevString  Image format
shutter                       rw   DevString  Shutter mode
taps                          rw   DevString  Sensor taps
auto_exposure_gain            rw   DevBoolean Auto exposure and gain control
auto_white_balance            rw   DevBoolean Auto white balance control
horizontal_flip               rw   DevBoolean Horizontal flip
vertical_flip                 rw   DevBoolean Vertical flip
interline_exp_mode            rw   DevString  Interline exposure mode
binning_engine                rw   DevString  Binning engine selector
horizontal_binning_pattern    rw   DevString  Binning horizontal pattern
vertical_binning_pattern      rw   DevString  Binning vertical pattern
decimation_engine             rw   DevString  Decimation engine selector
horizontal_decimation         rw   DevLong    Horizontal decimation value
vertical_decimation           rw   DevLong    Vertical decimation value
horizontal_decimation_pattern rw   DevString  Decimation horizontal pattern
vertical_decimation_pattern   rw   DevString  Decimation vertical pattern
exposure_priority             rw   DevDouble  Exposure priority (e.g. 0.8 - exposure 80%, gain 20%)
auto_gain_limit               rw   DevLong    Gain limit for AEAG procedure
auto_exposure_limit           rw   DevLong    Exposure limit for AEAG procedure
auto_intensity_level          rw   DevDouble  Target average intensity for AEAG procedure
bandwidth_limit               rw   DevDouble  Bandwidth limit
bandwidth_limit_enabled       rw   DevBoolean Enable bandwidth limiting
available_bandwidth           r    DevDouble  Measured available bandwidth
frame_rate                    rw   DevDouble  Frame rate (or limit)
counter_selector              rw   DevString  Counter selector
counter_value                 r    DevLong    Selected counter value
acq_timing_mode               rw   DevString  Acquisition timing mode
trigger_delay                 rw   DevLong    Trigger delay
acq_status                    r    DevBoolean Acquisition status
feature_selector              rw   DevString  Sensor additional features
feature_value                 rw   DevLong    Selected feature value
plugin_version                r    DevString  Plugin version number
timeout                       rw   DevLong    Timeout for internal loop (on top of exposure time)
============================= ==== ========== ================================================================


Commands
--------

=======================	=============== ================== ============================================
Command name            Arg. in         Arg. out           Description
=======================	=============== ================== ============================================
Init                    DevVoid         DevVoid	           Do not use
State                   DevVoid         DevLong            Return the device state
Status                  DevVoid         DevString          Return the device state as a string
getAttrStringValueList  DevString:      DevVarStringArray: Return the authorized string value list for
                        Attribute name  String value list  a given attribute name
=======================	=============== ================== ============================================
