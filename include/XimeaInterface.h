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

#ifndef XIMEAINTERFACE_H
#define XIMEAINTERFACE_H

#include <ximea_export.h>

#include "lima/HwInterface.h"

namespace lima
{
	namespace Ximea
	{
		class Camera;
		class DetInfoCtrlObj;
		class SyncCtrlObj;
		class BinCtrlObj;
		class RoiCtrlObj;
		class EventCtrlObj;
		class XIMEA_EXPORT Interface : public HwInterface
		{
			DEB_CLASS_NAMESPC(DebModCamera, "XimeaInterface", "Ximea");

		public:
			Interface(Camera&);
			virtual ~Interface();

			//- From HwInterface
			virtual void getCapList(CapList&) const;
			virtual void reset(ResetLevel reset_level);
			virtual void prepareAcq();
			virtual void startAcq();
			virtual void stopAcq();
			virtual void getStatus(StatusType& status);
			virtual int	getNbHwAcquiredFrames();

			Camera& getCamera() { return this->m_cam; }
			const Camera& getCamera() const { return this->m_cam; }

		private:
			Camera& m_cam;
			// CapList m_cap_list;
			DetInfoCtrlObj* m_det_info;
			SyncCtrlObj* m_sync;
			BinCtrlObj*	m_bin;
			RoiCtrlObj*	m_roi;
			EventCtrlObj* m_event;
		};

	} // namespace Ximea
} // namespace lima

#endif // XIMEAINTERFACE_H
