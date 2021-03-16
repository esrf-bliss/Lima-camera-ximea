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

#ifndef XIMEACAMERA_H
#define XIMEACAMERA_H

#include "lima/Debug.h"
#include "lima/Exceptions.h"
#include "lima/HwMaxImageSizeCallback.h"
#include "lima/HwBufferMgr.h"

#ifdef WIN32
#	include "xiApi.h"
#else
#	include <m3api/xiApi.h>
#endif // WIN32

#include <ximea_export.h>

#include "MagicNumbers.h"

namespace lima
{
	namespace Ximea
	{
		class AcqThread;
		class XIMEA_EXPORT Camera
		{
			DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Ximea");

			friend class Interface;
			friend class SyncCtrlObj;
			friend class AcqThread;

		public:
			enum Status {
				Ready, Exposure, Readout, Latency, Fault
			};

			enum Preset {
				Preset_12_STD_L = XI_US_12_STD_L,
				Preset_12_STD_H = XI_US_12_STD_H,
				Preset_14_STD_L = XI_US_14_STD_L,
				Preset_None = XI_US_NONE,
				Preset_14_STD_H = XI_US_14_STD_H,
				Preset_2_12_CMS_S_L = XI_US_2_12_CMS_S_L,
				Preset_2_12_CMS_S_H = XI_US_2_12_CMS_S_H,
				Preset_2_14_CMS_S_L = XI_US_2_14_CMS_S_L,
				Preset_2_14_CMS_S_H = XI_US_2_14_CMS_S_H,
				Preset_4_12_CMS_S_L = XI_US_4_12_CMS_S_L,
				Preset_4_12_CMS_S_H = XI_US_4_12_CMS_S_H,
				Preset_4_14_CMS_S_L = XI_US_4_14_CMS_S_L,
				Preset_4_14_CMS_S_H = XI_US_4_14_CMS_S_H,
				Preset_2_12_HDR_HL = XI_US_2_12_HDR_HL,
				Preset_2_12_HDR_L = XI_US_2_12_HDR_L,
				Preset_2_12_HDR_H = XI_US_2_12_HDR_H,
				Preset_4_12_CMS_HDR_HL = XI_US_4_12_CMS_HDR_HL,
				Preset_2_14_HDR_L = XI_US_2_14_HDR_L,
				Preset_2_14_HDR_H = XI_US_2_14_HDR_H,
				Preset_2_12_CMS_A_L = XI_US_2_12_CMS_A_L,
				Preset_2_12_CMS_A_H = XI_US_2_12_CMS_A_H
			};

			enum GainSelector {
				GainSelector_All = XI_GAIN_SELECTOR_ALL,
				GainSelector_Analog_All = XI_GAIN_SELECTOR_ANALOG_ALL,
				GainSelector_Digital_All = XI_GAIN_SELECTOR_DIGITAL_ALL,
				GainSelector_Analog_Tap1 = XI_GAIN_SELECTOR_ANALOG_TAP1,
				GainSelector_Analog_Tap2 = XI_GAIN_SELECTOR_ANALOG_TAP2,
				GainSelector_Analog_Tap3 = XI_GAIN_SELECTOR_ANALOG_TAP3,
				GainSelector_Analog_Tap4 = XI_GAIN_SELECTOR_ANALOG_TAP4,
				GainSelector_Analog_North = XI_GAIN_SELECTOR_ANALOG_N,
				GainSelector_Analog_South = XI_GAIN_SELECTOR_ANALOG_S
			};

			enum TempControlMode {
				TempControlMode_Off = XI_TEMP_CTRL_MODE_OFF,
				TempControlMode_Auto = XI_TEMP_CTRL_MODE_AUTO,
				TempControlMode_Manual = XI_TEMP_CTRL_MODE_MANUAL
			};

			enum Thermometer {
				Thermometer_Die_Raw = XI_TEMP_IMAGE_SENSOR_DIE_RAW,
				Thermometer_Sensor_Die = XI_TEMP_IMAGE_SENSOR_DIE,
				Thermometer_Sensor_Board = XI_TEMP_SENSOR_BOARD,
				Thermometer_Interface_Board = XI_TEMP_INTERFACE_BOARD,
				Thermometer_Front_Housing = XI_TEMP_FRONT_HOUSING,
				Thermometer_Rear_Housing = XI_TEMP_REAR_HOUSING,
				Thermometer_TEC1_Cold = XI_TEMP_TEC1_COLD,
				Thermometer_TEC1_Hot = XI_TEMP_TEC1_HOT
			};

