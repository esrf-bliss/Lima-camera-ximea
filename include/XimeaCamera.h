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

// #include "XimeaVideoCtrlObj.h"
#include "MagicNumbers.h"

namespace lima
{
	namespace Ximea
	{
		// class VideoCtrlObj;
		class AcqThread;
		class XIMEA_EXPORT Camera
		{
			DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Ximea");

			friend class Interface;
			friend class SyncCtrlObj;
			friend class VideoCtrlObj;
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

			// Extra attributes
			void getPreset(Preset& p);
			void setPreset(Preset p);


		private:
			HANDLE xiH;
			XI_RETURN xi_status;

			Camera::Status m_status;
			int m_nb_frames;
			int m_image_number;
			size_t m_buffer_size;
			AcqThread* m_acq_thread;
			SoftBufferCtrlObj m_buffer_ctrl_obj;
			// VideoCtrlObj* m_video;

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
