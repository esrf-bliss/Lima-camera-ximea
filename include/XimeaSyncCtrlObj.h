//###########################################################################
// This file is part of LImA, a Library for Image Acquisition
//
// Copyright (C) : 2009-2011
// European Synchrotron Radiation Facility
// BP 220, Grenoble 38043
// FRANCE
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
#ifndef XIMEASYNCCTRLOBJ_H
#define XIMEASYNCCTRLOBJ_H

#include <ximea_export.h>

#include "lima/HwSyncCtrlObj.h"
#include "lima/HwInterface.h"

namespace lima
{
	namespace Ximea
	{
		class Camera;
		class SyncCtrlObj : public HwSyncCtrlObj
		{
			DEB_CLASS_NAMESPC(DebModCamera, "SyncCtrlObj", "Ximea");

		public:
			SyncCtrlObj(Camera&);
			virtual ~SyncCtrlObj();

			virtual bool checkTrigMode(TrigMode trig_mode);
			virtual void setTrigMode(TrigMode  trig_mode);
			virtual void getTrigMode(TrigMode& trig_mode);

			virtual void setExpTime(double  exp_time);
			virtual void getExpTime(double& exp_time);
			virtual bool checkAutoExposureMode(AutoExposureMode mode) const;
			virtual void setHwAutoExposureMode(AutoExposureMode mode);

			virtual void setLatTime(double  lat_time);
			virtual void getLatTime(double& lat_time);

			virtual void setNbHwFrames(int  nb_frames);
			virtual void getNbHwFrames(int& nb_frames);

			virtual void getValidRanges(ValidRangesType& valid_ranges);

			void startAcq();
			void stopAcq(bool clearQueue = true);
			
			void getStatus(HwInterface::StatusType&);

		private:
			Camera& m_cam;
			TrigMode m_trig_mode;
			int m_nb_frames;
			bool m_started;
		};

	} // namespace Ximea
} // namespace lima

#endif // XIMEASYNCCTRLOBJ_H