			enum ThermalElement {
				ThermalElement_TEC1 = XI_TEMP_ELEM_TEC1,
				ThermalElement_TEC2 = XI_TEMP_ELEM_TEC2,
				ThermalElement_Fan1 = XI_TEMP_ELEM_FAN1,
				ThermalElement_Fan_Start_Threshold = XI_TEMP_ELEM_FAN1_THRS_TEMP
			};

			enum ExposureSelector {
				ExposureSelector_Common = XI_EXPOSURE_TIME_SELECTOR_COMMON,
				ExposureSelector_Group1 = XI_EXPOSURE_TIME_SELECTOR_GROUP1,
				ExposureSelector_Group2 = XI_EXPOSURE_TIME_SELECTOR_GROUP2
			};

			enum Downsampling {
				Downsampling_1x1 = XI_DWN_1x1,
				Downsampling_2x2 = XI_DWN_2x2,
				Downsampling_3x3 = XI_DWN_3x3,
				Downsampling_4x4 = XI_DWN_4x4,
				Downsampling_5x5 = XI_DWN_5x5,
				Downsampling_6x6 = XI_DWN_6x6,
				Downsampling_7x7 = XI_DWN_7x7,
				Downsampling_8x8 = XI_DWN_8x8,
				Downsampling_9x9 = XI_DWN_9x9,
				Downsampling_10x10 = XI_DWN_10x10,
				Downsampling_16x16 = XI_DWN_16x16
			};

			enum DownsamplingType {
				DownsamplingType_Binning = XI_BINNING,
				DownsamplingType_Skipping = XI_SKIPPING
			};

			enum TestPatternGenerator {
				TestPatternGenerator_Sensor = XI_TESTPAT_GEN_SENSOR,
				TestPatternGenerator_FPGA = XI_TESTPAT_GEN_FPGA
			};

			enum TestPattern {
				TestPattern_Off = XI_TESTPAT_OFF,
				TestPattern_Black = XI_TESTPAT_BLACK,
				TestPattern_White = XI_TESTPAT_WHITE,
				TestPattern_Grey_Horizontal = XI_TESTPAT_GREY_HORIZ_RAMP,
				TestPattern_Grey_Vertical = XI_TESTPAT_GREY_VERT_RAMP,
				TestPattern_Grey_Horizontal_Moving = XI_TESTPAT_GREY_HORIZ_RAMP_MOVING,
				TestPattern_Grey_Vertical_Moving = XI_TESTPAT_GREY_VERT_RAMP_MOVING,
				TestPattern_Line_Horizontal_Moving = XI_TESTPAT_HORIZ_LINE_MOVING,
				TestPattern_Line_Vertical_Moving = XI_TESTPAT_VERT_LINE_MOVING,
				TestPattern_Color_Bar = XI_TESTPAT_COLOR_BAR,
				TestPattern_Frame_Counter = XI_TESTPAT_FRAME_COUNTER,
				TestPattern_Counter = XI_TESTPAT_DEVICE_SPEC_COUNTER
			};

			enum ImageFormat {
				ImageFormat_Mono8 = XI_MONO8,
				ImageFormat_Mono16 = XI_MONO16,
				ImageFormat_RGB24 = XI_RGB24,
				ImageFormat_RGB32 = XI_RGB32,
				ImageFormat_RGB_Planar = XI_RGB_PLANAR,
				ImageFormat_Raw8 = XI_RAW8,
				ImageFormat_Raw16 = XI_RAW16,
				ImageFormat_Transport = XI_FRM_TRANSPORT_DATA,
				ImageFormat_RGB48 = XI_RGB48,
				ImageFormat_RGB64 = XI_RGB64,
				ImageFormat_RGB16_Planar = XI_RGB16_PLANAR,
				ImageFormat_Raw8x2 = XI_RAW8X2,
				ImageFormat_Raw8x4 = XI_RAW8X4,
				ImageFormat_Raw16x2 = XI_RAW16X2,
				ImageFormat_Raw16x4 = XI_RAW16X4,
				ImageFormat_Raw32 = XI_RAW32,
				ImageFormat_Raw32_Float = XI_RAW32FLOAT
			};

			enum Shutter {
				Shutter_Global = XI_SHUTTER_GLOBAL,
				Shutter_Rolling = XI_SHUTTER_ROLLING,
				Shutter_Reset_Release = XI_SHUTTER_GLOBAL_RESET_RELEASE
			};

			enum Taps {
				Taps_1 = XI_TAP_CNT_1,
				Taps_2 = XI_TAP_CNT_2,
				Taps_4 = XI_TAP_CNT_4
			};

