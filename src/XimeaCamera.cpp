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
#include "XimeaAcqThread.h"

using namespace lima;
using namespace lima::Ximea;
using namespace std;

//---------------------------
//- Ctor
//---------------------------
Camera::Camera(int camera_id, GPISelector trigger_gpi_port, unsigned int timeout, TempControlMode startup_temp_control_mode, double startup_target_temp, Mode startup_mode)
	: cam_id(camera_id),
	  xiH(nullptr),
	  xi_status(XI_OK),
	  m_status(Camera::Ready),
	  m_image_number(0),
	  m_buffer_size(0),
	  m_acq_thread(nullptr),
	  m_trig_polarity(Camera::TriggerPolarity_High_Rising),
	  m_trigger_gpi_port(trigger_gpi_port),
	  m_timeout(timeout),
	  m_startup_temp_control_mode(startup_temp_control_mode),
	  m_startup_target_temp(startup_target_temp),
	  m_startup_mode(startup_mode),
	  m_soft_trigger_issued(false),
	  m_max_height(0),
	  m_max_width(0),
	  m_latency_time(0)
{
	DEB_CONSTRUCTOR();
	this->_startup();
	DEB_TRACE() << "Camera " << camera_id << " opened; xi_status: " << this->xi_status;
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

void Camera::_startup()
{
	DEB_MEMBER_FUNCT();

	this->xi_status = xiOpenDevice(this->cam_id, &this->xiH);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not open camera " << this->cam_id << "; status: " << this->xi_status;

	this->m_camera_model = this->_get_param_str(XI_PRM_DEVICE_NAME);

	// set debug level
	this->_set_param_int(XI_PRM_DEBUG_LEVEL, XI_DL_DISABLED);

	// set buffer policy to managed by application
	this->_set_param_int(XI_PRM_BUFFER_POLICY, XI_BP_SAFE);

	// set startup temperature control values
	this->setTempControlMode(this->m_startup_temp_control_mode);
	this->setTempTarget(this->m_startup_target_temp);

	// set startup acquisition configuration
	this->setTrigMode(IntTrig);
	this->setNbFrames(1);

	// set startup and default mode
	this->setMode(this->m_startup_mode);
	this->_set_param_int(XI_PRM_USER_SET_DEFAULT, this->m_startup_mode);

	// read max frame size
	this->m_max_width = this->_get_param_max(XI_PRM_WIDTH);
	this->m_max_height = this->_get_param_max(XI_PRM_HEIGHT);
}

void Camera::getPluginVersion(string& version)
{
	version = string(XIMEA_PACKAGE_VERSION);
}

bool Camera::_check_model(std::string model)
{
	return (this->m_camera_model.rfind(model, 0) == 0);
}

void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();

	this->_stop_acq_thread();
	this->m_image_number = 0;
	this->m_buffer_size = this->m_buffer_ctrl_obj.getBuffer().getFrameDim().getMemSize();
	
	this->m_acq_thread = new AcqThread(*this, this->_get_trigger_timeout());
	this->_set_status(Camera::Ready);
}

void Camera::startAcq()
{
	DEB_MEMBER_FUNCT();

	if(this->m_trigger_mode == IntTrigMult && this->m_acq_thread->m_thread_started)
		this->_generate_soft_trigger();
	else
	{
		if(!this->m_image_number)
			this->m_buffer_ctrl_obj.getBuffer().setStartTimestamp(Timestamp::now());

		xiStartAcquisition(this->xiH);
		this->m_acq_thread->m_quit = false;
		this->m_acq_thread->start();
		if(this->m_trigger_mode == IntTrigMult)
			this->_generate_soft_trigger();
	}
}

void Camera::stopAcq()
{
	DEB_MEMBER_FUNCT();

	this->_stop_acq_thread();
	xiStopAcquisition(this->xiH);
	this->_set_status(Camera::Ready);
}

void Camera::reset()
{
	DEB_MEMBER_FUNCT();
	this->stopAcq();
	this->_set_param_int(XI_PRM_DEVICE_RESET, XI_ON);
	xiCloseDevice(this->xiH);
	this->xiH = nullptr;
	this->_startup();
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
	model = this->m_camera_model;
}

void Camera::getPixelSize(double& x_size, double& y_size)
{
	// Return pixel size based on camera model
	// This does not make much sense as for now, since the only "officially"
	// supported camera happens to have the same pixel size as the default.
	// The check is however done for demonstration purposes as well as to
	// simplify integration of future cameras.
	if(this->_check_model("MX377MR"))
	{
		x_size = 1e-5;
		y_size = 1e-5;
	}
	else
	{
		printf("!!! Unknown pixel size for this camera model! Defaults used !!!\n");
		x_size = 1e-5;
		y_size = 1e-5;
	}
}

