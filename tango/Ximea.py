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
	def __init__(self, *args) :
		PyTango.Device_4Impl.__init__(self, *args)

		self.__Preset = {
			"Preset_12_STD_L": Xi.Camera.Preset_12_STD_L,
			"Preset_12_STD_H": Xi.Camera.Preset_12_STD_H,
			"Preset_14_STD_L": Xi.Camera.Preset_14_STD_L,
			"Preset_None": Xi.Camera.Preset_None,
			"Preset_14_STD_H": Xi.Camera.Preset_14_STD_H,
			"Preset_2_12_CMS_S_L": Xi.Camera.Preset_2_12_CMS_S_L,
			"Preset_2_12_CMS_S_H": Xi.Camera.Preset_2_12_CMS_S_H,
			"Preset_2_14_CMS_S_L": Xi.Camera.Preset_2_14_CMS_S_L,
			"Preset_2_14_CMS_S_H": Xi.Camera.Preset_2_14_CMS_S_H,
			"Preset_4_12_CMS_S_L": Xi.Camera.Preset_4_12_CMS_S_L,
			"Preset_4_12_CMS_S_H": Xi.Camera.Preset_4_12_CMS_S_H,
			"Preset_4_14_CMS_S_L": Xi.Camera.Preset_4_14_CMS_S_L,
			"Preset_4_14_CMS_S_H": Xi.Camera.Preset_4_14_CMS_S_H,
			"Preset_2_12_HDR_HL": Xi.Camera.Preset_2_12_HDR_HL,
			"Preset_2_12_HDR_L": Xi.Camera.Preset_2_12_HDR_L,
			"Preset_2_12_HDR_H": Xi.Camera.Preset_2_12_HDR_H,
			"Preset_4_12_CMS_HDR_HL": Xi.Camera.Preset_4_12_CMS_HDR_HL,
			"Preset_2_14_HDR_L": Xi.Camera.Preset_2_14_HDR_L,
			"Preset_2_14_HDR_H": Xi.Camera.Preset_2_14_HDR_H,
			"Preset_2_12_CMS_A_L": Xi.Camera.Preset_2_12_CMS_A_L,
			"Preset_2_12_CMS_A_H": Xi.Camera.Preset_2_12_CMS_A_H
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
