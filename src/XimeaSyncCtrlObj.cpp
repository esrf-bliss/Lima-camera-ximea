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
	switch(trig_mode)
	{
		case IntTrig:
		case IntTrigMult:
		// case ExtTrigSingle:
		// not supoorted - lack of camera support for XI_PRM_EXPOSURE_BURST_COUNT
		case ExtTrigMult:
		// case ExtGate:
		// not supported - lack of camera support for XI_TRG_SEL_EXPOSURE_ACTIVE
			return true;

		default:
			return false;
	}
}

void SyncCtrlObj::setTrigMode(TrigMode trig_mode)
{
	DEB_MEMBER_FUNCT();
	if(!checkTrigMode(trig_mode))
		THROW_HW_ERROR(InvalidValue) << "Invalid trig_mode " << DEB_VAR1(trig_mode);
	this->m_cam.setTrigMode(trig_mode);
}

void SyncCtrlObj::getTrigMode(TrigMode& trig_mode)
{
	this->m_cam.getTrigMode(trig_mode);
}

void SyncCtrlObj::setExpTime(double exp_time)
{
	this->m_cam.setExpTime(exp_time);
}

void SyncCtrlObj::getExpTime(double& exp_time)
{
	this->m_cam.getExpTime(exp_time);
}

void SyncCtrlObj::setLatTime(double lat_time)
{
}

void SyncCtrlObj::getLatTime(double& lat_time)
{
}

void SyncCtrlObj::setNbHwFrames(int nb_frames)
{
	this->m_cam.setNbFrames(nb_frames);
}

void SyncCtrlObj::getNbHwFrames(int& nb_frames)
{
	this->m_cam.getNbFrames(nb_frames);
}

void SyncCtrlObj::getValidRanges(ValidRangesType& valid_ranges)
{
	DEB_MEMBER_FUNCT();

	// TODO: fill these example values with real ones
	valid_ranges.min_exp_time = 0;
	valid_ranges.max_exp_time = 10;
}

bool SyncCtrlObj::checkAutoExposureMode(HwSyncCtrlObj::AutoExposureMode mode) const
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

	return true;
}

void SyncCtrlObj::setHwAutoExposureMode(HwSyncCtrlObj::AutoExposureMode mode)
{
	DEB_MEMBER_FUNCT();
	DEB_PARAM() << DEB_VAR1(mode);

	this->m_cam._set_param_int(XI_PRM_AEAG,
		mode == HwSyncCtrlObj::ON ? XI_ON : XI_OFF
	);
}
