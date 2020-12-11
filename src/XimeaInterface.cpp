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

#include "XimeaInterface.h"
#include "XimeaCamera.h"
#include "XimeaDetInfoCtrlObj.h"
#include "XimeaSyncCtrlObj.h"
#include "XimeaVideoCtrlObj.h"

using namespace lima;
using namespace lima::Ximea;

Interface::Interface(Camera& cam) : m_cam(cam)
{
	DEB_CONSTRUCTOR();
	this->m_det_info = new DetInfoCtrlObj(cam);
	this->m_sync = new SyncCtrlObj(cam);
	this->m_video = new VideoCtrlObj(cam);
}

Interface::~Interface()
{
	DEB_DESTRUCTOR();
	delete this->m_det_info;
	delete this->m_sync;
	delete this->m_video;
}

void Interface::getCapList(CapList &cap_list) const
{
	cap_list.push_back(HwCap(this->m_det_info));
	cap_list.push_back(HwCap(this->m_sync));
	cap_list.push_back(HwCap(this->m_video));
	cap_list.push_back(HwCap(&(this->m_video->getHwBufferCtrlObj())));
}

void Interface::reset(ResetLevel reset_level)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(reset_level);
}

void Interface::prepareAcq()
{
	DEB_MEMBER_FUNCT();
}

void Interface::startAcq()
{
	DEB_MEMBER_FUNCT();
}

void Interface::stopAcq()
{
	DEB_MEMBER_FUNCT();
}

void Interface::getStatus(StatusType& status)
{
	DEB_MEMBER_FUNCT();
}

int Interface::getNbHwAcquiredFrames()
{
	DEB_MEMBER_FUNCT();
	return 0;
}
