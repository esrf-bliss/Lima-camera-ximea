############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2020
# European Synchrotron Radiation Facility
# CS40220 38043 Grenoble Cedex 9
# FRANCE
#
# Contact: lima@esrf.fr
#
# This is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses/>.
############################################################################

# ----------------------------------------------------------------------------
# The Ximea camera plugin TANGO interface
# ----------------------------------------------------------------------------

import PyTango

from Lima import Core
from Lima import Ximea as Xi
from Lima.Server import AttrHelper


# this is needed by get_control, so needs to be outside class Ximea
_Mode = {
	"12_STD_L": Xi.Camera.Mode_12_STD_L,
	"12_STD_H": Xi.Camera.Mode_12_STD_H,
	"14_STD_L": Xi.Camera.Mode_14_STD_L,
	"NONE": Xi.Camera.Mode_None,
	"14_STD_H": Xi.Camera.Mode_14_STD_H,
	"2_12_CMS_S_L": Xi.Camera.Mode_2_12_CMS_S_L,
	"2_12_CMS_S_H": Xi.Camera.Mode_2_12_CMS_S_H,
	"2_14_CMS_S_L": Xi.Camera.Mode_2_14_CMS_S_L,
	"2_14_CMS_S_H": Xi.Camera.Mode_2_14_CMS_S_H,
	"4_12_CMS_S_L": Xi.Camera.Mode_4_12_CMS_S_L,
	"4_12_CMS_S_H": Xi.Camera.Mode_4_12_CMS_S_H,
	"4_14_CMS_S_L": Xi.Camera.Mode_4_14_CMS_S_L,
	"4_14_CMS_S_H": Xi.Camera.Mode_4_14_CMS_S_H,
	"2_12_HDR_HL": Xi.Camera.Mode_2_12_HDR_HL,
	"2_12_HDR_L": Xi.Camera.Mode_2_12_HDR_L,
	"2_12_HDR_H": Xi.Camera.Mode_2_12_HDR_H,
	"4_12_CMS_HDR_HL": Xi.Camera.Mode_4_12_CMS_HDR_HL,
	"2_14_HDR_L": Xi.Camera.Mode_2_14_HDR_L,
	"2_14_HDR_H": Xi.Camera.Mode_2_14_HDR_H,
	"2_12_CMS_A_L": Xi.Camera.Mode_2_12_CMS_A_L,
	"2_12_CMS_A_H": Xi.Camera.Mode_2_12_CMS_A_H
}

_GpiSelector = {
	"PORT_1": Xi.Camera.GPISelector_Port_1,
	"PORT_2": Xi.Camera.GPISelector_Port_2,
	"PORT_3": Xi.Camera.GPISelector_Port_3,
	"PORT_4": Xi.Camera.GPISelector_Port_4,
	"PORT_5": Xi.Camera.GPISelector_Port_5,
	"PORT_6": Xi.Camera.GPISelector_Port_6,
	"PORT_7": Xi.Camera.GPISelector_Port_7,
	"PORT_8": Xi.Camera.GPISelector_Port_8,
	"PORT_9": Xi.Camera.GPISelector_Port_9,
	"PORT_10": Xi.Camera.GPISelector_Port_10,
	"PORT_11": Xi.Camera.GPISelector_Port_11,
	"PORT_12": Xi.Camera.GPISelector_Port_12,
}

_TempControlMode = {
	"OFF": Xi.Camera.TempControlMode_Off,
	"AUTO": Xi.Camera.TempControlMode_Auto,
	"MANUAL": Xi.Camera.TempControlMode_Manual,
}