void Camera::getDetectorMaxImageSize(Size& size)
{
	size = Size(this->m_max_width, this->m_max_height);
}

void Camera::getDetectorImageSize(Size& size)
{
	size = Size(this->_get_param_int(XI_PRM_WIDTH), this->_get_param_int(XI_PRM_HEIGHT));
}

void Camera::setTrigMode(TrigMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

	if(mode == IntTrig)
	{
		this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_OFF);
		this->_set_param_int(XI_PRM_TRG_SELECTOR, XI_TRG_SEL_FRAME_BURST_START);
	}
	else if(mode == IntTrigMult)
	{
		// this has nothing to do with internal trigger !!!
		// IntTrigMult is basically a software trigger with extra steps
		this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_SOFTWARE);
		this->_set_param_int(XI_PRM_TRG_SELECTOR, XI_TRG_SEL_FRAME_START);
	}
	else if(mode == ExtTrigSingle)
	{
		// this trigger configuration is not supported -
		// - lack of camera support for XI_PRM_EXPOSURE_BURST_COUNT
		this->_setup_gpio_trigger();
		if(this->m_trig_polarity == TriggerPolarity_Low_Falling)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_EDGE_FALLING);
		else if(this->m_trig_polarity == TriggerPolarity_High_Rising)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);

		this->_set_param_int(XI_PRM_TRG_SELECTOR, XI_TRG_SEL_FRAME_BURST_START);
	}
	else if(mode == ExtTrigMult)
	{
		this->_setup_gpio_trigger();
		if(this->m_trig_polarity == TriggerPolarity_Low_Falling)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_EDGE_FALLING);
		else if(this->m_trig_polarity == TriggerPolarity_High_Rising)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_EDGE_RISING);

		this->_set_param_int(XI_PRM_TRG_SELECTOR, XI_TRG_SEL_FRAME_START);
	}
	else if(mode == ExtGate)
	{
		// this trigger configuration is not supported -
		// - lack of camera support for XI_TRG_SEL_EXPOSURE_ACTIVE
		if(this->m_trig_polarity == TriggerPolarity_Low_Falling)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_LEVEL_LOW);
		else if(this->m_trig_polarity == TriggerPolarity_High_Rising)
			this->_set_param_int(XI_PRM_TRG_SOURCE, XI_TRG_LEVEL_HIGH);

		this->_set_param_int(XI_PRM_TRG_SELECTOR, XI_TRG_SEL_EXPOSURE_ACTIVE);
	}

	this->m_trigger_mode = mode;
}

