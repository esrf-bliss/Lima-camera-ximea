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

#include <stdio.h>
#include "xiXCamera.h"

using namespace lima;
using namespace lima::xiX;
using namespace std;


//---------------------------
//- Ctor
//---------------------------
Camera::Camera(int camera_id)
{
	DEB_CONSTRUCTOR();

	this->status = xiOpenDevice(camera_id, &this->xiH);
	if(this->status != XI_OK) THROW_HW_ERROR(Error) << "Could not open camera " << camera_id << "; status: " << this->status;

	DEB_TRACE() << "Camera " << camera_id << " opened; status: " << this->status;
	printf("camera ok");
}

//---------------------------
//- Dtor
//---------------------------
Camera::~Camera()
{
	DEB_DESTRUCTOR();

	if(this->xiH) xiCloseDevice(this->xiH);
}