class Ximea(PyTango.Device_4Impl):
	Core.DEB_CLASS(Core.DebModApplication, 'LimaCCDs')

	# ------------------------------------------------------------------
	#    Device constructor
	# ------------------------------------------------------------------
	def __init__(self, *args):
		PyTango.Device_4Impl.__init__(self, *args)

		self.__Mode = _Mode

		self.__GainSelector = {
			"ALL": Xi.Camera.GainSelector_All,
			"ANALOG_ALL": Xi.Camera.GainSelector_Analog_All,
			"DIGITAL_ALL": Xi.Camera.GainSelector_Digital_All,
			"ANALOG_TAP1": Xi.Camera.GainSelector_Analog_Tap1,
			"ANALOG_TAP2": Xi.Camera.GainSelector_Analog_Tap2,
			"ANALOG_TAP3": Xi.Camera.GainSelector_Analog_Tap3,
			"ANALOG_TAP4": Xi.Camera.GainSelector_Analog_Tap4,
			"ANALOG_NORTH": Xi.Camera.GainSelector_Analog_North,
			"ANALOG_SOUTH": Xi.Camera.GainSelector_Analog_South,
		}

		self.__TempControlMode = _TempControlMode

		self.__Thermometer = {
			"DIE_RAW": Xi.Camera.Thermometer_Die_Raw,
			"SENSOR_DIE": Xi.Camera.Thermometer_Sensor_Die,
			"SENSOR_BOARD": Xi.Camera.Thermometer_Sensor_Board,
			"INTERFACE_BOARD": Xi.Camera.Thermometer_Interface_Board,
			"FRONT_HOUSING": Xi.Camera.Thermometer_Front_Housing,
			"REAR_HOUSING": Xi.Camera.Thermometer_Rear_Housing,
			"TEC1_COLD": Xi.Camera.Thermometer_TEC1_Cold,
			"TEC1_HOT": Xi.Camera.Thermometer_TEC1_Hot,
		}

		self.__ThermalElement = {
			"TEC1": Xi.Camera.ThermalElement_TEC1,
			"TEC2": Xi.Camera.ThermalElement_TEC2,
			"FAN1": Xi.Camera.ThermalElement_Fan1,
			"FAN_START_THRESHOLD": Xi.Camera.ThermalElement_Fan_Start_Threshold,
		}

		self.__ExposureSelector = {
			"COMMON": Xi.Camera.ExposureSelector_Common,
			"GROUP1": Xi.Camera.ExposureSelector_Group1,
			"GROUP2": Xi.Camera.ExposureSelector_Group2,
		}

		self.__Downsampling = {
			"1X1": Xi.Camera.Downsampling_1x1,
			"2X2": Xi.Camera.Downsampling_2x2,
			"3X3": Xi.Camera.Downsampling_3x3,
			"4X4": Xi.Camera.Downsampling_4x4,
			"5X5": Xi.Camera.Downsampling_5x5,
			"6X6": Xi.Camera.Downsampling_6x6,
			"7X7": Xi.Camera.Downsampling_7x7,
			"8X8": Xi.Camera.Downsampling_8x8,
			"9X9": Xi.Camera.Downsampling_9x9,
			"10X10": Xi.Camera.Downsampling_10x10,
			"16X16": Xi.Camera.Downsampling_16x16,
		}

		self.__DownsamplingType = {
			"BINNING": Xi.Camera.DownsamplingType_Binning,
			"SKIPPING": Xi.Camera.DownsamplingType_Skipping,
		}

		self.__TestPatternGenerator = {
			"SENSOR": Xi.Camera.TestPatternGenerator_Sensor,
			"FPGA": Xi.Camera.TestPatternGenerator_FPGA,
		}

		self.__TestPattern = {
			"OFF": Xi.Camera.TestPattern_Off,
			"BLACK": Xi.Camera.TestPattern_Black,
			"WHITE": Xi.Camera.TestPattern_White,
			"GREY_HORIZONTAL": Xi.Camera.TestPattern_Grey_Horizontal,
			"GREY_VERTICAL": Xi.Camera.TestPattern_Grey_Vertical,
			"GREY_HORIZONTAL_MOVING": Xi.Camera.TestPattern_Grey_Horizontal_Moving,
			"GREY_VERTICAL_MOVING": Xi.Camera.TestPattern_Grey_Vertical_Moving,
			"LINE_HORIZONTAL_MOVING": Xi.Camera.TestPattern_Line_Horizontal_Moving,
			"LINE_VERTICAL_MOVING": Xi.Camera.TestPattern_Line_Vertical_Moving,
			"COLOR_BAR": Xi.Camera.TestPattern_Color_Bar,
			"FRAME_COUNTER": Xi.Camera.TestPattern_Frame_Counter,
			"COUNTER": Xi.Camera.TestPattern_Counter,
		}

		self.__ImageFormat = {
			"MONO8": Xi.Camera.ImageFormat_Mono8,
			"MONO16": Xi.Camera.ImageFormat_Mono16,
			"RGB24": Xi.Camera.ImageFormat_RGB24,
			"RGB32": Xi.Camera.ImageFormat_RGB32,
			"RGB_PLANAR": Xi.Camera.ImageFormat_RGB_Planar,
			"RAW8": Xi.Camera.ImageFormat_Raw8,
			"RAW16": Xi.Camera.ImageFormat_Raw16,
			"TRANSPORT": Xi.Camera.ImageFormat_Transport,
			"RGB48": Xi.Camera.ImageFormat_RGB48,
			"RGB64": Xi.Camera.ImageFormat_RGB64,
			"RGB16_PLANAR": Xi.Camera.ImageFormat_RGB16_Planar,
			"RAW8X2": Xi.Camera.ImageFormat_Raw8x2,
			"RAW8X4": Xi.Camera.ImageFormat_Raw8x4,
			"RAW16X2": Xi.Camera.ImageFormat_Raw16x2,
			"RAW16X4": Xi.Camera.ImageFormat_Raw16x4,
			"RAW32": Xi.Camera.ImageFormat_Raw32,
			"RAW32_FLOAT": Xi.Camera.ImageFormat_Raw32_Float,
		}

		self.__Shutter = {
			"GLOBAL": Xi.Camera.Shutter_Global,
			"ROLLING": Xi.Camera.Shutter_Rolling,
			"RESET_RELEASE": Xi.Camera.Shutter_Reset_Release,
		}

		self.__Taps = {
			"1": Xi.Camera.Taps_1,
			"2": Xi.Camera.Taps_2,
			"4": Xi.Camera.Taps_4,
		}

		self.__InterlineExpMode = {
			"OFF": Xi.Camera.InterlineExpMode_Off,
			"ON": Xi.Camera.InterlineExpMode_On,
		}

		self.__BinningEngine = {
			"SENSOR": Xi.Camera.BinningEngine_Sensor,
			"FPGA": Xi.Camera.BinningEngine_FPGA,
			"HOST": Xi.Camera.BinningEngine_Host,
		}

		self.__BinningPattern = {
			"MONO": Xi.Camera.BinningPattern_Mono,
			"BAYER": Xi.Camera.BinningPattern_Bayer,
		}

		self.__DecimationEngine = {
			"SENSOR": Xi.Camera.DecimationEngine_Sensor,
			"FPGA": Xi.Camera.DecimationEngine_FPGA,
			"HOST": Xi.Camera.DecimationEngine_Host,
		}

		self.__DecimationPattern = {
			"MONO": Xi.Camera.DecimationPattern_Mono,
			"BAYER": Xi.Camera.DecimationPattern_Bayer,
		}

		self.__CounterSelector = {
			"SKIPPED_FRAMES_TRANSPORT": Xi.Camera.CounterSelector_Skipped_Frames_Transport,
			"SKIPPED_FRAMES_API": Xi.Camera.CounterSelector_Skipped_Frames_API,
			"MISSED_TRIGGER_OVERLAP": Xi.Camera.CounterSelector_Missed_Trigger_Overlap,
			"MISSED_TRIGGER_BUFFER_FULL": Xi.Camera.CounterSelector_Missed_Trigger_Buffer_Full,
			"FRAME_BUFFER_FULL": Xi.Camera.CounterSelector_Frame_Buffer_Full,
		}

		self.__AcqTimingMode = {
			"FREE_RUN": Xi.Camera.AcqTimingMode_Free_Run,
			"FRAME_RATE": Xi.Camera.AcqTimingMode_Frame_Rate,
			"FRAME_RATE_LIMIT": Xi.Camera.AcqTimingMode_Frame_Rate_Limit,
		}

		self.__FeatureSelector = {
			"ZERO_ROT_ENABLE": Xi.Camera.FeatureSelector_Zero_ROT_Enable,
			"BLACK_LEVEL_CLAMP": Xi.Camera.FeatureSelector_Black_Level_Clamp,
			"FPGA_DIGITAL_GAIN_DISABLE": Xi.Camera.FeatureSelector_FPGA_Digital_Gain_Disable,
			"ACQUISITION_RUNNING": Xi.Camera.FeatureSelector_Acquisition_Running,
			"TIMING_MODE": Xi.Camera.FeatureSelector_Timing_Mode,
			"PARALLEL_ADC": Xi.Camera.FeatureSelector_Parallel_ADC,
			"BLACK_LEVEL_OFFSET_RAW": Xi.Camera.FeatureSelector_Black_Level_Offset_Raw,
		}

		self.__TriggerPolarity = {
			"LOW / FALLING": Xi.Camera.TriggerPolarity_Low_Falling,
			"HIGH / RISING": Xi.Camera.TriggerPolarity_High_Rising
		}

		self.__GpiSelector = _GpiSelector

		self.__GpiMode = {
			"OFF": Xi.Camera.GPIMode_Off,
			"TRIGGER": Xi.Camera.GPIMode_Trigger,
			"EXT_EVENT": Xi.Camera.GPIMode_Ext_Event,
		}

		self.__GpoSelector = {
			"PORT_1": Xi.Camera.GPOSelector_Port_1,
			"PORT_2": Xi.Camera.GPOSelector_Port_2,
			"PORT_3": Xi.Camera.GPOSelector_Port_3,
			"PORT_4": Xi.Camera.GPOSelector_Port_4,
			"PORT_5": Xi.Camera.GPOSelector_Port_5,
			"PORT_6": Xi.Camera.GPOSelector_Port_6,
			"PORT_7": Xi.Camera.GPOSelector_Port_7,
			"PORT_8": Xi.Camera.GPOSelector_Port_8,
			"PORT_9": Xi.Camera.GPOSelector_Port_9,
			"PORT_10": Xi.Camera.GPOSelector_Port_10,
			"PORT_11": Xi.Camera.GPOSelector_Port_11,
			"PORT_12": Xi.Camera.GPOSelector_Port_12,
		}

		self.__GpoMode = {
			"OFF": Xi.Camera.GPOMode_Off,
			"ON": Xi.Camera.GPOMode_On,
			"FRAME_ACTIVE": Xi.Camera.GPOMode_Frame_Active,
			"NEG_FRAME_ACTIVE": Xi.Camera.GPOMode_Neg_Frame_Active,
			"EXPOSURE_ACTIVE": Xi.Camera.GPOMode_Exposure_Active,
			"NEG_EXPOSURE_ACTIVE": Xi.Camera.GPOMode_Neg_Exposure_Active,
			"FRAME_TRIGGER_WAIT": Xi.Camera.GPOMode_Frame_Trigger_Wait,
			"NEG_FRAME_TRIGGER_WAIT": Xi.Camera.GPOMode_Neg_Frame_Trigger_Wait,
			"EXPOSURE_PULSE": Xi.Camera.GPOMode_Exposure_Pulse,
			"NEG_EXPOSURE_PULSE": Xi.Camera.GPOMode_Neg_Exposure_Pulse,
			"BUSY": Xi.Camera.GPOMode_Busy,
			"NEG_BUSY": Xi.Camera.GPOMode_Neg_Busy,
			"HIGH_IMPEDANCE": Xi.Camera.GPOMode_High_Impedance,
			"FRAME_BUFFER_OVERFLOW": Xi.Camera.GPOMode_Frame_Buffer_Overflow,
			"EXPOSURE_ACTIVE_FIRST_ROW": Xi.Camera.GPOMode_Exposure_Active_First_Row,
			"NEG_EXPOSURE_ACTIVE_FIRST_ROW": Xi.Camera.GPOMode_Neg_Exposure_Active_First_Row,
			"EXPOSURE_ACTIVE_ALL_ROWS": Xi.Camera.GPOMode_Exposure_Active_All_Rows,
			"NEG_EXPOSURE_ACTIVE_ALL_ROWS": Xi.Camera.GPOMode_Neg_Exposure_Active_All_Rows,
		}

		self.__LedSelector = {
			"LED_1": Xi.Camera.LEDSelector_1,
			"LED_2": Xi.Camera.LEDSelector_2,
			"LED_3": Xi.Camera.LEDSelector_3,
			"LED_4": Xi.Camera.LEDSelector_4,
			"LED_5": Xi.Camera.LEDSelector_5,
		}

		self.__LedMode = {
			"HEARTBEAT": Xi.Camera.LEDMode_Heartbeat,
			"TRIGGER_ACTIVE": Xi.Camera.LEDMode_Trigger_Active,
			"EXT_EVENT_ACTIVE": Xi.Camera.LEDMode_Ext_Event_Active,
			"LINK": Xi.Camera.LEDMode_Link,
			"ACQUISITION": Xi.Camera.LEDMode_Acquisition,
			"EXPOSURE_ACTIVE": Xi.Camera.LEDMode_Exposure_Active,
			"FRAME_ACTIVE": Xi.Camera.LEDMode_Frame_Active,
			"OFF": Xi.Camera.LEDMode_Off,
			"ON": Xi.Camera.LEDMode_On,
			"BLINK": Xi.Camera.LEDMode_Blink,
		}

		self.init_device()

	# ------------------------------------------------------------------
	#    Device destructor
	# ------------------------------------------------------------------
	def delete_device(self):
		pass

	# ------------------------------------------------------------------
	#    Device initialization
	# ------------------------------------------------------------------
	@Core.DEB_MEMBER_FUNCT
	def init_device(self):
		self.set_state(PyTango.DevState.ON)
		self.get_device_properties(self.get_device_class())

	# ------------------------------------------------------------------
	#    getAttrStringValueList command:
	#
	#    Description: return a list of authorized values if any
	#    argout: DevVarStringArray
	# ------------------------------------------------------------------
	@Core.DEB_MEMBER_FUNCT
	def getAttrStringValueList(self, attr_name):
		# use AttrHelper
		return AttrHelper.get_attr_string_value_list(self, attr_name)

	# ------------------------------------------------------------------
	#
	#    Ximea read/write attribute methods
	#
	# ------------------------------------------------------------------
	def __getattr__(self, name):
		# use AttrHelper
		return AttrHelper.get_attr_4u(self, name, _XimeaCam)


