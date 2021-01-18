############################################################################
# This file is part of LImA, a Library for Image Acquisition
#
# Copyright (C) : 2009-2011
# European Synchrotron Radiation Facility
# BP 220, Grenoble 38043
# FRANCE
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

		self.__Preset = {
			"12_STD_L": Xi.Camera.Preset_12_STD_L,
			"12_STD_H": Xi.Camera.Preset_12_STD_H,
			"14_STD_L": Xi.Camera.Preset_14_STD_L,
			"None": Xi.Camera.Preset_None,
			"14_STD_H": Xi.Camera.Preset_14_STD_H,
			"2_12_CMS_S_L": Xi.Camera.Preset_2_12_CMS_S_L,
			"2_12_CMS_S_H": Xi.Camera.Preset_2_12_CMS_S_H,
			"2_14_CMS_S_L": Xi.Camera.Preset_2_14_CMS_S_L,
			"2_14_CMS_S_H": Xi.Camera.Preset_2_14_CMS_S_H,
			"4_12_CMS_S_L": Xi.Camera.Preset_4_12_CMS_S_L,
			"4_12_CMS_S_H": Xi.Camera.Preset_4_12_CMS_S_H,
			"4_14_CMS_S_L": Xi.Camera.Preset_4_14_CMS_S_L,
			"4_14_CMS_S_H": Xi.Camera.Preset_4_14_CMS_S_H,
			"2_12_HDR_HL": Xi.Camera.Preset_2_12_HDR_HL,
			"2_12_HDR_L": Xi.Camera.Preset_2_12_HDR_L,
			"2_12_HDR_H": Xi.Camera.Preset_2_12_HDR_H,
			"4_12_CMS_HDR_HL": Xi.Camera.Preset_4_12_CMS_HDR_HL,
			"2_14_HDR_L": Xi.Camera.Preset_2_14_HDR_L,
			"2_14_HDR_H": Xi.Camera.Preset_2_14_HDR_H,
			"2_12_CMS_A_L": Xi.Camera.Preset_2_12_CMS_A_L,
			"2_12_CMS_A_H": Xi.Camera.Preset_2_12_CMS_A_H
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
		"preset": [
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
