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
