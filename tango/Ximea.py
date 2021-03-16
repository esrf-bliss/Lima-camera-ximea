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


class Ximea(PyTango.Device_4Impl):
	Core.DEB_CLASS(Core.DebModApplication, 'LimaCCDs')

	# ------------------------------------------------------------------
	#    Device constructor
	# ------------------------------------------------------------------
	def __init__(self, *args):
		PyTango.Device_4Impl.__init__(self, *args)

		self.__Mode = {
			"12_STD_L": Xi.Camera.Mode_12_STD_L,
			"12_STD_H": Xi.Camera.Mode_12_STD_H,
			"14_STD_L": Xi.Camera.Mode_14_STD_L,
			"None": Xi.Camera.Mode_None,
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

		self.__GainSelector = {
			"All": Xi.Camera.GainSelector_All,
			"Analog_All": Xi.Camera.GainSelector_Analog_All,
			"Digital_All": Xi.Camera.GainSelector_Digital_All,
			"Analog_Tap1": Xi.Camera.GainSelector_Analog_Tap1,
			"Analog_Tap2": Xi.Camera.GainSelector_Analog_Tap2,
			"Analog_Tap3": Xi.Camera.GainSelector_Analog_Tap3,
			"Analog_Tap4": Xi.Camera.GainSelector_Analog_Tap4,
			"Analog_North": Xi.Camera.GainSelector_Analog_North,
			"Analog_South": Xi.Camera.GainSelector_Analog_South,
		}

		self.__TempControlMode = {
			"Off": Xi.Camera.TempControlMode_Off,
			"Auto": Xi.Camera.TempControlMode_Auto,
			"Manual": Xi.Camera.TempControlMode_Manual,
		}

		self.__Thermometer = {
			"Die_Raw": Xi.Camera.Thermometer_Die_Raw,
			"Sensor_Die": Xi.Camera.Thermometer_Sensor_Die,
			"Sensor_Board": Xi.Camera.Thermometer_Sensor_Board,
			"Interface_Board": Xi.Camera.Thermometer_Interface_Board,
			"Front_Housing": Xi.Camera.Thermometer_Front_Housing,
			"Rear_Housing": Xi.Camera.Thermometer_Rear_Housing,
			"TEC1_Cold": Xi.Camera.Thermometer_TEC1_Cold,
			"TEC1_Hot": Xi.Camera.Thermometer_TEC1_Hot,
		}

		self.__ThermalElement = {
			"TEC1": Xi.Camera.ThermalElement_TEC1,
			"TEC2": Xi.Camera.ThermalElement_TEC2,
			"Fan1": Xi.Camera.ThermalElement_Fan1,
			"Fan_Start_Threshold": Xi.Camera.ThermalElement_Fan_Start_Threshold,
		}

		self.__ExposureSelector = {
			"Common": Xi.Camera.ExposureSelector_Common,
			"Group1": Xi.Camera.ExposureSelector_Group1,
			"Group2": Xi.Camera.ExposureSelector_Group2,
		}

		self.__Downsampling = {
			"1x1": Xi.Camera.Downsampling_1x1,
			"2x2": Xi.Camera.Downsampling_2x2,
			"3x3": Xi.Camera.Downsampling_3x3,
			"4x4": Xi.Camera.Downsampling_4x4,
			"5x5": Xi.Camera.Downsampling_5x5,
			"6x6": Xi.Camera.Downsampling_6x6,
			"7x7": Xi.Camera.Downsampling_7x7,
			"8x8": Xi.Camera.Downsampling_8x8,
			"9x9": Xi.Camera.Downsampling_9x9,
			"10x10": Xi.Camera.Downsampling_10x10,
			"16x16": Xi.Camera.Downsampling_16x16,
		}

		self.__DownsamplingType = {
			"Binning": Xi.Camera.DownsamplingType_Binning,
			"Skipping": Xi.Camera.DownsamplingType_Skipping,
		}

		self.__TestPatternGenerator = {
			"Sensor": Xi.Camera.TestPatternGenerator_Sensor,
			"FPGA": Xi.Camera.TestPatternGenerator_FPGA,
		}

		self.__TestPattern = {
			"Off": Xi.Camera.TestPattern_Off,
			"Black": Xi.Camera.TestPattern_Black,
			"White": Xi.Camera.TestPattern_White,
			"Grey_Horizontal": Xi.Camera.TestPattern_Grey_Horizontal,
			"Grey_Vertical": Xi.Camera.TestPattern_Grey_Vertical,
			"Grey_Horizontal_Moving": Xi.Camera.TestPattern_Grey_Horizontal_Moving,
			"Grey_Vertical_Moving": Xi.Camera.TestPattern_Grey_Vertical_Moving,
			"Line_Horizontal_Moving": Xi.Camera.TestPattern_Line_Horizontal_Moving,
			"Line_Vertical_Moving": Xi.Camera.TestPattern_Line_Vertical_Moving,
			"Color_Bar": Xi.Camera.TestPattern_Color_Bar,
			"Frame_Counter": Xi.Camera.TestPattern_Frame_Counter,
			"Counter": Xi.Camera.TestPattern_Counter,
		}

		self.__ImageFormat = {
			"Mono8": Xi.Camera.ImageFormat_Mono8,
			"Mono16": Xi.Camera.ImageFormat_Mono16,
			"RGB24": Xi.Camera.ImageFormat_RGB24,
			"RGB32": Xi.Camera.ImageFormat_RGB32,
			"RGB_Planar": Xi.Camera.ImageFormat_RGB_Planar,
			"Raw8": Xi.Camera.ImageFormat_Raw8,
			"Raw16": Xi.Camera.ImageFormat_Raw16,
			"Transport": Xi.Camera.ImageFormat_Transport,
			"RGB48": Xi.Camera.ImageFormat_RGB48,
			"RGB64": Xi.Camera.ImageFormat_RGB64,
			"RGB16_Planar": Xi.Camera.ImageFormat_RGB16_Planar,
			"Raw8x2": Xi.Camera.ImageFormat_Raw8x2,
			"Raw8x4": Xi.Camera.ImageFormat_Raw8x4,
			"Raw16x2": Xi.Camera.ImageFormat_Raw16x2,
			"Raw16x4": Xi.Camera.ImageFormat_Raw16x4,
			"Raw32": Xi.Camera.ImageFormat_Raw32,
			"Raw32_Float": Xi.Camera.ImageFormat_Raw32_Float,
		}

		self.__Shutter = {
			"Global": Xi.Camera.Shutter_Global,
			"Rolling": Xi.Camera.Shutter_Rolling,
			"Reset_Release": Xi.Camera.Shutter_Reset_Release,
		}

		self.__Taps = {
			"1": Xi.Camera.Taps_1,
			"2": Xi.Camera.Taps_2,
			"4": Xi.Camera.Taps_4,
		}

		self.__InterlineExpMode = {
			"Off": Xi.Camera.InterlineExpMode_Off,
			"On": Xi.Camera.InterlineExpMode_On,
		}

		self.__BinningEngine = {
			"Sensor": Xi.Camera.BinningEngine_Sensor,
			"FPGA": Xi.Camera.BinningEngine_FPGA,
			"Host": Xi.Camera.BinningEngine_Host,
		}

		self.__BinningPattern = {
			"Mono": Xi.Camera.BinningPattern_Mono,
			"Bayer": Xi.Camera.BinningPattern_Bayer,
		}

		self.__DecimationEngine = {
			"Sensor": Xi.Camera.DecimationEngine_Sensor,
			"FPGA": Xi.Camera.DecimationEngine_FPGA,
			"Host": Xi.Camera.DecimationEngine_Host,
		}

		self.__DecimationPattern = {
			"Mono": Xi.Camera.DecimationPattern_Mono,
			"Bayer": Xi.Camera.DecimationPattern_Bayer,
		}

		self.__CounterSelector = {
			"Skipped_Frames_Transport": Xi.Camera.CounterSelector_Skipped_Frames_Transport,
			"Skipped_Frames_API": Xi.Camera.CounterSelector_Skipped_Frames_API,
			"Missed_Trigger_Overlap": Xi.Camera.CounterSelector_Missed_Trigger_Overlap,
			"Missed_Trigger_Buffer_Full": Xi.Camera.CounterSelector_Missed_Trigger_Buffer_Full,
			"Frame_Buffer_Full": Xi.Camera.CounterSelector_Frame_Buffer_Full,
		}

		self.__AcqTimingMode = {
			"Free_Run": Xi.Camera.AcqTimingMode_Free_Run,
			"Frame_Rate": Xi.Camera.AcqTimingMode_Frame_Rate,
			"Frame_Rate_Limit": Xi.Camera.AcqTimingMode_Frame_Rate_Limit,
		}

		self.__FeatureSelector = {
			"Zero_ROT_Enable": Xi.Camera.FeatureSelector_Zero_ROT_Enable,
			"Black_Level_Clamp": Xi.Camera.FeatureSelector_Black_Level_Clamp,
			"FPGA_Digital_Gain_Disable": Xi.Camera.FeatureSelector_FPGA_Digital_Gain_Disable,
			"Acquisition_Running": Xi.Camera.FeatureSelector_Acquisition_Running,
			"Timing_Mode": Xi.Camera.FeatureSelector_Timing_Mode,
			"Parallel_ADC": Xi.Camera.FeatureSelector_Parallel_ADC,
			"Black_Level_Offset_Raw": Xi.Camera.FeatureSelector_Black_Level_Offset_Raw,
		}

		self.__TriggerPolarity = {
			"LOW / FALLING": Xi.Camera.TriggerPolarity_Low_Falling,
			"HIGH / RISING": Xi.Camera.TriggerPolarity_High_Rising
		}

		self.__GpiSelector = {
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
			}
		],
		"target_temp": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ_WRITE],
			{
				'unit': '*C',
				'format': '',
				'description': 'Target temperature',
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
		"chip_temp": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera sensor temperature',
			}
		],
		"housing_temp": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera housing temperature',
			}
		],
		"back_temp": [
			[PyTango.DevDouble, PyTango.SCALAR, PyTango.READ],
			{
				'unit': '*C',
				'format': '',
				'description': 'Camera housing back side temperature',
			}
		],
		"sensor_temp": [
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
	}

	def __init__(self, name):
		PyTango.DeviceClass.__init__(self, name)
		self.set_type(name)


# ----------------------------------------------------------------------------
# Plugins
# ----------------------------------------------------------------------------
_XimeaCam = None
_XimeaInterface = None


def get_control(**keys):
	global _XimeaCam
	global _XimeaInterface

	if 'camera_id' in keys:
		camera_id = keys['camera_id']
	else:
		# TODO: probably want to throw an exception here
		pass

	print("Ximea camera_id:", camera_id)

	# all properties are passed as string from LimaCCDs device get_control helper
	# so need to be converted to correct type
	if _XimeaCam is None:
		_XimeaCam = Xi.Camera(int(camera_id))
		_XimeaInterface = Xi.Interface(_XimeaCam)
	return Core.CtControl(_XimeaInterface)


def get_tango_specific_class_n_device():
	return XimeaClass, Ximea
