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

#include <sstream>
#include "XimeaSyncCtrlObj.h"
#include "XimeaCamera.h"

using namespace lima;
using namespace lima::Ximea;

SyncCtrlObj::SyncCtrlObj(Camera& cam) : m_cam(cam)
{
}

SyncCtrlObj::~SyncCtrlObj()
{
}

bool SyncCtrlObj::checkTrigMode(TrigMode trig_mode)
{
	return false;
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
}

void SyncCtrlObj::getTrigMode(TrigMode &trig_mode)
{
}

void SyncCtrlObj::setExpTime(double exp_time)
{
}

void SyncCtrlObj::getExpTime(double &exp_time)
{
}

void SyncCtrlObj::setLatTime(double lat_time)
{
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
}

void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	DEB_MEMBER_FUNCT();
}

bool SyncCtrlObj::checkAutoExposureMode(HwSyncCtrlObj::AutoExposureMode mode) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);
	return false;
}

void SyncCtrlObj::setHwAutoExposureMode(AutoExposureMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);
}
