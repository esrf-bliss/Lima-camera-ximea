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
// #include "XimeaVideoCtrlObj.h"
#include "XimeaAcqThread.h"

using namespace lima;
using namespace lima::Ximea;
using namespace std;

//---------------------------
//- Ctor
//---------------------------
Camera::Camera(int camera_id)
	: xiH(nullptr),
	  m_image_number(0),
	  m_buffer_size(0),
	  m_acq_thread(nullptr)
{
	DEB_CONSTRUCTOR();

	this->status = xiOpenDevice(camera_id, &this->xiH);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not open camera " << camera_id << "; status: " << this->status;

	DEB_TRACE() << "Camera " << camera_id << " opened; status: " << this->status;
	this->_set_param_int(XI_PRM_BUFFER_POLICY, XI_BP_UNSAFE);
}

//---------------------------
//- Dtor
//---------------------------
Camera::~Camera()
{
	DEB_DESTRUCTOR();

	this->_stop_acq_thread();
	if(this->xiH)
		xiCloseDevice(this->xiH);
}

void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();

	this->_stop_acq_thread();
	this->m_image_number = 0;
	this->m_buffer_size = this->m_buffer_ctrl_obj.getBuffer().getFrameDim().getMemSize();
	this->m_acq_thread = new AcqThread(*this);
}

void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();

	if(!this->m_image_number)
		this->m_buffer_ctrl_obj.getBuffer().setStartTimestamp(Timestamp::now());

	xiStartAcquisition(this->xiH);
	this->m_acq_thread->m_quit = false;
	this->m_acq_thread->start();
}

void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();

	this->_stop_acq_thread();
	xiStopAcquisition(this->xiH);
}

void Camera::getImageType(ImageType& type)
{
	DEB_MEMBER_FUNCT();

	XI_BIT_DEPTH depth = (XI_BIT_DEPTH)this->_get_param_int(XI_PRM_IMAGE_DATA_BIT_DEPTH);
	switch(depth)
	{
		case XI_BPP_8:
			type = Bpp8;
			break;
		case XI_BPP_10:
			type = Bpp10;
			break;
		case XI_BPP_12:
			type = Bpp12;
			break;
		case XI_BPP_14:
			type = Bpp14;
			break;
		case XI_BPP_16:
			type = Bpp16;
			break;
		case XI_BPP_24:
			type = Bpp24;
			break;
		case XI_BPP_32:
			type = Bpp32;
			break;

		case XI_BPP_9:
		case XI_BPP_11:
			THROW_HW_ERROR(Error) << "Bit depth is not supported by Lima: " << depth;
			break;

		default:
			THROW_HW_ERROR(Error) << "Unsupported bit depth: " << depth;
	}
}

void Camera::setImageType(ImageType type)
{
	DEB_MEMBER_FUNCT();

	XI_BIT_DEPTH depth;
	switch(type)
	{
		case Bpp8:
			depth = XI_BPP_8;
			break;
		case Bpp10:
			depth = XI_BPP_10;
			break;
		case Bpp12:
			depth = XI_BPP_12;
			break;
		case Bpp14:
			depth = XI_BPP_14;
			break;
		case Bpp16:
			depth = XI_BPP_16;
			break;
		case Bpp24:
			depth = XI_BPP_24;
			break;
		case Bpp32:
			depth = XI_BPP_32;
			break;

		default:
			THROW_HW_ERROR(Error) << "Unsupported image type: " << type;
	}

	this->_set_param_int(XI_PRM_SENSOR_DATA_BIT_DEPTH, depth);
	this->_set_param_int(XI_PRM_OUTPUT_DATA_BIT_DEPTH, depth);
	this->_set_param_int(XI_PRM_IMAGE_DATA_BIT_DEPTH, depth);
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

void Camera::setExpTime(double exp_time)
{
	// convert exposure from s to us
	int v = int(exp_time * TIME_HW);
	this->_set_param_int(XI_PRM_EXPOSURE, v);
}

void Camera::getExpTime(double& exp_time)
{
	int r = this->_get_param_int(XI_PRM_EXPOSURE);
	// convert exposure from us to s
	exp_time = (double)(r / TIME_HW);
}

void Camera::setNbFrames(int nb_frames)
{
	this->m_nb_frames = nb_frames;
}

void Camera::getNbFrames(int& nb_frames)
{
	nb_frames = this->m_nb_frames;
}

void Camera::getNbHwAcquiredFrames(int& nb_acq_frames)
{
	nb_acq_frames = this->m_image_number;
}

HwBufferCtrlObj* Camera::getBufferCtrlObj()
{
	return &this->m_buffer_ctrl_obj;
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

void Camera::_read_image(XI_IMG* image, int timeout)
{
	DEB_MEMBER_FUNCT();

	image->size = sizeof(XI_IMG);
	this->status = xiGetImage(this->xiH, timeout, image);
	if(this->status != XI_OK)
		THROW_HW_ERROR(Error) << "Image readout failed; status: " << this->status;
}

void Camera::_stop_acq_thread()
{
	if(this->m_acq_thread)
	{
		if(this->m_acq_thread->hasStarted() && !this->m_acq_thread->hasFinished())
			this->m_acq_thread->m_quit = true;
		delete this->m_acq_thread;
		this->m_acq_thread = NULL;
	}
}
