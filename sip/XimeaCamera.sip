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

namespace Ximea
{
	class Camera
	{
%TypeHeaderCode
#include <XimeaCamera.h>
%End

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
		Camera(int camera_id);
		~Camera();

		void prepareAcq();
		void startAcq();
		void stopAcq();

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

	private:
		Camera(const Ximea::Camera&);
	};
};