# ------------------------------------------------------------------
#
#    BaslerClass class definition
#
# ------------------------------------------------------------------
class XimeaClass(PyTango.DeviceClass):

	class_property_list = {}

	device_property_list = {
		'camera_id': [
			PyTango.DevString,
			"Camera ID",
			None
		],
		"trigger_gpi_port": [
			PyTango.DevString,
			"GPI port used by default for trigger input",
			"PORT_2"
		],
		"trigger_timeout": [
			PyTango.DevLong,
			"Timeout for external trigger",
			Xi.Camera.TIMEOUT_MAX
		],
		"internal_timeout": [
			PyTango.DevLong,
			"Timeout for internal trigger",
			200
		],
		"startup_temp_control_mode": [
			PyTango.DevString,
			"Startup temperature control mode",
			"AUTO"
		],
		"startup_target_temp": [
			PyTango.DevDouble,
			"Startup target temperature",
			25.0
		],
		"startup_mode": [
			PyTango.DevString,
			"Startup camera mode",
			"2_12_HDR_HL"
		]
	}

	cmd_list = {
		'getAttrStringValueList': [
			[PyTango.DevString, "Attribute name"],
			[PyTango.DevVarStringArray, "Authorized String value list"]
		],
	}

	attr_list = {
		"trigger_polarity": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select trigger polarity',
			}
		],
		"software_trigger": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Software trigger; write to generate trigger, reads always false',
			}
		],
		"gpi_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select GPI to configure',
			}
		],
		"gpi_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select GPI mode',
			}
		],
		"gpi_level": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Read GPI level',
			}
		],
		"gpi_level_at_exp_start": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Read GPI level at exposure start',
			}
		],
		"gpi_level_at_exp_end": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Read GPI level at exposure end',
			}
		],
		"gpi_debounce": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Enable GPI debounce',
			}
		],
		"gpo_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select GPO to configure',
			}
		],
		"gpo_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select GPO mode',
			}
		],
		"led_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select LED to configure',
			}
		],
		"led_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select LED mode',
			}
		],
		"mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select configuration preset',
			}
		],
		"gain_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select gain type',
			}
		],
		"gain": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'dB',
				'format': '',
				'description': 'Gain value',
			}
		],
		"is_cooled": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Is the camera cooled',
			}
		],
		"temp_control_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Temperature control mode',
				'memorized': 'true',
			}
		],
		"temp_target": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': '*C',
				'format': '',
				'description': 'Target temperature',
				'memorized': 'true',
			}
		],
		"thermometer": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Select thermometer',
			}
		],
		"temperature": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Thermometer temperature',
			}
		],
		"temp_chip": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera sensor temperature',
			}
		],
		"temp_housing": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera housing temperature',
			}
		],
		"temp_back": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera housing back side temperature',
			}
		],
		"temp_sensor": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Sensor board temperature',
			}
		],
		"thermal_element": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Thermal control element',
			}
		],
		"thermal_element_value": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': '%',
				'format': '',
				'description': 'Thermal element control value',
			}
		],
		"exposure_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Exposure mode selector',
			}
		],
		"burst_count": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Burst count',
			}
		],
		"downsampling": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Downsampling value',
			}
		],
		"downsampling_type": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Downsampling type',
			}
		],
		"test_pattern_generator": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Test pattern generator',
			}
		],
		"test_pattern": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Test pattern',
			}
		],
		"image_format": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Image format',
			}
		],
		"shutter": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Shutter mode',
			}
		],
		"taps": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Sensor taps',
			}
		],
		"auto_exposure_gain": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Auto exposure and gain control',
			}
		],
		"auto_white_balance": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Auto white balance control',
			}
		],
		"horizontal_flip": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Horizontal flip',
			}
		],
		"vertical_flip": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Vertical flip',
			}
		],
		"interline_exp_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Interline exposure mode',
			}
		],
		"binning_engine": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Binning engine selector',
			}
		],
		"horizontal_binning_pattern": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Binning horizontal pattern',
			}
		],
		"vertical_binning_pattern": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Binning vertical pattern',
			}
		],
		"decimation_engine": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Decimation engine selector',
			}
		],
		"horizontal_decimation": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Horizontal decimation value',
			}
		],
		"vertical_decimation": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Vertical decimation value',
			}
		],
		"horizontal_decimation_pattern": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Decimation horizontal pattern',
			}
		],
		"vertical_decimation_pattern": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Decimation vertical pattern',
			}
		],
		"exposure_priority": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Exposure priority (0.8 - exposure 80%, gain 20%)',
			}
		],
		"auto_gain_limit": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Gain limit for AEAG procedure',
			}
		],
		"auto_exposure_limit": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'us',
				'format': '',
				'description': 'Exposure limit for AEAG procedure',
			}
		],
		"auto_intensity_level": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': '%',
				'format': '',
				'description': 'Target average intensity for AEAG procedure',
			}
		],
		"bandwidth_limit": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'Mb/s',
				'format': '',
				'description': 'Bandwidth limit',
			}
		],
		"bandwidth_limit_enabled": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Enable bandwidth limiting',
			}
		],
		"available_bandwidth": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'Mb/s',
				'format': '',
				'description': 'Measured available bandwidth',
			}
		],
		"frame_rate": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'Hz',
				'format': '',
				'description': 'Frame rate (or limit)',
			}
		],
		"counter_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Counter selector',
			}
		],
		"counter_value": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Selected counter value',
			}
		],
		"acq_timing_mode": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Acquisition timing mode',
			}
		],
		"trigger_delay": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'us',
				'format': '',
				'description': 'Trigger delay',
			}
		],
		"acq_status": [
			[PyTango.DevBoolean, PyTango.SCALAR, PyTango.READ],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Acquisition status',
			}
		],
		"feature_selector": [
			[PyTango.DevString, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Sensor additional features',
			}
		],
		"feature_value": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Selected feature value',
			}
		],
		"internal_timeout": [
			[PyTango.DevLong, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': 'N/A',
				'format': '',
				'description': 'Timeout for internal trigger',
				'memorized': 'true',
			}
		],
	}

	def __init__(self, name):
		PyTango.DeviceClass.__init__(self, name)
		self.set_type(name)


# ----------------------------------------------------------------------------
# Plugins
# ----------------------------------------------------------------------------
_XimeaCam = None
_XimeaInterface = None


def get_control(
	camera_id,
	trigger_gpi_port="PORT_2", trigger_timeout=Xi.Camera.TIMEOUT_MAX, internal_timeout=200,
	startup_temp_control_mode="AUTO", startup_target_temp=25.0,
	startup_mode="2_12_HDR_HL", **keys
):
	global _XimeaCam
	global _XimeaInterface

	print("Ximea camera_id:", camera_id)

	# all properties are passed as string from LimaCCDs device get_control helper
	# so need to be converted to correct type
	if _XimeaCam is None:
		_XimeaCam = Xi.Camera(
			int(camera_id),
			_GpiSelector[trigger_gpi_port.upper()], int(trigger_timeout), int(internal_timeout),
			_TempControlMode[startup_temp_control_mode.upper()], float(startup_target_temp),
			_Mode[startup_mode.upper()]
		)
		_XimeaInterface = Xi.Interface(_XimeaCam)
	return Core.CtControl(_XimeaInterface)


def get_tango_specific_class_n_device():
	return XimeaClass, Ximea