void Camera::getTrigMode(TrigMode& mode)
{
	DEB_MEMBER_FUNCT();

	mode = this->m_trigger_mode;

	DEB_RETURN() << DEB_VAR1(this->m_trigger_mode);
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

void Camera::setLatTime(double lat_time)
{
	this->m_latency_time = lat_time;
}

void Camera::getLatTime(double& lat_time)
{
	lat_time = this->m_latency_time;
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

void Camera::getStatus(Camera::Status& status)
{
	DEB_MEMBER_FUNCT();

	status = this->m_status;
	DEB_RETURN() << DEB_VAR1(status);
}

void Camera::checkRoi(const Roi& set_roi, Roi& hw_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(set_roi);

	// set offsets to zero
	// this->_set_param_int(XI_PRM_OFFSET_X, 0);
	// this->_set_param_int(XI_PRM_OFFSET_Y, 0);

	// get W/H parameters info
	int w_min = this->_get_param_min(XI_PRM_WIDTH);
	int w_max = this->m_max_width;
	int w_inc = this->_get_param_inc(XI_PRM_WIDTH);
	int h_min = this->_get_param_min(XI_PRM_HEIGHT);
	int h_max = this->m_max_height;
	int h_inc = this->_get_param_inc(XI_PRM_HEIGHT);

	// set W/H to max values
	this->_set_param_int(XI_PRM_WIDTH, w_max);
	this->_set_param_int(XI_PRM_HEIGHT, h_max);
	
	// get offset increment as it should not change with W/H
	int x_inc = this->_get_param_inc(XI_PRM_OFFSET_X);
	int y_inc = this->_get_param_inc(XI_PRM_OFFSET_Y);

	int w = set_roi.getSize().getWidth();
	int h = set_roi.getSize().getHeight();
	int x = set_roi.getTopLeft().x;
	int y = set_roi.getTopLeft().y;

	if(w == 0 && h == 0 && x == 0 && y == 0)
	{
		// zero means infinity in Lima language
		w = w_max;
		h = h_max;
	}
	else if(w < 32 || h < 32)
	{
		// ROI cannot be too small for correct nb_buffers calculation
		THROW_HW_ERROR(Error) << "ROI must be at least 32x32";
	}
	else
	{
		// if cannot set precise ROI, use closest divisible by increment
		int nx = floor(double(x) / x_inc) * x_inc;
		int ny = floor(double(y) / y_inc) * y_inc;
		int nw = w + (x - nx);
		int nh = h + (y - ny);

		w = ceil(double(nw) / w_inc) * w_inc;
		h = ceil(double(nh) / h_inc) * h_inc;

		// check W/H min-max
		w = min(w_max, max(w_min, w));
		h = min(h_max, max(h_min, h));

		// set calculated W/H
		this->_set_param_int(XI_PRM_HEIGHT, h);
		this->_set_param_int(XI_PRM_WIDTH, w);

		// get offset limits for given W/H
		int x_min = this->_get_param_min(XI_PRM_OFFSET_X);
		int x_max = this->_get_param_max(XI_PRM_OFFSET_X);
		int y_min = this->_get_param_min(XI_PRM_OFFSET_Y);
		int y_max = this->_get_param_max(XI_PRM_OFFSET_Y);

		// check offset min-max
		x = min(x_max, max(x_min, nx));
		y = min(y_max, max(y_min, ny));
	}

	Roi r(x, y, w, h);
	hw_roi = r;

	DEB_RETURN() << DEB_VAR1(hw_roi);
}

void Camera::setRoi(const Roi& ask_roi)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(ask_roi);

	// check if new ROI is the same as currently set one
	Roi r;
	this->getRoi(r);
	if(r == ask_roi) return;

	if(ask_roi.isActive())
	{
		// reset offsets
		this->_set_param_int(XI_PRM_OFFSET_X, 0);
		this->_set_param_int(XI_PRM_OFFSET_Y, 0);

		// then set the new ROI
		// order is important, first we need to set w/h and only then the offsets
		this->_set_param_int(XI_PRM_WIDTH, ask_roi.getSize().getWidth());
		this->_set_param_int(XI_PRM_HEIGHT, ask_roi.getSize().getHeight());
		this->_set_param_int(XI_PRM_OFFSET_X, ask_roi.getTopLeft().x);
		this->_set_param_int(XI_PRM_OFFSET_Y, ask_roi.getTopLeft().y);
	}
}

void Camera::getRoi(Roi& hw_roi)
{
	DEB_MEMBER_FUNCT();

	int x = this->_get_param_int(XI_PRM_OFFSET_X);
	int y = this->_get_param_int(XI_PRM_OFFSET_Y);
	int w = this->_get_param_int(XI_PRM_WIDTH);
	int h = this->_get_param_int(XI_PRM_HEIGHT);

	Roi r(x, y, w, h);
	hw_roi = r;

	DEB_RETURN() << DEB_VAR1(hw_roi);
}

void Camera::getTriggerPolarity(TriggerPolarity& p)
{
	p = this->m_trig_polarity;
}

void Camera::setTriggerPolarity(TriggerPolarity p)
{
	this->m_trig_polarity = p;
}

void Camera::getSoftwareTrigger(bool& t)
{
	// always return false
	t = false;
}

void Camera::setSoftwareTrigger(bool t)
{
	this->_generate_soft_trigger();
}

void Camera::getGpiSelector(GPISelector& s)
{
	s = (GPISelector)this->_get_param_int(XI_PRM_GPI_SELECTOR);
}

void Camera::setGpiSelector(GPISelector s)
{
	this->_set_param_int(XI_PRM_GPI_SELECTOR, (int)s);
}

void Camera::getGpiMode(GPIMode& m)
{
	m = (GPIMode)this->_get_param_int(XI_PRM_GPI_MODE);
}

void Camera::setGpiMode(GPIMode m)
{
	this->_set_param_int(XI_PRM_GPI_MODE, (int)m);
}

void Camera::getGpiLevel(int& l)
{
	l = this->_get_param_int(XI_PRM_GPI_LEVEL);
}

void Camera::setGpiLevel(int l)
{
	this->_set_param_int(XI_PRM_GPI_LEVEL, l);
}

void Camera::getGpiLevelAtExpStart(int& l)
{
	l = this->_get_param_int(XI_PRM_GPI_LEVEL_AT_IMAGE_EXP_START);
}

void Camera::setGpiLevelAtExpStart(int l)
{
	this->_set_param_int(XI_PRM_GPI_LEVEL_AT_IMAGE_EXP_START, l);
}

void Camera::getGpiLevelAtExpEnd(int& l)
{
	l = this->_get_param_int(XI_PRM_GPI_LEVEL_AT_IMAGE_EXP_END);
}

void Camera::setGpiLevelAtExpEnd(int l)
{
	this->_set_param_int(XI_PRM_GPI_LEVEL_AT_IMAGE_EXP_END, l);
}

void Camera::getGpiDebounce(bool& e)
{
	e = (bool)this->_get_param_int(XI_PRM_DEBOUNCE_EN);
}

void Camera::setGpiDebounce(bool e)
{
	this->_set_param_int(XI_PRM_DEBOUNCE_EN, (int)e);
}

void Camera::getGpoSelector(GPOSelector& s)
{
	s = (GPOSelector)this->_get_param_int(XI_PRM_GPO_SELECTOR);
}

void Camera::setGpoSelector(GPOSelector s)
{
	this->_set_param_int(XI_PRM_GPO_SELECTOR, (int)s);
}

void Camera::getGpoMode(GPOMode& m)
{
	m = (GPOMode)this->_get_param_int(XI_PRM_GPO_MODE);
}

void Camera::setGpoMode(GPOMode m)
{
	this->_set_param_int(XI_PRM_GPO_MODE, (int)m);
}

void Camera::getLedSelector(LEDSelector& s)
{
	s = (LEDSelector)this->_get_param_int(XI_PRM_LED_SELECTOR);
}

void Camera::setLedSelector(LEDSelector s)
{
	this->_set_param_int(XI_PRM_LED_SELECTOR, (int)s);
}

void Camera::getLedMode(LEDMode& m)
{
	m = (LEDMode)this->_get_param_int(XI_PRM_LED_MODE);
}

void Camera::setLedMode(LEDMode m)
{
	this->_set_param_int(XI_PRM_LED_MODE, (int)m);
}

void Camera::checkBin(Bin &aBin)
{
	DEB_MEMBER_FUNCT();
	
	// get binning parameters info
	int h_max = this->_get_param_max(XI_PRM_BINNING_HORIZONTAL);
	int h_inc = this->_get_param_inc(XI_PRM_BINNING_HORIZONTAL);
	int v_max = this->_get_param_max(XI_PRM_BINNING_VERTICAL);
	int v_inc = this->_get_param_inc(XI_PRM_BINNING_VERTICAL);

	int binX, binY, expo, max_expo;
	int requestedX = aBin.getX();
	int requestedY = aBin.getY();

	// get binX supported by camera
	max_expo = log2l(h_max);
	for(expo = max_expo; expo >= 0; expo -= h_inc)
	{
		binX = pow(2, expo);
		if(requestedX % binX == 0) break;
	}

	// get binY supported by camera
	max_expo = log2l(v_max);
	for(expo = max_expo; expo >= 0; expo -= v_inc)
	{
		binY = pow(2, expo);
		if(requestedY % binY == 0) break;
	}

	aBin = Bin(binX, binY);

	DEB_RETURN() << DEB_VAR1(aBin);
}

void Camera::setBin(const Bin &aBin)
{
	DEB_MEMBER_FUNCT();

	// only sum mode is supported by Lima
	this->_set_param_int(XI_PRM_BINNING_HORIZONTAL_MODE, XI_BIN_MODE_SUM);
	this->_set_param_int(XI_PRM_BINNING_VERTICAL_MODE, XI_BIN_MODE_SUM);

	this->_set_param_int(XI_PRM_BINNING_HORIZONTAL, aBin.getX());
	this->_set_param_int(XI_PRM_BINNING_VERTICAL, aBin.getY());

	DEB_RETURN() << DEB_VAR1(aBin);
}

void Camera::getBin(Bin &aBin)
{
	DEB_MEMBER_FUNCT();

	int h = this->_get_param_int(XI_PRM_BINNING_HORIZONTAL);
	int v = this->_get_param_int(XI_PRM_BINNING_VERTICAL);
	aBin = Bin(h, v);

	DEB_RETURN() << DEB_VAR1(aBin);
}

int Camera::_get_param_int(const char* param)
{
	DEB_MEMBER_FUNCT();

	int r = 0;
	this->xi_status = xiGetParamInt(this->xiH, param, &r);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; xi_status: " << this->xi_status;
	return r;
}

double Camera::_get_param_dbl(const char* param)
{
	DEB_MEMBER_FUNCT();

	float r;
	this->xi_status = xiGetParamFloat(this->xiH, param, &r);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; xi_status: " << this->xi_status;
	return (double)r;
}

std::string Camera::_get_param_str(const char* param)
{
	DEB_MEMBER_FUNCT();

	char r[PARAMSTR_LEN];
	this->xi_status = xiGetParamString(this->xiH, param, (void*)r, PARAMSTR_LEN);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not get parameter " << param << "; xi_status: " << this->xi_status;
	return std::string(r);
}

void Camera::_set_param_int(const char* param, int value)
{
	DEB_MEMBER_FUNCT();

	this->xi_status = xiSetParamInt(this->xiH, param, value);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; xi_status: " << this->xi_status;
}

void Camera::_set_param_dbl(const char* param, double value)
{
	DEB_MEMBER_FUNCT();

	this->xi_status = xiSetParamFloat(this->xiH, param, (float)value);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; xi_status: " << this->xi_status;
}

void Camera::_set_param_str(const char* param, std::string value, int size)
{
	DEB_MEMBER_FUNCT();

	if(size == -1)
		size = value.length();

	this->xi_status = xiSetParamString(this->xiH, param, (void*)value.c_str(), size);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not set parameter " << param << " to " << value << "; xi_status: " << this->xi_status;
}

int Camera::_get_param_min(const char* param)
{
	string param_str(param);
	string info_str(XI_PRM_INFO_MIN);
	return this->_get_param_int((param_str + info_str).c_str());
}

int Camera::_get_param_max(const char* param)
{
	string param_str(param);
	string info_str(XI_PRM_INFO_MAX);
	return this->_get_param_int((param_str + info_str).c_str());
}

int Camera::_get_param_inc(const char* param)
{
	string param_str(param);
	string info_str(XI_PRM_INFO_INCREMENT);
	return this->_get_param_int((param_str + info_str).c_str());
}

void Camera::_read_image(XI_IMG* image, int timeout)
{
	DEB_MEMBER_FUNCT();

	image->size = sizeof(XI_IMG);
	this->xi_status = xiGetImage(this->xiH, timeout, image);
	
	// The following seems to be not necessary since AcqThread checks for
	// xi_status anyway. And throwing exceptions here breaks Lima...

	// if(this->xi_status != XI_OK)
	// 	THROW_HW_ERROR(Error) << "Image readout failed; xi_status: " << this->xi_status;
}

void Camera::_generate_soft_trigger(void)
{
	this->_set_param_int(XI_PRM_TRG_SOFTWARE, XI_ON);
	this->m_soft_trigger_issued = true;
}

bool Camera::_soft_trigger_issued(void)
{
	if(this->m_soft_trigger_issued)
	{
		this->m_soft_trigger_issued = false;
		return true;
	}
	return false;
}

void Camera::_setup_gpio_trigger(void)
{
	GPISelector selected_gpi;
	this->getGpiSelector(selected_gpi);

	this->setGpiSelector(this->m_trigger_gpi_port);
	this->setGpiMode(Camera::GPIMode_Trigger);

	this->setGpiSelector(selected_gpi);
}

int Camera::_get_trigger_timeout(void)
{
	// timeout + expo time
	double exp_time = 0;
	this->getExpTime(exp_time);
	int exp_ms = int(exp_time * TIME_HW / 1e3);	// convert to ms
	int timeout = this->m_timeout + exp_ms;
	return timeout;
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

void Camera::_set_status(Camera::Status status)
{
	DEB_MEMBER_FUNCT();
	
	this->m_status = this->xi_status == XI_OK ? status : Camera::Fault;
}

// Extra attributes

void Camera::getMode(Mode& m)
{
	m = (Mode)this->_get_param_int(XI_PRM_USER_SET_SELECTOR);
}

void Camera::setMode(Mode m)
{
	this->_set_param_int(XI_PRM_USER_SET_SELECTOR, (int)m);
	this->_set_param_int(XI_PRM_USER_SET_LOAD, 0);
}

void Camera::getGainSelector(GainSelector &s)
{
	s = (GainSelector)this->_get_param_int(XI_PRM_GAIN_SELECTOR);
}

void Camera::setGainSelector(GainSelector s)
{
	this->_set_param_int(XI_PRM_GAIN_SELECTOR, (int)s);
}

void Camera::getGain(int& g)
{
	g = this->_get_param_int(XI_PRM_GAIN);
}

void Camera::setGain(int g)
{
	this->_set_param_int(XI_PRM_GAIN, g);
}

void Camera::getIsCooled(bool& c)
{
	c = (bool)this->_get_param_int(XI_PRM_IS_COOLED);
}

void Camera::getTempControlMode(TempControlMode& m)
{
	m = (TempControlMode)this->_get_param_int(XI_PRM_COOLING);
}

void Camera::setTempControlMode(TempControlMode m)
{
	this->_set_param_int(XI_PRM_COOLING, (int)m);
}

void Camera::getTempTarget(double& t)
{
	t = this->_get_param_dbl(XI_PRM_TARGET_TEMP);
}

void Camera::setTempTarget(double t)
{
	this->_set_param_dbl(XI_PRM_TARGET_TEMP, t);
}

void Camera::getThermometer(Thermometer& t)
{
	t = (Thermometer)this->_get_param_int(XI_PRM_TEMP_SELECTOR);
}

void Camera::setThermometer(Thermometer t)
{
	this->_set_param_int(XI_PRM_TEMP_SELECTOR, (int)t);
}

void Camera::getTemperature(double& t)
{
	t = this->_get_param_dbl(XI_PRM_TEMP);
}

void Camera::getTempChip(double& t)
{
	t = this->_get_param_dbl(XI_PRM_CHIP_TEMP);
}

void Camera::getTempHousing(double& t)
{
	t = this->_get_param_dbl(XI_PRM_HOUS_TEMP);
}

void Camera::getTempBack(double& t)
{
	t = this->_get_param_dbl(XI_PRM_HOUS_BACK_SIDE_TEMP);
}

void Camera::getTempSensor(double& t)
{
	t = this->_get_param_dbl(XI_PRM_SENSOR_BOARD_TEMP);
}

void Camera::getThermalElement(ThermalElement& e)
{
	e = (ThermalElement)this->_get_param_int(XI_PRM_TEMP_ELEMENT_SEL);
}

void Camera::setThermalElement(ThermalElement e)
{
	this->_set_param_int(XI_PRM_TEMP_ELEMENT_SEL, (int)e);
}

void Camera::getThermalElementValue(double& v)
{
	v = this->_get_param_dbl(XI_PRM_TEMP_ELEMENT_VALUE);
}

void Camera::setThermalElementValue(double v)
{
	this->_set_param_dbl(XI_PRM_TEMP_ELEMENT_VALUE, v);
}

void Camera::getExposureSelector(ExposureSelector& s)
{
	s = (ExposureSelector)this->_get_param_int(XI_PRM_EXPOSURE_TIME_SELECTOR);
}

void Camera::setExposureSelector(ExposureSelector s)
{
	this->_set_param_int(XI_PRM_EXPOSURE_TIME_SELECTOR, (int)s);
}

void Camera::getBurstCount(int& c)
{
	c = this->_get_param_int(XI_PRM_EXPOSURE_BURST_COUNT);
}

void Camera::setBurstCount(int c)
{
	this->_set_param_int(XI_PRM_EXPOSURE_BURST_COUNT, c);
}

void Camera::getDownsampling(Downsampling& d)
{
	d = (Downsampling)this->_get_param_int(XI_PRM_DOWNSAMPLING);
}

void Camera::setDownsampling(Downsampling d)
{
	this->_set_param_int(XI_PRM_DOWNSAMPLING, (int)d);
}

void Camera::getDownsamplingType(DownsamplingType& t)
{
	t = (DownsamplingType)this->_get_param_int(XI_PRM_DOWNSAMPLING_TYPE);
}

void Camera::setDownsamplingType(DownsamplingType t)
{
	this->_set_param_int(XI_PRM_DOWNSAMPLING_TYPE, (int)t);
}

void Camera::getTestPatternGenerator(TestPatternGenerator& g)
{
	g = (TestPatternGenerator)this->_get_param_int(XI_PRM_TEST_PATTERN_GENERATOR_SELECTOR);
}

void Camera::setTestPatternGenerator(TestPatternGenerator g)
{
	this->_set_param_int(XI_PRM_TEST_PATTERN_GENERATOR_SELECTOR, (int)g);
}

void Camera::getTestPattern(TestPattern& p)
{
	p = (TestPattern)this->_get_param_int(XI_PRM_TEST_PATTERN);
}

void Camera::setTestPattern(TestPattern p)
{
	this->_set_param_int(XI_PRM_TEST_PATTERN, (int)p);
}

void Camera::getImageFormat(ImageFormat& f)
{
	f = (ImageFormat)this->_get_param_int(XI_PRM_IMAGE_DATA_FORMAT);
}

void Camera::setImageFormat(ImageFormat f)
{
	this->_set_param_int(XI_PRM_IMAGE_DATA_FORMAT, (int)f);
}

void Camera::getShutter(Shutter& s)
{
	s = (Shutter)this->_get_param_int(XI_PRM_SHUTTER_TYPE);
}

void Camera::setShutter(Shutter s)
{
	this->_set_param_int(XI_PRM_SHUTTER_TYPE, (int)s);
}

void Camera::getTaps(Taps& t)
{
	t = (Taps)this->_get_param_int(XI_PRM_SENSOR_TAPS);
}

void Camera::setTaps(Taps t)
{
	this->_set_param_int(XI_PRM_SENSOR_TAPS, (int)t);
}

void Camera::getAutoExposureGain(bool& a)
{
	a = (bool)this->_get_param_int(XI_PRM_AEAG);
}

void Camera::setAutoExposureGain(bool a)
{
	this->_set_param_int(XI_PRM_AEAG, (int)a);
}

void Camera::getAutoWhiteBalance(bool& a)
{
	a = (bool)this->_get_param_int(XI_PRM_AUTO_WB);
}

void Camera::setAutoWhiteBalance(bool a)
{
	this->_set_param_int(XI_PRM_AUTO_WB, (int)a);
}

void Camera::getHorizontalFlip(bool& f)
{
	f = (bool)this->_get_param_int(XI_PRM_HORIZONTAL_FLIP);
}

void Camera::setHorizontalFlip(bool f)
{
	this->_set_param_int(XI_PRM_HORIZONTAL_FLIP, (int)f);
}

void Camera::getVerticalFlip(bool& f)
{
	f = (bool)this->_get_param_int(XI_PRM_VERTICAL_FLIP);
}

void Camera::setVerticalFlip(bool f)
{
	this->_set_param_int(XI_PRM_VERTICAL_FLIP, (int)f);
}

void Camera::getInterlineExpMode(InterlineExpMode& m)
{
	m = (InterlineExpMode)this->_get_param_int(XI_PRM_INTERLINE_EXPOSURE_MODE);
}

void Camera::setInterlineExpMode(InterlineExpMode m)
{
	this->_set_param_int(XI_PRM_INTERLINE_EXPOSURE_MODE, (int)m);
}

void Camera::getBinningEngine(BinningEngine &e)
{
	e = (BinningEngine)this->_get_param_int(XI_PRM_BINNING_SELECTOR);
}

void Camera::setBinningEngine(BinningEngine e)
{
	this->_set_param_int(XI_PRM_BINNING_SELECTOR, (int)e);
}

void Camera::getHorizontalBinningPattern(BinningPattern &p)
{
	p = (BinningPattern)this->_get_param_int(XI_PRM_BINNING_HORIZONTAL_PATTERN);
}

void Camera::setHorizontalBinningPattern(BinningPattern p)
{
	this->_set_param_int(XI_PRM_BINNING_HORIZONTAL_PATTERN, (int)p);
}

void Camera::getVerticalBinningPattern(BinningPattern &p)
{
	p = (BinningPattern)this->_get_param_int(XI_PRM_BINNING_VERTICAL_PATTERN);
}

void Camera::setVerticalBinningPattern(BinningPattern p)
{
	this->_set_param_int(XI_PRM_BINNING_VERTICAL_PATTERN, (int)p);
}

void Camera::getDecimationEngine(DecimationEngine &e)
{
	e = (DecimationEngine)this->_get_param_int(XI_PRM_DECIMATION_SELECTOR);
}

void Camera::setDecimationEngine(DecimationEngine e)
{
	this->_set_param_int(XI_PRM_DECIMATION_SELECTOR, (int)e);
}

void Camera::getHorizontalDecimation(int& d)
{
	d = this->_get_param_int(XI_PRM_DECIMATION_HORIZONTAL);
}

void Camera::setHorizontalDecimation(int d)
{
	this->_set_param_int(XI_PRM_DECIMATION_HORIZONTAL, d);
}

void Camera::getVerticalDecimation(int& d)
{
	d = this->_get_param_int(XI_PRM_DECIMATION_VERTICAL);
}

void Camera::setVerticalDecimation(int d)
{
	this->_set_param_int(XI_PRM_DECIMATION_VERTICAL, d);
}

void Camera::getHorizontalDecimationPattern(DecimationPattern &p)
{
	p = (DecimationPattern)this->_get_param_int(XI_PRM_DECIMATION_HORIZONTAL_PATTERN);
}

void Camera::setHorizontalDecimationPattern(DecimationPattern p)
{
	this->_set_param_int(XI_PRM_DECIMATION_HORIZONTAL_PATTERN, (int)p);
}

void Camera::getVerticalDecimationPattern(DecimationPattern &p)
{
	p = (DecimationPattern)this->_get_param_int(XI_PRM_DECIMATION_VERTICAL_PATTERN);
}

void Camera::setVerticalDecimationPattern(DecimationPattern p)
{
	this->_set_param_int(XI_PRM_DECIMATION_VERTICAL_PATTERN, (int)p);
}

void Camera::getExposurePriority(double& p)
{
	p = this->_get_param_dbl(XI_PRM_EXP_PRIORITY);
}

void Camera::setExposurePriority(double p)
{
	this->_set_param_dbl(XI_PRM_EXP_PRIORITY, p);
}

void Camera::getAutoGainLimit(int& l)
{
	l = this->_get_param_int(XI_PRM_AG_MAX_LIMIT);
}

void Camera::setAutoGainLimit(int l)
{
	this->_set_param_int(XI_PRM_AG_MAX_LIMIT, l);
}

void Camera::getAutoExposureLimit(int& l)
{
	l = this->_get_param_int(XI_PRM_AE_MAX_LIMIT);
}

void Camera::setAutoExposureLimit(int l)
{
	this->_set_param_int(XI_PRM_AE_MAX_LIMIT, l);
}

void Camera::getAutoIntensityLevel(int& l)
{
	l = this->_get_param_int(XI_PRM_AEAG_LEVEL);
}

void Camera::setAutoIntensityLevel(int l)
{
	this->_set_param_int(XI_PRM_AEAG_LEVEL, l);
}

void Camera::getBandwidthLimit(double& l)
{
	l = this->_get_param_dbl(XI_PRM_LIMIT_BANDWIDTH);
}

void Camera::setBandwidthLimit(double l)
{
	this->_set_param_dbl(XI_PRM_LIMIT_BANDWIDTH, l);
}

void Camera::getBandwidthLimitEnabled(bool& e)
{
	e = (bool)this->_get_param_int(XI_PRM_LIMIT_BANDWIDTH_MODE);
}

void Camera::setBandwidthLimitEnabled(bool e)
{
	this->_set_param_int(XI_PRM_LIMIT_BANDWIDTH_MODE, (int)e);
}

void Camera::getAvailableBandwidth(double& b)
{
	b = this->_get_param_dbl(XI_PRM_AVAILABLE_BANDWIDTH);
}

void Camera::getFrameRate(double& r)
{
	r = this->_get_param_dbl(XI_PRM_FRAMERATE);
}

void Camera::setFrameRate(double r)
{
	this->_set_param_dbl(XI_PRM_FRAMERATE, r);
}

void Camera::getCounterSelector(CounterSelector& s)
{
	s = (CounterSelector)this->_get_param_int(XI_PRM_COUNTER_SELECTOR);
}

void Camera::setCounterSelector(CounterSelector s)
{
	this->_set_param_int(XI_PRM_COUNTER_SELECTOR, (int)s);
}

void Camera::getCounterValue(int& v)
{
	v = this->_get_param_int(XI_PRM_COUNTER_VALUE);
}

void Camera::getAcqTimingMode(AcqTimingMode& m)
{
	m = (AcqTimingMode)this->_get_param_int(XI_PRM_ACQ_TIMING_MODE);
}

void Camera::setAcqTimingMode(AcqTimingMode m)
{
	this->_set_param_int(XI_PRM_ACQ_TIMING_MODE, (int)m);
}

void Camera::getTriggerDelay(int& d)
{
	d = this->_get_param_int(XI_PRM_TRG_DELAY);
}

void Camera::setTriggerDelay(int d)
{
	this->_set_param_int(XI_PRM_TRG_DELAY, d);
}

void Camera::getAcqStatus(bool& s)
{
	s = (bool)this->_get_param_int(XI_PRM_ACQUISITION_STATUS);
}

void Camera::getFeatureSelector(FeatureSelector& s)
{
	s = (FeatureSelector)this->_get_param_int(XI_PRM_SENSOR_FEATURE_SELECTOR);
}

void Camera::setFeatureSelector(FeatureSelector s)
{
	this->_set_param_int(XI_PRM_SENSOR_FEATURE_SELECTOR, (int)s);
}

void Camera::getFeatureValue(int& v)
{
	v = this->_get_param_int(XI_PRM_SENSOR_FEATURE_VALUE); 
}

void Camera::setFeatureValue(int v)
{
	this->_set_param_int(XI_PRM_SENSOR_FEATURE_VALUE, v);
}

void Camera::reportException(Exception& e, std::string name)
{
	DEB_MEMBER_FUNCT();

	ostringstream err_msg;
	err_msg << name << " failed: " << e;
	Event::Code err_code = Event::CamCommError;
	Event *event = new Event(Hardware, Event::Error, Event::Camera, 
				 err_code, err_msg.str());
	DEB_EVENT(*event) << DEB_VAR1(*event);
	reportEvent(event);
}

void Camera::getTimeout(int &t)
{
	t = this->m_timeout;
}

void Camera::setTimeout(int t)
{
	DEB_MEMBER_FUNCT();

	if(t > Camera::TIMEOUT_MAX)
	{
		THROW_HW_ERROR(Error) << "Timeout " << t << "ms exceeds maximum allowed value of " << Camera::TIMEOUT_MAX << "ms.";
		return;
	}
	this->m_timeout = t;
}
