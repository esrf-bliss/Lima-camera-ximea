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

#ifndef XIMEAFRAMEGETTER_H
#define XIMEAFRAMEGETTER_H

#include <lima/SizeUtils.h>
#include <lima/HwMaxImageSizeCallback.h>

#include <ximea_export.h>

#include "XimeaCamera.h"

namespace lima
{
	class FrameDim;
	class Exception;
	namespace Ximea
	{
		struct XIMEA_EXPORT FrameGetter : public HwMaxImageSizeCallbackGen 
		{
			virtual ~FrameGetter() {}

			virtual Camera::Mode getMode() const = 0;
			virtual void prepareAcq() = 0;

			virtual bool getNextFrame(unsigned char *ptr) = 0;
			virtual unsigned long getFrameNr() const = 0;
			virtual void resetFrameNr(unsigned long frame_nr = 0) = 0;

			virtual void setFrameDim(const FrameDim &frame_dim) = 0;
			virtual void getFrameDim(FrameDim &frame_dim) const = 0;
			virtual void getEffectiveFrameDim(FrameDim &frame_dim) const = 0;
			virtual void getMaxImageSize(Size &max_image_size) const = 0;
		};

	} // namespace Ximea
} // namespace lima

#endif // XIMEAFRAMEGETTER_H
