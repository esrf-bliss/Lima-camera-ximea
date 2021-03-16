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
Camera::Camera(int camera_id)
	: xiH(nullptr),
	  xi_status(XI_OK),
	  m_status(Camera::Ready),
	  m_image_number(0),
	  m_buffer_size(0),
	  m_acq_thread(nullptr)
{
	DEB_CONSTRUCTOR();

	this->xi_status = xiOpenDevice(camera_id, &this->xiH);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Could not open camera " << camera_id << "; status: " << this->xi_status;

	// set buffer policy to managed by application
	this->_set_param_int(XI_PRM_BUFFER_POLICY, XI_BP_SAFE);

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

void Camera::prepareAcq()
{
	DEB_MEMBER_FUNCT();

	this->_stop_acq_thread();
	this->m_image_number = 0;
	this->m_buffer_size = this->m_buffer_ctrl_obj.getBuffer().getFrameDim().getMemSize();
	this->m_acq_thread = new AcqThread(*this);
	this->_set_status(Camera::Ready);
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
	this->_set_status(Camera::Ready);
}

void Camera::reset()
{
	DEB_MEMBER_FUNCT();

	this->_set_param_int(XI_PRM_DEVICE_RESET, XI_ON);
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

void Camera::getStatus(Camera::Status& status)
{
    DEB_MEMBER_FUNCT();

    status = this->m_status;
    DEB_RETURN() << DEB_VAR1(status);
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

void Camera::_read_image(XI_IMG* image, int timeout)
{
	DEB_MEMBER_FUNCT();

	image->size = sizeof(XI_IMG);
	this->xi_status = xiGetImage(this->xiH, timeout, image);
	if(this->xi_status != XI_OK)
		THROW_HW_ERROR(Error) << "Image readout failed; xi_status: " << this->xi_status;
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

void Camera::getPreset(Preset& p)
{
	p = (Preset)this->_get_param_int(XI_PRM_USER_SET_SELECTOR);
}

void Camera::setPreset(Preset p)
{
	this->_set_param_int(XI_PRM_USER_SET_SELECTOR, (int)p);
	this->_set_param_int(XI_PRM_USER_SET_LOAD, XI_ON);
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

void Camera::getTargetTemp(double& t)
{
	t = this->_get_param_dbl(XI_PRM_TARGET_TEMP);
}

void Camera::setTargetTemp(double t)
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

void Camera::getChipTemp(double& t)
{
	t = this->_get_param_dbl(XI_PRM_CHIP_TEMP);
}

void Camera::getHousingTemp(double& t)
{
	t = this->_get_param_dbl(XI_PRM_HOUS_TEMP);
}

void Camera::getBackTemp(double& t)
{
	t = this->_get_param_dbl(XI_PRM_HOUS_BACK_SIDE_TEMP);
}

void Camera::getSensorTemp(double& t)
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