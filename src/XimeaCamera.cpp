//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2020
// European Synchrotron Radiation Facility
// CS40220 38043 Grenoble Cedex 9 
// FRANCE
//
// Contact: lima@esrf.fr
//
// This is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//###########################################################################

#include "XimeaCamera.h"

using namespace lima;
using namespace lima::Ximea;
using namespace std;

//---------------------------
//- Ctor
//---------------------------
Camera::Camera(int camera_id) : xiH(nullptr)
{
	DEB_CONSTRUCTOR();

	this->status = xiOpenDevice(camera_id, &this->xiH);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not open camera " << camera_id << "; status: " << this->status;

	DEB_TRACE() << "Camera " << camera_id << " opened; status: " << this->status;
}

//---------------------------
//- Dtor
//---------------------------
Camera::~Camera()
{
	DEB_DESTRUCTOR();

	if(this->xiH)
		xiCloseDevice(this->xiH);
}

void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();
}

void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();

	xiStartAcquisition(this->xiH);
}

void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();

	xiStopAcquisition(this->xiH);
}

void Camera::getImageType(ImageType& type)
{
}

void Camera::setImageType(ImageType type)
{
}

void Camera::getDetectorType(std::string& type)
{
	type = this->_get_param_str(XI_PRM_DEVICE_TYPE);
}

void Camera::getDetectorModel(std::string& model)
{
	model = this->_get_param_str(XI_PRM_DEVICE_MODEL_ID);
}

void Camera::getDetectorImageSize(Size& size)
{
	size = Size(this->_get_param_int(XI_PRM_WIDTH), this->_get_param_int(XI_PRM_HEIGHT));
}

void Camera::getExpTime(double& exp_time)
{
	int r = this->_get_param_int(XI_PRM_EXPOSURE);
	// convert exposure from us to s
	exp_time = (double)(r / 1e6);
}

void Camera::setExpTime(double exp_time)
{
	// convert exposure from s to us
	this->_set_param_int(XI_PRM_EXPOSURE, int(exp_time * 1e6));
}

int Camera::_get_param_int(const char* param)
{
	DEB_MEMBER_FUNCT();

	int r = 0;
	this->status = xiGetParamInt(this->xiH, param, &r);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; status: " << this->status;
	return r;
}

double Camera::_get_param_dbl(const char* param)
{
	DEB_MEMBER_FUNCT();

	float r;
	this->status = xiGetParamFloat(this->xiH, param, &r);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; status: " << this->status;
	return (double)r;
}

std::string Camera::_get_param_str(const char* param)
{
	DEB_MEMBER_FUNCT();

	char r[PARAMSTR_LEN];
	this->status = xiGetParamString(this->xiH, param, (void*)r, PARAMSTR_LEN);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; status: " << this->status;
	return std::string(r);
}

void Camera::_set_param_int(const char* param, int value)
{
	DEB_MEMBER_FUNCT();

	this->status = xiSetParamInt(this->xiH, param, value);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; status: " << this->status;
}

void Camera::_set_param_dbl(const char* param, double value)
{
	DEB_MEMBER_FUNCT();

	this->status = xiSetParamFloat(this->xiH, param, (float)value);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; status: " << this->status;
}

void Camera::_set_param_str(const char* param, std::string value, int size)
{
	DEB_MEMBER_FUNCT();

	if(size == -1)
		size = value.length();

	this->status = xiSetParamString(this->xiH, param, (void*)value.c_str(), size);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; status: " << this->status;
}
