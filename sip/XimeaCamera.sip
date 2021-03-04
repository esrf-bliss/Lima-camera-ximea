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

		enum TriggerPolarity {
			TriggerPolarity_Low_Falling, TriggerPolarity_High_Rising
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

		// Trigger polarity
		void getTriggerPolarity(TriggerPolarity& p);
		void setTriggerPolarity(TriggerPolarity p);

		// Software trigger
		void getSoftwareTrigger(bool &t);
		void setSoftwareTrigger(bool t);

	private:
		Camera(const Ximea::Camera&);
	};
};