			enum InterlineExpMode {
				InterlineExpMode_Off = XI_INTERLINE_EXPOSURE_MODE_OFF,
				InterlineExpMode_On = XI_INTERLINE_EXPOSURE_MODE_ON
			};

			enum BinningEngine {
				BinningEngine_Sensor = XI_BIN_SELECT_SENSOR,
				BinningEngine_FPGA = XI_BIN_SELECT_DEVICE_FPGA,
				BinningEngine_Host = XI_BIN_SELECT_HOST_CPU
			};

			enum BinningPattern {
				BinningPattern_Mono = XI_BIN_MONO,
				BinningPattern_Bayer = XI_BIN_BAYER
			};

			enum DecimationEngine {
				DecimationEngine_Sensor = XI_DEC_SELECT_SENSOR,
				DecimationEngine_FPGA = XI_DEC_SELECT_DEVICE_FPGA,
				DecimationEngine_Host = XI_DEC_SELECT_HOST_CPU
			};

			enum DecimationPattern {
				DecimationPattern_Mono = XI_DEC_MONO,
				DecimationPattern_Bayer = XI_DEC_BAYER
			};

			enum CounterSelector {
				CounterSelector_Skipped_Frames_Transport = XI_CNT_SEL_TRANSPORT_SKIPPED_FRAMES,
				CounterSelector_Skipped_Frames_API = XI_CNT_SEL_API_SKIPPED_FRAMES,
				CounterSelector_Missed_Trigger_Overlap = XI_CNT_SEL_FRAME_MISSED_TRIGGER_DUETO_OVERLAP,
				CounterSelector_Missed_Trigger_Buffer_Full = XI_CNT_SEL_FRAME_MISSED_TRIGGER_DUETO_FRAME_BUFFER_OVR,
				CounterSelector_Frame_Buffer_Full = XI_CNT_SEL_FRAME_BUFFER_OVERFLOW
			};

			enum AcqTimingMode {
				AcqTimingMode_Free_Run = XI_ACQ_TIMING_MODE_FREE_RUN,
				AcqTimingMode_Frame_Rate = XI_ACQ_TIMING_MODE_FRAME_RATE,
				AcqTimingMode_Frame_Rate_Limit = XI_ACQ_TIMING_MODE_FRAME_RATE_LIMIT
			};

			enum FeatureSelector {
				FeatureSelector_Zero_ROT_Enable = XI_SENSOR_FEATURE_ZEROROT_ENABLE,
				FeatureSelector_Black_Level_Clamp = XI_SENSOR_FEATURE_BLACK_LEVEL_CLAMP,
				FeatureSelector_FPGA_Digital_Gain_Disable = XI_SENSOR_FEATURE_MD_FPGA_DIGITAL_GAIN_DISABLE,
				FeatureSelector_Acquisition_Running = XI_SENSOR_FEATURE_ACQUISITION_RUNNING,
				FeatureSelector_Timing_Mode = XI_SENSOR_FEATURE_TIMING_MODE,
				FeatureSelector_Parallel_ADC = XI_SENSOR_FEATURE_PARALLEL_ADC,
				FeatureSelector_Black_Level_Offset_Raw = XI_SENSOR_FEATURE_BLACK_LEVEL_OFFSET_RAW
			};

			Camera(int camera_id);
			~Camera();

			void prepareAcq();
			void startAcq();
			void stopAcq();

			void reset();

			// DetInfoCtrlObj
			void getImageType(ImageType& type);
			void setImageType(ImageType type);

			void getDetectorType(std::string& type);
			void getDetectorModel(std::string& model);
			void getDetectorImageSize(Size& size);

			// SyncCtrlObj
			void setExpTime(double exp_time);
			void getExpTime(double& exp_time);

			void setNbFrames(int nb_frames);
			void getNbFrames(int& nb_frames);

			void getNbHwAcquiredFrames(int& nb_acq_frames);

			void getStatus(Camera::Status& status);

			// Buffer control object
			HwBufferCtrlObj* getBufferCtrlObj();

			// ========== Extra attributes ==========

			// Presets
			void getPreset(Preset& p);
			void setPreset(Preset p);

			// Gain
			void getGainSelector(GainSelector& s);
			void setGainSelector(GainSelector s);
			void getGain(int& g);
			void setGain(int g);

