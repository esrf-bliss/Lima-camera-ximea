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

#ifdef WIN32
#	include "xiApi.h"
#else
#	include <m3api/xiApi.h>
#endif // WIN32

#include <ximea_export.h>

namespace lima
{
	namespace Ximea
	{

		class XIMEA_EXPORT Camera
		{
			DEB_CLASS_NAMESPC(DebModCamera, "Camera", "Ximea");

			friend class Interface;
			friend class SyncCtrlObj;

		public:
			Camera(int camera_id);
			~Camera();

		private:
			HANDLE xiH;
			XI_RETURN status;
		};

  	} // namespace Ximea
} // namespace lima


#endif // XIMEACAMERA_H