			// Temperature
			void getIsCooled(bool& c);
			void getTempControlMode(TempControlMode& m);
			void setTempControlMode(TempControlMode m);
			void getTargetTemp(double& t);
			void setTargetTemp(double t);
			void getThermometer(Thermometer& t);
			void setThermometer(Thermometer t);
			void getTemperature(double& t);
			void getChipTemp(double& t);
			void getHousingTemp(double& t);
			void getBackTemp(double& t);
			void getSensorTemp(double& t);
			void getThermalElement(ThermalElement& e);
			void setThermalElement(ThermalElement e);
			void getThermalElementValue(double& v);
			void setThermalElementValue(double v);

			// Other parameters
			void getExposureSelector(ExposureSelector& s);
			void setExposureSelector(ExposureSelector s);
			void getBurstCount(int& c);
			void setBurstCount(int c);
			void getDownsampling(Downsampling& d);
			void setDownsampling(Downsampling d);
			void getDownsamplingType(DownsamplingType& t);
			void setDownsamplingType(DownsamplingType t);
			void getTestPatternGenerator(TestPatternGenerator& g);
			void setTestPatternGenerator(TestPatternGenerator g);
			void getTestPattern(TestPattern& p);
			void setTestPattern(TestPattern p);
			void getImageFormat(ImageFormat& f);
			void setImageFormat(ImageFormat f);
			void getShutter(Shutter& s);
			void setShutter(Shutter s);
			void getTaps(Taps& t);
			void setTaps(Taps t);
			void getAutoExposureGain(bool& a);
			void setAutoExposureGain(bool a);
			void getAutoWhiteBalance(bool& a);
			void setAutoWhiteBalance(bool a);
			void getHorizontalFlip(bool& f);
			void setHorizontalFlip(bool f);
			void getVerticalFlip(bool& f);
			void setVerticalFlip(bool f);
			void getInterlineExpMode(InterlineExpMode& m);
			void setInterlineExpMode(InterlineExpMode m);
			void getBinningEngine(BinningEngine& e);
			void setBinningEngine(BinningEngine e);
			void getHorizontalBinningPattern(BinningPattern& p);
			void setHorizontalBinningPattern(BinningPattern p);
			void getVerticalBinningPattern(BinningPattern& p);
			void setVerticalBinningPattern(BinningPattern p);
			void getDecimationEngine(DecimationEngine& e);
			void setDecimationEngine(DecimationEngine e);
			void getHorizontalDecimation(int& d);
			void setHorizontalDecimation(int d);
			void getVerticalDecimation(int& d);
			void setVerticalDecimation(int d);
			void getHorizontalDecimationPattern(DecimationPattern& p);
			void setHorizontalDecimationPattern(DecimationPattern p);
			void getVerticalDecimationPattern(DecimationPattern& p);
			void setVerticalDecimationPattern(DecimationPattern p);
			void getExposurePriority(double& p);
			void setExposurePriority(double p);
			void getAutoGainLimit(int& l);
			void setAutoGainLimit(int l);
			void getAutoExposureLimit(int& l);
			void setAutoExposureLimit(int l);
			void getAutoIntensityLevel(int& l);
			void setAutoIntensityLevel(int l);
			void getBandwidthLimit(double& l);
			void setBandwidthLimit(double l);
			void getBandwidthLimitEnabled(bool& e);
			void setBandwidthLimitEnabled(bool e);
			void getAvailableBandwidth(double& b);
			void getFrameRate(double& r);
			void setFrameRate(double r);
			void getCounterSelector(CounterSelector& s);
			void setCounterSelector(CounterSelector s);
			void getCounterValue(int& v);
			void getAcqTimingMode(AcqTimingMode& m);
			void setAcqTimingMode(AcqTimingMode m);
			void getTriggerDelay(int& d);
			void setTriggerDelay(int d);
			void getAcqStatus(bool& s);
			void getFeatureSelector(FeatureSelector& s);
			void setFeatureSelector(FeatureSelector s);
			void getFeatureValue(int& v);
			void setFeatureValue(int v);

		private:
			HANDLE xiH;
			XI_RETURN xi_status;

			Camera::Status m_status;
			int m_nb_frames;
			int m_image_number;
			size_t m_buffer_size;
			AcqThread* m_acq_thread;
			SoftBufferCtrlObj m_buffer_ctrl_obj;

			int _get_param_int(const char* param);
			double _get_param_dbl(const char* param);
			std::string _get_param_str(const char* param);

			void _set_param_int(const char* param, int value);
			void _set_param_dbl(const char* param, double value);
			void _set_param_str(const char* param, std::string value, int size=-1);

			void _read_image(XI_IMG* image, int timeout);

			void _stop_acq_thread();

			void _set_status(Camera::Status status);
		};

	} // namespace Ximea
} // namespace lima


#endif // XIMEACAMERA_H